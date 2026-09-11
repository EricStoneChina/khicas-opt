# Integration generalization corpus

`tests/generalization-corpus.json` contains **152 original scalar integral cases**:
81 indefinite and 71 definite. The deterministic generator constructs references
from explicit identities, without consulting problem numbers, contest answers, or
the output of the integrator under test. It does not modify engine branches.

The cases comprise **58 parameter variants, 32 near misses with actual reference
answers, and 62 broader coverage cases**. A near miss violates a hypothesis or
syntactic restriction of a recent fast path; the general integrator may still
solve it. An unevaluated integral is never its reference answer.

## Coverage units

Use the **17 broad mathematical groups** below when discussing breadth. The
53 `family` labels are diagnostic subdivisions, not 53 independent mathematical
ideas. Neither these groups nor the cases form an unbiased statistical sample of
all integration problems. For example, the 18 binomial-chain cases count as one
substitution family, despite changes in exponent, coefficient, and interval.

| `mathematical_group` | Cases | Structures exercised |
| --- | ---: | --- |
| algebraic-substitution | 30 | Binomial chains, polynomial expansion, real roots, principal powers |
| reciprocal-quartic | 12 | Inverse sine / inverse hyperbolic sine reductions and failed coefficient identities |
| periodic-trig-reciprocal | 11 | Continuous quartic-trig primitive, changed denominator identities |
| oscillatory-dirichlet | 15 | Odd sine powers, squared sinc, damping, shifted phase |
| logistic-density | 13 | Even/odd moments, frequency scaling, shifts, changed denominator |
| weighted-reflection | 8 | Linear weights, argument orientation, changed weight/coefficient/interval |
| gaussian | 7 | Moments, translation, Fourier factor, quartic exponential neighbor |
| integration-by-parts | 8 | Polynomial-exponential/trig/log products, inverse functions |
| rational-functions | 6 | Simple/repeated poles, irreducible quadratics, division, logarithmic derivative |
| quadratic-radicals | 5 | Trigonometric and hyperbolic substitutions, exterior-domain radical |
| elementary-composition | 7 | Exponential/log substitutions and nested chain rules |
| trig-hyperbolic-identities | 5 | Products, power reduction, logarithmic and hyperbolic primitives |
| absolute-values | 6 | Affine/quadratic absolute values and periodic sign changes |
| special-primitives | 2 | Error and sine integrals |
| endpoint-improper | 5 | Beta kernels and integrable logarithmic endpoint singularities |
| infinite-decay | 7 | Gamma integrals, rational tails, Fermi-type integrals |
| log-integral-identities | 5 | Mellin differentiation, logarithmic trig integrals, Frullani cancellation |

The corpus deliberately includes rules' boundary conditions: binomial
`q=(m+1)/k` from 1 through 8, nonintegral q and q=9; fractional powers and degrees
257/4096; all three reciprocal-quartic inverse-function regimes; negative and
rational frequencies; intervals spanning several trig periods; reversed finite
and infinite bounds; odd sine powers through 31 and the out-of-range power 33;
logistic moments through 8 and the out-of-range moment 10.

Other near misses change the quartic numerator, radical constant, or sign of d;
shift a logistic density; replace a Gaussian exponent by x^4; use a nonlinear
reflection weight or a nonsymmetric interval; and introduce a sine phase with a
different lower bound. Every such case has its own correct answer.

## Reference construction

The generator uses these identities, with explicit numeric parameters:

* For `x^(k*q-1)*(a+b*x^k)^n`, substitute `u=a+b*x^k` and expand
  `(u-a)^(q-1)`. The reference is the sum of
  `binomial(q-1,j)*(-a)^(q-1-j)*u^(n+j+1)/(k*b^q*(n+j+1))`.
  Definite polynomial examples instead expand the original polynomial, providing
  a different reference representation. Out-of-range q polynomial neighbors use
  termwise integration; negative exponents use rational/logarithmic identities.
* For the reciprocal quartic, `u=x/(x^2+d)` has derivative
  `(d-x^2)/(x^2+d)^2`. With `m=2*d-L`, the reference is
  `-asin(sqrt(m)*u)/sqrt(m)` if m>0, `-u` if m=0, and
  `-asinh(sqrt(-m)*u)/sqrt(-m)` if m<0. Coefficients are carried through explicitly.
  Nonmatching examples use perfect-square radicals and partial fractions on the
  documented domain, rather than assuming this substitution still applies.
* For `u=a*x+b`, the continuous quartic-trig reference is
  `(sqrt(2)*u-atan(sin(4*u)/(3+2*sqrt(2)+cos(4*u)))/sqrt(2))/a`.
  Its atan denominator is positive. Complete periods integrate to
  `sqrt(2)` times the interval length, including reversed orientation.
* For odd n, the half-line sine reference is
  `sign(a)*pi*binomial(n-1,(n-1)/2)/2^n`. This follows from the finite sine
  harmonic expansion and the Dirichlet integral. Full-line values are twice
  the half-line value; reversing bounds negates it. Squared-sinc references are
  `abs(a)*pi/2`, damped sine references are `atan(a/b)`, and
  `integral(cos(x)/x,x,1,+infinity)=-Ci(1)`.
* The density `1/(exp(x)+exp(-x)+2)` has total integral 1, zero odd moments,
  and even moments `(2^n-2)*abs(B_n)*pi^n` for n>=2. Scaling divides by
  `abs(a)^(n+1)`. Translating the second moment adds the squared shift.
  Moments are written as explicit rational multiples of pi powers.
