#!/usr/bin/env python3
"""Generate eight fixed cycle-5 composition integrals; never queries Giac."""
import argparse
from collections import Counter
import json
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
OUTPUT=ROOT/'tests/generalization-cycle5.json'


def build_corpus():
    cases=[]
    def add(family,f,expected,bounds,domain,answer_class='elementary-closed-form',**verification):
        c=dict(id=f'C5-{family}-01',family=family,intent='structural-composition',
               domain=domain,answer_class=answer_class,f=f,expected=expected,bounds=list(bounds))
        if verification:c['verification']=verification
        cases.append(c)
    add('gaussian-erf-exponential','exp(-x^2+erf(x))','sqrt(pi)*sinh(1)',
        ('-infinity','+infinity'),'real line; Gaussian decay at both ends')
    add('beta-affine-logit','ln((1+x)/(1-x))^2/sqrt(1-x^2)','pi^3',
        ('-1','1'),'-1<x<1; positive logarithm argument and integrable radical/log endpoints')
    add('frullani-root-log','(atan(2*sqrt(x))-atan(3*sqrt(x)))*ln(x)/x',
        'pi*(ln(3)^2-ln(2)^2)',('0','+infinity'),'x>0; ordinary absolute convergence',
        stable_f='-atan(sqrt(x)/(1+6*x))*ln(x)/x')
    add('loglog-frullani-second','(x-x^3)*ln(-ln(x))^2/(-ln(x))',
        '7*ln(2)^3/3+3*euler_gamma*ln(2)^2+(euler_gamma^2+pi^2/6)*ln(2)',
        ('0','1'),'0<x<1; both logarithms real; integrable endpoint singularities',
        answer_class='special-function-closed-form',
        alternate_f='2*exp(-3*x)*sinh(x)*ln(x)^2/x',alternate_bounds=['0','+infinity'])
    add('laplace-second-log','(exp(-x)-2*exp(-2*x)+exp(-3*x))*ln(x)/x^2',
        '(1-euler_gamma)*(3*ln(3)-4*ln(2))-3*ln(3)^2/2+2*ln(2)^2',
        ('0','+infinity'),'x>0; numerator vanishes to second order at zero',
        answer_class='special-function-closed-form',
        stable_f='4*exp(-2*x)*sinh(x/2)^2*ln(x)/x^2')
    t='(sqrt(x)-1/sqrt(x))'
    add('reciprocal-gaussian-erf',f'(x+1)*exp(-{t}^2)*erf({t})^2/x^(3/2)',
        '2*sqrt(pi)/3',('0','+infinity'),'x>0; reciprocal Gaussian suppresses both endpoints')
    h='atan(sqrt(2))'
    add('arc-quartic-pullback','4*x*acos((3-x^4)/(5-x^4))/(1+x^4)',
        f'2*pi*{h}-2*{h}^2-2*pi*atan(sqrt(5))+pi^2/2',
        ('0','2^(1/4)'),'0<=x<=2^(1/4); principal real acos with argument in [1/3,3/5]')
    add('atan-rectangle-pair',
        '2*atan(3/sqrt(4*x^2+2))/((1+4*x^2)*sqrt(4*x^2+2))+3*atan(2/sqrt(9*x^2+2))/((1+9*x^2)*sqrt(9*x^2+2))',
        'atan(2)*atan(3)',('0','1'),'continuous positive integrand on [0,1]')
    assert len(cases)==8 and len({c['id'] for c in cases})==8
    return {'sources':{'derivations':'docs/GENERALIZATION-CYCLE5.md','generator':'tests/generate-coverage-cycle5.py'},
            'notes':'Independent eight-case audit after f8af451, checkpoint/integration-cycle4-2026a. Frozen before engine evaluation. All ordinary integrals converge and have proved exact references; no PV or unknown-closed-form cases.',
            'summary':{'cases':8,'definite':8,'indefinite':0,
                       'by_answer_class':dict(sorted(Counter(c['answer_class'] for c in cases).items()))},
            'cases':cases}


def verify(data):
    import mpmath as mp
    import sympy as sp
    mp.mp.dps=80
    x=sp.Symbol('x',real=True)
    names=dict(x=x,ln=sp.log,infinity=sp.oo,euler_gamma=sp.EulerGamma)
    def fn(s):return sp.lambdify(x,sp.sympify(s.replace('^','**'),locals=names),'mpmath')
    def number(s):return fn(s)(mp.mpf(0))
    def quad(f,bounds):
        lo,hi=map(number,bounds)
        points=([lo,-16,-4,0,4,16,hi] if lo==-mp.inf else
                [lo,1,4,16,64,hi] if hi==mp.inf else
                [lo,lo+(hi-lo)/4,lo+(hi-lo)/2,lo+3*(hi-lo)/4,hi])
        return mp.quad(f,points)
    max_error=mp.mpf(0);worst=None;checks=0
    for c in data['cases']:
        opts=c.get('verification',{});target=number(c['expected'])
        value=quad(fn(opts.get('stable_f',c['f'])),c['bounds'])
        error=abs(value-target)/max(1,abs(target));checks+=1
        if 'alternate_f' in opts:
            alternate=quad(fn(opts['alternate_f']),opts['alternate_bounds'])
            error=max(error,abs(alternate-target)/max(1,abs(target)));checks+=1
        if not mp.isfinite(error) or error>mp.mpf('1e-25'):
            raise AssertionError(f'{c["id"]}: scaled error {error}')
        if error>max_error:max_error=error;worst=c['id']
        print(f'VERIFY {c["id"]} quadrature error={mp.nstr(error,10)}',flush=True)
    print(json.dumps({'cases':len(data['cases']),'quadrature_checks':checks,
                      'max_scaled_error':mp.nstr(max_error,12),'worst':worst}))


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--check',action='store_true');p.add_argument('--verify',action='store_true')
    args=p.parse_args();data=build_corpus();payload=json.dumps(data,indent=2)+'\n'
    if args.check:
        if OUTPUT.read_text()!=payload:raise SystemExit('Regenerate cycle-5 corpus')
    else:OUTPUT.write_text(payload)
    print(json.dumps(data['summary'],sort_keys=True),flush=True)
    if args.verify:verify(data)


if __name__=='__main__':main()
