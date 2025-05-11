#!/usr/bin/env python3
"""
Convert PlantUML .puml files to TikZ-UML LaTeX code with manual positioning.
Usage:
  puml_to_tikz.py [input.puml] [output.txt]
  - If no arguments: reads ../config_class.puml and writes ./tikz.txt
  - If one  argument: reads input.puml and writes ./tikz.txt
  - If two arguments: reads input.puml and writes output.txt
"""
import os
import re
import sys
import argparse
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

# Default file paths
DEFAULT_IN  = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'config_class.puml'))
DEFAULT_OUT = os.path.abspath(os.path.join(os.getcwd(), 'tikz.txt'))

# LaTeX-escape special characters
def escape_latex(s: str) -> str:
    return (s.replace('&', r'\&')
             .replace('{', r'\{')
             .replace('}', r'\}')
             .replace('#', r'\#')
             .replace('_', r'\_')
             .replace('~', r'\raisebox{0.5ex}{\texttildelow}'))

# Map PlantUML arrows to TikZ-UML commands
ARROW_MAP = {
    '--|>': 'umlinherit', '<|--': 'umlinherit',
    '--':   'umlassoc',
    '-->':  'umluniassoc', '<--':  'umluniassoc',
    '..>':  'umldep',    '<..':  'umldep',
    '0--':  'umlaggreg', '--0':  'umlaggreg',
    'o--':  'umlaggreg', '--o':  'umlaggreg',
    '0..>': 'umluniaggreg','<..0':'umluniaggreg',
    'o..>': 'umluniaggreg','<..o':'umluniaggreg',
    '*--':  'umlcompo',   '--*':  'umlcompo',
    '*-->': 'umlunicompo','<--*':'umlunicompo',
    '..|>': 'umlimpl',    '<|..': 'umlimpl',
    '+--':  'umlnest',    '--+':  'umlnest'
}

# Regex for relations
REL_RE = re.compile(
    r'^(?P<A>\w+)\s*'
    r'(?:"(?P<mult1>[^\"]+)")?\s*'
    r'(?P<arrow>(' + '|'.join(map(re.escape, ARROW_MAP)) + r'))\s*'
    r'(?:"(?P<mult2>[^\"]+)")?\s*'
    r'(?P<B>\w+)'  
    r'(?:\s*:\s*(?P<name>\S+))?'
    r'$'
)

# Definition regex
DEF_RE = re.compile(r'^(?P<kind>abstract|class|enum)\s+"(?P<name>[^\"]+)"\s+as\s+(?P<ref>\w+)')

class Definition:
    def __init__(self, kind, name, ref):
        self.kind = kind
        self.name = escape_latex(name)
        self.ref = ref
        self.ctors = []
        self.methods = []
        self.fields = []

    def finalize(self):
        # Omit default ctor/destructor if alone
        base = self.name
        default_ctor = f"{base}()"
        default_dtor = f"~{base}()"
        if self.ctors:
            non_defaults = [c for c in self.ctors if c.strip() not in (default_ctor, default_dtor)]
            if not non_defaults and len(self.ctors) == 1:
                logging.info(f"Omitting default constructor/destructor in {self.name}")
                self.ctors = []

        # Prepend remaining ctors to methods
        if self.ctors:
            self.methods = self.ctors + self.methods
            self.ctors = []

    def to_tikz(self):
        # Build options: always include position, and abstract flag if needed
        opts = ['x=0', 'y=0']
        if self.kind == 'abstract':
            opts.append('type=abstract')
        opt_str = '[' + ', '.join(opts) + ']' if self.kind in ('class', 'abstract') else '[x=0, y=0]'

        lines = []
        if self.kind == 'enum':
            # Enums: enum command uses only position opts
            lines.append(f"\\umlenum{opt_str}{{{self.name}}}")
            lines.append('{')
            for idx, entry in enumerate(self.fields):
                sep = ' \\\\' if idx < len(self.fields)-1 else ''
                lines.append(f'  {escape_latex(entry)}{sep}')
            lines.append('}')
            lines.append('{}')
        else:
            lines.append(f"\\umlclass{opt_str}{{{self.name}}}")
            # Fields block
            lines.append('{')
            for idx, fld in enumerate(self.fields):
                sep = ' \\\\' if idx < len(self.fields)-1 else ''
                lines.append(f'  {escape_latex(fld)}{sep}')
            lines.append('}')
            # Methods block
            lines.append('{')
            for idx, mth in enumerate(self.methods):
                sep = ' \\\\' if idx < len(self.methods)-1 else ''
                lines.append(f'  {escape_latex(mth)}{sep}')
            lines.append('}')
        return lines


