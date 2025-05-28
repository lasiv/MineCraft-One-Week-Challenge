#!/usr/bin/env python3
import sys
import xml.etree.ElementTree as ET
import html
import math
from typing import List, Tuple, Optional, Dict

# Faktor zum Skalieren der Umlet-Koordinaten
STRETCH_FACTOR = 0.02

def escape_latex(s: str) -> str:
    return (s.replace('&', r'\&')
             .replace('{', r'\{')
             .replace('}', r'\}')
             .replace('#', r'\#')
             .replace('~', r'\textasciitilde{}')
             .replace('<', r'\textless{}')
             .replace('>', r'\textgreater{}')
             .replace('_', r'\_'))

def warn(msg: str) -> None:
    print(f"Warning: {msg}", file=sys.stderr)

# Mapping für Relationstypen lt=...
REL_CMD_MAP = {
    '-':     'umlassoc',
    '<-':    'umluniassoc',
    '<.':    'umldep',
    '<<-':   'umlinherit',
    '<<.':   'umlimpl',
    '<<<<-': 'umlaggreg',
    '<<<<<-':'umlcompo',
    '<<<<->':'umluniaggreg',
    '<<<<<->':'umlunicompo',
}

class Package:
    def __init__(self, type_: str, xml_id: str, code: str,
                 x: int, y: int, w: int, h: int):
        self.type = type_
        self.id   = xml_id
        self.code = code.splitlines()
        self.x, self.y, self.w, self.h = x, y, w, h
        self.posx1 = self.posy1 = self.posx2 = self.posy2 = 0.0
        self.packages: List['Package'] = []
        self.elements: List['Element']  = []

    def derive_coords(self) -> None:
        raw_x1, raw_y1 = self.x,              self.y
        raw_x2, raw_y2 = self.x + self.w,     self.y - self.h
        self.posx1 = raw_x1 * STRETCH_FACTOR
        self.posy1 = -raw_y1 * STRETCH_FACTOR
        self.posx2 = raw_x2 * STRETCH_FACTOR
        self.posy2 = -raw_y2 * STRETCH_FACTOR

    def contains(self, px: float, py: float) -> bool:
        return (self.posx1 <= px <= self.posx2
                and self.posy2 <= py <= self.posy1)

class Element:
    def __init__(self, type_: str, xml_id: str, code: str,
                 x: int, y: int, w: int, h: int):
        self.type = type_
        self.id   = xml_id
        self.code = code
        self.x, self.y, self.w, self.h = x, y, w, h
        self.posx = self.posy = 0.0
        self.posx1 = self.posy1 = 0.0
        self.posx2 = self.posy2 = 0.0

    def derive_coords(self) -> None:
        # Mittelpunkt
        raw_cx = self.x + self.w/2
        raw_cy = self.y - self.h/2
        self.posx = raw_cx * STRETCH_FACTOR
        self.posy = -raw_cy * STRETCH_FACTOR
        # Bounding-Box
        raw_x1, raw_y1 = self.x,              self.y
        raw_x2, raw_y2 = self.x + self.w,     self.y - self.h
        self.posx1 = raw_x1 * STRETCH_FACTOR
        self.posy1 = -raw_y1 * STRETCH_FACTOR
        self.posx2 = raw_x2 * STRETCH_FACTOR
        self.posy2 = -raw_y2 * STRETCH_FACTOR

    def contains_point(self, px: float, py: float) -> bool:
        return (self.posx1 <= px <= self.posx2
                and self.posy2 <= py <= self.posy1)

    def connects(self, pts: List[Tuple[float, float]]) -> Optional[Tuple[int, float]]:
        if len(pts) < 1:
            return None
        ends = [
            (0,  pts[0], pts[1] if len(pts)>1 else None),
            (-1, pts[-1], pts[-2] if len(pts)>1 else None)
        ]
        for end_index, ex, ey, neighbor in ends:
            px, py = ex, ey
            if self.contains_point(px, py):
                if neighbor is None:
                    return (end_index, None)
                nx, ny = neighbor
                dx, dy = nx - self.posx, ny - self.posy
                angle = math.degrees(math.atan2(dy, dx))
                return (end_index, angle)
        return None

    def render(self) -> str:
        self.derive_coords()
        # Nutze ersten Code-Zeile als Name
        name = self.code.splitlines()[0] if self.code else ""
        return f"\\umlclass[x={self.posx:.2f},y={self.posy:.2f}]{{{escape_latex(name)}}}"

