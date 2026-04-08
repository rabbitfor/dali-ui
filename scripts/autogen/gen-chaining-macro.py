import re
import sys
import os
import glob

# C++ Fluent API chaining code generator
#
# Tags:
#   @CHAIN_CLASS(ClassName[, ParentClassName])  — register class + parent relationship
#   @CHAIN                                      — collect the next method declaration
#   @CHAIN_SELF                                 — collect next method with body, replace ClassName→ChildClass
#
# 2-pass processing:
#   Pass 1: Scan all headers, collect chain methods per class, merge parent methods
#   Pass 2: For each class with a parent, generate:
#     - classname.autogen.h       — real inline functions (internal use, #include inside class body)
#     - classname.macro.autogen.h — macro for external derived classes (#include in original header)

DOC_COLLECT_LIMIT = 120

# ---------------------------------------------------------------------------
# Utility helpers (kept from original)
# ---------------------------------------------------------------------------

def _find_function_end(lines, start_idx):
    """From start_idx, find the closing } by brace-balancing."""
    depth = 0
    i = start_idx
    while i < len(lines):
        for c in lines[i]:
            if c == '{':
                depth += 1
            elif c == '}':
                depth -= 1
                if depth == 0:
                    return i
        i += 1
    return start_idx


def _extract_body_lines(lines, body_start, end_idx):
    """Extract and normalize body lines between { and }."""
    body_lines = []
    if body_start == end_idx:
        line = lines[body_start]
        open_idx = line.index('{')
        close_idx = line.rindex('}')
        inner = line[open_idx + 1:close_idx].strip()
        if inner:
            body_lines = [inner]
    else:
        first_content = lines[body_start][lines[body_start].index('{') + 1:].strip()
        if first_content:
            body_lines.append(first_content)
        for li in range(body_start + 1, end_idx):
            body_lines.append(lines[li].rstrip())
        last_content = lines[end_idx][:lines[end_idx].rindex('}')].strip()
        if last_content:
            body_lines.append(last_content)
    non_empty = [l for l in body_lines if l.strip()]
    if non_empty:
        min_indent = min(len(l) - len(l.lstrip()) for l in non_empty)
        body_lines = [l[min_indent:] if l.strip() else '' for l in body_lines]
    return body_lines


def extract_arg_names(args_raw):
    """Extract parameter names from a C++ argument list string."""
    names, curr, angle, paren = [], "", 0, 0
    parts = []
    for c in args_raw:
        if c == '<': angle += 1
        elif c == '>': angle -= 1
        elif c == '(': paren += 1
        elif c == ')': paren -= 1
        if c == ',' and angle == 0 and paren == 0:
            parts.append(curr.strip())
            curr = ""
        else:
            curr += c
    parts.append(curr.strip())
    for p in parts:
        if p:
            clean = p.split('=')[0].strip()
            m = re.search(r'\*(\w+)\s*\)\s*\(', clean)
            if m:
                names.append(m.group(1))
            else:
                m = re.search(r'([\w]+)$', clean)
                if m:
                    names.append(m.group(1))
    return names


def _find_class_end(content, class_name):
    """Find the real end of a class body by brace matching."""
    m = re.search(r"class\s+.*?\b" + re.escape(class_name) + r"\b", content)
    if not m:
        return None
    brace_start = content.find('{', m.end())
    if brace_start == -1:
        return None
    depth = 0
    i = brace_start
    while i < len(content):
        c = content[i]
        if c == '{':
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                semi = content.find(';', i)
                return (i, semi if semi != -1 else i)
        i += 1
    return None


# ---------------------------------------------------------------------------
# Pass 1: Collect chain methods from a header file
# ---------------------------------------------------------------------------

def _collect_doc(lines, before_idx):
    """Collect doxygen comment block above a line."""
    doc = []
    cursor = before_idx
    while cursor >= 0 and (before_idx - cursor) <= DOC_COLLECT_LIMIT:
        c_line = lines[cursor].strip()
        if c_line.startswith(('/', '*')):
            if '@CHAIN' not in c_line:
                doc.insert(0, c_line)
            if '/**' in c_line:
                break
            cursor -= 1
        elif not c_line or re.match(r'^\s*template\s*<', c_line):
            cursor -= 1
        else:
            break
    return doc


