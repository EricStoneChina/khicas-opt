#!/usr/bin/env python3
"""Generate an integration coverage corpus from explicit mathematical identities.

Generation uses only the Python standard library. --verify additionally requires
SymPy and mpmath; it checks references independently of the KhiCAS engine.
"""
import argparse
from collections import Counter
from fractions import Fraction as Q
import json
from math import comb, factorial
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / 'tests/generalization-corpus.json'


def rat(value):
    return str(Q(value))


def build_corpus():
    cases = []

    def add(family, f, expected, *, bounds=None, samples=None,
            intent='coverage', domain='real line', **verification):
        index = 1 + sum(c['family'] == family for c in cases)
        row = dict(id=f'GEN-{family}-{index:02d}', family=family,
                   intent=intent, domain=domain, f=f, expected=expected)
        if bounds is not None:
            row['bounds'] = list(bounds)
        else:
            row['samples'] = samples or ['-7/5', '1/5', '11/5']
        if verification:
            row['verification'] = verification
        cases.append(row)

    # u=a+b*x^k, with an explicit binomial expansion of (u-a)^(q-1).
    for k, q, a, b, n in [
        (2, 1, 2, 3, 7), (3, 2, 1, 2, 9), (2, 3, 3, 1, 4),
        (4, 4, 2, 1, 3), (2, 5, 1, 1, 2), (2, 6, 2, 1, 3),
        (2, 7, 1, 2, 2), (2, 8, 3, 1, 1), (5, 1, 3, -1, 11),
        (3, 3, -2, 3, 5), (2, 2, 1, 3, Q(1, 2)),
        (3, 2, 2, 1, Q(2, 3)), (4, 1, 1, 2, Q(3, 2)),
        (2, 1, 1, 1, 257), (3, 2, 1, -1, 4096),
    ]:
        u = f'({a}+({b})*x^{k})'
        expected = '+'.join(
            f'({rat(Q(comb(q-1,j)*(-a)**(q-1-j), k*b**q)/(n+j+1))})*{u}^({rat(n+j+1)})'
            for j in range(q))
        add('binomial-chain', f'x^{k*q-1}*{u}^({rat(n)})', expected,
            samples=['1/10', '1/3', '3/5'], domain='0<x<1; fractional bases positive',
            intent='parameter-variant')
    for k, q, n, lo, hi in [(3, 2, 17, '0', '1'), (2, 4, 9, '1', '0'),
                             (5, 1, 23, '-1', '0')]:
        # Expand the original integrand, a different representation of its primitive.
        ev = lambda t: sum(Q(comb(n,j)*(-1)**j,k*q+k*j)*Q(t)**(k*q+k*j)
                           for j in range(n+1))
        add('binomial-chain', f'x^{k*q-1}*(1-x^{k})^{n}', rat(ev(hi)-ev(lo)),
            bounds=(lo,hi), intent='parameter-variant')

    # Near misses: q nonintegral / >8, negative n, and extra polynomial terms.
    for m,k,n in [(2,2,5), (4,3,4), (17,2,3), (26,3,2)]:
        expected = '+'.join(f'({rat(Q(comb(n,j)*2**j,m+k*j+1))})*x^{m+k*j+1}'
                            for j in range(n+1))
        add('polynomial-expansion', f'x^{m}*(1+2*x^{k})^{n}', expected,
            intent='near-miss: binomial q outside dispatch')
    for f,p in [('x/(1+x^2)', 'ln(1+x^2)/2'),
                ('x^3/(1+x^2)', '(x^2-ln(1+x^2))/2'),
                ('x/(1+x^2)^2', '-1/(2*(1+x^2))'),
                ('x*(1+x^2+x^4)^2', 'x^2/2+x^4/2+x^6/2+x^8/4+x^10/10')]:
        add('binomial-neighbor', f,p,intent='near-miss: negative exponent or third term')

    for d,L,c in [(1,0,1),(2,1,3),(3,6,1),(2,7,-2),(1,-1,2),(4,12,1)]:
        m=2*d-L
        u=f'x/(x^2+{d})'
        p=(f'-({c})*asin(sqrt({m})*{u})/sqrt({m})' if m>0 else
           f'-({c})*asinh(sqrt({-m})*{u})/sqrt({-m})' if m<0 else f'-({c})*{u}')
        add('reciprocal-quartic',f'({c})*(x^2-{d})/((x^2+{d})*sqrt(x^4+({L})*x^2+{d*d}))',
            p, samples=['-3','-1/3','1/2','4'],intent='parameter-variant')
    # A changed numerator destroys the reciprocal-quartic substitution; L=2d
    # makes these counterexamples elementary and gives an independent reference.
    for d,c in [(1,1),(2,3),(3,-2)]:
        add('quartic-neighbor',f'(x^2+({c}))/((x^2+{d})*sqrt(x^4+{2*d}*x^2+{d*d}))',
            f'({rat(Q(d+c,2*d))})*atan(x/sqrt({d}))/sqrt({d})+({rat(Q(c-d,2*d))})*x/(x^2+{d})',
            intent='near-miss: numerator identity fails')
    for d,c in [(1,2),(3,1)]:
        add('quartic-neighbor',f'(x^2-{d})/((x^2+{d})*sqrt(x^4+{2*c}*x^2+{c*c}))',
            f'({rat(Q(-2*d,c-d))})*atan(x/sqrt({d}))/sqrt({d})+'
            f'({rat(Q(c+d,c-d))})*atan(x/sqrt({c}))/sqrt({c})',
            intent='near-miss: radical constant identity fails')
    add('quartic-neighbor','(x^2+1)/((x^2-1)*sqrt(x^4-2*x^2+1))',
        '-x/(x^2-1)',samples=['3/2','2','4'],domain='x>1',
        intent='near-miss: d negative, local real domain')

    for a,b in [(1,0),(2,1),(-3,2),(Q(1,2),-1),(Q(-2,3),Q(1,3))]:
        u=f'(({rat(a)})*x+({rat(b)}))'
        p=f'(sqrt(2)*{u}-atan(sin(4*{u})/(3+2*sqrt(2)+cos(4*{u})))/sqrt(2))/({rat(a)})'
        add('quartic-trig',f'1/(sin({u})^4+cos({u})^4)',p,
            samples=['-7','-pi/2','0','pi/2','8'],intent='parameter-variant')
    for a,b,lo,hi in [(1,0,'-pi','pi'),(2,1,'0','pi'),(-2,0,'pi','0')]:
        u=f'({a}*x+{b})'
        add('quartic-trig',f'1/(sin({u})^4+cos({u})^4)',
            f'sqrt(2)*(({hi})-({lo}))',bounds=(lo,hi),intent='parameter-variant')
    for f,p in [('1/(sin(x)^2+cos(x)^2)','x'),
                ('1/(sin(x)^4-cos(x)^4)','-ln(abs(1/cos(2*x)+tan(2*x)))/2'),
                ('1/(sin(x)^4+cos(x)^4+2*sin(x)^2*cos(x)^2)','x')]:
        add('trig-identity-neighbor',f,p,samples=['-1/5','1/10','3/10'],
            domain='-pi/4<x<pi/4',intent='near-miss: denominator changes')

    for n,a,lo,hi in [(1,1,'0','+infinity'),(3,2,'0','+infinity'),
                      (5,-3,'0','+infinity'),(7,Q(1,2),'-infinity','0'),
                      (9,2,'-infinity','+infinity'),(11,-1,'+infinity','0'),
                      (15,3,'0','+infinity'),(31,1,'0','+infinity'),
                      (33,2,'0','+infinity')]:
        halves=2 if lo=='-infinity' and hi=='+infinity' else 1
        orientation=-1 if lo=='+infinity' else 1
        value=Q((1 if a>0 else -1)*halves*orientation*comb(n-1,(n-1)//2),2**n)
        add('sine-dirichlet',f'sin(({rat(a)})*x)^{n}/x',f'({rat(value)})*pi',
            bounds=(lo,hi),intent='near-miss: odd power exceeds dispatch' if n==33 else 'parameter-variant',
            domain='removable at zero; conditionally convergent at infinity',
            method='quadosc',omega=rat(abs(a)),removable_zero=rat(a) if n==1 else '0')
    for a in [1,2,3]:
        add('squared-sinc',f'sin({a}*x)^2/x^2',f'{a}*pi/2',bounds=('0','+infinity'),
            intent='near-miss: even power requires different decay',method='quadosc',omega=str(a),
            removable_zero=str(a*a))
    for a,b in [(1,2),(3,1)]:
        add('damped-dirichlet',f'exp(-{b}*x)*sin({a}*x)/x',f'atan({a}/{b})',
            bounds=('0','+infinity'),intent='near-miss: nonconstant multiplier',removable_zero=str(a))
    add('shifted-dirichlet','sin(x+pi/2)/x','-Ci(1)',bounds=('1','+infinity'),
        intent='near-miss: phase and lower bound change value',method='quadosc',omega='1',
        domain='x>=1; conditionally convergent at infinity')

    moments={0:'1',2:'pi^2/3',4:'7*pi^4/15',6:'31*pi^6/21',8:'127*pi^8/15',10:'2555*pi^10/33'}
    for n,a in [(0,1),(2,2),(4,3),(6,Q(1,2)),(8,-2),(1,3),(3,-1),(7,2),(10,1)]:
        add('logistic-moment',f'x^{n}/(exp(({rat(a)})*x)+exp(-({rat(a)})*x)+2)',
            '0' if n%2 else f'({moments[n]})/({rat(abs(a))})^{n+1}',
            bounds=('-infinity','+infinity'),
            intent='near-miss: degree exceeds dispatch' if n==10 else 'parameter-variant')
    add('logistic-moment','x^2/(exp(2*x)+exp(-2*x)+2)','-pi^2/24',
        bounds=('+infinity','-infinity'),intent='parameter-variant')
    for b in [Q(1,2),2]:
        add('logistic-shift',f'x^2/(exp(x-({rat(b)}))+exp(-x+({rat(b)}))+2)',
            f'pi^2/3+({rat(b)})^2',bounds=('-infinity','+infinity'),
            intent='near-miss: exponential phases change moment')
    add('logistic-neighbor','1/(exp(x)+exp(-x))','pi/2',
        bounds=('-infinity','+infinity'),intent='near-miss: denominator constant removed')

    for a,b,p,q,A,B,reverse in [(1,0,1,0,1,1,False),(2,1,3,2,2,3,False),
                               (-2,1,1,-1,3,2,False),(3,-2,-1,4,1,4,True),
                               (Q(1,2),0,2,1,4,1,False)]:
        lo=f'-({b})/({rat(a)})'; hi=f'(pi-({b}))/({rat(a)})'
        # Bounds here follow the argument 0 -> pi, so the signed a is essential.
        value=f'(({p})*((({lo})+({hi}))/2)+({q}))*2*atan(sqrt({B}/{A}))/(({rat(a)})*sqrt({A*B}))'
        if reverse: lo,hi=hi,lo;value=f'-({value})'
        u=f'(({rat(a)})*x+({b}))'
        add('weighted-reflection',f'(({p})*x+({q}))*sin({u})/({A}+{B}*cos({u})^2)',
            value,bounds=(lo,hi),intent='parameter-variant')
    add('weighted-reflection-neighbor','x^2*sin(x)','pi^2-4',bounds=('0','pi'),
        intent='near-miss: nonlinear weight')
    add('weighted-reflection-neighbor','x*sin(x)/(2-cos(x)^2)',
        'pi*atanh(1/sqrt(2))/sqrt(2)',bounds=('0','pi'),
        intent='near-miss: negative cosine-square coefficient')
    add('weighted-reflection-neighbor','x*sin(x)','1',bounds=('0','pi/2'),
        intent='near-miss: interval lacks reflection symmetry')

    for n,a in [(0,1),(1,2),(2,3),(3,Q(1,2))]:
        coefficient=Q(factorial(2*n),4**n*factorial(n))
        add('gaussian-moment',f'x^{2*n}*exp(-({rat(a)})*x^2)',
            f'({rat(coefficient)})*sqrt(pi)/({rat(a)})^({rat(Q(2*n+1,2))})',
            bounds=('-infinity','+infinity'),intent='parameter-variant')
    add('gaussian-shift','x^2*exp(-2*(x-3)^2)','(37/4)*sqrt(pi/2)',bounds=('-infinity','+infinity'))
    add('gaussian-fourier','exp(-x^2)*cos(3*x)','sqrt(pi)*exp(-9/4)',bounds=('-infinity','+infinity'))
    add('gaussian-neighbor','exp(-x^4)','Gamma(1/4)/2',bounds=('-infinity','+infinity'),
        intent='near-miss: quartic exponential')

    # Independent textbook families and compositions, not parameter repeats.
    primitives = [
        ('parts-exponential','x*exp(2*x)','exp(2*x)*(2*x-1)/4'),
        ('parts-exponential','x^2*exp(-x)','-exp(-x)*(x^2+2*x+2)'),
        ('parts-trigonometric','x*cos(3*x)','x*sin(3*x)/3+cos(3*x)/9'),
        ('parts-trigonometric','x^2*sin(x)','-x^2*cos(x)+2*x*sin(x)+2*cos(x)'),
        ('parts-logarithmic','ln(x)','x*ln(x)-x'),
        ('parts-logarithmic','x*ln(x)^2','x^2*(ln(x)^2-ln(x)+1/2)/2'),
        ('inverse-function-parts','atan(x)','x*atan(x)-ln(1+x^2)/2'),
        ('inverse-function-parts','asin(x)','x*asin(x)+sqrt(1-x^2)'),
        ('rational-simple-poles','1/((x+1)*(x+3))','(ln(abs(x+1))-ln(abs(x+3)))/2'),
        ('rational-repeated-pole','1/(x+2)^3','-1/(2*(x+2)^2)'),
        ('rational-irreducible','1/(x^2+4)','atan(x/2)/2'),
        ('rational-irreducible','1/(x^2+1)^2','x/(2*(x^2+1))+atan(x)/2'),
        ('rational-division','x^4/(1+x^2)','x^3/3-x+atan(x)'),
        ('rational-log-derivative','(3*x^2+2)/(x^3+2*x+5)','ln(abs(x^3+2*x+5))'),
        ('trig-substitution','sqrt(4-x^2)','x*sqrt(4-x^2)/2+2*asin(x/2)'),
        ('trig-substitution','1/sqrt(9-x^2)','asin(x/3)'),
        ('hyperbolic-substitution','sqrt(x^2+4)','x*sqrt(x^2+4)/2+2*asinh(x/2)'),
        ('hyperbolic-substitution','1/sqrt(x^2+9)','asinh(x/3)'),
        ('radical-logarithm','1/sqrt(x^2-1)','ln(x+sqrt(x^2-1))'),
        ('exponential-substitution','exp(x)/(1+exp(2*x))','atan(exp(x))'),
        ('exponential-substitution','exp(2*x)/(1+exp(x))','exp(x)-ln(1+exp(x))'),
        ('logarithmic-substitution','ln(x)^3/x','ln(x)^4/4'),
        ('logarithmic-substitution','1/(x*(1+ln(x)^2))','atan(ln(x))'),
        ('trig-product','sin(2*x)*cos(3*x)','-cos(5*x)/10+cos(x)/2'),
        ('trig-power','sin(x)^3','-cos(x)+cos(x)^3/3'),
        ('trig-power','cos(x)^4','3*x/8+sin(2*x)/4+sin(4*x)/32'),
        ('trig-logarithm','tan(x)','-ln(abs(cos(x)))'),
        ('hyperbolic-product','sinh(x)*cosh(x)^3','cosh(x)^4/4'),
        ('absolute-value','abs(x)','x*abs(x)/2'),
        ('absolute-value','abs(2*x-1)','(2*x-1)*abs(2*x-1)/4'),
        ('real-odd-root','surd(x,3)','3*x*surd(x,3)/4'),
        ('real-odd-root','1/surd(x,3)','3*surd(x,3)^2/2'),
        ('principal-power','x^(1/3)','3*x^(4/3)/4'),
        ('composition','2*x*exp(x^2)/(1+exp(x^2))','ln(1+exp(x^2))'),
        ('composition','cos(x)*ln(2+sin(x))','(2+sin(x))*ln(2+sin(x))-(2+sin(x))'),
        ('composition','x*cos(x^2)*exp(sin(x^2))','exp(sin(x^2))/2'),
        ('special-function','exp(-x^2)','sqrt(pi)*erf(x)/2'),
        ('special-function','sin(x)/x','Si(x)'),
    ]
    for family,f,p in primitives:
        samples=['-7/5','1/5','11/5'];domain='real line away from poles'
        if 'ln(x)' in f:
            samples=['1/4','3/2','3'];domain='x>0'
        if family=='trig-substitution' or f=='asin(x)':
            samples=['-3/5','1/5','4/5'];domain='-1<x<1'
        if family=='radical-logarithm':
            samples=['3/2','2','4'];domain='x>1'
        if family=='trig-logarithm':
            samples=['-1/2','1/5','1'];domain='-pi/2<x<pi/2'
        if family=='principal-power':
            samples=['-3','-1','1/3','2'];domain='principal complex power; real x!=0'
        add(family,f,p,samples=samples,domain=domain)

    for family,f,lo,hi,p,extra in [
        ('endpoint-beta','1/sqrt(x*(1-x))','0','1','pi',{}),
        ('endpoint-beta','sqrt(x)*(1-x)^2','0','1','16/105',{}),
        ('endpoint-log','ln(x)','0','1','-1',{}),
        ('endpoint-log','ln(x)^2','0','1','2',{}),
        ('endpoint-log','ln(x)/sqrt(x)','0','1','-4',{}),
        ('gamma-decay','x^4*exp(-2*x)','0','+infinity','3/4',{}),
        ('gamma-decay','sqrt(x)*exp(-x)','0','+infinity','sqrt(pi)/2',{}),
        ('rational-tail','1/(1+x^2)','0','+infinity','pi/2',{}),
        ('rational-tail','1/(1+x^4)','0','+infinity','pi/(2*sqrt(2))',{}),
        ('rational-tail','x/(1+x^4)','0','+infinity','pi/4',{}),
        ('mellin-log','ln(x)/(1+x^2)','0','+infinity','0',{}),
        ('mellin-log','ln(x)^2/(1+x^2)','0','+infinity','pi^3/8',{}),
        ('absolute-value','abs(x-1)','-2','3','13/2',{'split':['1']}),
        ('absolute-value','abs(x^2-1)','-2','2','4',{'split':['-1','1']}),
        ('absolute-trig','abs(sin(x))','0','2*pi','4',{'split':['pi']}),
        ('absolute-trig','abs(cos(2*x))','-pi','pi','4',{'split':['-3*pi/4','-pi/4','pi/4','3*pi/4']}),
        ('log-trig','ln(sin(x))','0','pi','-pi*ln(2)',{}),
        ('log-trig','ln(cos(x))','0','pi/2','-pi*ln(2)/2',{}),
        ('frullani','(exp(-2*x)-exp(-5*x))/x','0','+infinity','ln(5/2)',{'removable_zero':'3'}),
        ('rational-exponential','1/(exp(x)+1)','0','+infinity','ln(2)',{}),
        ('rational-exponential','x/(exp(x)+1)','0','+infinity','pi^2/12',{}),
        ('root-substitution','surd(x,3)^2','-8','1','99/5',{'split':['0']}),
    ]:
        add(family,f,p,bounds=(lo,hi),domain='ordinary convergent improper integral where needed',**extra)

    # These broader groups are the coverage unit; family labels are diagnostic
    # subdivisions and must not be advertised as independent mathematical ideas.
    groups = {
        'algebraic-substitution': 'binomial-chain binomial-neighbor polynomial-expansion real-odd-root principal-power root-substitution',
        'reciprocal-quartic': 'reciprocal-quartic quartic-neighbor',
        'periodic-trig-reciprocal': 'quartic-trig trig-identity-neighbor',
        'oscillatory-dirichlet': 'sine-dirichlet squared-sinc damped-dirichlet shifted-dirichlet',
        'logistic-density': 'logistic-moment logistic-shift logistic-neighbor',
        'weighted-reflection': 'weighted-reflection weighted-reflection-neighbor',
        'gaussian': 'gaussian-moment gaussian-shift gaussian-fourier gaussian-neighbor',
        'integration-by-parts': 'parts-exponential parts-trigonometric parts-logarithmic inverse-function-parts',
        'rational-functions': 'rational-simple-poles rational-repeated-pole rational-irreducible rational-division rational-log-derivative',
        'quadratic-radicals': 'trig-substitution hyperbolic-substitution radical-logarithm',
        'elementary-composition': 'exponential-substitution logarithmic-substitution composition',
        'trig-hyperbolic-identities': 'trig-product trig-power trig-logarithm hyperbolic-product',
        'absolute-values': 'absolute-value absolute-trig',
        'special-primitives': 'special-function',
        'endpoint-improper': 'endpoint-beta endpoint-log',
        'infinite-decay': 'gamma-decay rational-tail rational-exponential',
        'log-integral-identities': 'mellin-log log-trig frullani',
    }
    group_for = {family:group for group,families in groups.items() for family in families.split()}
    for case in cases:
        case['mathematical_group'] = group_for[case['family']]
    assert len({c['id'] for c in cases}) == len(cases)
    assert all('bounds' in c or len(c['samples']) >= 3 for c in cases)
    return {
        'sources': {'generated-identities':'docs/GENERALIZATION-CORPUS.md',
                    'generator':'tests/generate-coverage.py'},
        'notes': 'Original deterministic identity-based examples, not a contest answer archive. '
                 'Family labels distinguish mathematical structures from parameter variants. '
                 'All expected fields are actual reference values or primitives; unresolved engine output is not success. '
                 'ln is natural logarithm; ^ uses principal powers; surd is a real odd root; infinity has explicit sign. '
                 'Indefinite sampled differentiation does not prove global continuity.',
        'summary': {'cases':len(cases),
                    'definite':sum('bounds' in c for c in cases),
                    'indefinite':sum('bounds' not in c for c in cases),
                    'family_labels':len({c['family'] for c in cases}),
                    'mathematical_groups':len(groups),
                    'by_mathematical_group':dict(sorted(Counter(c['mathematical_group'] for c in cases).items())),
                    'by_family':dict(sorted(Counter(c['family'] for c in cases).items())),
                    'by_intent':dict(sorted(Counter(c['intent'] for c in cases).items()))},
        'cases':cases,
    }


def verify(corpus):
    import mpmath as mp
    import sympy as sp
    mp.mp.dps = 50
    x = sp.Symbol('x', real=True)
    surd = sp.Function('surd')
    names = dict(x=x, ln=sp.log, abs=sp.Abs, surd=surd, infinity=sp.oo,
                 Gamma=sp.gamma, Si=sp.Si, Ci=sp.Ci)
    def parse(s):
        return sp.sympify(s.replace('^','**'),locals=names)
    def fn(s):
        return sp.lambdify(x,parse(s),[{'surd':lambda t,n: mp.sign(t)*abs(t)**(mp.mpf(1)/n)},'mpmath'])
    def number(s):
        return fn(s)(mp.mpf(0))
    checked = Counter()
    for case in corpus['cases']:
        f=fn(case['f']); expected=fn(case['expected'])
        if 'bounds' not in case:
            errors=[]
            for sample in case['samples']:
                t=number(sample); observed=mp.diff(expected,t); reference=f(t)
                errors.append(abs(observed-reference)/max(1,abs(reference)))
            error=max(errors); method='sampled derivative'
        else:
            opts=case.get('verification',{})
            lo,hi=map(number,case['bounds'])
            sign=1
            if lo>hi:lo,hi=hi,lo;sign=-1
            if 'removable_zero' in opts:
                raw=f; zero=number(opts['removable_zero'])
                f=lambda t:zero if t==0 else raw(t)
            if opts.get('method')=='quadosc':
                omega=number(opts['omega'])
                if lo==-mp.inf and hi==mp.inf:
                    observed=mp.quadosc(f,[-mp.inf,0],omega=omega)+mp.quadosc(f,[0,mp.inf],omega=omega)
                else:observed=mp.quadosc(f,[lo,hi],omega=omega)
                method='oscillatory quadrature'
            else:
                points=[lo]+[number(s) for s in opts.get('split',[])]+[hi]
                # Split unbounded intervals to resolve central peaks and tails.
                if lo==-mp.inf and hi==mp.inf:points=[lo,-8,-2,0,2,8,hi]
                elif hi==mp.inf:points=[lo,1,4,16,hi]
                observed=mp.quad(f,points); method='quadrature'
            observed*=sign;reference=expected(mp.mpf(0))
            error=abs(observed-reference)/max(1,abs(reference))
        if not mp.isfinite(error) or error>mp.mpf('1e-20'):
            raise AssertionError(f"{case['id']}: {method} relative/scaled error {error}")
        checked[method]+=1
        print(f"VERIFY {case['id']} {method} error={mp.nstr(error,4)}",flush=True)
    print(json.dumps(dict(checked),sort_keys=True))


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check',action='store_true',help='check checked-in JSON is reproducible; do not write')
    parser.add_argument('--verify',action='store_true',help='independently verify every mathematical reference')
    args=parser.parse_args()
    corpus=build_corpus()
    payload=json.dumps(corpus,indent=2,ensure_ascii=False)+'\n'
    if args.check:
        if OUTPUT.read_text()!=payload:raise SystemExit('Corpus differs; rerun the generator')
    else:
        OUTPUT.write_text(payload)
    print(json.dumps(corpus['summary'],sort_keys=True),flush=True)
    if args.verify:verify(corpus)


if __name__=='__main__':main()
