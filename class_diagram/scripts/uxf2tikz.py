#!/usr/bin/env python3
"""
umlet2tikz.py

Konvertiert Umlet UXF-Dateien in TikZ-UML-Diagramme.
Berücksichtigt <zoom_level> beim Einlesen, ID = <type><Laufnummer>, alias=<ID>.
Workflow:
  1. Einlesen der UXF-Daten (inkl. Zoom-Korrektur)
  2. Koordinaten anpassen (Elemente, Pakete, Relationen)
  3. Relationen an Elemente ankoppeln (Winkel & Geometrie)
  4. Rendern der Elemente (alias=<ID>, tags, …)
  5. Rendern der Relationen
"""
import re
import sys
import xml.etree.ElementTree as ET
import html
import math
from typing import List, Tuple, Optional, Dict

STRETCH_FACTOR = 0.02

def convert(x_raw: float, y_raw: float) -> Tuple[float, float]:
    return x_raw * STRETCH_FACTOR, -y_raw * STRETCH_FACTOR

def escape_latex(s: str) -> str:
    return (s.replace('&', r'\&').replace('{', r'\{').replace('}', r'\}')
             .replace('#', r'\#').replace('~', r'\textasciitilde{}')
             .replace('<', r'\textless{}').replace('>', r'\textgreater{}')
             .replace('_', r'\_'))

def warn(msg: str) -> None:
    print(f"Warning: {msg}", file=sys.stderr)

REL_CMD_MAP = {
    '-':     'umlassoc',
    '->':    'umluniassoc',
    '.>':    'umldep',
    '->>':   'umlinherit',
    '.>>':   'umlimpl',
    '<<<<-': 'umlaggreg',
    '<<<<<-':'umlcompo',
    '<<<<->':'umluniaggreg',
    '<<<<<->':'umlunicompo',
}

class Package:
    def __init__(self, type_: str, xml_id: str, code: str,
                 x: float, y: float, w: float, h: float):
        self.type, self.id = type_, xml_id
        self.code = code.splitlines()
        self.x, self.y, self.w, self.h = x, y, w, h
        self.posx1 = self.posy1 = self.posx2 = self.posy2 = 0.0
        self.packages: List['Package'] = []
        self.elements: List['Element'] = []

    def derive_coords(self) -> None:
        x1, y1 = self.x, self.y
        x2, y2 = self.x + self.w, self.y + self.h
        self.posx1, self.posy1 = convert(x1, y1)
        self.posx2, self.posy2 = convert(x2, y2)

    def contains(self, px: float, py: float) -> bool:
        return (self.posx1 <= px <= self.posx2
                and self.posy2 <= py <= self.posy1)

