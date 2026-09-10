#!/usr/bin/env python3
"""Generate sixteen independently proved cycle-4 integrals; --verify audits them."""
import argparse
from collections import Counter
import json
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
OUTPUT=ROOT/'tests/generalization-cycle4.json'


def build_corpus():
    cases=[]
    def add(family,f,expected,bounds,*,intent='structural-generalization',
            domain='real continuous integrand in the interior; convergent endpoints',
            answer_class='elementary-closed-form',**verification):
        c=dict(id=f'C4-{family}-{sum(r["family"]==family for r in cases)+1:02d}',
               family=family,intent=intent,domain=domain,answer_class=answer_class,
               f=f,expected=expected,bounds=list(bounds))
        if verification:c['verification']=verification
        cases.append(c)

    add('beta-mixed-third','ln(x)*ln(1-x)*ln(x*(1-x))/sqrt(x*(1-x))',
        'pi*(4*Zeta(3)-16*ln(2)^3)',('0','1'),answer_class='special-function-closed-form',domain='0<x<1')
    add('beta-skew-logit','sqrt(x/(1-x))*ln(x/(1-x))^3','3*pi^3',('0','1'),
        intent='near-miss: asymmetric weight has a nonzero odd logit moment',domain='0<x<1')
    add('mellin-distinct-factors','x^(-1/3)/((1+x^(2/3))*(1+4*x^(2/3)))','ln(2)',('0','+infinity'),
        intent='near-miss: distinct denominator factors need partial fractions',domain='x>0; principal powers are positive real')
    add('mellin-unmatched-log-scale','ln(x)^2/(1+9*x^2)^2',
        'pi^3/48+pi*ln(3)/6+pi*ln(3)^2/12',('0','+infinity'),
        intent='near-miss: scaling denominator does not scale the logarithm',domain='x>0')
    add('laplace-squared-cancellation','exp(-x)*(1-cos(2*x))^2/x^2',
        '4*atan(2)-2*atan(4)-ln(5)+ln(17)/4',('0','+infinity'),
        stable_f='4*exp(-x)*sin(x)^4/x^2',removable_zero='0')
    add('laplace-decay-difference','(exp(-x)-2*exp(-2*x)+exp(-3*x))/x^2',
        '3*ln(3)-4*ln(2)',('0','+infinity'),
        stable_f='4*exp(-2*x)*sinh(x/2)^2/x^2',removable_zero='1')
    add('gaussian-erf-shift','exp(-2*x^2)*erf(x+1)','sqrt(pi/2)*erf(sqrt(2/3))',
        ('-infinity','+infinity'),answer_class='special-function-closed-form')
    add('gaussian-erf-product','exp(-(x-1)^2)*erf(x-1)*erf(2*x-2)',
        '2*asin(2/sqrt(10))/sqrt(pi)',('-infinity','+infinity'),
        intent='near-miss: shifted product of two error functions is not an odd Gaussian')
    add('inverse-gaussian-square','exp(-(sqrt(x)-2/sqrt(x))^2)/sqrt(x)','sqrt(pi)',
        ('0','+infinity'),intent='syntax-composition: reciprocal radical square in exponent',domain='x>0')

    h='atan(sqrt(2))'
    add('arc-affine','acos((1+2*cos(2*x+1))/(2+3*cos(2*x+1)))',
        f'pi*{h}-{h}^2-pi*atan(sqrt(5))+pi^2/4',('-1/2',f'{h}-1/2'),
        intent='parameter-variant: affine phase and nonunit disk radius',domain='-1/2<=x<=atan(sqrt(2))-1/2; principal real acos')
    add('arc-complement-rational','2*asin((3-x^2)/(5-x^2))/(1+x^2)',
        f'-pi*{h}+2*{h}^2+2*pi*atan(sqrt(5))-pi^2/2',('0','sqrt(2)'),
        intent='near-miss: inverse sine complement changes the arc value',domain='0<=x<=sqrt(2); argument in [1/3,3/5]')
    add('atan-square-symmetry','atan(2/sqrt(x^2+2))/((1+x^2)*sqrt(x^2+2))',
        'atan(2)^2/2',('0','2'),intent='structural-generalization: reciprocal angle and larger symmetry square')
    add('frullani-log-weight','(atan(2*x)-atan(3*x))*ln(x)/x',
        'pi*(ln(3)^2-ln(2)^2)/4',('0','+infinity'),domain='x>0; logarithmic endpoint and absolutely convergent tail',
        stable_f='-atan(x/(1+6*x^2))*ln(x)/x')
    add('frullani-log-weight','(atan(2*x)-atan(x))*ln(x)^2/x',
        'pi^3*ln(2)/8+pi*ln(2)^3/6',('0','+infinity'),domain='x>0; logarithmic endpoint and absolutely convergent tail',
        stable_f='atan(x/(1+2*x^2))*ln(x)^2/x')
    c='(pi*ln(2*pi)/4+pi*ln(Gamma(3/4)/Gamma(1/4))/2)'
    add('loglog-fractional-scale','sqrt(x)*ln(-ln(x))/(1+x^3)',
        f'(2/3)*({c}-pi*ln(3/2)/4)',('0','1'),domain='0<x<1; both logarithms real',
        answer_class='special-function-closed-form',intent='parameter-variant: rational power and loglog scaling',
        alternate_f='ln(x)/(2*cosh(3*x/2))',alternate_bounds=['0','+infinity'])
    add('loglog-frullani','(x-x^3)*ln(-ln(x))/(-ln(x))','-3*ln(2)^2/2-euler_gamma*ln(2)',
        ('0','1'),domain='0<x<1; logarithmic singularity at one is integrable',
        answer_class='special-function-closed-form',
        alternate_f='(exp(-2*x)-exp(-4*x))*ln(x)/x',alternate_bounds=['0','+infinity'])
    assert len(cases)==16 and len({c['id'] for c in cases})==16
    return {'sources':{'derivations':'docs/GENERALIZATION-CYCLE4.md','generator':'tests/generate-coverage-cycle4.py',
                       'prior-independent-proofs':'docs/USER-CHALLENGE-INTEGRALS.md'},
            'notes':'Fixed independent corpus after checkpoint 0e5ed65, before querying the tested engine. All sixteen ordinary integrals converge and have proved exact references. No principal-value or unknown-closed-form placeholders.',
            'summary':{'cases':16,'definite':16,'indefinite':0,
                       'by_family':dict(sorted(Counter(c['family'] for c in cases).items())),
                       'by_intent':dict(sorted(Counter(c['intent'] for c in cases).items())),
                       'by_answer_class':dict(sorted(Counter(c['answer_class'] for c in cases).items()))},
            'cases':cases}


