from dataclasses import dataclass, field
from typing import List, Optional, Dict
import json

# --- Data Models ---
@dataclass
class SourceLocation:
    file: str
    line: int
    column: int
    translation_unit: str

@dataclass
class Metadata:
    clang_uml_version: str
    llvm_version: str
    schema_version: int

@dataclass
class Parameter:
    name: str
    type: str
    default_value: Optional[str] = None
    source_location: Optional[SourceLocation] = None

@dataclass
class TemplateParameter:
    kind: str
    name: Optional[str] = None
    type: Optional[str] = None
    default: Optional[str] = None
    is_variadic: bool = False
    template_parameters: List["TemplateParameter"] = field(default_factory=list)

@dataclass
class Member:
    name: str
    type: str
    access: str
    is_static: bool
    default_value: Optional[str] = None
    source_location: Optional[SourceLocation] = None

@dataclass
class Method:
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
    parameters: List[Parameter] = field(default_factory=list)
    template_parameters: List[TemplateParameter] = field(default_factory=list)
    source_location: Optional[SourceLocation] = None

@dataclass
class Base:
    id: str
    access: str
    is_virtual: bool

@dataclass
class Element:
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
    members: List[Member] = field(default_factory=list)
    methods: List[Method] = field(default_factory=list)
    template_parameters: List[TemplateParameter] = field(default_factory=list)
    bases: List[Base] = field(default_factory=list)
    source_location: Optional[SourceLocation] = None

@dataclass
class Relationship:
    source: str
    destination: str
    type: str
    access: Optional[str] = None
    label: Optional[str] = None
    multiplicity_source: Optional[str] = None
    multiplicity_destination: Optional[str] = None
    source_location: Optional[SourceLocation] = None

@dataclass
class Diagramm:
    name: str
    diagram_type: str
    package_type: str
    metadata: Metadata
    elements: List[Element] = field(default_factory=list)
    relationships: List[Relationship] = field(default_factory=list)

# --- Utilities ---
def escape_latex(s: str) -> str:
    return (s.replace('&', r'\\&')
             .replace('{', r'\\{')
             .replace('}', r'\\}')
             .replace('#', r'\\#')
             .replace('~', r'\\raisebox{0.5ex}{\\texttildelow}'))

# Relationship type to TikZ-UML mapping
REL_TIKZ_MAP: Dict[str, str] = {
    'extension': 'umlinherit',
    'dependency': 'umldep',
    'aggregation': 'umlaggreg',
    'association': 'umluniassoc',
    'composition': 'umlcompo',
    'constraint': 'umlconstraint',
    'instantiation': 'umlnest',
}

# Access visibility symbols including package (~)
ACCESS_SYM: Dict[str, str] = {
    'public': '+',
    'protected': '#',
    'private': '-',
    'package': '~'
}

# --- JSON Parsers ---

def parse_source_location(data: dict) -> SourceLocation:
    return SourceLocation(
        file=data['file'],
        line=data['line'],
        column=data['column'],
        translation_unit=data['translation_unit']
    )


def parse_template_parameter(data: dict) -> TemplateParameter:
    nested = [parse_template_parameter(tp) for tp in data.get('template_parameters', [])]
    return TemplateParameter(
        kind=data['kind'],
        name=data.get('name'),
        type=data.get('type'),
        default=data.get('default'),
        is_variadic=data.get('is_variadic', False),
        template_parameters=nested
    )


