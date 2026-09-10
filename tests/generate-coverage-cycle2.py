#!/usr/bin/env python3
"""Generate cycle-2 coverage independently of Giac; --verify audits references."""
import argparse
from collections import Counter
from fractions import Fraction as Q
import json
from math import comb, factorial
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / 'tests/generalization-cycle2.json'


def build_corpus():
    cases=[]
    def add(family,f,expected,bounds=None,samples=None,intent='new-family',
            domain='real integrand on the stated interval',answer_class='elementary-closed-form',**verification):
        row={'id':f'C2-{family}-{1+sum(c["family"]==family for c in cases):02d}',
             'family':family,'intent':intent,'domain':domain,'answer_class':answer_class,
             'f':f,'expected':expected}
        if bounds is not None:row['bounds']=list(bounds)
        else:row['samples']=samples or ['-3/2','1/3','5/2']
        if verification:row['verification']=verification
        cases.append(row)

    # Complete the square, then use centered normal moments. No engine oracle.
    for a,b,u,v,n in [(1,2,1,-2,0),(2,3,-1,2,1),(Q(1,2),Q(3,2),2,-1,6),(1,1,0,1,12)]:
        s=a+b;mu=Q(a*u+b*v,s);d=Q(a*b*(u-v)**2,s)
        moment=sum(Q(comb(n,2*k)*factorial(2*k),4**k*factorial(k))*mu**(n-2*k)/s**k
                   for k in range(n//2+1))
        add('gaussian-convolution',f'x^{n}*exp(-({a})*(x-({u}))^2-({b})*(x-({v}))^2)',
            f'({moment})*sqrt(pi/({s}))*exp(-({d}))',('-infinity','+infinity'),
            intent='new-family: convolution and noncentral high moments')
    for a,b,c in [(1,2,3),(2,3,-1),(Q(1,2),Q(3,2),2)]:
        value=Q(a+b,a*b*((a+b)**2+c*c))
        add('cauchy-convolution',f'1/((x^2+({a})^2)*((x-({c}))^2+({b})^2))',
            f'({value})*pi',('-infinity','+infinity'))
    for a,b,c in [(1,2,3),(3,1,-2)]:
        value=Q(c,b*((a+b)**2+c*c))
        add('cauchy-convolution',f'x/((x^2+{a*a})*((x-({c}))^2+{b*b}))',
            f'({value})*pi',('-infinity','+infinity'),intent='near-miss: odd numerator loses parity after shift')

    for p,q in [(Q(1,2),2),(Q(2,3),Q(5,3)),(Q(5,4),3),(2,5)]:
        add('mellin-rational',f'x^({p}-1)/(1+x^({q}))',f'pi/(({q})*sin(pi*({p})/({q})))',
            ('0','+infinity'),domain='x>0; 0<p<q; principal powers are positive real')
    for p,q,degree in [(1,3,1),(2,3,1),(Q(1,2),2,1),(1,4,2)]:
        t=f'(pi*({p})/({q}))'
        value=(f'-pi^2/({q})^2*cos({t})/sin({t})^2' if degree==1 else
               f'pi^3/({q})^3*(1+2*(cos({t})/sin({t}))^2)/sin({t})')
        add('mellin-log-weight',f'x^({p}-1)*ln(x)^{degree}/(1+x^({q}))',value,
            ('0','+infinity'),intent='near-miss: Mellin logarithmic weight changes value',domain='x>0; 0<p<q')
    for a,b in [(Q(1,3),Q(2,3)),(Q(2,3),Q(4,3)),(Q(3,4),Q(1,4)),(Q(3,2),Q(5,3))]:
        add('fractional-beta',f'x^({a}-1)*(1-x)^({b}-1)',
            f'Gamma({a})*Gamma({b})/Gamma({a+b})',('0','1'),
            domain='0<x<1; integrable endpoint singularities',answer_class='special-function-closed-form')
    for f,value in [('ln(x)','-2*pi*ln(2)'),('ln(x)^2','pi*(4*ln(2)^2+pi^2/3)'),
                    ('ln(x)*ln(1-x)','pi*(4*ln(2)^2-pi^2/6)')]:
        add('beta-log-weight',f'({f})/sqrt(x*(1-x))',value,('0','1'),
            domain='0<x<1; logarithmic endpoint singularities')

    for n,a,fermi in [(2,2,False),(4,Q(1,2),False),(9,3,False),
                      (3,2,True),(6,Q(3,2),True),(11,1,True)]:
        c=Q(factorial(n))*(1-Q(1,2**n) if fermi else 1)/a**(n+1)
        add('fermi-moment' if fermi else 'bose-moment',f'x^{n}/(exp(({a})*x){"+1" if fermi else "-1"})',
            f'({c})*Zeta({n+1})',('0','+infinity'),answer_class='special-function-closed-form',
            intent='parameter-variant: scaled high moment',removable_zero='0' if not fermi else None)
    for n,a,bernoulli in [(12,Q(3,2),Q(691,2730)),(14,2,Q(7,6))]:
        c=(2**n-2)*bernoulli/a**(n+1)
        add('logistic-high-moment',f'x^{n}/(exp(({a})*x)+exp(-({a})*x)+2)',
            f'({c})*pi^{n}',('-infinity','+infinity'),intent='parameter-variant: higher dispatch degree')
    for n,a,shift in [(4,2,3),(6,Q(1,2),-2)]:
        moments={0:'1',2:'pi^2/3',4:'7*pi^4/15',6:'31*pi^6/21'}
        value='+'.join(f'({Q(comb(n,k))*Q(shift)**(n-k)/a**(k+1)})*({moments[k]})'
                       for k in range(0,n+1,2))
        add('logistic-shifted-moment',f'x^{n}/(exp(({a})*(x-({shift})))+exp(-({a})*(x-({shift})))+2)',
            value,('-infinity','+infinity'),intent='near-miss: shift destroys centered moment')

    for a,b,phase in [(2,3,'pi/3'),(-3,2,'pi/4'),(Q(1,2),1,'-pi/6')]:
        add('phase-laplace',f'exp(-{b}*x)*(sin(({a})*x+({phase}))-sin({phase}))/x',
            f'cos({phase})*atan(({a})/{b})-sin({phase})*ln(1+(({a})/{b})^2)/2',
            ('0','+infinity'),intent='near-miss: phase requires cancellation at zero',
            removable_zero=f'({a})*cos({phase})')
    for a,c,b in [(1,3,2),(Q(1,2),2,3)]:
        add('cosine-frullani',f'exp(-{b}*x)*(cos(({a})*x)-cos(({c})*x))/x',
            f'ln(({b*b}+({c})^2)/({b*b}+({a})^2))/2',('0','+infinity'),removable_zero='0')
    for a,b in [(1,2),(3,1)]:
        add('conditional-cosine-difference',f'(cos({a}*x)-cos({b}*x))/x',f'ln({b}/{a})',
            ('0','+infinity'),domain='removable at zero; ordinary conditionally convergent integral',
            method='quadosc',omega='1',removable_zero='0')
    for a,b in [(1,3),(2,5)]:
        add('cross-sinc',f'sin({a}*x)*sin({b}*x)/x^2',f'{min(a,b)}*pi/2',
            ('0','+infinity'),method='quadosc',omega='1',removable_zero=str(a*b))
    for a,b,shift in [(1,2,0),(2,3,0),(2,1,3),(Q(1,2),2,-1)]:
        add('cauchy-fourier',f'cos({b}*x)/((x-({shift}))^2+({a})^2)',
            f'pi/({a})*exp(-({a})*{b})*cos({b}*({shift}))',('-infinity','+infinity'),
            intent='new-family: Fourier transform and translation',method='quadosc',omega=str(b))

    indefinite=[
        ('rational-power','x^(-2/3)/(1+x^(1/3))','3*ln(1+x^(1/3))',['1/8','1','8'],'x>0'),
        ('real-root-rational','1/(surd(x,3)^2*(1+surd(x,3))^2)','-3/(1+surd(x,3))',['-8','-1/8','1/8','8'],'real x excluding -1 and 0'),
        ('rational-power','x^(-1/3)*sqrt(2+3*x^(2/3))','(2+3*x^(2/3))^(3/2)/3',['1/8','1','8'],'x>0'),
        ('rational-power','x^(-1/2)/(1+sqrt(x))^2','-2/(1+sqrt(x))',['1/4','1','4'],'x>0'),
        ('shifted-gaussian-primitive','exp(-2*x^2+3*x)','sqrt(pi)*exp(9/8)*erf(sqrt(2)*x-3/(2*sqrt(2)))/(2*sqrt(2))',None,'real line'),
        ('hyperbolic-parts','asinh(x)','x*asinh(x)-sqrt(1+x^2)',None,'real line'),
        ('exponential-trig','exp(x)*cos(2*x)','exp(x)*(cos(2*x)+2*sin(2*x))/5',None,'real line'),
        ('nested-log','1/(x*ln(x)*ln(ln(x)))','ln(abs(ln(ln(x))))',['exp(2)','exp(3)','exp(4)'],'x>exp(1)'),
        ('nested-log','1/(x*sqrt(ln(x)))','2*sqrt(ln(x))',['2','3','5'],'x>1'),
        ('fractional-log-parts','sqrt(x)*ln(x)','x^(3/2)*(2*ln(x)/3-4/9)',['1/4','1','4'],'x>0'),
        ('fractional-log-parts','ln(x)/x^(3/2)','-(2*ln(x)+4)/sqrt(x)',['1/4','1','4'],'x>0'),
        ('absolute-exponential','abs(x-2)*exp(abs(x-2))','sign(x-2)*((abs(x-2)-1)*exp(abs(x-2))+1)',['-1','1','3','5'],'real line; continuous primitive at x=2'),
    ]
    for family,f,value,samples,domain in indefinite:
        add(family,f,value,samples=samples,domain=domain,
            answer_class='special-function-closed-form' if family=='shifted-gaussian-primitive' else 'elementary-closed-form')
    for a,b in [(1,1),(2,3)]:
        add('inverse-gaussian',f'exp(-{a}*x-{b}/x)/sqrt(x)',
            f'sqrt(pi/{a})*exp(-2*sqrt({a*b}))',('0','+infinity'),
            domain='x>0; exponential suppression at both endpoints')

    assert len(cases)==61
    assert len({c['id'] for c in cases})==len(cases)
    return {'sources':{'derivations':'docs/GENERALIZATION-CYCLE2.md','generator':'tests/generate-coverage-cycle2.py'},
            'notes':'Independent second-cycle examples. All have known closed-form references; no numerical-only or unknown-closed-form cases are silently assigned an exact reference. Principal powers and real surd remain distinct. No principal-value-only integrals.',
            'summary':{'cases':len(cases),'indefinite':sum('bounds' not in c for c in cases),
                       'definite':sum('bounds' in c for c in cases),
                       'family_labels':len({c['family'] for c in cases}),
                       'by_family':dict(sorted(Counter(c['family'] for c in cases).items())),
                       'by_answer_class':dict(sorted(Counter(c['answer_class'] for c in cases).items())),
                       'by_intent':dict(sorted(Counter(c['intent'] for c in cases).items()))},
            'cases':cases}


def verify(data):
    import mpmath as mp
    import sympy as sp
    mp.mp.dps=80
    x=sp.Symbol('x',real=True)
    names=dict(x=x,ln=sp.log,abs=sp.Abs,sign=sp.sign,surd=sp.Function('surd'),
               infinity=sp.oo,Gamma=sp.gamma,Zeta=sp.zeta)
    def fn(s):
        e=sp.sympify(s.replace('^','**'),locals=names)
        return sp.lambdify(x,e,[{'surd':lambda t,n:mp.sign(t)*abs(t)**(mp.mpf(1)/n)},'mpmath'])
    def number(s):return fn(s)(mp.mpf(0))
    counts=Counter();max_error=mp.mpf(0);worst=None
    for c in data['cases']:
        f=fn(c['f']);reference=fn(c['expected']);opts=c.get('verification',{})
        if opts.get('removable_zero') is not None:
            raw=f;zero=number(opts['removable_zero']);f=lambda t:zero if t==0 else raw(t)
        if 'bounds' not in c:
            error=max(abs(mp.diff(reference,number(s))-f(number(s)))/max(1,abs(f(number(s))))
                      for s in c['samples'])
            method='sampled-derivative'
        else:
            lo,hi=map(number,c['bounds'])
            if opts.get('method')=='quadosc':
                omega=number(opts['omega'])
                value=(mp.quadosc(f,[lo,0],omega=omega)+mp.quadosc(f,[0,hi],omega=omega)
                       if lo==-mp.inf else mp.quadosc(f,[lo,hi],omega=omega))
                method='oscillatory-quadrature'
            else:
                points=([lo,-10,-3,0,3,10,hi] if lo==-mp.inf else
                        [lo,1,4,16,hi] if hi==mp.inf else [lo,mp.mpf('0.5'),hi])
                value=mp.quad(f,points)
                method='quadrature'
            target=reference(mp.mpf(0));error=abs(value-target)/max(1,abs(target))
        if not mp.isfinite(error) or error>mp.mpf('1e-20'):
            raise AssertionError(f'{c["id"]}: {method} scaled error {error}')
        if error>max_error:max_error=error;worst=c['id']
        counts[method]+=1
        print(f'VERIFY {c["id"]} {method} error={mp.nstr(error,6)}',flush=True)
    print(json.dumps({'checks':dict(counts),'max_scaled_error':mp.nstr(max_error,8),'worst':worst}))


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check',action='store_true')
    parser.add_argument('--verify',action='store_true')
    args=parser.parse_args();data=build_corpus();payload=json.dumps(data,indent=2)+'\n'
    if args.check:
        if OUTPUT.read_text()!=payload:raise SystemExit('Regenerate cycle-2 corpus')
    else:OUTPUT.write_text(payload)
    print(json.dumps(data['summary'],sort_keys=True),flush=True)
    if args.verify:verify(data)


if __name__=='__main__':main()
