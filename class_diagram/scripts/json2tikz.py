from dataclasses import dataclass, field
from typing import List, Optional, Dict, Tuple
import json
import sys
import os
import re
from collections import defaultdict

# --- JSON Data Models ---
@dataclass
class JsonSourceLocation:
    file: str
    line: int
    column: int
    translation_unit: str

@dataclass
class JsonMetadata:
    clang_uml_version: str
    llvm_version: str
    schema_version: int

@dataclass
class JsonParameter:
    name: str
    type: str
    default_value: Optional[str] = None
    source_location: Optional[JsonSourceLocation] = None

@dataclass
class JsonTemplateParameter:
    kind: str
    name: Optional[str] = None
    type: Optional[str] = None
    default: Optional[str] = None
    is_variadic: bool = False
    template_parameters: List['JsonTemplateParameter'] = field(default_factory=list)

@dataclass
class JsonMember:
    name: str
    type: str
    access: str
    is_static: bool
    default_value: Optional[str] = None
    source_location: Optional[JsonSourceLocation] = None

@dataclass
class JsonMethod:
    name: str
    display_name: str
    type: str
    access: str
    is_constructor: bool
    is_defaulted: bool
    is_deleted: bool
    is_copy_assignment: bool
    is_move_assignment: bool
    is_virtual: bool
    is_pure_virtual: bool
    is_noexcept: bool
    is_const: bool
    is_consteval: bool
    is_constexpr: bool
    is_static: bool
    is_operator: bool
    is_coroutine: bool
    parameters: List[JsonParameter] = field(default_factory=list)
    template_parameters: List[JsonTemplateParameter] = field(default_factory=list)
    source_location: Optional[JsonSourceLocation] = None

@dataclass
class JsonBase:
    id: str
    access: str
    is_virtual: bool

@dataclass
class JsonElement:
    id: str
    name: str
    display_name: str
    namespace: str
    type: str
    is_abstract: bool
    is_struct: bool
    is_union: bool
    is_template: bool
    is_nested: bool
    constants: List[str] = field(default_factory=list)
    members: List[JsonMember] = field(default_factory=list)
    methods: List[JsonMethod] = field(default_factory=list)
    template_parameters: List[JsonTemplateParameter] = field(default_factory=list)
    bases: List[JsonBase] = field(default_factory=list)
    source_location: Optional[JsonSourceLocation] = None

@dataclass
class JsonRelationship:
    source: str
    destination: str
    type: str
    access: Optional[str] = None
    label: Optional[str] = None
    multiplicity_source: Optional[str] = None
    multiplicity_destination: Optional[str] = None
    source_location: Optional[JsonSourceLocation] = None

@dataclass
class JsonDiagram:
    name: str
    diagram_type: str
    package_type: str
    metadata: JsonMetadata
    elements: List[JsonElement] = field(default_factory=list)
    relationships: List[JsonRelationship] = field(default_factory=list)

# --- JSON Parsing Helpers ---

def parse_source_location(data: dict) -> JsonSourceLocation:
    return JsonSourceLocation(**data)


def parse_template_parameter(data: dict) -> JsonTemplateParameter:
    nested = [parse_template_parameter(tp) for tp in data.get('template_parameters', [])]
    return JsonTemplateParameter(
        kind=data['kind'],
        name=data.get('name'),
        type=data.get('type'),
        default=data.get('default'),
        is_variadic=data.get('is_variadic', False),
        template_parameters=nested
    )

# --- Load JSON Diagram ---