class Relation:
    def __init__(self, type_: str, xml_id: str, code: str,
                 attrs: str, x: int, y: int, w: int, h: int):
        self.type     = type_
        self.id       = xml_id
        self.code     = code
        self.attrs    = attrs
        self.x, self.y, self.w, self.h = x, y, w, h
        self.points:    List[Tuple[float, float]] = []
        # Verbindungsdaten
        self.start_id:  Optional[str] = None
        self.end_id:    Optional[str] = None
        self.anchor1:   Optional[float] = None
        self.anchor2:   Optional[float] = None
        self.loop:      bool = False
        # aus panel_attributes
        self.lt:        Optional[str] = None
        self.m1:        Optional[str] = None
        self.m2:        Optional[str] = None
        self.r1:        Optional[str] = None
        self.r2:        Optional[str] = None
        self.title:     Optional[str] = None
        # berechnete Linie-Geometrie
        self.geometry:  Optional[str] = None

    def calculate_positions(self) -> None:
        nums = [float(n) for n in self.attrs.split(';') if n]
        pts: List[Tuple[float, float]] = []
        for i in range(0, len(nums), 2):
            dx, dy = nums[i], nums[i+1]
            raw_x, raw_y = self.x + dx, self.y + dy
            pts.append((raw_x * STRETCH_FACTOR, -raw_y * STRETCH_FACTOR))
        self.points = pts

    def parse_attributes(self) -> None:
        lines = self.code.splitlines()
        leftover: List[str] = []
        for L in lines:
            if '=' in L:
                key, val = (x.strip() for x in L.split('=', 1))
                if key == 'lt':   self.lt = val
                elif key == 'm1': self.m1 = val
                elif key == 'm2': self.m2 = val
                elif key == 'r1': self.r1 = val
                elif key == 'r2': self.r2 = val
                else:
                    leftover.append(L)
            else:
                leftover.append(L)
        if leftover:
            self.title = leftover[-1].strip()

    def determine_connections(self, elements: List[Element]) -> None:
        # Start
        found = False
        if len(self.points) >= 1:
            for el in elements:
                conn = el.connects(self.points)
                if conn and conn[0] == 0:
                    self.start_id, self.anchor1 = el.id, conn[1]
                    found = True
                    break
        if not found:
            warn(f"Relation {self.id}: Startpunkt nicht verbunden")

        # Ende
        found = False
        if len(self.points) >= 2:
            for el in elements:
                conn = el.connects(self.points)
                if conn and conn[0] == -1:
                    self.end_id, self.anchor2 = el.id, conn[1]
                    found = True
                    break
        if not found:
            warn(f"Relation {self.id}: Endpunkt nicht verbunden")

        if self.start_id and self.end_id and self.start_id == self.end_id:
            self.loop = True

    def determine_geometry(self) -> None:
        if self.loop:
            self.geometry = None
            return
        pts = self.points
        n = len(pts)
        # zwei Punkte oder rein horizontal/vertikal
        if n <= 2 or pts[0][0] == pts[-1][0] or pts[0][1] == pts[-1][1]:
            self.geometry = "--"
            return

        def slope(a, b):
            dx = b[0] - a[0]; dy = b[1] - a[1]
            return abs(dy/dx) if dx != 0 else float('inf')

        start, end = pts[0], pts[-1]
        if n == 3:
            mid = pts[1]
            self.geometry = "-|" if slope(start, end) > slope(start, mid) else "|-"
        elif n == 4:
            second = pts[1]
            self.geometry = "|-|" if slope(start, end) > slope(start, second) else "-|-"
        else:
            warn(f"Relation {self.id}: >4 Punkte, nutze 4-Punkte-Logik")
            second = pts[1]
            self.geometry = "|-|" if slope(start, end) > slope(start, second) else "-|-"

    def render(self) -> str:
        lines: List[str] = []

        # fehlende Enden als Knoten
        if not self.start_id and len(self.points) >= 1:
            x0, y0 = self.points[0]
            node1 = f"{self.id}node1"
            lines.append(f"\\node at ({x0:.2f},{y0:.2f}) [name={node1}]{{}};")
            start_ref = node1
        else:
            start_ref = self.start_id

        if not self.end_id and len(self.points) >= 2:
            x1, y1 = self.points[-1]
            node2 = f"{self.id}node2"
            lines.append(f"\\node at ({x1:.2f},{y1:.2f}) [name={node2}]{{}};")
            end_ref = node2
        else:
            end_ref = self.end_id

        # Befehl und Argumentliste
        cmd = REL_CMD_MAP.get(self.lt, 'umlassoc')
        args: List[str] = [f"name={self.id}"]

        if self.loop:
            args.append("loop=1.5")
        else:
            if self.geometry:
                args.append(f"geometry={self.geometry}")
            if self.anchor1 is not None:
                args.append(f"anchor1={self.anchor1:.1f}")
            if self.anchor2 is not None:
                args.append(f"anchor2={self.anchor2:.1f}")

        # Multiplizitäten & Rollen
        if self.m1: args.append(f"mult1={self.m1}")
        if self.m2: args.append(f"mult2={self.m2}")
        if self.r1: args.append(f"arg1={escape_latex(self.r1)}")
        if self.r2: args.append(f"arg2={escape_latex(self.r2)}")

        lines.append(f"\\{cmd}[{','.join(args)}]{{{start_ref}}}{{{end_ref}}}")

        # Titel-Node
        if self.title:
            suffix = "-3" if self.geometry and len(self.geometry)>=3 else "-1"
            align  = "above"
            if suffix == "-3":
                mid_char = self.geometry[len(self.geometry)//2]
                if mid_char == "|":
                    align = "right"
            lines.append(f"\\node[{align}] at ({self.id}{suffix}) {{{escape_latex(self.title)}}};")

        return "\n".join(lines)

class Diagram:
    def __init__(self):
        self.packages:  List[Package]  = []
        self.elements:  List[Element]  = []
        self.relations: List[Relation] = []

    def calculate_all(self) -> None:
        # 1) Pakete
        for pkg in self.packages:
            pkg.derive_coords()
        # 2) Elemente
        for el in self.elements:
            el.derive_coords()
        # 3) Relationen
        for rel in self.relations:
            rel.calculate_positions()
            rel.parse_attributes()
            rel.determine_connections(self.elements)
            rel.determine_geometry()

    def render(self) -> List[str]:
        lines: List[str] = ["\\begin{tikzpicture}"]
        # Elemente
        for el in self.elements:
            txt = el.render()
            if txt:
                lines.extend(txt.splitlines())
        # Relationen
        for rel in self.relations:
            txt = rel.render()
            if txt:
                lines.extend(txt.splitlines())
        lines.append("\\end{tikzpicture}")
        return lines

def parse_uxf(path: str) -> Diagram:
    tree = ET.parse(path)
    root = tree.getroot()
    diag = Diagram()
    counters: Dict[str,int] = {}

    for el in root.findall("element"):
        type_ = el.findtext("id","").strip()
        raw   = el.findtext("panel_attributes","").strip()
        code  = html.unescape(raw)
        c     = el.find("coordinates")
        x, y, w, h = (int(c.findtext(tag,"0")) for tag in ("x","y","w","h"))

        idx = counters.get(type_, 0)
        counters[type_] = idx + 1
        xml_id = str(idx)

        if type_ == "UMLPackage":
            diag.packages.append(Package(type_, xml_id, code, x, y, w, h))
        elif type_ == "Relation":
            raw_attrs = el.findtext("additional_attributes","").strip()
            attrs     = html.unescape(raw_attrs)
            diag.relations.append(Relation(type_, xml_id, code, attrs, x, y, w, h))
        else:
            diag.elements.append(Element(type_, xml_id, code, x, y, w, h))

    return diag

def main(infile: str, outfile: str) -> None:
    diagram = parse_uxf(infile)
    diagram.calculate_all()
    lines = diagram.render()
    with open(outfile, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input.uxf> <output.tex>", file=sys.stderr)
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])
