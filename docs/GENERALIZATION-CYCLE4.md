# Independent integration coverage: cycle 4

This fixed round contains **16 ordinary convergent definite integrals**. It was
constructed after checkpoint `0e5ed65` (`checkpoint/integration-cycle3-2026a`),
without querying the tested engine. The independently verified corpus was frozen
before its engine evaluation:

```text
tests/generalization-cycle4.json
SHA256 d2f42dcc6d89a4ce2e617adb1ef45638073e1f052ddc1ab2610d77f95474f4dd
```

The deterministic generator is `tests/generate-coverage-cycle4.py`. This round
preserves earlier generators, corpora, and engine sources. There are 12 written
elementary references and 4 references using Zeta, Gamma, erf, or Euler's constant.
All are proved exact expressions; no unknown closed form or numerical-only
placeholder is counted as an exact answer.

## Coverage units

| Broad mathematical group | Cases | Structural issue |
| --- | ---: | --- |
| Beta logarithmic moments | 2 | Third mixed derivative; nonzero odd logit moment under an asymmetric weight |
| Mellin substitutions | 2 | Distinct denominator factors; denominator scaling unmatched by logarithmic numerator |
| Laplace cancellation | 2 | Squared trigonometric cancellation; three exponential terms cancelling twice |
| Gaussian and reciprocal-Gaussian compositions | 3 | Shifted erf convolution, product of two erfs, square of reciprocal radicals in exponent |
| Arc and inverse-tangent symmetry | 3 | Affine phase, asin complement in rational coordinates, larger square with a reciprocal angle |
| Weighted inverse-tangent Frullani | 2 | First and second logarithmic weights |
| Iterated logarithms | 2 | Fractional-power scaling; composition with a Frullani difference |

The 15 `family` labels are diagnostic subdivisions, not 15 independent new
methods. Two examples are explicitly marked parameter variants. Five are marked
near misses; their actual references must be used instead of applying a nearby
rule with an invalid symmetry, coefficient, or branch assumption.

## Proofs of the references

### Beta moments

For the normalized arcsine beta density, put `X=ln(x)`, `Y=ln(1-x)`, and
`L=ln(2)`. Derivatives of `Beta(a,b)` at a=b=1/2 give

```text
E[X]=E[Y]=-2L,
var(X)=pi^2/3,
cov(X,Y)=-pi^2/6,
third joint cumulant(X,X,Y)=2*Zeta(3).
```

Thus `E[X^2*Y]=-8L^3+2*Zeta(3)`. By symmetry the same holds for `X*Y^2`;
multiplying their sum by the unnormalized integral pi gives
`pi*(4*Zeta(3)-16*ln(2)^3)`.

For the beta weight with a=3/2,b=1/2, the logit `X-Y` has mean 2, variance
`pi^2-4`, and third cumulant 16. Its third raw moment is therefore
`2^3+3*2*(pi^2-4)+16=6*pi^2`. Multiplying by `Beta(3/2,1/2)=pi/2` gives
`3*pi^3`. The asymmetric weight invalidates odd-logit cancellation.

### Mellin kernels

In the distinct-factor example, substitute `t=x^(2/3)` to get
`(3/2)*integral_0^infinity 1/((1+t)*(1+4*t)) dt`. Partial fractions give
`(3/2)*(ln(4)/3)=ln(2)`; each separated logarithm is combined before the
infinite limit is taken.

For the unmatched log-scale example use t=3x, so `ln(x)=ln(t)-ln(3)`.
The three centered integrals of `1`, `ln(t)`, and `ln(t)^2`, each divided by
`(1+t^2)^2`, are respectively `pi/4`, `-pi/4`, and `pi^3/16`.
Expanding the square and including the factor 1/3 proves the supplied result.

### Laplace cancellation

Let
`C(a,b)=a*atan(a/b)-b*ln(1+(a/b)^2)/2`, the integral of
`exp(-b*x)*(1-cos(a*x))/x^2`. The exact identity

```text
(1-cos(2*x))^2 = 2*(1-cos(2*x))-(1-cos(4*x))/2
```

gives `2*C(2,1)-C(4,1)/2`, the written reference.

For the exponential difference the coefficients cancel both the constant and
linear terms at zero. Integration by parts gives the general convergent identity

```text
integral(sum(c_j*exp(-a_j*x))/x^2, x, 0, +infinity)
  = sum(c_j*a_j*ln(a_j)),
provided sum(c_j)=sum(c_j*a_j)=0 and a_j>0.
```

Applying a=(1,2,3), c=(1,-2,1) yields `3*ln(3)-4*ln(2)`.

### Gaussian compositions

For a>0, differentiation in c and completion of the square prove

```text
integral(exp(-a*x^2)*erf(b*x+c), x, -infinity, +infinity)
  = sqrt(pi/a)*erf(c*sqrt(a/(a+b^2))).
```

The constant of integration is zero at c=0 by parity. This gives the shifted-erf
case with a=2,b=c=1.

For a>0, differentiation in b and an integration by parts prove

```text
integral(exp(-a*x^2)*erf(b*x)*erf(c*x), x, -infinity, +infinity)
  = 2/(sqrt(pi)*sqrt(a))*asin(b*c/sqrt((a+b^2)*(a+c^2))).
```

