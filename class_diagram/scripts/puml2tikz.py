#!/usr/bin/env python3
import re
import sys

def escape_latex(s: str) -> str:
    """Escape LaTeX special characters in the given string."""
    return (s.replace('&', r'\&')
             .replace('{', r'\{')
             .replace('}', r'\}')
             .replace('#', r'\#'))

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

# Regex for relations (optional multiplicities & label)
REL_RE = re.compile(
    r'^(?P<A>\w+)\s*'
    r'(?:"(?P<mult1>[^"]+)")?\s*'
    r'(?P<arrow>(' + '|'.join(map(re.escape, ARROW_MAP)) + r'))\s*'
    r'(?:"(?P<mult2>[^"]+)")?\s*'
    r'(?P<B>\w+)'  # B reference
    r'(?:\s*:\s*(?P<name>\S+))?'  # optional :name
    r'$'
)

def parse_relations(lines, ref_to_name):
    rels = []
    for ln in lines:
        m = REL_RE.match(ln.strip())
        if not m:
            continue
        gd = m.groupdict()
        arrow = gd['arrow']
        cmd = ARROW_MAP[arrow]

        # Always point toward B
        A_ref, B_ref = gd['A'], gd['B']
        if arrow.startswith('<'):
            A_ref, B_ref = B_ref, A_ref

        # Dereference to real names
        A = ref_to_name.get(A_ref, escape_latex(A_ref))
        B = ref_to_name.get(B_ref, escape_latex(B_ref))

        # Warn and ignore multiplicities
        if gd['mult1'] or gd['mult2']:
            print(f"Warning: ignoring multiplicities on relation '{ln.strip()}'", file=sys.stderr)

        # Build args list
        args = ['pos1=0.5']
        if gd.get('name'):
            args.insert(0, f'arg1={escape_latex(gd["name"])})')

        rels.append(f'\t\\{cmd}[{", ".join(args)}]{{{A}}}{{{B}}}')
    return rels


def transform(text: str) -> str:
    lines = text.splitlines()
    # Build ref->name map
    ref_to_name = {}
    DEF_RE = re.compile(r'^(abstract|class|enum)\s+"([^"]+)"\s+as\s+(\w+)')
    for ln in lines:
        m = DEF_RE.match(ln)
        if m:
            _, rawname, ref = m.groups()
            ref_to_name[ref] = escape_latex(rawname)

    out = [r'\begin{tikzpicture}', '']
    i = 0
    while i < len(lines):
        m = DEF_RE.match(lines[i])
        if not m:
            i += 1
            continue
        kind, rawname, ref = m.groups()
        name = ref_to_name[ref]
        i += 1
        # skip to opening brace
        while i < len(lines) and not re.match(rf'^(?:{kind})\s+{re.escape(ref)}\s*\{{', lines[i]):
            i += 1
        i += 1  # skip '{'
        body = []
        while i < len(lines) and lines[i].strip() != '}':
            body.append(lines[i].strip())
            i += 1

        # Header & body
        if kind == 'enum':
            # Enums: simple handling
            out.append(f'\t\\umlenum{{{name}}}')
            out.append('\t{')
            for idx, entry in enumerate(body):
                if not entry: continue
                sep = ' \\\\' if idx < len(body)-1 else ''
                out.append(f'\t\t{escape_latex(entry)}{sep}')
            out.append('\t}\n')
        else:
            # Classes/Abstract: split into ctors/methods/fields
            ctors, methods, fields = [], [], []
            if '..' in body and '__' in body:
                idx_ctor = body.index('..')
                idx_sep  = body.index('__')
                ctors    = body[:idx_ctor]
                methods  = [ln for ln in body[idx_ctor+1:idx_sep] if ln != '..']
                fields   = [ln for ln in body[idx_sep+1:] if ln]
            else:
                # No constructors: everything before '__' is methods
                if '__' in body:
                    idx_sep = body.index('__')
                    methods = [ln for ln in body[:idx_sep] if ln != '..']
                    fields  = [ln for ln in body[idx_sep+1:] if ln]
                else:
                    methods = [ln for ln in body if ln not in ('..', '__')]
            # Emit class/abstract
            optstr = '[type=abstract]' if kind=='abstract' else ''
            out.append(f'\t\\umlclass{optstr}{{{name}}}')
            # Log omitted constructors (commented)
            #print("Omitted constructor: {escape_latex(ctor)}")
            # Fields block
            out.append('\t{')
            for idx, fld in enumerate(fields):
                sep = ' \\\\' if idx < len(fields)-1 else ''
                out.append(f'\t\t{escape_latex(fld)}{sep}')
            out.append('\t}')
            # Methods block
            out.append('\t{')
            for idx, mth in enumerate(methods):
                sep = ' \\\\' if idx < len(methods)-1 else ''
                out.append(f'\t\t{escape_latex(mth)}{sep}')
            out.append('\t}\n')

    # Relations
    out.extend(parse_relations(lines, ref_to_name))
    out.append('\n' + r'\end{tikzpicture}')
    return '\n'.join(out)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} in.puml out.tex", file=sys.stderr)
        sys.exit(1)
    text = open(sys.argv[1]).read()
    open(sys.argv[2], 'w').write(transform(text))
    print(f"Wrote {sys.argv[2]}")
