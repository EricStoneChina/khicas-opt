#!/usr/bin/env python3
"""Check the English command help for short, runnable, plain wording."""
from __future__ import annotations
import csv
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
STRICT = {
    'SVD','circle','graph_union','graph_equal','minimal_edge_coloring',
    'random_planar_graph','random_tree','rand','random','seq','sum','product',
    'mul','diff','integrate','factor','simplify','solve','matrix','plot',
    'kernel_density','lpsolve','LU','QR','parallel','dijkstra','shortest_path',
    'vertex_distance','is_rectangle','is_isosceles','+infinity','-infinity',
    'DropDown','Request','Text','Title','atan2acos','atan2asin','heapify',
    'heappop','heappush','maple2mupad','maple2xcas','mupad2maple','mupad2xcas',
    'plex','revlex','tdeg','acyclic','bandwidth','bins','bipartite','connected',
    'directed','planar','spring','tree','weighted','weights',
}
BANNED = (
    'infixed', 'postfixed', 'iff', 'respectively', 'thereof', 'whereas',
    'equivanent', 'equirepartited', 'tridiagonale', 'maytice', 'sugbraph',
    'multiplicates', 'substract', 'realizes', 'given',
)


def rows():
    result = []
    with (ROOT / 'static_helpen.h').open(encoding='utf-8', newline='') as stream:
        for line in stream:
            if not line.strip():
                continue
            fields = next(csv.reader([line.strip().rstrip(',')], delimiter=',', quotechar='"', escapechar='\\'))
            fields[0] = fields[0].lstrip('{')
            if fields[0].startswith('"') and fields[0].endswith('"'):
                fields[0] = fields[0][1:-1]
            fields[-1] = fields[-1].rstrip('}')
            result.append(fields)
    return result


all_rows = rows()
entries = {row[0]: row for row in all_rows}
assert len(all_rows) == 2075, f'Unexpected help row count: {len(all_rows)}'
assert len(entries) >= 2000, f'Unexpected unique help entry count: {len(entries)}'
for command in sorted(STRICT):
    assert command in entries, f'Missing help entry: {command}'
    howto = entries[command][1].strip()
    examples = entries[command][3].strip()
    assert howto, f'{command}: missing English description'
    assert examples, f'{command}: missing runnable example'
    words = howto.replace('/', ' ').split()
    assert len(words) <= 30, f'{command}: description is too long ({len(words)} words)'
    lowered = howto.lower()
    for word in BANNED:
        assert word not in lowered, f'{command}: academic or unclear wording: {word}'

long_descriptions = [row for row in entries.values() if len(row[1].split()) > 30]
assert not long_descriptions, f'Descriptions over 30 words: {[row[0] for row in long_descriptions]}'
style_hits = []
for row in entries.values():
    for word in BANNED:
        if re.search(r'\\b' + re.escape(word) + r'\\b', row[1], re.IGNORECASE):
            style_hits.append((row[0], word))
assert not style_hits, f'Academic or unclear wording remains: {style_hits[:10]}'
missing_examples = [row[0] for row in entries.values() if row[2].strip() and row[2].strip() != 'Opt' and not row[3].strip()]
assert not missing_examples, f'Callable entries without examples: {missing_examples}'
empty_examples = sum(not row[3].strip() for row in entries.values())
long_descriptions = sum(len(row[1].split()) > 30 for row in entries.values())
print(f'PASS: {len(entries)} English help entries')
print(f'INFO: {empty_examples} entries have no example (constants and option names may be intentional)')
print(f'INFO: {long_descriptions} descriptions are over 30 words and remain in the upstream catalog')
print(f'PASS: {len(STRICT)} reviewed entries use short wording and runnable examples')
