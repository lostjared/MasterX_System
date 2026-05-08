#!/usr/bin/env python3
"""Extract a single PNG frame from an Xcursor file.

Xcursor format reference: https://www.x.org/releases/current/doc/man/man3/Xcursor.3.xhtml
This pulls the largest frame whose nominal size is <= max_size and writes it
as a PNG using PIL.
"""
import struct
import sys
from PIL import Image


def extract(src: str, dst: str, target_size: int = 32) -> None:
    with open(src, "rb") as f:
        data = f.read()
    magic, header_sz, version, ntoc = struct.unpack_from("<4sIII", data, 0)
    assert magic == b"Xcur", f"not an xcursor: {magic!r}"
    toc = []
    off = header_sz
    for _ in range(ntoc):
        t, subtype, pos = struct.unpack_from("<III", data, off)
        toc.append((t, subtype, pos))
        off += 12
    # type 0xfffd0002 = image
    images = [(s, p) for (t, s, p) in toc if t == 0xfffd0002]
    if not images:
        raise SystemExit(f"no images in {src}")
    # pick the size closest to target (prefer >= target, else largest)
    images.sort(key=lambda sp: sp[0])
    chosen = None
    for s, p in images:
        if s >= target_size:
            chosen = (s, p)
            break
    if chosen is None:
        chosen = images[-1]
    size, pos = chosen
    # image chunk header: header(I) type(I) subtype(I) version(I)
    # then width(I) height(I) xhot(I) yhot(I) delay(I)
    (hdr_sz, ctype, csub, cver,
     w, h, xhot, yhot, delay) = struct.unpack_from("<IIIIIIIII", data, pos)
    px_off = pos + hdr_sz
    pixels = data[px_off:px_off + w * h * 4]
    # Xcursor stores pre-multiplied ARGB little-endian -> PIL wants RGBA
    img = Image.frombytes("RGBA", (w, h), pixels, "raw", "BGRA")
    img.save(dst, "PNG")
    print(f"{src} -> {dst}  ({w}x{h}, hot=({xhot},{yhot}))")


if __name__ == "__main__":
    pairs = [
        ("/usr/share/icons/Adwaita/cursors/ew-resize",   "ew-resize.png"),
        ("/usr/share/icons/Adwaita/cursors/ns-resize",   "ns-resize.png"),
        ("/usr/share/icons/Adwaita/cursors/nwse-resize", "nwse-resize.png"),
        ("/usr/share/icons/Adwaita/cursors/nesw-resize", "nesw-resize.png"),
    ]
    for src, dst in pairs:
        extract(src, dst, target_size=32)
