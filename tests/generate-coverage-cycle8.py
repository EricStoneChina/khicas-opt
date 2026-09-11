#!/usr/bin/env python3
"""Freeze cycle-8 references by independent derivation and quadrature, without tested-engine calls."""
import argparse
from collections import Counter
import hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
OUTPUT=ROOT/'tests/generalization-cycle8.json'
BASELINE='4c97dc5'
def build_corpus():
    cases=[]
    def add(family,f,expected,domain,*,bounds=None,samples=None,intent='structural-composition',answer_class='elementary-closed-form',verification=None):
        c=dict(id=f'C8-{family}-01',family=family,intent=intent,domain=domain,answer_class=answer_class,f=f,expected=expected)
        if bounds is not None:c['bounds']=list(bounds)
        if samples is not None:c['samples']=list(samples)
        if verification:c['verification']=verification
        cases.append(c)
    add('quadratic-asinh-parts','(2*x+1)*asinh(x^2+x+1)',
        '(x^2+x+1)*asinh(x^2+x+1)-sqrt(1+(x^2+x+1)^2)',
        'all real x; principal real asinh and positive square root',samples=['-3','-1/2','1/5','1','4'])
    add('composed-binomial-parts-boundary','(2*x+1)*(x^2+x)^63*(1+(x^2+x)^32)^1024',
        '(1+(x^2+x)^32)^1026/32832-(1+(x^2+x)^32)^1025/32800',
        'all real x; integer powers and strictly positive 1+(x^2+x)^32',samples=['-1/2','1/20','1/3','1','2'],
        intent='resource-expression-growth',verification=dict(resource_note='Expanded integrand degree 65663; t=(x^2+x)^32 reduces to t*(1+t)^1024/32 and a two-term primitive.',symbolic_proof='generic-binomial-parts'))
    add('exponential-beta-boundary','exp(-2*x)*(1-exp(-x))^4096','1/(4097*4098)',
        'x>=0; O(x^4096) at zero and O(exp(-2*x)) at infinity',bounds=['0','+infinity'],
        intent='resource-expression-growth',verification=dict(alternate_f='x*(1-x)^4096',alternate_bounds=['0','1'],resource_note='Expanding the binomial creates 4097 exponential terms; t=exp(-x) leaves a single beta moment.'))
    add('laplace-erf-square-root','exp(-x)*erf(sqrt(x))','1/sqrt(2)',
        'x>=0; principal square root; O(sqrt(x)) at zero and exponential decay at infinity',bounds=['0','+infinity'],
        verification=dict(alternate_f='2*exp(-2*x^2)/sqrt(pi)',alternate_bounds=['0','+infinity']))
    add('atan-log-cauchy-moment','atan(x)*ln(x)/(1+x^2)',
        '7*Zeta(3)/16-pi*(Psi(1/4,1)-Psi(3/4,1))/64',
        '0<x<=1; O(x*ln(x)) at zero; principal real atan',bounds=['0','1'],answer_class='special-function-closed-form',
        verification=dict(alternate_f='x*ln(tan(x))',alternate_bounds=['0','pi/4']))
    add('opposite-trig-logarithm-product','ln(1+sin(x))*ln(1-sin(x))',
        'pi*ln(2)^2/2-pi^3/12',
        '0<=x<pi/2; both log arguments positive in the interior; ordinary integrable logarithmic endpoint singularity',bounds=['0','pi/2'],
        verification=dict(stable_near_endpoint='sine-opposite-log-product',alternate_f='(ln(2)+2*ln(sin(x)))*(ln(2)+2*ln(cos(x)))',alternate_bounds=['0','pi/2']))
    add('atan-square-cauchy-square','atan(x)^2/(1+x^2)^2','pi^3/48-pi/8',
        'x>=0; O(x^2) at zero and O(x^-4) at infinity',bounds=['0','+infinity'],
        verification=dict(alternate_f='x^2*cos(x)^2',alternate_bounds=['0','pi/2']))
    add('gaussian-log-square-moment','exp(-x^2)*ln(x)^2',
        'sqrt(pi)*(Psi(1/2)^2+Psi(1/2,1))/8',
        'x>0; logarithmic-square singularity integrable at zero; Gaussian decay at infinity',bounds=['0','+infinity'],answer_class='special-function-closed-form')
    assert len(cases)==8 and len({c['id'] for c in cases})==8
    return dict(sources=dict(derivations='docs/GENERALIZATION-CYCLE8.md',generator='tests/generate-coverage-cycle8.py'),
        notes=f'Independent audit frozen after {BASELINE}, checkpoint/integration-cycle7-2026a, before tested-engine evaluation. Two proved primitives, six ordinarily absolutely convergent definite integrals, two explicit resource-growth probes. No unknown-closed-form or PV placeholders.',
        summary=dict(cases=8,definite=6,indefinite=2,resource_growth=2,by_answer_class=dict(sorted(Counter(c['answer_class'] for c in cases).items()))),cases=cases)
def verify(data):
    import mpmath as mp
    import sympy as sp
    x=sp.Symbol('x',real=True)
    names=dict(x=x,ln=sp.log,infinity=sp.oo,Psi=lambda z,n=0:sp.polygamma(n,z),Zeta=sp.zeta)
    def symbolic(s):return sp.sympify(s.replace('^','**'),locals=names)
    def fn(s):return sp.lambdify(x,symbolic(s),'mpmath')
    def number(s):return fn(s)(mp.mpf(0))
    def quad(f,bounds):
        lo,hi=map(number,bounds)
        points=[lo,1,4,8,16,32,hi] if hi==mp.inf else [lo,lo+(hi-lo)/4,lo+(hi-lo)/2,lo+3*(hi-lo)/4,hi]
        return mp.quad(f,points)
    for c in data['cases']:
        if 'samples' not in c:continue
        if c.get('verification',{}).get('symbolic_proof')=='generic-binomial-parts':
            # Prove the reduced identity for a symbolic exponent, preventing
            # the independent verifier itself from expanding a degree-65663 polynomial.
            t=sp.Symbol('t',positive=True);n=sp.Symbol('n',positive=True)
            F=(1+t)**(n+2)/(32*(n+2))-(1+t)**(n+1)/(32*(n+1))
            assert sp.simplify(sp.diff(F,t)-t*(1+t)**n/32)==0
            q=x*x+x
            assert sp.simplify(sp.diff(q**32,x)/q**31-32*(2*x+1))==0
        elif sp.simplify(sp.diff(symbolic(c['expected']),x)-symbolic(c['f']))!=0:
            raise AssertionError('Symbolic derivative proof failed: '+c['id'])
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
                if opts.get('stable_near_endpoint')=='sine-opposite-log-product':
                    f=lambda t:mp.log1p(mp.sin(t))*(2*mp.log(abs(mp.cos(t)))-mp.log1p(mp.sin(t)))
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
        if OUTPUT.read_text()!=payload:raise SystemExit('Regenerate cycle-8 corpus')
    else:OUTPUT.write_text(payload)
    print(json.dumps(dict(summary=data['summary'],baseline=BASELINE,sha256=hashlib.sha256(payload.encode()).hexdigest())),flush=True)
    if args.verify:verify(data)
if __name__=='__main__':main()