For example, the derivative in b of the left side is
`2*c/(sqrt(pi)*(a+b^2)*sqrt(a+b^2+c^2))`, equal to that of the right side;
both vanish at b=0. Translate x by 1 and take a=b=1,c=2 for the corpus case.

Finally,
`-(sqrt(x)-2/sqrt(x))^2=-x-4/x+4` for x>0. The inverse-Gaussian identity
`integral(x^(-1/2)*exp(-a*x-b/x))=sqrt(pi/a)*exp(-2*sqrt(a*b))`
with a=1,b=4 cancels the outside factor exp(4), leaving `sqrt(pi)`.

### Arc and square symmetry

The independent weighted-disk argument extends the user acos example as follows.
Put `h=atan(sqrt(q))`, q>0, and define

```text
F(q)=integral_0^(2h) acos((q-1+q*cos(u))/(q+(q+1)*cos(u))) du.
```

Then
`F(q)=2*pi*h-2*h^2-2*pi*atan(sqrt(1+2*q))+pi^2/2`.
To prove it, substitute t=tan(u/2). The angle becomes
`pi-2*atan(sqrt(2*q-t^2))`. Reflection across the diagonal shows that twice
the resulting weighted partial-disk integral is the entire quarter-disk
integral plus the square integral `h^2`. The quarter-disk weight
`1/((1+t^2)*(1+y^2))` integrates in polar coordinates to
`pi*(atan(sqrt(1+R^2))-pi/4)` at radius R. Taking R=sqrt(2q) gives the formula.

The affine corpus case is F(2)/2. For the rational asin case, first take the
principal-angle complement `asin(z)=pi/2-acos(z)` and then t=tan(u/2).
Its integral is `pi*h-F(2)`. On the stated interval the rational argument lies
in [1/3,3/5], so this complement has no branch ambiguity.

The reciprocal-angle integral is the integral over [0,2]^2 of
`1/((1+x^2)*(2+x^2+y^2))`. Adding its reflected kernel cancels the common
sum in the denominator and gives `1/((1+x^2)*(1+y^2))`. Therefore its value
is `atan(2)^2/2`.

### Logarithmically weighted Frullani integrals

Represent the inverse-tangent difference as an integral over its positive
frequency parameter. Substitution t=a*x after differentiation in a gives

```text
d/da integral((atan(a*x)-atan(b*x))*ln(x)/x) = -pi*ln(a)/(2*a)
d/da integral((atan(a*x)-atan(b*x))*ln(x)^2/x)
  = (pi^3/8+pi*ln(a)^2/2)/a.
```

Integrating between the positive parameter endpoints proves both references.
The endpoint logarithms are integrable at zero, and the tails decay as
`ln(x)^k/x^2`; the differences are never split into divergent separate integrals.

### Iterated logarithms

The independently proved constant from
`docs/USER-CHALLENGE-INTEGRALS.md` is

```text
C = integral_0^infinity ln(t)/(2*cosh(t)) dt
  = pi*ln(2*pi)/4+pi*ln(Gamma(3/4)/Gamma(1/4))/2.
```

For the fractional-scale case, t=-ln(x) gives `ln(t)/(2*cosh(3*t/2))`.
Rescale by 3/2 to obtain `(2/3)*(C-pi*ln(3/2)/4)`.

For the loglog-Frullani case, t=-ln(x) gives
`(exp(-2*t)-exp(-4*t))*ln(t)/t`. For positive a,b,

```text
integral((exp(-a*t)-exp(-b*t))*t^(s-1), t, 0, +infinity)
  = Gamma(s)*(a^(-s)-b^(-s)).
```

The difference is analytic near s=0. Using
`Gamma(s)=1/s-euler_gamma+O(s)` and differentiating at zero yields
`(ln(a)^2-ln(b)^2)/2-euler_gamma*ln(b/a)`. With a=2,b=4 this is
`-3*ln(2)^2/2-euler_gamma*ln(2)`.

## Reproduction and verification

```sh
python3 tests/generate-coverage-cycle4.py
python3 tests/generate-coverage-cycle4.py --check
python3 tests/generate-coverage-cycle4.py --check --verify
```

Generation uses the standard library. Verification requires SymPy and mpmath;
SymPy only parses the expressions and creates numeric callables. The audit uses
80 working digits and requires scaled error
`abs(observed-reference)/max(1,abs(reference)) <= 1e-25`.

**All 16 passed**, with 18 independent quadratures including alternative
variable-substitution checks for both loglog examples. The largest observed
scaled error was `5.5921527803e-38`, for the asymmetric third-logit Beta moment.
The stable trigonometric/exponential forms and inverse-tangent difference
identities used to avoid numerical cancellation are recorded in the JSON's
`verification` fields; the original engine expressions are preserved.

All fractional powers use their principal value, and their stated real domains
keep the bases positive. The two logs in loglog expressions are real on (0,1).
There are no divergent or Cauchy-principal-value cases. Numerical agreement is
an additional check, not a substitute for the proofs and convergence conditions.

Run the actual engine separately, retaining distinctions between exact,
sampled, numerical, unresolved, and timeout outcomes:

```sh
python3 tests/run-calculus.py --corpus tests/generalization-cycle4.json \
  --timeout 10 --report /tmp/khicas-generalization-cycle4.json
```

This corpus does not claim hardware resource exhaustion or CG50 timings.
