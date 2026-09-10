#!/usr/bin/env python3
"""Original user inputs, exact validation and a separate 64 KiB stack regression."""
import argparse,hashlib,json,re,resource,statistics,subprocess,tempfile
from pathlib import Path
from integration_build import ROOT,build
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--report',type=Path,required=True)
p.add_argument('--repeats',type=int,default=3)
args=p.parse_args();assert args.repeats>0
cases=json.loads((ROOT/'tests/user-integrals.json').read_text())
report={'base_commit':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),
        'scope':'actual yintg, zintgab and normalization; other dependencies including outer simplify are host Giac; not CG50 timings',
        'source_sha256':{n:hashlib.sha256((ROOT/n).read_bytes()).hexdigest() for n in ('yintg.cc','zintgab.cc','ysym2poly.cc','integration_guard.h')},
        'timeout_seconds':30,'virtual_address_limit_MiB':768,'cases':[]}
def small_stack():resource.setrlimit(resource.RLIMIT_STACK,(65536,65536))
failed=False
with tempfile.TemporaryDirectory(prefix='khicas-user-integrals-') as tmp:
    exe=build(Path(tmp))
    for case in cases:
        row=dict(case);row['runs']=[]
        for _ in range(args.repeats):
            try:
                r=subprocess.run([str(exe),case['input'],case['reference'],case['mode'],case['integrand']]+case['samples'],capture_output=True,text=True,timeout=30)
                ck=re.findall(r'^CHECK (\w+)',r.stderr,re.M)
                result={'exit':r.returncode,'status':ck[-1] if ck else {2:'unevaluated',3:'undefined'}.get(r.returncode,'error'),
                        'result':r.stdout.strip(),'stderr':r.stderr}
                tm=re.search(r'^SECONDS ([\d.e+-]+)',r.stderr,re.M)
                if tm:result['seconds']=float(tm[1])
            except subprocess.TimeoutExpired:
                result={'status':'timeout'}
            row['runs'].append(result)
            if result['status']!='exact':failed=True;break
        ts=[r['seconds'] for r in row['runs'] if 'seconds' in r]
        if ts:row['median_host_seconds']=statistics.median(ts)
        try:
            r=subprocess.run([str(exe),case['input']],capture_output=True,text=True,timeout=30,preexec_fn=small_stack)
            row['stack_64KiB']={'exit':r.returncode,'result':r.stdout.strip(),'stderr':r.stderr,
                                'same_as_normal_stack':r.stdout.strip()==row['runs'][0].get('result')}
            if r.returncode or not row['stack_64KiB']['same_as_normal_stack']:failed=True
        except subprocess.TimeoutExpired:
            row['stack_64KiB']={'status':'timeout'};failed=True
        report['cases'].append(row)
        args.report.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
        print(case['id'],row['runs'][0]['status'],row.get('median_host_seconds'),row['stack_64KiB'].get('exit'),flush=True)
if failed:raise SystemExit(1)
print('PASS: all eight exact; all eight complete with identical results under a 64 KiB stack')
