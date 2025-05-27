#!/usr/bin/env python3
"""
umlet2tikz.py

Konvertiert Umlet UXF-Dateien in TikZ-UML-Diagramme.
Stufe 6+: Typ- und XML-ID-Unterscheidung, eindeutige Knoten-IDs.
"""
import sys
import xml.etree.ElementTree as ET
import html
import re
from typing import List, Tuple, Optional, Dict

# Konstante zum Skalieren der Umlet-Koordinaten
STRETCH_FACTOR = 0.02

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

# --- Hilfsfunktionen ---
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

# --- Modelle ---
class Package:
    def __init__(self, type_: str, xml_id: str, code: str,
                 x: int, y: int, w: int, h: int):
        self.type    = type_
        self.id      = xml_id
        self.code    = code.splitlines()
        self.x, self.y, self.w, self.h = x, y, w, h
        # spätere, skalierte Eckkoordinaten
        self.posx1 = self.posy1 = self.posx2 = self.posy2 = 0.0
        self.packages: List['Package'] = []
        self.elements: List['Element']  = []

    def derive_coords(self) -> None:
        # raw_x1/y1 = obere linke Ecke
        raw_x1, raw_y1 = self.x,              self.y
        # raw_x2/y2 = untere rechte Ecke (y wächst nach unten in Umlet)
        raw_x2, raw_y2 = self.x + self.w,      self.y - self.h
        self.posx1 = raw_x1 * STRETCH_FACTOR
        self.posy1 = -raw_y1 * STRETCH_FACTOR
        self.posx2 = raw_x2 * STRETCH_FACTOR
        self.posy2 = -raw_y2 * STRETCH_FACTOR

    def contains(self, px: float, py: float) -> bool:
        return (self.posx1 <= px <= self.posx2
                and self.posy2 <= py <= self.posy1)

    def render_tree(self, depth: int = 0) -> List[str]:
        # 1) ersten beiden Zeilen Code auswerten: Stereotyp und Properties
        stereo = None
        props  = None
        name   = ""
        if self.code:
            # erste Zeile: evtl. <<stereo>>
            first = self.code[0].strip()
            if first.startswith("<<") and first.endswith(">>"):
                stereo = first[2:-2].strip()
                # falls nächstes eine Zeile ist, nehmen wir sie als Name
                if len(self.code) > 1:
                    name = self.code[1].strip()
            else:
                name = first

            # suche nach {tags}
            for line in self.code[1:]:
                ln = line.strip()
                if ln.startswith("{") and ln.endswith("}"):
                    props = ln[1:-1].strip()
                    break

        name_esc = escape_latex(name)

        # 2) Optionen: immer x,y zuerst; dann stereo als type und props als tags
        opts: List[str] = [
            f"x={self.posx1:.2f}",
            f"y={self.posy1:.2f}"
        ]
        if stereo:
            opts.append(f"type={{{escape_latex(stereo)}}}")
        if props:
            opts.append(f"tags={{{escape_latex(props)}}}")

        opt_str = f"[{','.join(opts)}]" if opts else ""

        # 3) Begin/Content/End
        lines: List[str] = []
        # Begin-Package
        lines.append(f"\\begin{{umlpackage}}{opt_str}{{{name_esc}}}")

        # nested packages
        for pkg in self.packages:
            lines.extend(pkg.render_tree(depth+1))

        # Elemente im Package
        for elem in self.elements:
            txt = elem.render()
            if txt:
                # wenn Multi-Line, dann indentieren
                for l in txt.splitlines():
                    lines.append("  " * (depth+1) + l)
        # End-Package
        lines.append("\\end{umlpackage}")
        return lines

