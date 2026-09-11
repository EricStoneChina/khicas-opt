#!/usr/bin/env python3
"""Generate the GB18030 UI translation table used by both add-ins."""

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = json.loads((ROOT / "tools" / "ui-zh.json").read_text(encoding="utf-8"))


def c_ascii(text: str) -> str:
    out = ['"']
    for ch in text:
        if ch == '"':
            out.append('\\"')
        elif ch == '\\':
            out.append('\\\\')
        elif ch == '\n':
            out.append('\\n')
        else:
            out.append(ch)
    out.append('"')
    return ''.join(out)


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


lines = [
    '#include "zh_ui.h"',
    '#include "catalogGUI.hpp"',
    '#include "khicas_gb18030.h"',
    '#include <fxcg/display.h>',
    '#include <string.h>',
    '',
    'struct zh_ui_entry { const char *source; const char *translated; };',
    'static const zh_ui_entry zh_ui_entries[] = {',
]
for key, value in source.items():
    lines.append(f'  {{{c_ascii(key)},{c_gb(value)}}},')
lines.extend([
    '};',
    '',
    'const char *zh_ui_translate(const char *text) {',
    '  if (!text || !zh_ui_enabled || (unsigned char)text[0] == 0x01) return text;',
    '  for (unsigned i=0; i<sizeof(zh_ui_entries)/sizeof(zh_ui_entries[0]); ++i)',
    '    if (!strcmp(text,zh_ui_entries[i].source)) return zh_ui_entries[i].translated;',
    '  return text;',
    '}',
    '',
    'void zh_ui_define_status(const char *text) {',
    '  const char *source = zh_ui_translate(text);',
    '  const char *display = source;',
    '  const int gb = khicas_gb_strip(source, &display);',
    '  if (gb) khicas_enable_gb18030();',
    '  DefineStatusMessage((char *)display, 1, 0, 0);',
    '  if (gb) khicas_disable_gb18030();',
    '}',
    '',
    'void zh_ui_bdisp_mmprint(int x, int y, const char *text, int mode,',
    '                         unsigned int xlimit, int p6, int p7, int color,',
    '                         int background, int writeflag, int p11) {',
    '  const char *source = zh_ui_translate(text);',
    '  const char *display = source;',
    '  const int gb = khicas_gb_strip(source, &display);',
    '  if (gb) khicas_enable_gb18030();',
    '  Bdisp_MMPrint(x, y, display, mode, xlimit, p6, p7, color, background,',
    '                writeflag, p11);',
    '  if (gb) khicas_disable_gb18030();',
    '}',
    '',
    'void zh_ui_printmini(int *x, int *y, const char *text, int mode,',
    '                     unsigned int xlimit, int p6, int p7, int color,',
    '                     int background, int writeflag, int p11) {',
    '  const char *source = zh_ui_translate(text);',
    '  const char *display = source;',
    '  const int gb = khicas_gb_strip(source, &display);',
    '  if (gb) khicas_enable_gb18030();',
    '  PrintMini(x, y, (unsigned char *)display, mode, xlimit, p6, p7, color,',
    '            background, writeflag, p11);',
    '  if (gb) khicas_disable_gb18030();',
    '}',
    '',
])
(ROOT / "zh_ui.cpp").write_text("\n".join(lines), encoding="ascii")
print(f"wrote zh_ui.cpp ({len(source)} UI strings)")
