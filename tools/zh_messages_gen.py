#!/usr/bin/env python3
"""Generate GB18030-marked compile-time strings for the Chinese add-in."""

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
data = json.loads((ROOT / "tools" / "zh_messages.json").read_text(encoding="utf-8"))


def c_gb(text: str) -> str:
    out = ['"\\x01""']
    for byte in text.encode("gb18030", errors="replace"):
        if byte >= 0x80:
            out.append(f'""\\x{byte:02x}""')
        elif byte == 0x22:
            out.append('\\"')
        elif byte == 0x5c:
            out.append('\\\\')
        elif byte == 0x0a:
            out.append('\\n')
        else:
            out.append(chr(byte))
    out.append('"')
    return ''.join(out)


lines = ["#ifndef KHICAS_ZH_MESSAGES_H", "#define KHICAS_ZH_MESSAGES_H", ""]
for name, value in data.items():
    if isinstance(value, list):
        value = "\\n".join(value)
    lines.append(f"#define ZH_{name.upper()} {c_gb(value)}")
lines.extend(["", "#endif", ""])
(ROOT / "zh_messages.h").write_text("\n".join(lines), encoding="ascii")
print(f"wrote zh_messages.h ({len(data)} strings)")