def load_diagram(path: str) -> JsonDiagram:
    with open(path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    md = data['metadata']
    metadata = JsonMetadata(**md)
    elements: List[JsonElement] = []
    for el in data.get('elements', []):
        sl = parse_source_location(el['source_location']) if el.get('source_location') else None
        members = [JsonMember(
            name=m['name'],
            type=m['type'],
            access=m['access'],
            is_static=m['is_static'],
            default_value=m.get('default_value'),
            source_location=parse_source_location(m['source_location']) if m.get('source_location') else None
        ) for m in el.get('members', [])]
        methods = []
        for mm in el.get('methods', []):
            msl = parse_source_location(mm['source_location']) if mm.get('source_location') else None
            params = [JsonParameter(
                name=p['name'],
                type=p['type'],
                default_value=p.get('default_value'),
                source_location=parse_source_location(p['source_location']) if p.get('source_location') else None
            ) for p in mm.get('parameters', [])]
            tmpls = [parse_template_parameter(tp) for tp in mm.get('template_parameters', [])]
            methods.append(JsonMethod(
                name=mm['name'],
                display_name=mm['display_name'],
                type=mm['type'],
                access=mm['access'],
                is_constructor=mm['is_constructor'],
                is_defaulted=mm['is_defaulted'],
                is_deleted=mm['is_deleted'],
                is_copy_assignment=mm['is_copy_assignment'],
                is_move_assignment=mm['is_move_assignment'],
                is_virtual=mm['is_virtual'],
                is_pure_virtual=mm['is_pure_virtual'],
                is_noexcept=mm['is_noexcept'],
                is_const=mm['is_const'],
                is_consteval=mm['is_consteval'],
                is_constexpr=mm['is_constexpr'],
                is_static=mm['is_static'],
                is_operator=mm['is_operator'],
                is_coroutine=mm['is_coroutine'],
                parameters=params,
                template_parameters=tmpls,
                source_location=msl
            ))
        tps = [parse_template_parameter(tp) for tp in el.get('template_parameters', [])]
        bases = [JsonBase(**b) for b in el.get('bases', [])]
        elements.append(JsonElement(
            id=el['id'],
            name=el['name'],
            display_name=el['display_name'],
            namespace=el.get('namespace', ''),
            type=el['type'],
            is_abstract=el.get('is_abstract', False),
            is_struct=el.get('is_struct', False),
            is_union=el.get('is_union', False),
            is_template=el.get('is_template', False),
            is_nested=el.get('is_nested', False),
            constants=el.get('constants', []),
            members=members,
            methods=methods,
            template_parameters=tps,
            bases=bases,
            source_location=sl
        ))
    relationships = [JsonRelationship(
        **{k: v for k, v in r.items() if k != 'source_location'},
        source_location=parse_source_location(r['source_location']) if r.get('source_location') else None
    ) for r in data.get('relationships', [])]
    return JsonDiagram(
        name=data['name'],
        diagram_type=data['diagram_type'],
        package_type=data.get('package_type', ''),
        metadata=metadata,
        elements=elements,
        relationships=relationships
    )

# --- TikZ Models ---
class TikzMember:
    def __init__(self, name: str, type: str, access: str, is_static: bool, default: Optional[str]):
        self.name = name
        self.type = type
        self.access = access
        self.is_static = is_static
        self.default = default

    @classmethod
    def from_json(cls, jm: JsonMember) -> 'TikzMember':
        return cls(jm.name, jm.type, jm.access, jm.is_static, jm.default_value)

    def render(self) -> str:
        sym = {'public': '+', 'protected': '#', 'private': '-', 'package': '~'}.get(self.access, '')
        text = f"{sym} {self.name}: {self.type}" + (f" = {self.default}" if self.default else '')
        return f"\\umlstatic{{{text}}}" if self.is_static else text

class TikzMethod:
    def __init__(
        self, name: str, access: str, is_const: bool, is_noexcept: bool,
        is_constexpr: bool, is_consteval: bool, is_deleted: bool,
        is_static: bool, is_virtual: bool,
        parameters: List[Tuple[str, str]], template_names: List[str]
    ):
        self.name = name
        self.access = access
        self.is_const = is_const
        self.is_noexcept = is_noexcept
        self.is_constexpr = is_constexpr
        self.is_consteval = is_consteval
        self.is_deleted = is_deleted
        self.is_static = is_static
        self.is_virtual = is_virtual
        self.parameters = parameters
        self.template_names = template_names

    @classmethod
    def from_json(cls, jm: JsonMethod) -> 'TikzMethod':
        params = [(p.name, p.type) for p in jm.parameters]
        tnames = [tp.name or tp.type for tp in jm.template_parameters]
        return cls(
            jm.name,
            jm.access,
            jm.is_const,
            jm.is_noexcept,
            jm.is_constexpr,
            jm.is_consteval,
            jm.is_deleted,
            jm.is_static,
            jm.is_virtual or jm.is_pure_virtual,
            params,
            tnames
        )

    def render(self) -> str:
        parts = []
        if self.template_names:
            parts.append(f"template<{', '.join(self.template_names)}> ")
        sym = {'public': '+', 'protected': '#', 'private': '-', 'package': '~'}.get(self.access, '')
        params = ', '.join(f"{n}: {t}" for n, t in self.parameters)
        parts.append(f"{sym} {self.name}({params})")
        if self.is_const:
            parts.append(' const')
        if self.is_noexcept:
            parts.append(' noexcept')
        if self.is_constexpr:
            parts.insert(0, 'constexpr ')
        if self.is_consteval:
            parts.insert(0, 'consteval ')
        if self.is_deleted:
            parts.append(' = delete')
        text = ''.join(parts)
        if self.is_static:
            text = f"\\umlstatic{{{text}}}"
        if self.is_virtual:
            text = f"\\umlvirt{{{text}}}"
        return text

class TikzElement:
    def __init__(
        self,
        display_name: str,
        type: str,
        is_abstract: bool,
        template_names: List[str],
        members: List[TikzMember],
        methods: List[TikzMethod],
        x: float = 0,
        y: float = 0
    ):
        self.display_name = display_name
        self.type = type
        self.is_abstract = is_abstract
        self.template_names = template_names
        self.members = members
        self.methods = methods
        self.x = x
        self.y = y

    def render(self) -> List[str]:
        opts = [f"x={self.x}", f"y={self.y}"]
        if self.is_abstract:
            opts.append('type=abstract')
        if self.template_names:
            opts.append(f"template={{ {', '.join(self.template_names)} }}")
        cmd = 'umlclass'
        if self.type == 'enum':
            cmd = 'umlenum'
        elif self.type == 'interface':
            cmd = 'umlinterface'
        elif self.type == 'class' and self.is_abstract:
            cmd = 'umlabstract'
        opt_str = '[' + ','.join(opts) + ']'
        lines: List[str] = [f"\\{cmd}{opt_str}{{{self.display_name}}}"]
        lines.append('{')
        for i, m in enumerate(self.members):
            suffix = ' \\\\' if i < len(self.members) - 1 else ''
            lines.append('  ' + m.render() + suffix)
        lines.append('}')
        lines.append('{')
        for i, m in enumerate(self.methods):
            suffix = ' \\\\' if i < len(self.methods) - 1 else ''
            lines.append('  ' + m.render() + suffix)
        lines.append('}')
        return lines

class TikzRelationship:
    def __init__(
        self,
        cmd: str,
        a: str,
        b: str,
        label: Optional[str],
        m1: Optional[str],
        m2: Optional[str],
        name: str
    ):
        self.cmd = cmd
        self.a = a
        self.b = b
        self.label = label
        self.m1 = m1
        self.m2 = m2
        self.name = name

    @classmethod
    def from_json(cls, jr: JsonRelationship, nm: Dict[str, str]) -> 'TikzRelationship':
        cmd_map = {
            'extension': 'umlinherit',
            'dependency': 'umldep',
            'aggregation': 'umlaggreg',
            'association': 'umluniassoc',
            'composition': 'umlcompo',
            'constraint': 'umlconstraint',
            'instantiation': 'umlnest'
        }
        cmd = cmd_map.get(jr.type, 'umluniassoc')
        a = nm.get(jr.source, jr.source)
        b = nm.get(jr.destination, jr.destination)
        name = f"{cmd}{a}{b}"
        return cls(cmd, a, b, jr.label, jr.multiplicity_source, jr.multiplicity_destination, name)

    def key(self) -> Tuple[str, str, str]:
        return (self.cmd, self.a, self.b)

    def render(self) -> List[str]:
        args: List[str] = []
        args.append(f"name={self.name}")
        if self.m1:
            args.extend([f"mult1={self.m1}", "pos1=0.1"])
        if self.m2:
            args.extend([f"mult2={self.m2}", "pos2=0.9"])
        arg_str = '[' + ','.join(args) + ']' if args else ''
        lines = [f"\\{self.cmd}{arg_str}{{{self.a}}}{{{self.b}}}"]
        if self.label:
            lines.append(f"\\node[above] at ({self.name}-1) {{{self.label}}};")
        return lines

class TikzDiagram:
    def __init__(self, elements: List[TikzElement], relationships: List[TikzRelationship]):
        self.elements = elements
        self.relationships = relationships

    def group_relations(self) -> None:
        grouped = defaultdict(lambda: {'rels': [], 'labels': []})
        for r in self.relationships:
            key = r.key()
            grouped[key]['rels'].append(r)
            if r.label and r.label not in grouped[key]['labels']:
                grouped[key]['labels'].append(r.label)
        merged: List[TikzRelationship] = []
        for (cmd, a, b), info in grouped.items():
            base = info['rels'][0]
            combined_label = ', '.join(info['labels'])
            name = f"{cmd}{a}{b}"
            merged.append(TikzRelationship(cmd, a, b, combined_label, base.m1, base.m2, name))
        self.relationships = merged

    def render(self) -> List[str]:
        lines: List[str] = ['\\begin{tikzpicture}']
        for el in self.elements:
            lines.extend(el.render())
            lines.append('')
        self.group_relations()
        for r in self.relationships:
            lines.extend(r.render())
            lines.append('')
        lines.append('\\end{tikzpicture}')
        return lines

# --- Conversion ---
def convert_json_to_tikz(jd: JsonDiagram) -> TikzDiagram:
    elems: List[TikzElement] = []
    for el in jd.elements:
        members = [TikzMember.from_json(m) for m in el.members]
        methods = [TikzMethod.from_json(m) for m in el.methods]
        tnames = [tp.name or tp.type for tp in el.template_parameters]
        elems.append(TikzElement(el.display_name, el.type, el.is_abstract, tnames, members, methods))
    name_map = {el.id: el.display_name for el in jd.elements}
    rels = [TikzRelationship.from_json(r, name_map) for r in jd.relationships]
    return TikzDiagram(elems, rels)

# --- Main CLI ---
if __name__ == '__main__':
    if len(sys.argv) not in (2, 3):
        print(f"Usage: {os.path.basename(sys.argv[0])} <input.json> [<output.txt>]", file=sys.stderr)
        sys.exit(1)
    inp = sys.argv[1]
    out = sys.argv[2] if len(sys.argv) == 3 else os.path.splitext(inp)[0] + '_tikz.txt'
    jd = load_diagram(inp)
    td = convert_json_to_tikz(jd)
    lines = td.render()
    with open(out, 'w', encoding='utf-8') as f:
        f.write("\n".join(lines))
    print(f"Wrote {out}")