class Element:
    def __init__(self, type_: str, xml_id: str, code: str, x: int, y: int, w: int, h: int):
        self.type = type_
        self.id = xml_id
        self.code = code
        self.x, self.y, self.w, self.h = x, y, w, h
        self.posx = self.posy = 0.0

    def calculate_position(self) -> Tuple[float, float]:
        raw_x = self.x + self.w/2
        raw_y = self.y - self.h/2
        self.posx = raw_x * STRETCH_FACTOR
        self.posy = -raw_y * STRETCH_FACTOR
        return self.posx, self.posy

    def render(self) -> str:
        self.calculate_position()
        if self.type == 'UMLNote':
            name = f"note_{self.id}"
            line1 = f"\\node at ({self.posx+1:.2f},{self.posy:.2f}) [name={name}]{{}};"
            text = escape_latex(self.code)
            line2 = (f"\\umlnote[x={self.posx:.2f},y={self.posy:.2f}]"
                     f"{{{name}}}{{{text}}}")
            return f"{line1}\n{line2}"
        lines = self.code.splitlines()
        parts, buf = [], []
        for L in lines:
            if L.strip() == '--': parts.append(buf); buf = []
            else: buf.append(L)
        parts.append(buf)
        if len(parts) > 3: parts = parts[:3]
        if len(parts) == 2:
            sec = " ".join(parts[1])
            parts = [parts[0], [], parts[1]] if '(' in sec or ')' in sec else [parts[0], parts[1], []]
        simple = len(parts) == 1
        cmd = 'umlsimpleclass' if simple else 'umlclass'
        sec0 = parts[0]
        type_val: Optional[str] = None
        template: Optional[str] = None
        name = ''
        if sec0 and sec0[0].lower().startswith('template='):
            template = sec0.pop(0).split('=',1)[1]
        while sec0 and sec0[0].startswith('<<') and sec0[0].endswith('>>'):
            type_val = sec0.pop(0)[2:-2].strip()
        if sec0:
            raw_name = sec0[0]
            if raw_name.startswith('_') and raw_name.endswith('_'):
                return ''
            if raw_name.startswith('/') and raw_name.endswith('/'):
                type_val = 'abstract'; name = raw_name.strip('/').strip()
            else:
                name = raw_name
        name_esc = escape_latex(name)
        opts = [f"x={self.posx:.2f}", f"y={self.posy:.2f}"]
        if type_val: opts.append(f"type={{{escape_latex(type_val)}}}")
        if template: opts.append(f"template={{{escape_latex(template)}}}")
        opt_str = '[' + ','.join(opts) + ']' if opts else ''
        out = [f"\\{cmd}{opt_str}{{{name_esc}}}"]
        if not simple:
            def rl(sec: List[str]) -> List[str]:
                rendered = []
                for L in sec:
                    txt = L if not L.startswith('-') else '-' + L
                    if txt.startswith('_') and txt.endswith('_'):
                        rendered.append(f"\\umlstatic{{{escape_latex(txt.strip('_'))}}}")
                    elif txt.startswith('/') and txt.endswith('/'):
                        rendered.append(f"\\umlvirt{{{escape_latex(txt.strip('/'))}}}")
                    else:
                        rendered.append(escape_latex(txt))
                return rendered
            members = rl(parts[1])
            methods = rl(parts[2])
            out.append('{')
            for m in members: out.append(f"  {m} \\\\")
            out.append('}')
            out.append('{')
            for m in methods: out.append(f"  {m} \\\\")
            out.append('}')
        return '\n'.join(out)