def _parse_args(line, args_start_pos):
    """Parse balanced parentheses to extract argument string."""
    depth, args_str = 1, ""
    rmd = line[args_start_pos:]
    for char in rmd:
        if char == '(':
            depth += 1
        elif char == ')':
            depth -= 1
        if depth == 0:
            break
        args_str += char
    return args_str.strip() if depth == 0 else None


def _parse_declaration(lines, k):
    """Parse a function declaration at line k. Returns dict or None.
    If line k is a template<...> line, the actual declaration is on line k+1."""
    line = lines[k]
    stripped = line.strip()
    if stripped.startswith(('/', '*')) or not stripped:
        return None

    # If current line is template<...>, the declaration is on the next line
    template_line = None
    decl_k = k
    if re.match(r'\s*template\s*<', line):
        template_line = stripped
        decl_k = k + 1
        if decl_k >= len(lines):
            return None
        line = lines[decl_k]
    else:
        # Check for template on previous non-comment line
        prev = k - 1
        while prev >= 0 and (lines[prev].strip().startswith(('/', '*')) or not lines[prev].strip()):
            prev -= 1
        if prev >= 0 and re.match(r'\s*template\s*<', lines[prev]):
            template_line = lines[prev].strip()

    m = re.search(r'(?P<ret>[\w&:*<>\s]+)\s+(?P<name>\w+)\s*\(', line)
    if not m:
        return None

    args_str = _parse_args(line, m.end())
    if args_str is None:
        return None

    return {
        'name': m.group('name'),
        'args': args_str,
        'template': template_line,
        'decl_idx': decl_k,
    }


def scan_header(file_path, registry):
    """
    Scan a header file for @CHAIN_CLASS, @CHAIN, @CHAIN_SELF tags.
    Populates registry[class_name] with collected methods.
    Returns (class_name, parent_name) or None if no @CHAIN_CLASS found.
    """
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    class_name = None
    parent_name = ""
    methods = []

    i = 0
    while i < len(lines):
        line = lines[i]

        # @CHAIN_CLASS(ClassName[, Parent])
        m = re.search(r'//\s*@CHAIN_CLASS\((\w+)(?:\s*,\s*(\w+))?\)', line)
        if m:
            class_name = m.group(1)
            parent_name = m.group(2) or ""
            i += 1
            continue

        # @CHAIN_SELF — collect method with body, mark for ClassName→ChildClass substitution
        if re.search(r'//\s*@CHAIN_SELF\b', line):
            # Skip comments and blank lines to next declaration (template or function)
            k = i + 1
            while k < len(lines):
                stripped = lines[k].strip()
                if stripped and not stripped.startswith(('/', '*')) and not stripped.startswith('@'):
                    break
                k += 1
            if k < len(lines):
                decl = _parse_declaration(lines, k)
                if decl:
                    doc = _collect_doc(lines, decl['decl_idx'] - 1)
                    # Find and extract body
                    body_start = decl['decl_idx']
                    while body_start < len(lines) and '{' not in lines[body_start]:
                        body_start += 1
                    body_lines = []
                    end_idx = decl['decl_idx']
                    if body_start < len(lines):
                        end_idx = _find_function_end(lines, body_start)
                        body_lines = _extract_body_lines(lines, body_start, end_idx)
                    methods.append({
                        'name': decl['name'],
                        'args': decl['args'],
                        'doc': doc,
                        'template': decl['template'],
                        'body': body_lines,
                        'is_self': True,
                    })
                    i = end_idx + 1
                    continue
            i += 1
            continue

        # @CHAIN — collect next method declaration
        if re.search(r'//\s*@CHAIN\b', line) and not re.search(r'@CHAIN_', line):
            # Skip comments and blank lines to next declaration (template or function)
            k = i + 1
            while k < len(lines):
                stripped = lines[k].strip()
                if stripped and not stripped.startswith(('/', '*')) and not stripped.startswith('@'):
                    break
                k += 1
            if k < len(lines):
                decl = _parse_declaration(lines, k)
                if decl:
                    doc = _collect_doc(lines, decl['decl_idx'] - 1)
                    method = {
                        'name': decl['name'],
                        'args': decl['args'],
                        'doc': doc,
                        'template': decl['template'],
                    }
                    # If method has inline body, skip past it
                    body_start = decl['decl_idx']
                    while body_start < len(lines) and '{' not in lines[body_start]:
                        # Check if we hit a semicolon first (declaration only)
                        if ';' in lines[body_start]:
                            break
                        body_start += 1
                    if body_start < len(lines) and '{' in lines[body_start]:
                        end_idx = _find_function_end(lines, body_start)
                        i = end_idx + 1
                    else:
                        i = decl['decl_idx'] + 1
                    methods.append(method)
                    continue
            i += 1
            continue

        i += 1

    if class_name is None:
        return None

    registry[class_name] = {
        'methods': methods,
        'parent': parent_name,
        'file_path': file_path,
        'all_methods': None,  # built in pass 1.5
    }
    return (class_name, parent_name)


