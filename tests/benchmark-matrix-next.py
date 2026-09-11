#!/usr/bin/env python3
"""Paired cold-process host timing for six previously solved matrix cases."""
import argparse,hashlib,json,os,re,statistics,subprocess
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--before',type=Path,required=True);p.add_argument('--after',type=Path,required=True);p.add_argument('--report',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1];audit=json.loads((root/'docs/benchmarks/user-acceptance-matrix-next-2026a.json').read_text())['runs'];env=dict(os.environ);env.pop('KHICAS_TEST_STACK_KIB',None);env.pop('LD_BIND_NOW',None)
rows=[]
for id in ['A4','A5','C1','D6','E3','E4']:
 e=next(r['input'] for r in audit if r['id']==id and r['mode']=='plain' and r['stack']=='normal');times={'before':[],'after':[]};answers={}
 for i in range(21):
  for name in (['before','after'] if i%2 else ['after','before']):
   run=subprocess.run([str(getattr(a,name)),e],capture_output=True,text=True,env=env,timeout=5);assert run.returncode==0,(id,name,run.stderr)
   seconds=float(re.search(r'^SECONDS ([\d.e+-]+)',run.stderr,re.M)[1]);times[name].append(seconds);answers[name]=run.stdout.strip()
 rows.append({'id':id,'input':e,'host_seconds':times,'median_seconds':{k:statistics.median(v) for k,v in times.items()},'median_ratio_before_over_after':statistics.median(times['before'])/statistics.median(times['after']),'actual_output':answers})
a.report.write_text(json.dumps({'scope':'21 paired alternating fresh-process runs per version; time includes parser and computation on host, normal stack. Neither a CG50 speedup claim nor an overall workload speedup. Only previously solved cases compared.','binary_sha256':{k:hashlib.sha256(getattr(a,k).read_bytes()).hexdigest() for k in ['before','after']},'runs':rows},indent=2)+'\n')
for r in rows:print(r['id'],r['median_seconds'],r['median_ratio_before_over_after'])
