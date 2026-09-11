#!/usr/bin/env python3
"""Fixed third-round integral corpus with an independent reference audit."""
import argparse
from collections import Counter
import json
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
OUTPUT=ROOT/'tests/generalization-cycle3.json'


def build_corpus():
    cases=[]
    def add(family,f,expected,bounds=None,samples=None,intent='structural-generalization',
            domain='real integrand on the stated interval',answer_class='elementary-closed-form',
            scale=None,**verification):
        row=dict(id=f'C3-{family}-{sum(c["family"]==family for c in cases)+1:02d}',
                 family=family,intent=intent,domain=domain,answer_class=answer_class,
                 f=f,expected=expected)
        if bounds is not None:row['bounds']=list(bounds)
        else:row['samples']=samples or ['-3/2','1/3','5/2']
        if scale is not None:row['scale']=scale
        if verification:row['verification']=verification
        cases.append(row)

    add('mellin-denominator-power','x^(-1/2)/(1+x^(3/2))^2','8*pi/(9*sqrt(3))',('0','+infinity'),domain='x>0; integrable endpoint and tail')
    add('mellin-denominator-power','x^(3/2)/(1+x^2)^3','Gamma(5/4)*Gamma(7/4)/4',('0','+infinity'),answer_class='special-function-closed-form',domain='x>0')
    add('mellin-log-denominator-power','ln(x)/(1+x^2)^2','-pi/4',('0','+infinity'),domain='x>0')
    add('mellin-log-denominator-power','ln(x)^2/(1+x^2)^2','pi^3/16',('0','+infinity'),domain='x>0')
    add('beta-logit','ln(x/(1-x))^2/sqrt(x*(1-x))','pi^3',('0','1'),domain='0<x<1; integrable logarithmic endpoints')
    add('beta-logit','sqrt(x/(1-x))*ln(x/(1-x))','pi',('0','1'),intent='near-miss: asymmetric weight prevents odd-logit cancellation',domain='0<x<1')
    add('beta-mixed-log','ln(x)^3/sqrt(x*(1-x))','-pi*(8*ln(2)^3+2*pi^2*ln(2)+12*Zeta(3))',('0','1'),answer_class='special-function-closed-form',domain='0<x<1')
    add('beta-mixed-log','sqrt(x*(1-x))*ln(x)*ln(1-x)','pi/8*((1/2-2*ln(2))^2-pi^2/6+5/4)',('0','1'),domain='0<x<1')

    add('laplace-second-cancellation','exp(-2*x)*(1-cos(3*x))/x^2','3*atan(3/2)-ln(13/4)',('0','+infinity'),removable_zero='9/2')
    add('laplace-second-cancellation','exp(-3*x)*(sin(2*x)-2*x)/x^2','2-3*atan(2/3)-ln(13/9)',('0','+infinity'),removable_zero='0')
    add('laplace-second-cancellation','exp(-2*x)*(sin(3*x+pi/3)-sin(pi/3)-3*x*cos(pi/3))/x^2',
        '(3-2*atan(3/2)-3*ln(13/4)/2)/2-sqrt(3)*(3*atan(3/2)-ln(13/4))/2',('0','+infinity'),
        intent='near-miss: phase needs both constant and linear cancellation',removable_zero='-9*sqrt(3)/4',
        stable_f='exp(-2*x)*((sin(3*x)-3*x)/2-sqrt(3)*sin(3*x/2)^2)/x^2')
    add('inverse-gaussian-weight','x^(-3/2)*exp(-2*x-3/x)','sqrt(pi/3)*exp(-2*sqrt(6))',('0','+infinity'),domain='x>0; exponential suppression at both ends')
    add('inverse-gaussian-weight','sqrt(x)*exp(-2*x-3/x)','sqrt(pi)*exp(-2*sqrt(6))*(1/(4*sqrt(2))+sqrt(3)/2)',('0','+infinity'),domain='x>0')
    add('inverse-gaussian-quadratic','x^2*exp(-3*x^2-2/x^2)','sqrt(pi)*exp(-2*sqrt(6))*(1/(12*sqrt(3))+sqrt(2)/6)',('0','+infinity'),domain='x>0')

    add('quartic-beta-radical','1/sqrt(1+x^4)','Gamma(1/4)^2/(4*sqrt(pi))',('0','+infinity'),answer_class='special-function-closed-form')
    add('quartic-beta-radical','1/sqrt(1-x^4)','Gamma(1/4)*sqrt(pi)/(4*Gamma(3/4))',('0','1'),answer_class='special-function-closed-form',domain='0<=x<1; integrable radical endpoint')
    add('logarithmic-zeta-kernel','ln(1+x)/x','pi^2/12',('0','1'),removable_zero='1')
    add('logarithmic-zeta-kernel','ln(x)*ln(1-x)/x','Zeta(3)',('0','1'),answer_class='special-function-closed-form',domain='0<x<1')
    add('hyperbolic-log-moment','ln(cosh(x))/cosh(x)','pi*ln(2)',('-infinity','+infinity'))
    add('inverse-trig-log-moment','atan(x)/(x*(1+x^2))','pi*ln(2)/2',('0','+infinity'),removable_zero='1')

    add('composed-parts','x*asinh(x)/sqrt(1+x^2)','sqrt(1+x^2)*asinh(x)-x')
    add('composed-parts','ln(1+x^2)/x^2','-ln(1+x^2)/x+2*atan(x)',samples=['-2','1/3','3'],domain='real x!=0; removable integrand at zero')
    add('root-rational-primitive','1/(sqrt(x)+x)','2*ln(1+sqrt(x))',samples=['1/4','1','4'],domain='x>0')
    add('root-inverse-trig-primitive','atan(sqrt(x))/sqrt(x)','2*sqrt(x)*atan(sqrt(x))-ln(1+x)',samples=['1/4','1','4'],domain='x>0')
    add('exterior-radical-primitive','1/(x*sqrt(x^2-1))','atan(sqrt(x^2-1))',samples=['-3','-2','2','3'],domain='x<-1 or x>1; separate connected components')

    add('stress-beta-polynomial','2^512*x^255*(1-x)^255','2^512*factorial(255)^2/factorial(511)',('0','1'),
        intent='stress',scale={'endpoint_degree':255,'expanded_degree':510},domain='0<=x<=1')
    add('stress-binomial-substitution','2^128*x^63*(1-x^2)^127','2^128*factorial(31)*factorial(127)/(2*factorial(159))',('0','1'),
        intent='stress',scale={'q':32,'outer_power':127,'expanded_degree':317},domain='0<=x<=1')
    add('stress-beta-logit','ln(x/(1-x))^16/sqrt(x*(1-x))','19391512145*pi^17',('0','1'),
        intent='stress',scale={'logit_moment':16},domain='0<x<1; integrable endpoint singularities',
        transformed_f='x^16/(2*cosh(x/2))',transformed_bounds=['-infinity','+infinity'],substitution='original x=1/(1+exp(-t)); verification variable x represents t')
    add('stress-fermi-moment','x^24/(exp(2*x)+1)','(1-2^(-24))*factorial(24)*Zeta(25)/2^25',('0','+infinity'),
        intent='stress',scale={'moment_degree':24,'exponential_scale':2},answer_class='special-function-closed-form')
    add('stress-harmonic-cancellation','(sin(31*x)-31*sin(x))/x^3','-465*pi/2',('0','+infinity'),
        intent='stress',scale={'largest_frequency':31,'cancellation_order':3},domain='removable at zero; absolutely convergent tail',
        method='quadosc',omega='1',removable_zero='-4960')

    assert len(cases)==30 and len({c['id'] for c in cases})==30
    return {'sources':{'derivations':'docs/GENERALIZATION-CYCLE3.md','generator':'tests/generate-coverage-cycle3.py'},
            'notes':'Fixed before engine evaluation after checkpoint 895c848. Every reference has a known closed form; no divergent principal-value cases. Stress labels describe input scale, not demonstrated resource exhaustion.',
            'summary':{'cases':len(cases),'definite':sum('bounds' in c for c in cases),'indefinite':sum('bounds' not in c for c in cases),
                       'by_family':dict(sorted(Counter(c['family'] for c in cases).items())),
                       'by_intent':dict(sorted(Counter(c['intent'] for c in cases).items())),
                       'by_answer_class':dict(sorted(Counter(c['answer_class'] for c in cases).items()))},
            'cases':cases}