# ---------------------------------------------------------------------------
# Pass 1.5: Build all_methods by merging parent chains
# ---------------------------------------------------------------------------

def build_all_methods(class_name, registry):
    """Recursively build all_methods for a class."""
    entry = registry.get(class_name)
    if entry is None:
        return []
    if entry['all_methods'] is not None:
        return entry['all_methods']

    parent = entry['parent']
    if parent and parent in registry:
        parent_methods = build_all_methods(parent, registry)
        # Tag each parent method with its source class
        tagged_parent = []
        for m in parent_methods:
            if 'source_class' not in m:
                tagged = dict(m, source_class=class_name)
                tagged_parent.append(tagged)
            else:
                tagged_parent.append(m)
        entry['all_methods'] = tagged_parent + [dict(m, source_class=class_name) for m in entry['methods']]
    else:
        entry['all_methods'] = [dict(m, source_class=class_name) for m in entry['methods']]

    return entry['all_methods']


# ---------------------------------------------------------------------------
# Pass 2: Generate autogen.h files
# ---------------------------------------------------------------------------

def generate_autogen(class_name, registry):
    """Generate classname.autogen.h with real inline functions."""
    entry = registry[class_name]
    parent = entry['parent']
    if not parent:
        return  # base class — no autogen needed

    file_path = entry['file_path']
    base_dir = os.path.dirname(file_path)
    base_name = os.path.splitext(os.path.basename(file_path))[0]
    autogen_path = os.path.join(base_dir, base_name + ".autogen.h")

    # Collect parent's all_methods — these are what we override
    parent_entry = registry.get(parent)
    if not parent_entry:
        sys.stderr.write(f"Warning: parent '{parent}' not found in registry for '{class_name}'\n")
        return

    parent_all = parent_entry.get('all_methods', [])

    output = ["// Generated by dali-ui preprocessor. Do not modify.", "#pragma once", ""]

    # Group methods by source class for readability
    current_source = None
    for m in parent_all:
        source = m.get('source_class', parent)
        if source != current_source:
            if current_source is not None:
                output.append("")
            output.append(f"// --- Chaining methods from {source} ---")
            current_source = source

        _write_method(output, m, class_name, source)

    output.append("")

    with open(autogen_path, 'w', encoding='utf-8') as f:
        f.write("\n".join(output))


def _write_method(output, m, child_class, source_class):
    """Write a single chaining method as real inline function code."""
    # Doc comments
    for d in m.get('doc', []):
        output.append(d)

    if m.get('is_self'):
        # Body-copy method (e.g. With) — replace source class name with child class
        base_class = source_class
        retyped_args = re.sub(rf'\b{re.escape(base_class)}\b', child_class, m['args'])
        if m.get('template'):
            output.append(m['template'])
        output.append(f"{child_class}& {m['name']}({retyped_args})")
        output.append("{")
        for bl in m.get('body', []):
            replaced = re.sub(rf'\b{re.escape(base_class)}\b', child_class, bl)
            output.append(f"  {replaced}" if replaced.strip() else "")
        output.append("}")
    else:
        # Normal delegation method
        arg_names = extract_arg_names(m['args'])
        if m.get('template'):
            output.append(m['template'])
        call = f"{source_class}::{m['name']}({', '.join(arg_names)})"
        output.append(f"{child_class}& {m['name']}({m['args']}) {{ {call}; return *this; }}")