def parse_definitions(lines):
    defs = []
    ref_map = {}
    i = 0
    while i < len(lines):
        m = DEF_RE.match(lines[i])
        if not m:
            i += 1
            continue
        kind, name, ref = m.group('kind'), m.group('name'), m.group('ref')
        d = Definition(kind, name, ref)
        ref_map[ref] = d.name
        i += 1
        # Skip to '{'
        while i < len(lines) and '{' not in lines[i]:
            i += 1
        i += 1
        # Collect body
        body = []
        while i < len(lines) and '}' not in lines[i]:
            body.append(lines[i].strip())
            i += 1
        i += 1
        # Split into ctor/method/field sections
        if kind != 'enum':
            if '..' in body and '__' in body:
                idx_ctor = body.index('..')
                idx_sep  = body.index('__')
                d.ctors    = body[:idx_ctor]
                d.methods  = [ln for ln in body[idx_ctor+1:idx_sep] if ln != '..']
                d.fields   = [ln for ln in body[idx_sep+1:] if ln]
            else:
                if '__' in body:
                    idx = body.index('__')
                    d.methods = [ln for ln in body[:idx] if ln not in ('..','__')]
                    d.fields  = [ln for ln in body[idx+1:] if ln]
                else:
                    d.methods = [ln for ln in body if ln not in ('..','__')]
        else:
            d.fields = [ln for ln in body if ln]
        d.finalize()
        defs.append(d)
    return defs, ref_map


def parse_relations(lines, ref_map):
    rels = []
    for ln in lines:
        m = REL_RE.match(ln.strip())
        if not m:
            continue
        gd = m.groupdict()
        arrow = gd['arrow']
        cmd = ARROW_MAP[arrow]

        # Determine direction
        A_ref, B_ref = gd['A'], gd['B']
        if arrow.startswith('<'):
            A_ref, B_ref = B_ref, A_ref

        # Dereference names
        A = ref_map.get(A_ref, escape_latex(A_ref))
        B = ref_map.get(B_ref, escape_latex(B_ref))

        # Build argument list
        args = ['pos1=0.5']
        if gd.get('name'):
            args.insert(0, f'arg1={escape_latex(gd["name"])}')
        if gd.get('mult1') or gd.get('mult2'):
            logging.warning(f"Ignoring multiplicities on relation '{ln.strip()}'")

        arg_str = '[' + ', '.join(args) + ']' if gd.get('name') else '[pos1=0.5]'
        rels.append(f"\\{cmd}{arg_str}{{{A}}}{{{B}}}")
    return rels


def transform(text: str) -> str:
    lines = [l for l in text.splitlines() if not l.strip().startswith('@startuml') and not l.strip().startswith('@enduml')]
    defs, ref_map = parse_definitions(lines)
    tikz = ['\\begin{tikzpicture}', '']
    for d in defs:
        tikz.extend(d.to_tikz())
        tikz.append('')
    tikz.extend(parse_relations(lines, ref_map))
    tikz.append('')
    tikz.append('\\end{tikzpicture}')
    return '\n'.join(tikz)


def main():
    parser = argparse.ArgumentParser(description='Convert PUML to TikZ-UML with positions')
    parser.add_argument('input', nargs='?', default=DEFAULT_IN, help='Input PUML file')
    parser.add_argument('output', nargs='?', help='Output TXT file')
    args = parser.parse_args()
    inp = os.path.abspath(args.input)
    outp = os.path.abspath(args.output) if args.output else DEFAULT_OUT
    if not os.path.isfile(inp):
        logging.error(f"Input file not found: {inp}")
        sys.exit(1)
    text = open(inp, 'r', encoding='utf-8').read()
    result = transform(text)
    os.makedirs(os.path.dirname(outp), exist_ok=True)
    with open(outp, 'w', encoding='utf-8') as f:
        f.write(result)
    logging.info(f"Wrote TikZ output to {outp}")


if __name__ == '__main__':
    main()