class Element:
    def __init__(self, type_: str, xml_id: str, code: str,
                 x: float, y: float, w: float, h: float):
        self.type, self.id = type_, xml_id
        self.code = code
        self.x, self.y, self.w, self.h = x, y, w, h
        self.posx = self.posy = 0.0
        self.posx1 = self.posy1 = self.posx2 = self.posy2 = 0.0

    def derive_coords(self) -> None:
        cx, cy = self.x + self.w/2, self.y + self.h/2
        self.posx, self.posy = convert(cx, cy)
        x1, y1 = self.x, self.y
        x2, y2 = self.x + self.w, self.y + self.h
        self.posx1, self.posy1 = convert(x1, y1)
        self.posx2, self.posy2 = convert(x2, y2)

    def contains_point(self, px: float, py: float) -> bool:
        return (self.posx1 <= px <= self.posx2
                and self.posy2 <= py <= self.posy1)

    def anchor_angle(self, ex: float, ey: float) -> float:
        """
        Berechnet den Winkel vom Mittelpunkt (self.posx,self.posy)
        bis zum Schnittpunkt der Geraden (Mittelpunkt→(ex,ey)) mit der Bounding-Box.
        """
        cx, cy = self.posx, self.posy
        dx, dy = ex - cx, ey - cy
        # alle möglichen Intersections sammeln
        cands: List[Tuple[float, float, float]] = []

        # vertikale Kanten x = posx1 / posx2
        if dx != 0:
            for edge_x in (self.posx1, self.posx2):
                t = (edge_x - cx) / dx
                if t > 0:
                    y_int = cy + t * dy
                    if self.posy2 <= y_int <= self.posy1:
                        cands.append((t, edge_x, y_int))

        # horizontale Kanten y = posy1 / posy2
        if dy != 0:
            for edge_y in (self.posy1, self.posy2):
                t = (edge_y - cy) / dy
                if t > 0:
                    x_int = cx + t * dx
                    if self.posx1 <= x_int <= self.posx2:
                        cands.append((t, x_int, edge_y))

        if not cands:
            # kein Schnittpunkt gefunden → einfach zum Endpunkt
            return math.degrees(math.atan2(dy, dx))

        # kleinster positiver Parameter t ist die tatsächliche Berührung
        _, ix, iy = min(cands, key=lambda item: item[0])
        return math.degrees(math.atan2(iy - cy, ix - cx))

    def connects(self, pts: List[Tuple[float, float]]) -> Optional[Tuple[int, float]]:
        """
        Prüft, ob eines der Enden an dieses Element anschließt.
        Liefert (end_index, anchor_angle) zurück.
        """
        if not pts:
            return None
        ends = [
            (0,  pts[0]),
            (-1, pts[-1])
        ]
        for end_index, (px, py) in ends:
            if self.contains_point(px, py):
                # berechne jetzt den Winkel zur Bounding-Box
                angle = self.anchor_angle(px, py)
                return end_index, angle
        return None

    def render(self) -> str:

        # 2) Sonderfall Note
        if self.type == 'UMLNote':
            name = f"note_{self.id}"
            line1 = f"\\node at ({self.posx+1:.2f},{self.posy:.2f}) [name={name}]{{}};"
            text = escape_latex(self.code)
            line2 = (
                f"\\umlnote[x={self.posx:.2f},y={self.posy:.2f}]"
                f"{{{name}}}{{{text}}}"
            )
            return f"{line1}\n{line2}"

        # 3) Sonderfall Interface: erzwinge <<interface>> als erster Header-Eintrag
        code = self.code
        if self.type == 'UMLInterface':
            code = f"<<interface>>\n{code}"

        # 4) Aufteilen in bis zu 3 Sektionen per "--"
        lines = code.splitlines()
        parts, buf = [], []
        for L in lines:
            if L.strip() == '--':
                parts.append(buf)
                buf = []
            else:
                buf.append(L)
        parts.append(buf)
        if len(parts) > 3:
            parts = parts[:3]
        if len(parts) == 2:
            sec = " ".join(parts[1])
            parts = (
                [parts[0], [], parts[1]]
                if '(' in sec or ')' in sec
                else [parts[0], parts[1], []]
            )

        # 5) simple vs. full class
        simple = len(parts) == 1
        cmd = 'umlsimpleclass' if simple else 'umlclass'

        # 6) Header parsen: template=, stereotypes <<…>>, name
        sec0 = parts[0].copy()
        template = type_val = tags = None
        name = ''

        # template=
        if sec0 and sec0[0].lower().startswith('template='):
            template = sec0.pop(0).split('=',1)[1]

        # stereotypes: die erste <<…>>
        while sec0 and sec0[0].startswith('<<') and sec0[0].endswith('>>'):
            type_val = sec0.pop(0)[2:-2].strip()

        # tags {…}
        for entry in list(sec0):
            if entry.startswith('{') and entry.endswith('}'):
                tags = entry[1:-1].strip()
                sec0.remove(entry)
                break

        # Name
        if sec0:
            raw = sec0[0]
            if raw.startswith('_') and raw.endswith('_'):
                return ''           # skip objects
            if raw.startswith('/') and raw.endswith('/'):
                type_val = 'abstract'
                name = raw.strip('/').strip()
            else:
                name = raw

        name_esc = escape_latex(name)

        # 7) Optionen: x,y, alias, type, template, tags
        opts = [
            f"x={self.posx:.2f}",
            f"y={self.posy:.2f}",
            f"alias={self.id}"
        ]
        if type_val:  opts.append(f"type={{{escape_latex(type_val)}}}")
        if template:  opts.append(f"template={{{escape_latex(template)}}}")
        if tags:      opts.append(f"tags={{{escape_latex(tags)}}}")
        opt_str = '[' + ','.join(opts) + ']'

        # 8) Ausgabe beginnen
        out = [f"\\{cmd}{opt_str}{{{name_esc}}}"]

        # 9) Bei full class: Mitglieder & Methoden
        if not simple:
            def render_section(sec: List[str]) -> List[str]:
                res = []
                for L in sec:
                    txt = L if not L.startswith('-') else '-' + L
                    if txt.startswith('_') and txt.endswith('_'):
                        res.append(f"\\umlstatic{{{escape_latex(txt.strip('_'))}}}")
                    elif txt.startswith('/') and txt.endswith('/'):
                        res.append(f"\\umlvirt{{{escape_latex(txt.strip('/'))}}}")
                    else:
                        res.append(escape_latex(txt))
                return res

            members = render_section(parts[1])
            methods = render_section(parts[2])

            out.append('{')
            for m in members:
                out.append(f"  {m} \\\\")
            out.append('}')
            out.append('{')
            for m in methods:
                out.append(f"  {m} \\\\")
            out.append('}')

        return '\n'.join(out)


