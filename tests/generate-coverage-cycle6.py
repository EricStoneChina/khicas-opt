#!/usr/bin/env python3
"""Freeze eight independently derived cycle-6 integrals; never queries Giac."""
import argparse
from collections import Counter
import hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
OUTPUT=ROOT/'tests/generalization-cycle6.json'
BASELINE='dff6974'
def build_corpus():
    cases=[]
    def add(family,f,expected,bounds,domain,answer_class='elementary-closed-form',**verification):
        case=dict(id=f'C6-{family}-01',family=family,intent='structural-composition',domain=domain,
                  answer_class=answer_class,f=f,expected=expected,bounds=list(bounds))
        if verification:case['verification']=verification
        cases.append(case)
    add('gaussian-atan-parts','x*exp(-x^2)*atan(x)','pi*exp(1)*erfc(1)/4',('0','+infinity'),
        'x>=0; smooth at zero and Gaussian decay at infinity','special-function-closed-form')
    add('half-period-log-sine-moment','x*ln(sin(x))','7*Zeta(3)/16-pi^2*ln(2)/8',('0','pi/2'),
        '0<x<=pi/2; x*ln(x) endpoint is integrable','special-function-closed-form')
    add('opposite-logarithm-product','ln(1-x)*ln(1+x)/x','-5*Zeta(3)/8',('0','1'),
        '0<x<1; both logarithms real; logarithmic endpoint singularity is integrable','special-function-closed-form')
    add('mixed-degree-mellin-log','ln(x)/((1+x)*(1+x^2))','-pi^2/16',('0','+infinity'),
        'x>0; no denominator zeros; logarithmic zero endpoint and cubic decay at infinity')
    add('trig-log-quarter-period','ln(1+sin(x))','(Psi(1/4,1)-Psi(3/4,1))/8-pi*ln(2)/2',('0','pi/2'),
        'continuous real nonnegative logarithm on [0,pi/2]','special-function-closed-form')
    add('mobius-atan-log-measure','atan(x)/(1+x)','pi*ln(2)/8',('0','1'),
        'continuous principal real atan; denominator strictly positive')
    add('oscillatory-third-order-cancellation','(sin(x)-x*cos(x))/x^3','pi/4',('0','+infinity'),
        'removable limit 1/3 at zero; absolutely integrable oscillatory tail',
        oscillation_frequency='1')
    add('fermi-logarithm-first-moment','x*ln(1+exp(-x))','3*Zeta(3)/4',('0','+infinity'),
        'x>=0; finite zero endpoint and exponential tail','special-function-closed-form',
        alternate_f='-ln(x)*ln(1+x)/x',alternate_bounds=['0','1'])
    assert len(cases)==8 and len({c['id'] for c in cases})==8
    return dict(sources=dict(derivations='docs/GENERALIZATION-CYCLE6.md',generator='tests/generate-coverage-cycle6.py'),
        notes=f'Independent eight-case audit after {BASELINE}, checkpoint/integration-cycle5-2026a. Frozen before engine evaluation. Ordinary absolute convergence, proved exact references, no principal values.',
        summary=dict(cases=8,definite=8,indefinite=0,by_answer_class=dict(sorted(Counter(c['answer_class'] for c in cases).items()))),cases=cases)
def verify(data):
    import mpmath as mp
    import sympy as sp
    x=sp.Symbol('x',real=True)
    names=dict(x=x,ln=sp.log,infinity=sp.oo,Zeta=sp.zeta,Psi=lambda z,n=0:sp.polygamma(n,z))
    def fn(s):return sp.lambdify(x,sp.sympify(s.replace('^','**'),locals=names),'mpmath')
    def number(s):return fn(s)(mp.mpf(0))
    def quad(f,bounds):
        lo,hi=map(number,bounds)
        points=[lo,1,4,16,64,hi] if hi==mp.inf else [lo,lo+(hi-lo)/4,lo+(hi-lo)/2,lo+3*(hi-lo)/4,hi]
        return mp.quad(f,points)
    worst=mp.mpf(0);checks=0
    for precision in (80,110):
        mp.mp.dps=precision
        for case in data['cases']:
            target=number(case['expected']);opts=case.get('verification',{})
            if 'oscillation_frequency' in opts:
                # Use the convergent Taylor series near the removable endpoint
                # and oscillation-aware quadrature for the unchanged tail.
                def stable(t):
                    if abs(t)<mp.mpf('0.1'):
                        return mp.fsum([(-1)**(k+1)*2*k*t**(2*k-2)/mp.factorial(2*k+1) for k in range(1,42)])
                    return (mp.sin(t)-t*mp.cos(t))/t**3
                value=mp.quadosc(stable,[0,mp.inf],omega=number(opts['oscillation_frequency']))
            else:value=quad(fn(case['f']),case['bounds'])
            error=abs(value-target)/max(1,abs(target));checks+=1
            if 'alternate_f' in opts:
                error=max(error,abs(quad(fn(opts['alternate_f']),opts['alternate_bounds'])-target)/max(1,abs(target)));checks+=1
            if not mp.isfinite(error) or error>mp.mpf('1e-45'):raise AssertionError(f'{case["id"]}: scaled error {error}')
            worst=max(worst,error)
            print(f'VERIFY {precision} digits {case["id"]}: value={mp.nstr(value,55)}, scaled_error={mp.nstr(error,8)}',flush=True)
    print(json.dumps(dict(cases=8,precisions=[80,110],quadrature_checks=checks,max_scaled_error=mp.nstr(worst,12))),flush=True)
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--check',action='store_true');p.add_argument('--verify',action='store_true');args=p.parse_args()
    data=build_corpus();payload=json.dumps(data,indent=2)+'\n'
    if args.check:
        if OUTPUT.read_text()!=payload:raise SystemExit('Regenerate cycle-6 corpus')
    else:OUTPUT.write_text(payload)
    print(json.dumps(dict(summary=data['summary'],baseline=BASELINE,sha256=hashlib.sha256(payload.encode()).hexdigest())),flush=True)
    if args.verify:verify(data)
if __name__=='__main__':main()
