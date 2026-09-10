# Five user challenge integrals: independent references

The fixed inputs and references are in `tests/user-challenge-integrals.json`.
These are user-supplied problems, not attributed to a competition. The tested
KhiCAS engine was not queried to obtain any reference. Every answer below has
an independent derivation. All five were also checked with mpmath quadrature
at 70 and 100 decimal digits. Host numerical checks are not CG50 measurements.

| Case | Exact result | Numerical value (rounded) |
| --- | --- | --- |
| 01 | `5*pi^2/96` | 0.514041895890070761397629739577 |
| 02 | `pi/2` | 1.570796326794896619231321691640 |
| 03 | `5*pi^2/24` | 2.056167583560283045590518958308 |
| 04 | `pi*ln(pi)/2` | 1.798137499864579099043000875823 |
| 05 | `pi*ln(2*pi)/4 + pi*ln(Gamma(3/4)/Gamma(1/4))/2` | -0.260442806300988445540093868790 |

## 01: inverse tangent and a positive square root

Write `w=sqrt(x^2+2)`. The kernel satisfies

```
d/dx atan(x/w) = 1/((1+x^2)*w),
atan(w) = pi/2-atan(1/w).
```

Therefore the integral is `pi^2/12-J`, where

```
J = integral_[0,1]^2 1/((1+x^2)*(2+x^2+y^2)) dx dy.
```

The identity `atan(1/w)/w=integral_0^1 1/(w^2+y^2) dy` gives this
representation. Interchange `x` and `y` and add the two integrands. Their sum
is `1/((1+x^2)*(1+y^2))`, so `2J=(pi/4)^2`. The answer is `5*pi^2/96`.
Every denominator is positive on the square, so the interchange is ordinary
integration of a continuous function.

## 02: a reciprocal trigonometric function over a complete period

Reflection around `pi` reduces the interval to twice `(0,pi)`. On this
half-period set `t=tan(x/2)`. Then

```
dx/(5-3*cos(x)) = dt/(1+4*t^2).
```

Consequently the full-period integral is
`2*integral_0^infinity dt/(1+4*t^2)=pi/2`. In particular no pole or
principal-value convention is involved: `5-3*cos(x)>=2`.

## 03: principal inverse cosine and weighted disk symmetry

This case has an exact proof, independently supplied by the mathematical
review agent. It is not classified from its numerical proximity to a
multiple of `pi^2`.

Set `t=tan(x/2)` on `0<=x<=pi/2`. All square roots are positive and the
principal-angle identity is

```
acos(cos(x)/(1+2*cos(x))) = 2*atan(1/sqrt(2-t^2)).
```

Thus, for `K=integral_0^1 atan(sqrt(2-t^2))/(1+t^2) dt`, the requested
integral is `pi^2/2-4K`.

Interpret `K` as the integral of the symmetric positive weight
`1/((1+t^2)*(1+y^2))` over the portion of the first-quadrant disk
`t^2+y^2<=2` with `t<=1`. Its reflected portion has `y<=1`. Their union is
the whole quarter-disk, since both coordinates cannot exceed 1 inside it.
Their intersection is the unit square. Therefore `2K=D+(pi/4)^2`, where
`D` is the integral of the weight over the quarter-disk of radius `sqrt(2)`.

For radius `R`, polar coordinates give

```
D(R) = integral_0^R pi*r/((2+r^2)*sqrt(1+r^2)) dr
     = pi*(atan(sqrt(1+R^2))-pi/4).
```

For the angular integral, use
`1/(A*B)=(1/A+1/B)/(2+r^2)`, with
`A=1+r^2*cos(theta)^2`, `B=1+r^2*sin(theta)^2`.
Hence `D(sqrt(2))=pi^2/12`, `K=7*pi^2/96`, and the result is `5*pi^2/24`.

## 04: inverse-tangent parameter difference

For positive `a,b`, differentiate the parameter-dependent difference:

```
d/da integral_0^infinity (atan(a*x)-atan(b*x))/x dx
  = integral_0^infinity 1/(1+a^2*x^2) dx
  = pi/(2*a).
```

Integrating in `a` from `b` yields `(pi/2)*ln(a/b)`. Equivalently, write
the inverse-tangent difference as `integral_b^a x/(1+s^2*x^2) ds` and
interchange the positive integrals when `a>b`; this justifies the argument
without separately integrating either divergent term. Here `a=pi,b=1`.