# ---------------------------------------------------------------------------
# Pass 2b: Generate macro.autogen.h files (for external derived classes)
# ---------------------------------------------------------------------------

def generate_macro(class_name, registry):
    """Generate classname.macro.autogen.h with a macro wrapping own methods + parent macro call."""
    entry = registry[class_name]
    methods = entry.get('methods', [])
    parent = entry.get('parent', '')

    if not methods and not parent:
        return

    file_path = entry['file_path']
    base_dir = os.path.dirname(file_path)
    base_name = os.path.splitext(os.path.basename(file_path))[0]
    macro_path = os.path.join(base_dir, base_name + ".macro.autogen.h")

    cls_upper = class_name.upper()
    macro_name = f"DALI_UI_CHAIN_{cls_upper}_METHODS"

    header = [f"// Generated by dali-ui preprocessor. Do not modify.",
              f"#ifndef {macro_name}",
              f"",
              f"#define {macro_name}(ChildClass) \\"]
    items = []

    # Include parent macro call
    if parent:
        parent_upper = parent.upper()
        items.append(f"  DALI_UI_CHAIN_{parent_upper}_METHODS(ChildClass)")

    # Own methods only
    for m in methods:
        lines = []
        for d in m.get('doc', []):
            lines.append("  " + d)
        if m.get('is_self'):
            base_class = class_name
            retyped_args = re.sub(rf'\b{re.escape(base_class)}\b', 'ChildClass', m['args'])
            if m.get('template'):
                lines.append("  " + m['template'])
            lines.append(f"  ChildClass& {m['name']}({retyped_args})")
            lines.append("  {")
            for bl in m.get('body', []):
                replaced = re.sub(rf'\b{re.escape(base_class)}\b', 'ChildClass', bl)
                lines.append("  " + replaced if replaced.strip() else "")
            lines.append("  }")
        else:
            arg_names = extract_arg_names(m['args'])
            if m.get('template'):
                lines.append("  " + m['template'])
            call = f"{class_name}::{m['name']}({', '.join(arg_names)})"
            lines.append(f"  ChildClass& {m['name']}({m['args']}) {{ {call}; return *this; }}")
        items.append("\n".join(lines))

    output = _build_macro(header, items)
    output.append("")
    output.append(f"#endif // {macro_name}")
    output.append("")

    with open(macro_path, 'w', encoding='utf-8') as f:
        f.write("\n".join(output))


def _build_macro(header, items):
    """Format macro with line continuations."""
    res = list(header)
    for i, item in enumerate(items):
        sub = item.split('\n')
        for j, ln in enumerate(sub):
            is_last = (i == len(items) - 1 and j == len(sub) - 1)
            res.append(ln + ("" if is_last else " \\"))
    return res


# ---------------------------------------------------------------------------
# Header update: insert #include for autogen
# ---------------------------------------------------------------------------

