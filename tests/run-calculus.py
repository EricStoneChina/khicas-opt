#!/usr/bin/env python3
"""Bounded MIT / Princeton calculus benchmark: actual old/new repository integrator.

An unresolved result/timeout is recorded, not counted as a correct answer.
Use --strict to fail unless all selected problems have exact validation.
Host RSS includes the host library and is not calculator RAM usage.
"""
import argparse, hashlib, json, re, subprocess, tempfile, time
from collections import Counter
from pathlib import Path
from integration_build import ROOT, BASE, build

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compare', action='store_true')
parser.add_argument('--timeout', type=float, default=10)
parser.add_argument('--report', type=Path, required=True)
parser.add_argument('--strict', action='store_true')
parser.add_argument('--only', help='Comma-separated problem IDs')
args = parser.parse_args()
data = json.loads((ROOT/'tests/calculus-corpus.json').read_text())
cases = data['cases']
if args.only:
    selected = args.only.split(',')
    cases = [c for c in cases if c['id'] in selected]
    assert len(cases) == len(selected), 'Unknown or duplicate problem ID'
report = {'baseline': BASE, 'timeout_seconds': args.timeout,
          'scope': 'actual yintg, zintgab and normalization, host Giac dependencies; not CG50 timings',
          'source_sha256': {name: hashlib.sha256((ROOT/name).read_bytes()).hexdigest()
                            for name in ('yintg.cc','zintgab.cc','ysym2poly.cc','integration_guard.h')},
          'corpus_sha256': hashlib.sha256((ROOT/'tests/calculus-corpus.json').read_bytes()).hexdigest(),
          'sources': data['sources'], 'runs': {}}
with tempfile.TemporaryDirectory(prefix='khicas-calculus-') as tmp:
    for ref in (('baseline', 'current') if args.compare else ('current',)):
        exe = build(Path(tmp)/ref, ref)
        rows = []
        for case in cases:
            tail = ','.join(['x'] + case.get('bounds', []))
            expr = f"integrate({case['f']},{tail})"
            mode = 'definite' if 'bounds' in case else 'indefinite'
            command = [str(exe), expr, case['expected'], mode, case['f']]
            command += case.get('samples', ['-2', '-1', '1/3', '1', '2'])
            start = time.monotonic()
            try:
                p = subprocess.run(command, capture_output=True, text=True, timeout=args.timeout)
                checks = re.findall(r'^CHECK (\w+)', p.stderr, re.M)
                status = checks[-1] if checks else {2:'unevaluated', 3:'undefined'}.get(p.returncode, 'error')
                row = {'id':case['id'], 'status':status, 'exit':p.returncode,
                       'result':p.stdout.strip()[:4096], 'result_characters':len(p.stdout.strip()),
                       'result_sha256':hashlib.sha256(p.stdout.strip().encode()).hexdigest(),
                       'stderr':p.stderr[-2500:]}
                for label, key, convert in [('SECONDS', 'integration_seconds', float),
                                           ('PARSER_CALLS', 'parser_calls', int),
                                           ('MAX_RSS_KB', 'host_max_rss_kb', int)]:
                    match = re.search(r'^'+label+r' ([\d.e+-]+)', p.stderr, re.M)
                    if match: row[key] = convert(match[1])
            except subprocess.TimeoutExpired as error:
                status='validation_timeout' if b'SECONDS ' in (error.stderr or b'') else 'integration_timeout'
                row = {'id':case['id'], 'status':status,
                       'result':(error.stdout or b'').decode(errors='replace')[:2500],
                       'stderr':(error.stderr or b'').decode(errors='replace')[-2500:]}
            row['wall_seconds'] = time.monotonic()-start
            rows.append(row)
            print(ref, row['id'], row['status'], f"{row['wall_seconds']:.3f}s", flush=True)
            report['runs'][ref] = rows
            args.report.write_text(json.dumps(report, indent=2, ensure_ascii=False)+'\n')
        print(ref, dict(Counter(row['status'] for row in rows)), flush=True)
if args.strict and any(row['status'] != 'exact' for row in report['runs']['current']):
    raise SystemExit(1)