def load_diagram(file_path: str) -> Diagramm:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    meta = Metadata(
        clang_uml_version=data['metadata']['clang_uml_version'],
        llvm_version=data['metadata']['llvm_version'],
        schema_version=data['metadata']['schema_version']
    )
    elements: List[Element] = []
    for el in data.get('elements', []):
        sl = parse_source_location(el['source_location']) if el.get('source_location') else None
        members = [
            Member(
                name=m['name'], type=m['type'], access=m['access'],
                is_static=m['is_static'], default_value=m.get('default_value'),
                source_location=parse_source_location(m['source_location']) if m.get('source_location') else None
            ) for m in el.get('members', [])
        ]
        methods_raw = el.get('methods', [])
        # find non-default constructors/destructors
        non_def_ctors = [m for m in methods_raw if m.get('is_constructor') and not m.get('is_defaulted')]
        non_def_dtors = [m for m in methods_raw if m.get('name', '').startswith('~') and not m.get('is_defaulted')]
        methods: List[Method] = []
        for mm in methods_raw:
            # skip unneeded defaulted ctor/dtor if not overloaded
            if mm.get('is_defaulted'):
                if mm.get('is_constructor') and not non_def_ctors:
                    continue
                if mm.get('name', '').startswith('~') and not non_def_dtors:
                    continue
            mm_sl = parse_source_location(mm['source_location']) if mm.get('source_location') else None
            params = [
                Parameter(
                    name=p['name'], type=p['type'], default_value=p.get('default_value'),
                    source_location=parse_source_location(p['source_location']) if p.get('source_location') else None
                ) for p in mm.get('parameters', [])
            ]
            tmpls = [parse_template_parameter(tp) for tp in mm.get('template_parameters', [])]
            methods.append(Method(
                name=mm['name'], display_name=mm['display_name'], type=mm['type'],
                access=mm['access'], is_constructor=mm['is_constructor'],
                is_defaulted=mm['is_defaulted'], is_deleted=mm['is_deleted'],
                is_copy_assignment=mm['is_copy_assignment'], is_move_assignment=mm['is_move_assignment'],
                is_virtual=mm['is_virtual'], is_pure_virtual=mm['is_pure_virtual'],
                is_noexcept=mm['is_noexcept'], is_const=mm['is_const'],
                is_consteval=mm['is_consteval'], is_constexpr=mm['is_constexpr'],
                is_static=mm['is_static'], is_operator=mm['is_operator'],
                is_coroutine=mm['is_coroutine'], parameters=params,
                template_parameters=tmpls, source_location=mm_sl
            ))
        class_tmps = [parse_template_parameter(tp) for tp in el.get('template_parameters', [])]
        bases = [Base(id=b['id'], access=b['access'], is_virtual=b['is_virtual']) for b in el.get('bases', [])]
        elements.append(Element(
            id=el['id'], name=el['name'], display_name=el['display_name'],
            namespace=el.get('namespace',''), type=el['type'],
            is_abstract=el.get('is_abstract',False), is_struct=el.get('is_struct',False),
            is_union=el.get('is_union',False), is_template=el.get('is_template',False),
            is_nested=el.get('is_nested',False), constants=el.get('constants',[]),
            members=members, methods=methods, template_parameters=class_tmps,
            bases=bases, source_location=sl
        ))
    rels: List[Relationship] = []
    for r in data.get('relationships', []):
        r_sl = parse_source_location(r['source_location']) if r.get('source_location') else None
        rels.append(Relationship(
            source=r['source'], destination=r['destination'], type=r['type'],
            access=r.get('access'), label=r.get('label'),
            multiplicity_source=r.get('multiplicity_source'),
            multiplicity_destination=r.get('multiplicity_destination'),
            source_location=r_sl
        ))
    return Diagramm(
        name=data['name'], diagram_type=data['diagram_type'],
        package_type=data.get('package_type',''), metadata=meta,
        elements=elements, relationships=rels
    )

# --- TikZ-UML Generation Helpers ---

def map_relation_type(rel_type: str) -> str:
    return REL_TIKZ_MAP.get(rel_type, 'umlassoc')

def member_to_tikz(m: Member) -> str:
    sym = ACCESS_SYM.get(m.access, '')
    text = f"{sym} {m.name}: {m.type}"
    if m.default_value is not None:
        text += f" = {m.default_value}"
    return f"\\umlstatic{{{escape_latex(text)}}}" if m.is_static else escape_latex(text)

def method_to_tikz(m: Method) -> str:
    tmpl_prefix = ''
    if m.template_parameters:
        names = [tp.name or tp.type for tp in m.template_parameters]
        tmpl_prefix = f"template<{', '.join(names)}> "
    sym = ACCESS_SYM.get(m.access, '')
    params = ", ".join(f"{p.name}: {p.type}" for p in m.parameters)
    sig = f"{m.name}({params})"
    if m.type:
        sig += f": {m.type}"
    text = f"{tmpl_prefix}{sym} {sig}"
    if m.is_const:
        text += ' const'
    if m.is_noexcept:
        text += ' noexcept'
    if m.is_constexpr:
        text = 'constexpr ' + text
    if m.is_consteval:
        text = 'consteval ' + text
    if m.is_deleted:
        text += ' = delete'
    if m.is_static:
        text = f"\\umlstatic{{{escape_latex(text)}}}"
    if m.is_virtual or m.is_pure_virtual:
        text = f"\\umlvirt{{{escape_latex(text)}}}"
    return text

def render_element(el: Element) -> List[str]:
    lines: List[str] = []
    opts = ['x=0', 'y=0']
    if el.is_abstract:
        opts.append('type=abstract')
    if el.template_parameters:
        names = [tp.name or tp.type for tp in el.template_parameters]
        opts.append(f"template={{ {', '.join(names)} }}")
    opt_str = '[' + ', '.join(opts) + ']'
    if el.type == 'enum':
        lines.append(f"\\umlenum{opt_str}{{{el.display_name}}}")
        lines.append('{')
        for v in el.constants:
            lines.append(f"  {escape_latex(v)} \\")
        lines.append('}')
        lines.append('{}')
        return lines
    lines.append(f"\\umlclass{opt_str}{{{el.display_name}}}")
    lines.append('{')
    for m in el.members:
        lines.append('  ' + member_to_tikz(m))
    lines.append('}')
    lines.append('{')
    for m in el.methods:
        lines.append('  ' + method_to_tikz(m))
    lines.append('}')
    return lines

def render_relationships(diag: Diagramm) -> List[str]:
    lines: List[str] = []
    name_map = {e.id: e.display_name for e in diag.elements}
    for r in diag.relationships:
        cmd = map_relation_type(r.type)
        args = ['pos1=0.5']
        if r.label:
            args.insert(0, f"arg1={escape_latex(r.label)}")
        if r.multiplicity_source:
            args.insert(0, f"mult1={r.multiplicity_source}")
        if r.multiplicity_destination:
            args.append(f"mult2={r.multiplicity_destination}")
        arg_str = '[' + ', '.join(args) + ']'
        a = escape_latex(name_map.get(r.source, r.source))
        b = escape_latex(name_map.get(r.destination, r.destination))
        lines.append(f"\\{cmd}{arg_str}{{{a}}}{{{b}}}")
    return lines
