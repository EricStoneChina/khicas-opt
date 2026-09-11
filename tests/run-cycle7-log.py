#!/usr/bin/env python3
"""Bounded log/trig identities, guards and optional full FXCG/64KiB pipeline."""
from pathlib import Path
import argparse,hashlib,json,os,subprocess,tempfile
import integration_build as ib
p=argparse.ArgumentParser();p.add_argument('--helpers',type=Path,help='Pending helper definitions; defaults to integrated repository source');p.add_argument('--full',action='store_true');p.add_argument('--report',type=Path);args=p.parse_args()
s=(ib.ROOT/'yintg.cc').read_text()
signatures=('  static gen integration_cauchy_log_moment(', '  static bool integration_log_affine_ratio(', '  static bool integrate_mobius_cauchy_log(', '  static bool integrate_log_sine_cosine_sum(')
helpers=args.helpers.read_text() if args.helpers else ''.join(ib.function(s,sig) for sig in signatures)
text='#include "giacPCH.h"\nnamespace giac {\n'
for sig in ('  void decompose_prod(', '  gen extract_cst(', '  static bool integration_rational(', '  static gen integration_syntax(', '  static bool integration_power(', '  static bool integration_one_plus(', '  static gen integration_coefficient(', '  static bool integration_monomial(', '  static bool integration_gaussian_quadratic(', '  static int integration_quarter_sigma('):text+=ib.function(s,sig)
text+=helpers
calls=' || '.join(name+'(e,x,lo,hi,r,contextptr)' for name in ('integrate_mobius_cauchy_log','integrate_log_sine_cosine_sum'))
text+='bool cycle7_log_rule(const gen &f,const gen &x,const gen &lo,const gen &hi,gen &r,GIAC_CONTEXT){gen e=integration_syntax(f,contextptr);return '+calls+';}\n}\n'
report=[]
with tempfile.TemporaryDirectory(prefix='khicas-cycle7-log-') as tmp:
 d=Path(tmp);(d/'rules.cc').write_text(text);flags,libs=ib.compiler_options()
 unit=d/'unit';subprocess.run(flags+[str(d/'rules.cc'),str(ib.ROOT/'tests/cycle7_log_integrals.cc')]+libs+['-pthread','-o',str(unit)],check=True)
 for stack in (False,True):subprocess.run([str(unit)],check=True,timeout=60,env=dict(os.environ,**({'KHICAS_TEST_STACK_KIB':'64'} if stack else {})))
 if args.full:
  original=ib.source
  marker='  static bool integrate_compact_definite('
  pos=s.index(marker);pre=s.rfind('#if defined(__GNUC__)',0,pos)
  if signatures[0] in s:
   candidate=s
   if args.helpers:
    for sig in signatures:candidate=candidate.replace(ib.function(candidate,sig).rstrip(),ib.function(helpers,sig).rstrip(),1)
  else:
   candidate=s[:pre]+helpers+'\n'+s[pre:]
   at=candidate.index('{',candidate.index(marker))+1
   dispatch=calls.replace(',r,contextptr)',',res,contextptr)')
   candidate=candidate[:at]+'\n    if('+dispatch+')return true;\n'+candidate[at:]
  # Constant inv(3) extracted from arithmetic syntax must become exact 1/3.
  # Pending companion dispatcher fix; no-op after integration.
  candidate=candidate.replace('gen c=integration_coefficient(e,x,contextptr);\n    if (!integration_rational(c)', 'gen c=integration_syntax(integration_coefficient(e,x,contextptr),contextptr);\n    if (!integration_rational(c)',1)
  (Path('/tmp')/'khicas-cycle7-log-yintg.cc').write_text(candidate)
  ib.source=lambda ref,name:candidate if name=='yintg.cc' else original(ref,name)
  probe=ib.build(d/'full',target_simplify=True);validator=ib.build_validation_probe(d/'validation')
  rows=subprocess.check_output([str(unit),'--list'],text=True).splitlines()
  for stack,outer in ((False,False),(False,True),(True,False),(True,True)):
   for row in rows:
    f,lo,hi,want=row.split('\t');expr=f'integrate({f},x,{lo},{hi})'
    if outer:expr='simplify('+expr+')'
    env=os.environ.copy();env.pop('KHICAS_TEST_STACK_KIB',None)
    if stack:env['KHICAS_TEST_STACK_KIB']='64'
    try:
     r=subprocess.run([str(probe),expr],text=True,capture_output=True,timeout=12,env=env)
     validation=subprocess.run([str(validator),r.stdout.strip(),want,'definite'],text=True,capture_output=True,timeout=12) if r.returncode==0 else None
     ok=r.returncode==0 and validation.returncode==0 and 'CHECK exact' in validation.stderr
     report.append(dict(f=f,stack_kib=64 if stack else None,outer=outer,returncode=r.returncode,output=r.stdout.strip(),exact=ok,validation=validation.stderr if validation else ''))
    except subprocess.TimeoutExpired:ok=False;report.append(dict(f=f,stack_kib=64 if stack else None,outer=outer,status='timeout',exact=False))
    print('PASS' if ok else 'FAIL',stack,outer,expr,flush=True)
  if args.report:args.report.write_text(json.dumps(dict(candidate_sha256=hashlib.sha256(candidate.encode()).hexdigest(),cases=report),indent=2)+'\n')
  if not all(r['exact'] for r in report):raise SystemExit(1)