def update_header(file_path, class_name, registry):
    """Update the .h file: insert #include for autogen.h (inside class) and macro.autogen.h (before class)."""
    entry = registry[class_name]
    if not entry['parent']:
        _cleanup_old_artifacts(file_path, class_name)
        return

    base_name = os.path.splitext(os.path.basename(file_path))[0]
    autogen_include = f'#include "{base_name}.autogen.h"'
    macro_include = f'#include "{base_name}.macro.autogen.h"'

    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    changed = False

    # Remove old macro calls: public:\n  DALI_UI_CHAIN_*_METHODS(...)
    new_content = re.sub(r'\n\s*public:\n\s*DALI_UI_CHAIN_\w+_METHODS\([^)]*\)', '', content)
    if new_content != content:
        content = new_content
        changed = True

    # Insert autogen.h inside class body, just before closing };
    if autogen_include not in content:
        c_end = _find_class_end(content, class_name)
        if c_end:
            idx, _ = c_end
            content = content[:idx] + f"\npublic:\n{autogen_include}\n" + content[idx:]
            changed = True

    # Insert macro.autogen.h before the class definition (so it's shipped with the header)
    if macro_include not in content:
        c_def = re.search(r"class\s+.*?\b" + re.escape(class_name) + r"\b", content)
        if c_def:
            ins = c_def.start()
            # Back up past any doc comment block before the class
            search = content[:ins]
            doc = search.rstrip().rfind('/**')
            if doc != -1 and '*/' in search[doc:]:
                ins = doc
            content = content[:ins] + f'{macro_include}\n' + content[ins:]
            changed = True

    if changed:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content.strip() + "\n")


def _update_base_header_macro(file_path, class_name):
    """For base classes: insert macro.autogen.h include before class, cleanup old artifacts."""
    base_name = os.path.splitext(os.path.basename(file_path))[0]
    macro_include = f'#include "{base_name}.macro.autogen.h"'

    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    original = content

    # Remove old macro calls
    content = re.sub(r'\n\s*public:\n\s*DALI_UI_CHAIN_\w+_METHODS\([^)]*\)', '', content)

    # Remove old autogen include (non-macro)
    content = re.sub(rf'#include "{re.escape(base_name)}.autogen.h"\n?', '', content)

    # Insert macro.autogen.h before class definition
    if macro_include not in content:
        c_def = re.search(r"class\s+.*?\b" + re.escape(class_name) + r"\b", content)
        if c_def:
            ins = c_def.start()
            search = content[:ins]
            doc = search.rstrip().rfind('/**')
            if doc != -1 and '*/' in search[doc:]:
                ins = doc
            content = content[:ins] + f'{macro_include}\n' + content[ins:]

    if content != original:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content.strip() + "\n")


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def collect_header_files(paths):
    """Collect all .h files from given paths (files or directories)."""
    headers = []
    for p in paths:
        if os.path.isdir(p):
            for root, dirs, files in os.walk(p):
                for f in sorted(files):
                    if f.endswith('.h') and not f.endswith('.autogen.h'):
                        headers.append(os.path.join(root, f))
        elif os.path.isfile(p) and p.endswith('.h'):
            headers.append(p)
    return headers


def resolve_parent(class_name, registry, all_headers):
    """If parent is not yet in registry, scan headers to find it."""
    entry = registry.get(class_name)
    if not entry or not entry['parent']:
        return
    parent = entry['parent']
    if parent in registry:
        return
    # Search all headers for the parent
    for h in all_headers:
        result = scan_header(h, registry)
        if result and result[0] == parent:
            resolve_parent(parent, registry, all_headers)
            return


def main():
    if len(sys.argv) < 2:
        print("Usage: gen-chaining-macro.py <file_or_dir> [file_or_dir ...]")
        sys.exit(1)

    all_headers = collect_header_files(sys.argv[1:])

    # Pass 1: Scan all headers
    registry = {}
    found_classes = []
    for h in all_headers:
        result = scan_header(h, registry)
        if result:
            found_classes.append(result[0])

    # Resolve any unresolved parents
    for cls in list(registry.keys()):
        resolve_parent(cls, registry, all_headers)

    # Pass 1.5: Build all_methods for each class
    for cls in registry:
        build_all_methods(cls, registry)

    # Pass 2: Generate autogen + macro files, update headers
    for cls in found_classes:
        entry = registry[cls]
        # Generate macro for all classes (base and derived)
        generate_macro(cls, registry)
        if entry['parent']:
            generate_autogen(cls, registry)
            update_header(entry['file_path'], cls, registry)
        else:
            # Base class — macro.autogen.h is generated above, insert include before class
            _update_base_header_macro(entry['file_path'], cls)


if __name__ == "__main__":
    main()