* Reflection uses `g(pi-u)=g(u)` and
  `integral(sin(u)/(A+B*cos(u)^2),u,0,pi)=2*atan(sqrt(B/A))/sqrt(A*B)`
  for A,B>0. Bounds following u from 0 to pi use **signed** a, so a negative
  frequency and reverse orientation are independently checked. The B<0 neighbor
  uses atanh instead; the quadratic-weight neighbor uses integration by parts.
* Gaussian moments follow differentiation of `sqrt(pi/a)`; the shifted second
  moment adds the squared mean. The Fourier Gaussian and quartic exponential
  use `sqrt(pi)*exp(-b^2/4)` and `Gamma(1/4)/2`, respectively.
* Remaining references use direct differentiation, partial fractions, power
  reduction, the beta/gamma integrals, Mellin differentiation, or Frullani's
  identity. Absolute values are integrated after splitting at their zeros.
  The special primitives are `sqrt(pi)*erf(x)/2` and `Si(x)`.

These are mathematical derivations, not engine-output oracles. The optional
verification step additionally evaluates the original integrands independently.

## Domain and branch conventions

Expressions use KhiCAS syntax: `ln`, explicit `+infinity`/`-infinity`, `^` for
principal powers, and `surd(x,3)` for the real cube root. In particular,
`x^(1/3)` at negative real x is complex; it is intentionally not replaced by
`surd(x,3)`. The principal-power case samples both negative and positive x.

Every indefinite case supplies at least three valid samples. Representative
choices are:

| Case class | Samples | Purpose |
| --- | --- | --- |
| Binomial chains | 1/10, 1/3, 3/5 | Keep fractional bases positive and large powers manageable |
| Reciprocal quartics | -3, -1/3, 1/2, 4 | Both signs and distinct substitution branches |
| Quartic trig | -7, -pi/2, 0, pi/2, 8 | Multiple periods and locations near old tangent representations' poles |
| Logarithmic substitutions | 1/4, 3/2, 3 | Stay in x>0 |
| Exterior radical / negative-d neighbor | 3/2, 2, 4 | Stay in x>1 |
| Principal cube-root power | -3, -1, 1/3, 2 | Check principal complex values separately from real roots |

Poles, nondifferentiable absolute-value zeros, and branch endpoints are excluded
from derivative samples. Definite absolute-value integrals split at the zeros.
Conditionally convergent cases are ordinary one-sided improper integrals (or
sums of the two convergent sides), not Cauchy principal values of divergent
integrals.

## Reproduce and validate

Generation and byte-for-byte reproducibility need only Python's standard library:

```sh
python3 tests/generate-coverage.py
python3 tests/generate-coverage.py --check
```

Independent reference verification requires SymPy and mpmath:

```sh
python3 tests/generate-coverage.py --check --verify
```

The verification uses 50 decimal digits, `mpmath.diff` at every declared sample,
ordinary `mpmath.quad` with domain splits for nonoscillatory definite integrals,
and `mpmath.quadosc` for oscillatory tails. SymPy parses expressions and creates
numeric callables; the generator does not ask SymPy to integrate them. The test
compares absolute error scaled by `max(1,abs(reference))` against `1e-20`.

On the initial reference audit (SymPy 1.14.0, mpmath 1.4.1), **all 152 passed**:
81 sampled-derivative checks, 58 ordinary quadratures, and 13 oscillatory
quadratures. The largest scaled error was approximately `4.536e-26`, for
`ln(x)/sqrt(x)` on (0,1). These are independent numerical checks of the formulas,
not formal proofs. The identities above explain why each reference is valid.

Run the actual engine using the shared benchmark runner:

```sh
python3 tests/run-calculus.py --corpus tests/generalization-corpus.json \
  --timeout 10 --report /tmp/khicas-generalization.json
```

Add `--compare` for baseline/current comparisons. Report exact, sampled, numeric,
unevaluated, and timeout outcomes separately, and aggregate both by
`mathematical_group` and by `intent`. A high parameter-variant pass rate must not
be presented as equivalent to broad mathematical coverage. The corpus is
intended to expose gaps; it does not require every engine case to pass.

## Initial engine comparison

The initial 152-case comparison against
`checkpoint/integration-coverage-2026a` produced:

| Engine | Exact | Sampled | Unevaluated | Undefined |
| --- | ---: | ---: | ---: | ---: |
| Checkpoint baseline | 124 | 2 | 25 | 1 |
| Current optimized engine | 140 | 2 | 10 | 0 |

There were no errors, timeouts, or per-case regressions; 16 cases gained exact
validation. The two sampled results are the negative-d reciprocal-quartic
neighbor and the hyperbolic product. They remain separate from exact results.

The ten unevaluated cases expose current coverage gaps: two damped Dirichlet
integrals, the tenth logistic moment, two shifted logistic second moments, the
negative-cosine-square reflection denominator, a Fourier Gaussian, the two
logarithmic trig integrals, and `x/(exp(x)+1)` on the positive half-line. Their
references passed the independent mathematical audit. No additional engine rule
was added merely to make these ten cases pass.

[Archived engine comparison](benchmarks/generalization-2026a.json) ·
[Independent reference audit](benchmarks/generalization-reference-verification.txt)

## Limits

Sampled derivatives can miss discontinuous constants between sample points;
the finite interval quartic-trig cases provide additional cross-period checks,
but do not prove global continuity of every returned primitive. Numeric
quadrature is not a proof of convergence or an exact symbolic identity.
The corpus covers convergent scalar integrals with known references, and omits
multivariate/path integrals, distributions, parameter-dependent branch tables,
and deliberately divergent cases. It is a reproducible diagnostic set, not a
claim of universal integration coverage or calculator hardware performance.