The original integrand tends to `pi-1` at zero and is `O(x^-2)` at infinity.
For numerical work, the stable difference is
`atan((pi-1)*x/(1+pi*x^2))`. A second check uses `x=exp(t)` and integrates
`atan((pi-1)/(exp(-t)+pi*exp(t)))` over the full real line.

## 05: an iterated logarithm and the Dirichlet beta derivative

The substitution `t=-ln(x)` gives

```
I = integral_0^infinity ln(t)/(2*cosh(t)) dt.
F(s) = integral_0^infinity t^(s-1)/(2*cosh(t)) dt = Gamma(s)*beta(s).
I = F'(1) = beta'(1)-EulerGamma*pi/4.
```

The Mellin identity follows first for `Re(s)>1` by expanding
`1/(2*cosh(t))` into its alternating exponential series, then extends to
`Re(s)>0` by analyticity. Differentiation near `s=1` is justified by the
integrable logarithmic singularity at zero and exponential tail.

For the odd character modulo 4, the completed function
`Lambda(s)=(4/pi)^((s+1)/2)*Gamma((s+1)/2)*beta(s)` satisfies
`Lambda(s)=Lambda(1-s)`. This is the corresponding specialization of the
[NIST DLMF Dirichlet L-function functional equation](https://dlmf.nist.gov/25.15.E5).
Also

```
beta(s) = 4^(-s)*(zeta(s,1/4)-zeta(s,3/4)),
beta(0) = 1/2,
beta'(0) = -ln(2)+ln(Gamma(1/4)/Gamma(3/4)).
```

The last identity uses the
[Hurwitz-zeta derivative at zero](https://dlmf.nist.gov/25.11.E18).
Logarithmically differentiating the completed functional equation gives

```
beta'(1)/(pi/4) = EulerGamma+ln(2*pi)-2*ln(Gamma(1/4)/Gamma(3/4)).
```

Subtracting the Euler-constant term proves the reference in the table.
The gamma arguments and their ratio are positive, so the real logarithm
has no branch ambiguity.

## Reproducing the independent numerical checks

The JSON stores 92 significant decimal digits from the 100-digit pass.
Repeating at 70 digits agrees with those references to at least 61 decimal
places. Across the independently proved formulas, quadrature discrepancies
were below `2e-70` at 70 digits and `2e-100` at 100 digits. These are observed
agreement checks, not certified interval-error bounds.

```python
import mpmath as m
for precision in (70, 100):
    m.mp.dps = precision
    direct = [
        m.quad(lambda x: m.atan(m.sqrt(x*x+2))/((x*x+1)*m.sqrt(x*x+2)), [0, 1]),
        m.quad(lambda x: 1/(5-3*m.cos(x)), [0, m.pi, 2*m.pi]),
        m.quad(lambda x: m.acos(m.cos(x)/(1+2*m.cos(x))), [0, m.pi/4, m.pi/2]),
        m.quad(lambda x: m.atan((m.pi-1)*x/(1+m.pi*x*x))/x, [0, 1, m.inf]),
        m.quad(lambda t: m.log(t)/(2*m.cosh(t)), [0, 1, 10, m.inf]),
    ]
    exact = [5*m.pi**2/96, m.pi/2, 5*m.pi**2/24, m.pi*m.log(m.pi)/2,
             m.pi*m.log(2*m.pi)/4 + m.pi*m.log(m.gamma(m.mpf(3)/4)/m.gamma(m.mpf(1)/4))/2]
    alternate3 = m.quad(lambda t: 4*m.atan(1/m.sqrt(2-t*t))/(1+t*t), [0, m.mpf(1)/2, 1])
    alternate4 = m.quad(lambda t: m.atan((m.pi-1)/(m.exp(-t)+m.pi*m.exp(t))), [-m.inf, 0, m.inf])
    print(precision, [m.nstr(abs(a-b), 6) for a,b in zip(direct, exact)])
    print(m.nstr(abs(direct[2]-alternate3), 6), m.nstr(abs(direct[3]-alternate4), 6))
```

The normal runner invocation is
`python3 tests/run-calculus.py --corpus tests/user-challenge-integrals.json --report REPORT.json`.
A result reported only as `numeric_constant` is numerical agreement, even
though the corpus itself contains an exact proved reference.
