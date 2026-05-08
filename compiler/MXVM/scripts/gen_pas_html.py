#!/usr/bin/env python3
"""
gen_pas_html.py
Converts every .pas file under src/frontend/pas to:
  1. <Name>.html        — syntax-highlighted Pascal source (PasHTMLGen, Borland theme)
  2. <Name>.mxvm.html   — syntax-highlighted MXVM bytecode (HTMLGen, Borland theme)
     (only produced when mxx compiles the file without error)

Then generates docs/examples/index.html with a collapsible tree linking both.

Run from the repository root:
    python3 scripts/gen_pas_html.py
"""

import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

TITLE_RE = re.compile(r'<title>[^<]*</title>', re.IGNORECASE)

REPO     = Path(__file__).resolve().parent.parent
PAS_ROOT = REPO / "src" / "frontend" / "pas"
OUT_ROOT = REPO / "docs" / "examples"
MXX      = "mxx"
HTML     = "mxvm-html"

# ── helpers ──────────────────────────────────────────────────────────────────

def ensure_dir(p: Path):
    p.mkdir(parents=True, exist_ok=True)


def fix_title(html_path: Path, title: str):
    """Replace whatever <title> mxvm-html emitted with a clean one."""
    text = html_path.read_text(encoding="utf-8")
    text = TITLE_RE.sub(f"<title>{title}</title>", text, count=1)
    html_path.write_text(text, encoding="utf-8")


def rel_html_path(pas_path: Path) -> Path:
    """src/frontend/pas/sub/Foo.pas → sub/Foo.html (relative to OUT_ROOT)"""
    return pas_path.relative_to(PAS_ROOT).with_suffix(".html")


def convert_pas(pas_path: Path, out_html: Path) -> bool:
    ensure_dir(out_html.parent)
    r = subprocess.run([HTML, "-i", str(pas_path), "-o", str(out_html)],
                       capture_output=True, text=True)
    if r.returncode != 0:
        print(f"  PAS ERROR {pas_path.name}: {r.stderr.strip()}", file=sys.stderr)
        return False
    fix_title(out_html, f"{pas_path.stem}.pas \u2014 Pascal Source \u2014 MXVM")
    return True


def compile_and_convert_mxvm(pas_path: Path, out_mxvm_html: Path) -> bool:
    """Compile .pas → .mxvm with mxx, then render .mxvm → .mxvm.html."""
    ensure_dir(out_mxvm_html.parent)
    with tempfile.NamedTemporaryFile(suffix=".mxvm", delete=False) as tf:
        tmp_mxvm = Path(tf.name)
    try:
        # compile
        r = subprocess.run([MXX, "-i", str(pas_path), "-o", str(tmp_mxvm)],
                           capture_output=True, text=True)
        if r.returncode != 0:
            print(f"  MXX  SKIP {pas_path.name}: {r.stderr.strip()}", file=sys.stderr)
            return False
        # render
        r2 = subprocess.run([HTML, "-i", str(tmp_mxvm), "-o", str(out_mxvm_html)],
                            capture_output=True, text=True)
        if r2.returncode != 0:
            print(f"  MXVM ERROR {pas_path.name}: {r2.stderr.strip()}", file=sys.stderr)
            return False
        fix_title(out_mxvm_html, f"{pas_path.stem}.mxvm \u2014 MXVM Bytecode \u2014 MXVM")
        return True
    finally:
        tmp_mxvm.unlink(missing_ok=True)


# ── tree builder ──────────────────────────────────────────────────────────────

class DirNode:
    def __init__(self, name: str):
        self.name     = name
        self.children: dict[str, "DirNode"] = {}
        # each entry: (label, rel_pas_html, rel_mxvm_html_or_None)
        self.files:    list[tuple[str, Path, Path | None]] = []

    def ensure_child(self, name: str) -> "DirNode":
        if name not in self.children:
            self.children[name] = DirNode(name)
        return self.children[name]


def build_tree(entries: list[tuple[Path, Path, Path | None]]) -> DirNode:
    root = DirNode("Pascal Examples")
    for pas_path, rel_html, rel_mxvm in entries:
        parts = rel_html.parts
        node  = root
        for part in parts[:-1]:
            node = node.ensure_child(part)
        node.files.append((pas_path.stem, rel_html, rel_mxvm))

    def sort_node(n: DirNode):
        n.files.sort(key=lambda t: t[0].lower())
        for c in n.children.values():
            sort_node(c)
    sort_node(root)
    return root


# ── HTML generation ───────────────────────────────────────────────────────────