class Relation:
    def __init__(self, type_: str, xml_id: str, code: str,
                 attrs: str, x: float, y: float, w: float, h: float):
        self.type, self.id = type_, xml_id
        self.code, self.attrs = code, attrs
        self.x, self.y, self.w, self.h = x, y, w, h
        self.points: List[Tuple[float, float]] = []
        self.start_id = self.end_id = None
        self.anchor1 = self.anchor2 = None
        self.loop = False
        self.lt = self.m1 = self.m2 = self.r1 = self.r2 = self.title = self.geometry = None
        self.mirrored = False

    def calculate_positions(self) -> None:
        nums = [float(n) for n in self.attrs.split(';') if n]
        pts = []
        for i in range(0, len(nums), 2):
            dx, dy = nums[i], nums[i+1]
            raw_x, raw_y = self.x + dx, self.y + dy
            pts.append(convert(raw_x, raw_y))
        self.points = pts

    def parse_attributes(self) -> None:
        lines, leftover = self.code.splitlines(), []
        for L in lines:
            if '=' in L:
                k, v = (x.strip() for x in L.split('=', 1))
                if   k == 'lt':   self.lt = v
                elif k == 'm1':   self.m1 = v
                elif k == 'm2':   self.m2 = v
                elif k == 'r1':   self.r1 = v
                elif k == 'r2':   self.r2 = v
                else:             leftover.append(L)
            else:
                leftover.append(L)
        if leftover:
            self.title = leftover[-1].strip()
        
        if self.lt and self.lt not in REL_CMD_MAP:
            # swap <→> and >→<
            swapped = ''.join('>' if c=='<' else '<' if c=='>' else c
                               for c in self.lt)
            # then reverse the string
            mirror = swapped[::-1]
            if mirror in REL_CMD_MAP:
                self.lt        = mirror
                self.mirrored = True

    def determine_connections(self, elements: List[Element]) -> None:
        found0 = False
        if len(self.points) >= 1:
            for el in elements:
                conn = el.connects(self.points)
                if conn and conn[0] == 0:
                    self.start_id, self.anchor1 = el.id, conn[1]
                    found0 = True
                    break
        if not found0:
            warn(f"Relation {self.id}: Startpunkt nicht verbunden")

        found1 = False
        if len(self.points) >= 2:
            for el in elements:
                conn = el.connects(self.points)
                if conn and conn[0] == -1:
                    self.end_id, self.anchor2 = el.id, conn[1]
                    found1 = True
                    break
        if not found1:
            warn(f"Relation {self.id}: Endpunkt nicht verbunden")

        if self.start_id and self.end_id and self.start_id == self.end_id:
            self.loop = True

    def determine_geometry(self) -> None:
        pts = self.points
        n = len(pts)

        # Self‐Loops und zu wenige Punkte → immer gerade Linien
        if self.loop or n < 2:
            self.geometry = "--"
            return

        # Bei exakt 2 Punkten → immer gerade Linien
        if n == 2:
            self.geometry = "--"
            return

        # Berechne Steigung von erstem zu letztem Punkt
        dx_end = pts[-1][0] - pts[0][0]
        dy_end = pts[-1][1] - pts[0][1]
        # Null- oder Unendliche Steigung → gerade
        if dx_end == 0 or dy_end == 0:
            self.geometry = "--"
            return
        s_end = abs(dy_end / dx_end)

        # Berechne Steigung von erstem zu zweitem Punkt
        dx_mid = pts[1][0] - pts[0][0]
        dy_mid = pts[1][1] - pts[0][1]
        s_mid = float('inf') if dx_mid == 0 else abs(dy_mid / dx_mid)

        # Vergleiche und wähle Geometrie
        if s_mid > s_end:
            # stärker steigend → vertikal‐dann‐horizontal
            self.geometry = "|-|" if n > 3 else "|-"
        elif s_mid < s_end:
            # stärker flach → horizontal‐dann‐vertikal
            self.geometry = "-|-" if n > 3 else "-|"
        else:
            # gleich → gerade
            self.geometry = "--"

    def render(self) -> str:
        lines: List[str] = []

        # --- Dummy-Knoten für unverbundene Enden ---
        if not self.start_id and len(self.points) >= 1:
            x0, y0 = self.points[0]
            n1 = f"{self.id}node1"
            lines.append(f"\\node at ({x0:.2f},{y0:.2f}) [name={n1}]{{}};")
            s_ref = n1
        else:
            s_ref = self.start_id

        if not self.end_id and len(self.points) >= 2:
            x1, y1 = self.points[-1]
            n2 = f"{self.id}node2"
            lines.append(f"\\node at ({x1:.2f},{y1:.2f}) [name={n2}]{{}};")
            e_ref = n2
        else:
            e_ref = self.end_id

        # --- Mirror: Enden tauschen & Geometrie umkehren ---
        if getattr(self, 'mirrored', False):
            s_ref, e_ref = e_ref, s_ref
            self.anchor1, self.anchor2 = self.anchor2, self.anchor1
            if self.geometry:
                self.geometry = self.geometry[::-1]

        # --- Basis-Befehl & Anchors/Geometry ---
        cmd = REL_CMD_MAP.get(self.lt, 'umlassoc')
        args = [f"name={self.id}"]
        if self.loop:
            args.append("loop=1.5")
        else:
            if self.geometry:
                args.append(f"geometry={self.geometry}")
            if self.anchor1 is not None:
                args.append(f"anchor1={self.anchor1:.1f}")
            if self.anchor2 is not None:
                args.append(f"anchor2={self.anchor2:.1f}")

        # --- mult1 / arg1 mit pos1=0.2 ---
        pos1_flag = False
        if self.m1:
            clean_m1 = re.sub(r'[^A-Za-z0-9\[\]\(\),\.\*]', '', self.m1)
            args.append(f"mult1={{{clean_m1}}}")
            pos1_flag = True
        if self.r1:
            clean_r1 = re.sub(r'[^A-Za-z0-9,]', '', self.r1)
            args.append(f"arg1={{{clean_r1}}}")
            pos1_flag = True
        if pos1_flag:
            args.append("pos1=0.2")

        # --- mult2 / arg2 mit pos2 berechnen ---
        pos2_flag = False
        if self.m2:
            clean_m2 = re.sub(r'[^A-Za-z0-9\[\]\(\),\.\*]', '', self.m2)
            args.append(f"mult2={{{clean_m2}}}")
            pos2_flag = True
        if self.r2:
            clean_r2 = re.sub(r'[^A-Za-z0-9,]', '', self.r2)
            args.append(f"arg2={{{clean_r2}}}")
            pos2_flag = True
        if pos2_flag:
            if self.geometry == "--" or not self.geometry:
                p2 = 0.8
            else:
                # len(self.geometry)-1 + 0.8  => |- → 1.8, |-| → 2.8, etc.
                p2 = (len(self.geometry) - 1) + 0.8
            args.append(f"pos2={p2:.1f}")

        # --- Relation ausgeben ---
        lines.append(f"\\{cmd}[{','.join(args)}]{{{s_ref}}}{{{e_ref}}}")

        # --- Titel, falls vorhanden ---
        if self.title:
            suff = "-3" if self.geometry and len(self.geometry) >= 3 else "-1"
            align = "right" if (suff == "-3" and self.geometry[len(self.geometry)//2] == "|") else "above"
            lines.append(
                f"\\node[{align}] at ({self.id}{suff}) {{{escape_latex(self.title)}}};"
            )

        return "\n".join(lines)


class Diagram:
    def __init__(self):
        self.packages:  List[Package]  = []
        self.elements:  List[Element]  = []
        self.relations: List[Relation] = []

    def calculate_all(self) -> None:
        for pkg in self.packages:
            pkg.derive_coords()
        for el in self.elements:
            el.derive_coords()
        for rel in self.relations:
            rel.calculate_positions()
            rel.parse_attributes()
            rel.determine_connections(self.elements)
            rel.determine_geometry()

    def render(self) -> List[str]:
        lines = ["\\begin{tikzpicture}"]
        for el in self.elements:
            txt = el.render()
            if txt:
                lines.extend(txt.splitlines())
        for rel in self.relations:
            txt = rel.render()
            if txt:
                lines.extend(txt.splitlines())
        lines.append("\\end{tikzpicture}")
        return lines

def parse_uxf(path: str) -> Diagram:
    tree = ET.parse(path)
    root = tree.getroot()

    zoom_level = int(root.findtext('zoom_level', '10'))
    zoom_div   = zoom_level / 10.0

    diag = Diagram()
    counters: Dict[str,int] = {}

    for el in root.findall("element"):
        type_ = el.findtext("id","").strip()
        raw   = el.findtext("panel_attributes","").strip()
        code  = html.unescape(raw)
        c     = el.find("coordinates")
        x = int(c.findtext('x','0')) / zoom_div
        y = int(c.findtext('y','0')) / zoom_div
        w = int(c.findtext('w','0')) / zoom_div
        h = int(c.findtext('h','0')) / zoom_div

        idx = counters.get(type_, 0)
        counters[type_] = idx + 1
        xml_id = f"{type_}{idx}"

        if type_ == "UMLPackage":
            diag.packages.append(Package(type_, xml_id, code, x, y, w, h))
        elif type_ == "Relation":
            attrs = html.unescape(el.findtext("additional_attributes","").strip())
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