class Relation:
    def __init__(self, type_: str, xml_id: str, code: str, attributes: str,
                 x: int, y: int, w: int, h: int):
        self.type = type_
        self.id = xml_id
        self.code = code
        self.attrs = attributes
        self.x, self.y, self.w, self.h = x, y, w, h
        self.points: List[Tuple[float, float]] = []

    def calculate_positions(self) -> List[Tuple[float, float]]:
        nums = [float(n) for n in self.attrs.split(';') if n]
        pts: List[Tuple[float, float]] = []
        for i in range(0, len(nums), 2):
            dx, dy = nums[i], nums[i+1]
            raw_x = self.x + dx; raw_y = self.y + dy
            px, py = raw_x * STRETCH_FACTOR, -raw_y * STRETCH_FACTOR
            pts.append((px, py))
        self.points = pts; return pts

    def render(self) -> str:
        pts = self.calculate_positions()
        if not pts: return ''
        lines, names = [], []
        for px, py in pts:
            nm = f"{self.id}_{int(px*100)}_{int(py*100)}"; names.append(nm)
            lines.append(f"\\node at ({px:.2f},{py:.2f}) [name={nm}]{{}};")
        if len(names) > 2:
            lines.append("\\draw (" + names[0] + ")" + ''.join(f" -- ({n})" for n in names[1:-1]) + ";")
        cmd = REL_CMD_MAP.get(self.code.split('=',1)[1], 'umlassoc')
        a, b = names[-2], names[-1]
        lines.append(f"\\{cmd}[name={self.id}]{{{a}}}{{{b}}}")
        return '\n'.join(lines)

class Diagram:
    def __init__(self):
        self.packages: List[Package] = []
        self.elements: List[Element] = []
        self.relations: List[Relation] = []

    def _find_deepest(self, pkg: Package, px: float, py: float) -> Optional[Package]:
        for c in pkg.packages:
            if c.contains(px,py):
                return self._find_deepest(c, px, py)
        return pkg if pkg.contains(px,py) else None

    def render(self) -> str:
        # 1) Positionen berechnen
        for pkg in self.packages:   pkg.derive_coords()
        for elem in self.elements:  elem.calculate_position()
        for rel in self.relations:  rel.calculate_positions()

        # 2) Paket-Hierarchie
        roots: List[Package] = []
        for pkg in self.packages:
            parent = next((p for p in self.packages
                           if p is not pkg
                           and p.contains(pkg.posx1,pkg.posy1)
                           and p.contains(pkg.posx2,pkg.posy2)), None)
            if parent: parent.packages.append(pkg)
            else:      roots.append(pkg)

        # 3) Elemente zuordnen
        unbound: List[Element] = []
        for elem in self.elements:
            assigned = False
            for root in roots:
                target = self._find_deepest(root, elem.posx, elem.posy)
                if target:
                    target.elements.append(elem)
                    assigned = True
                    break
            if not assigned:
                unbound.append(elem)

        # 4) Ausgabe
        lines = ["\\begin{tikzpicture}"]
        for pkg in roots:
            lines.extend(pkg.render_tree())
        for e in unbound:
            r = e.render()
            if r: lines.append(r)
        for r in self.relations:
            t = r.render()
            if t: lines.append(t)
        lines.append("\\end{tikzpicture}")
        return "\n".join(lines)

def parse_uxf(path: str) -> Diagram:
    tree = ET.parse(path)
    root = tree.getroot()
    diag = Diagram()
    counters: Dict[str,int] = {}
    for el in root.findall('element'):
        type_ = el.findtext('id','').strip()
        raw   = el.findtext('panel_attributes','').strip()
        code  = html.unescape(raw)
        c     = el.find('coordinates')
        x,y,w,h = (int(c.findtext(tag,'0')) for tag in ('x','y','w','h'))
        idx   = counters.get(type_,0); counters[type_]=idx+1
        xml_id = f"{type_}{idx}"
        if type_ == 'UMLPackage':
            diag.packages.append(Package(type_, xml_id, code, x,y,w,h))
        elif type_ == 'Relation':
            raw_attrs = el.findtext('additional_attributes','').strip()
            attrs     = html.unescape(raw_attrs)
            diag.relations.append(Relation(type_, xml_id, code, attrs, x,y,w,h))
        else:
            diag.elements.append(Element(type_, xml_id, code, x,y,w,h))
    return diag

def main(infile: str, outfile: str) -> None:
    diagram = parse_uxf(infile)
    tikz    = diagram.render()
    with open(outfile,'w',encoding='utf-8') as f:
        f.write(tikz)

if __name__ == '__main__':
    if len(sys.argv)!=3:
        print(f"Usage: {sys.argv[0]} <in.uxf> <out.tex>", file=sys.stderr)
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])