DELPHI_CSS = """
body {
    background: #FFFFFF;
    color: #000000;
    margin: 0;
    padding: 32px 48px;
    font: 13px/1.6 "Courier New", Courier, monospace;
    max-width: 960px;
}
h1 {
    font: bold 18px "Courier New", Courier, monospace;
    color: #000080;
    border-bottom: 2px solid #000080;
    padding-bottom: 6px;
    margin-bottom: 18px;
}
.tree { padding: 0; margin: 0; }
details { margin-left: 16px; }
details > summary {
    cursor: pointer;
    font-weight: bold;
    color: #000080;
    list-style: none;
    padding: 2px 0;
    user-select: none;
}
details > summary::before        { content: "▶ "; font-size: 10px; }
details[open] > summary::before  { content: "▼ "; }
details > summary::-webkit-details-marker { display: none; }
ul { list-style: none; padding-left: 20px; margin: 2px 0; }
li { padding: 2px 0; }
a { color: #000080; text-decoration: none; }
a:hover { text-decoration: underline; color: #0000FF; }
.mxvm-link { color: #008000; margin-left: 8px; font-size: 11px; }
.mxvm-link:hover { color: #006600; text-decoration: underline; }
.folder-icon::before { content: "📁 "; }
.file-icon::before   { content: "🗒 "; }
""".strip()


def render_node(node: DirNode, depth: int = 0) -> list[str]:
    lines = []
    if node.files:
        lines.append("<ul>")
        for label, rel_html, rel_mxvm in node.files:
            href = str(rel_html).replace("\\", "/")
            line = (f'  <li><span class="file-icon"></span>'
                    f'<a href="{href}" target="_blank">{label}.pas</a>')
            if rel_mxvm is not None:
                mhref = str(rel_mxvm).replace("\\", "/")
                line += (f'<a href="{mhref}" target="_blank" class="mxvm-link">'
                         f'[{label}.mxvm]</a>')
            line += "</li>"
            lines.append(line)
        lines.append("</ul>")
    for name, child in sorted(node.children.items(), key=lambda t: t[0].lower()):
        open_attr = " open" if depth < 1 else ""
        lines.append(f'<details{open_attr}>')
        lines.append(f'  <summary><span class="folder-icon"></span>{name}</summary>')
        lines.extend(render_node(child, depth + 1))
        lines.append("</details>")
    return lines


def make_index(root: DirNode, out_path: Path, total_pas: int, total_mxvm: int):
    tree_html = "\n".join(render_node(root))
    html = f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>MXVM Pascal Examples</title>
<style>
{DELPHI_CSS}
</style>
</head>
<body>
<h1>MXVM Pascal Examples</h1>
<p style="color:#808080;font-style:italic">{total_pas} Pascal source files &mdash;
{total_mxvm} with compiled MXVM bytecode &mdash;
click a filename to view syntax-highlighted source,
<span style="color:#008000">[name.mxvm]</span> to view compiled bytecode.</p>
<div class="tree">
{tree_html}
</div>
</body>
</html>
"""
    out_path.write_text(html, encoding="utf-8")
    print(f"index: {out_path}")


# ── main ─────────────────────────────────────────────────────────────────────

def main():
    pas_files = sorted(PAS_ROOT.rglob("*.pas"))
    print(f"Found {len(pas_files)} .pas files\n")

    entries: list[tuple[Path, Path, Path | None]] = []
    ok_pas = ok_mxvm = 0

    for pas in pas_files:
        rel_html      = rel_html_path(pas)
        out_html      = OUT_ROOT / rel_html
        rel_mxvm_html = rel_html.with_suffix("").with_suffix(".mxvm.html")  # x.pas → x.mxvm.html
        out_mxvm_html = OUT_ROOT / rel_mxvm_html

        print(f"  {pas.relative_to(REPO)}")

        pas_ok  = convert_pas(pas, out_html)
        mxvm_ok = compile_and_convert_mxvm(pas, out_mxvm_html)

        if pas_ok:  ok_pas  += 1
        if mxvm_ok: ok_mxvm += 1

        entries.append((pas, rel_html, rel_mxvm_html if mxvm_ok else None))

    print(f"\nPascal HTML : {ok_pas}/{len(pas_files)}")
    print(f"MXVM   HTML : {ok_mxvm}/{len(pas_files)}")

    root = build_tree(entries)
    make_index(root, OUT_ROOT / "index.html", ok_pas, ok_mxvm)
    print("Done.")


if __name__ == "__main__":
    main()

