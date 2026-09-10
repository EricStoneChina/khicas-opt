#!/usr/bin/env python3
"""Freeze cycle-7 composition/resource cases without querying an integration engine."""
import argparse
from collections import Counter
import hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
OUTPUT=ROOT/'tests/generalization-cycle7.json'
BASELINE='c977aa0'
def build_corpus():
    cases=[]
    def add(family,f,expected,domain,*,bounds=None,samples=None,intent='structural-composition',answer_class='elementary-closed-form',verification=None):
        c=dict(id=f'C7-{family}-01',family=family,intent=intent,domain=domain,answer_class=answer_class,f=f,expected=expected)
        if bounds is not None:c['bounds']=list(bounds)
        if samples is not None:c['samples']=list(samples)
        if verification:c['verification']=verification
        cases.append(c)
    add('quadratic-radical-atan-parts','(2*x+1)*atan(sqrt(x^2+x+1))/sqrt(x^2+x+1)',
        '2*sqrt(x^2+x+1)*atan(sqrt(x^2+x+1))-ln(x^2+x+2)',
        'all real x; x^2+x+1>=3/4; principal positive square root',samples=['-3','-1/2','1/5','1','4'])
    add('nested-root-degree-boundary','(1+x^(1/17))^(1/19)/x^(16/17)',
        '323*(1+x^(1/17))^(20/19)/20','x>0; all fractional-power bases positive',samples=['1/1000','1/7','1','17','1000'],
        intent='resource-expression-growth',verification=dict(resource_note='Keep two nested radicals compact; a generic algebraic extension can have degree 17*19=323.'))
    add('composed-high-power-beta-boundary','(2*x+1)*(x^2+x)^15*(1-(x^2+x)^16)^2048',
        '1/32784','0<=x<=(sqrt(5)-1)/2; x^2+x increases from 0 to 1',bounds=['0','(sqrt(5)-1)/2'],
        intent='resource-expression-growth',verification=dict(alternate_f='(1-x)^2048/16',alternate_bounds=['0','1'],resource_note='Nested expansion has degree 65567; substitution gives a short exact rational result.'))
    add('erf-erfc-product','erf(x)*erfc(x)','(sqrt(2)-1)/sqrt(pi)',
        'x>=0; O(x) at zero and Gaussian decay at infinity',bounds=['0','+infinity'])
    add('radical-cauchy-log-moment','ln(1+x^2)/(1+x^2)^(3/2)','2-2*ln(2)',
        'x>=0; O(x^2) at zero and O(ln(x)/x^3) at infinity',bounds=['0','+infinity'],
        verification=dict(alternate_f='-ln(1-x^2)',alternate_bounds=['0','1']))
    add('damped-sine-linear-cancellation','exp(-x)*(sin(2*x)-2*x)/x^2','2-atan(2)-ln(5)',
        'x>0; numerator cancels to third order; ordinary absolute convergence',bounds=['0','+infinity'],
        verification=dict(stable_near_zero='sine-minus-linear-series'))
    add('mobius-logit-cauchy','ln((1+x)/(1-x))^2/(1+x^2)','pi^3/16',
        '0<x<1; real logarithm; squared logarithmic singularity at 1 is integrable',bounds=['0','1'],
        verification=dict(alternate_f='ln(x)^2/(1+x^2)',alternate_bounds=['1','+infinity']))
    add('log-sine-cosine-sum','ln(sin(x)+cos(x))',
        '(Psi(1/4,1)-Psi(3/4,1))/16-pi*ln(2)/4',
        '0<=x<=pi/2; sin(x)+cos(x)>=1',bounds=['0','pi/2'],answer_class='special-function-closed-form')
    assert len(cases)==8 and len({c['id'] for c in cases})==8
    return dict(sources=dict(derivations='docs/GENERALIZATION-CYCLE7.md',generator='tests/generate-coverage-cycle7.py'),
        notes=f'Independent audit frozen after {BASELINE}, checkpoint/integration-cycle6-2026a, before tested-engine evaluation. Two proved primitives, six ordinarily absolutely convergent definite integrals, two explicit resource-growth probes. No unknown-closed-form or PV placeholders.',
        summary=dict(cases=8,definite=6,indefinite=2,resource_growth=2,by_answer_class=dict(sorted(Counter(c['answer_class'] for c in cases).items()))),cases=cases)