def verify(data):
    import mpmath as mp
    import sympy as sp
    mp.mp.dps=80
    x=sp.Symbol('x',real=True)
    names=dict(x=x,ln=sp.log,infinity=sp.oo,Gamma=sp.gamma,Zeta=sp.zeta,euler_gamma=sp.EulerGamma)
    def fn(s):return sp.lambdify(x,sp.sympify(s.replace('^','**'),locals=names),'mpmath')
    def number(s):return fn(s)(mp.mpf(0))
    def quad(f,bounds):
        lo,hi=map(number,bounds)
        points=([lo,-16,-4,0,1,4,16,hi] if lo==-mp.inf else
                [lo,1,4,16,64,hi] if hi==mp.inf else
                [lo,lo+(hi-lo)/4,lo+(hi-lo)/2,lo+3*(hi-lo)/4,hi])
        return mp.quad(f,points)
    max_error=mp.mpf(0);worst=None;checks=0
    for c in data['cases']:
        opts=c.get('verification',{});f=fn(opts.get('stable_f',c['f']))
        if 'removable_zero' in opts:
            raw=f;zero=number(opts['removable_zero']);f=lambda t:zero if t==0 else raw(t)
        expected=number(c['expected']);value=quad(f,c['bounds'])
        error=abs(value-expected)/max(1,abs(expected));checks+=1
        if 'alternate_f' in opts:
            alternate=quad(fn(opts['alternate_f']),opts['alternate_bounds'])
            error=max(error,abs(alternate-expected)/max(1,abs(expected)));checks+=1
        if not mp.isfinite(error) or error>mp.mpf('1e-25'):
            raise AssertionError(f'{c["id"]}: scaled error {error}')
        if error>max_error:max_error=error;worst=c['id']
        print(f'VERIFY {c["id"]} quadrature error={mp.nstr(error,9)}',flush=True)
    print(json.dumps({'cases':len(data['cases']),'quadrature_checks':checks,
                      'max_scaled_error':mp.nstr(max_error,11),'worst':worst}))


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--check',action='store_true');p.add_argument('--verify',action='store_true')
    args=p.parse_args();data=build_corpus();payload=json.dumps(data,indent=2)+'\n'
    if args.check:
        if OUTPUT.read_text()!=payload:raise SystemExit('Regenerate cycle-4 corpus')
    else:OUTPUT.write_text(payload)
    print(json.dumps(data['summary'],sort_keys=True),flush=True)
    if args.verify:verify(data)


if __name__=='__main__':main()