def verify(data):
    import mpmath as mp
    import sympy as sp
    mp.mp.dps=80
    x=sp.Symbol('x',real=True)
    names=dict(x=x,ln=sp.log,infinity=sp.oo,Gamma=sp.gamma,Zeta=sp.zeta,factorial=sp.factorial)
    def fn(s):return sp.lambdify(x,sp.sympify(s.replace('^','**'),locals=names),'mpmath')
    def number(s):return fn(s)(mp.mpf(0))
    counts=Counter();max_error=mp.mpf(0);worst=None
    for c in data['cases']:
        opts=c.get('verification',{});f=fn(opts.get('stable_f',c['f']));reference=fn(c['expected'])
        if 'removable_zero' in opts:
            raw=f;zero=number(opts['removable_zero']);f=lambda t:zero if t==0 else raw(t)
        if 'bounds' not in c:
            error=max(abs(mp.diff(reference,number(s))-f(number(s)))/max(1,abs(f(number(s)))) for s in c['samples'])
            method='sampled-derivative'
        else:
            lo,hi=map(number,opts.get('transformed_bounds',c['bounds']))
            if 'transformed_f' in opts:f=fn(opts['transformed_f'])
            if opts.get('method')=='quadosc':
                value=mp.quadosc(f,[lo,hi],omega=number(opts['omega']));method='oscillatory-quadrature'
            else:
                points=([lo,-64,-16,-4,0,4,16,64,hi] if lo==-mp.inf else
                        [lo,1,4,16,64,hi] if hi==mp.inf else [lo,mp.mpf('0.25'),mp.mpf('0.5'),mp.mpf('0.75'),hi])
                value=mp.quad(f,points);method='transformed-quadrature' if 'transformed_f' in opts else 'quadrature'
            target=reference(mp.mpf(0));error=abs(value-target)/max(1,abs(target))
        if not mp.isfinite(error) or error>mp.mpf('1e-20'):
            raise AssertionError(f'{c["id"]}: {method} scaled error {error}')
        if error>max_error:max_error=error;worst=c['id']
        counts[method]+=1
        print(f'VERIFY {c["id"]} {method} error={mp.nstr(error,8)}',flush=True)
    print(json.dumps({'checks':dict(counts),'max_scaled_error':mp.nstr(max_error,10),'worst':worst}))


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check',action='store_true');parser.add_argument('--verify',action='store_true')
    args=parser.parse_args();data=build_corpus();payload=json.dumps(data,indent=2)+'\n'
    if args.check:
        if OUTPUT.read_text()!=payload:raise SystemExit('Regenerate cycle-3 corpus')
    else:OUTPUT.write_text(payload)
    print(json.dumps(data['summary'],sort_keys=True),flush=True)
    if args.verify:verify(data)


if __name__=='__main__':main()