def verify(data):
    import mpmath as mp
    import sympy as sp
    x=sp.Symbol('x',positive=True)
    names=dict(x=x,ln=sp.log,infinity=sp.oo,Psi=lambda z,n=0:sp.polygamma(n,z))
    def symbolic(s):return sp.sympify(s.replace('^','**'),locals=names)
    def fn(s):return sp.lambdify(x,symbolic(s),'mpmath')
    def number(s):return fn(s)(mp.mpf(0))
    def quad(f,bounds):
        lo,hi=map(number,bounds)
        points=[lo,max(lo+1,2),8,32,128,hi] if hi==mp.inf else [lo,lo+(hi-lo)/4,lo+(hi-lo)/2,lo+3*(hi-lo)/4,hi]
        return mp.quad(f,points)
    for c in data['cases']:
        if 'samples' in c:
            if sp.simplify(sp.diff(symbolic(c['expected']),x)-symbolic(c['f']))!=0:raise AssertionError('Symbolic derivative proof failed: '+c['id'])
    worst=mp.mpf(0);quadratures=0;derivatives=0
    for precision in (80,110):
        mp.mp.dps=precision
        for c in data['cases']:
            f=fn(c['f']);reference=fn(c['expected']);opts=c.get('verification',{})
            if 'samples' in c:
                error=mp.mpf(0)
                for sample in c['samples']:
                    t=number(sample);value=mp.diff(reference,t);target=f(t)
                    error=max(error,abs(value-target)/max(1,abs(target)));derivatives+=1
            else:
                if opts.get('stable_near_zero')=='sine-minus-linear-series':
                    def stable(t):
                        if abs(t)<mp.mpf('0.1'):
                            return mp.exp(-t)*mp.fsum([(-1)**k*2**(2*k+1)*t**(2*k-1)/mp.factorial(2*k+1) for k in range(1,48)])
                        return mp.exp(-t)*(mp.sin(2*t)-2*t)/t**2
                    f=stable
                value=quad(f,c['bounds']);target=reference(mp.mpf(0));quadratures+=1
                error=abs(value-target)/max(1,abs(target))
                if 'alternate_f' in opts:
                    other=quad(fn(opts['alternate_f']),opts['alternate_bounds']);quadratures+=1
                    error=max(error,abs(other-target)/max(1,abs(target)))
            if not mp.isfinite(error) or error>mp.mpf('1e-45'):raise AssertionError(f'{c["id"]}: scaled error {error}')
            worst=max(worst,error)
            print(f'VERIFY {precision} digits {c["id"]} scaled_error={mp.nstr(error,10)}',flush=True)
    print(json.dumps(dict(cases=8,symbolic_derivatives=2,numerical_derivatives=derivatives,quadrature_checks=quadratures,precisions=[80,110],max_scaled_error=mp.nstr(worst,12))),flush=True)
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--check',action='store_true');p.add_argument('--verify',action='store_true');args=p.parse_args()
    data=build_corpus();payload=json.dumps(data,indent=2)+'\n'
    if args.check:
        if OUTPUT.read_text()!=payload:raise SystemExit('Regenerate cycle-7 corpus')
    else:OUTPUT.write_text(payload)
    print(json.dumps(dict(summary=data['summary'],baseline=BASELINE,sha256=hashlib.sha256(payload.encode()).hexdigest())),flush=True)
    if args.verify:verify(data)
if __name__=='__main__':main()
