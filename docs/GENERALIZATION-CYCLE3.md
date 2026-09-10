# Independent integration coverage: cycle 3

This round adds **30 scalar integrals: 25 definite and 5 indefinite**, including
5 explicitly parameterized stress cases. It was constructed after checkpoint
`895c848` (`checkpoint/integration-cycle2-2026a`) and fixed **before evaluating
the tested engine**. The frozen `tests/generalization-cycle3.json` SHA256 is:

```text
23964c872f7421e5710d8b7bca2eeb4a1cdf6d4db2c9b73abd15a5208ff29bf9
```

The generator is `tests/generate-coverage-cycle3.py`. It creates only this round's
JSON and does not modify earlier corpora or the engine. Every supplied reference
has a known closed form. References containing Gamma or Zeta are explicitly
marked `special-function-closed-form`; other references are marked
`elementary-closed-form`. This describes their written representation, not an
assertion that special-function expressions cannot simplify further.

## Coverage

| Mathematical group | Cases | Structural changes |
| --- | ---: | --- |
| Mellin/Beta with logarithmic weights | 8 | Higher denominator powers, squared logs, logit moments, asymmetric and mixed endpoint weights |
| Laplace cancellation | 3 | Second-order denominator, removal of the linear term, nonzero phase |
| Inverse-Gaussian extensions | 3 | Powers -3/2 and +1/2, quadratic reciprocal exponential |
| Other closed-form definite families | 6 | Quartic radical Gamma values, logarithmic Zeta kernels, hyperbolic and inverse-trig log moments |
| Composed primitives | 5 | Integration by parts, rational roots, inverse trig, both components of an exterior radical domain |
| Input-scale stress cases | 5 | Polynomial degree, substitution expansion size, high moments, high frequency cancellation |

Family labels subdivide these groups for diagnosis. Parameter variations and
pressure tests must not be counted as separate mathematical methods. The two
explicit near misses check that an asymmetric Beta weight does not have odd-logit
cancellation and that a phased Laplace numerator needs both its constant and
linear terms removed.

## Reference derivations

For `0<p<q*r`, the generalized Mellin identity is

```text
integral(x^(p-1)/(1+x^q)^r, x, 0, +infinity)
  = Gamma(p/q)*Gamma(r-p/q)/(q*Gamma(r)).
```

Substitution `t=x^q` reduces it to the beta integral. Differentiating once or
twice in p supplies the logarithmic weights; specifically, the integrals of
`ln(x)/(1+x^2)^2` and `ln(x)^2/(1+x^2)^2` are `-pi/4` and `pi^3/16`.

Beta parameter differentiation also supplies the four finite-interval log
references. The arcsine weight's logit has integral of its square equal to
`pi^3`. With weight `sqrt(x/(1-x))`, its first moment is `pi`, not zero.
For the arcsine weight, the log(x) cumulants needed for its third moment are
`-2*ln(2)`, `pi^2/3`, and `-12*Zeta(3)`, giving
`-pi*(8*ln(2)^3+2*pi^2*ln(2)+12*Zeta(3))`.
The mixed-log case is the mixed derivative of `Beta(3/2,3/2)`.

For b>0, integrate the usual Laplace/Dirichlet identities with respect to the
frequency to obtain

```text
C(a,b) = integral(exp(-b*x)*(1-cos(a*x))/x^2, x, 0, +infinity)
       = a*atan(a/b)-b*ln(1+(a/b)^2)/2
S(a,b) = integral(exp(-b*x)*(sin(a*x)-a*x)/x^2, x, 0, +infinity)
       = a-b*atan(a/b)-a*ln(1+(a/b)^2)/2.
```

The phased, twice-cancelled numerator has integral
`cos(phi)*S(a,b)-sin(phi)*C(a,b)`. Subtracting only the phase's constant would
leave a generally nonintegrable 1/x term at the origin.

Start the inverse-Gaussian cases with
`J(a,b)=sqrt(pi/a)*exp(-2*sqrt(a*b))`, the integral of
`x^(-1/2)*exp(-a*x-b/x)`. The two weighted cases are `-dJ/db` and `-dJ/da`.
For the quadratic reciprocal exponential, first substitute the square of x;
equivalently, differentiate
`sqrt(pi)*exp(-2*sqrt(a*b))/(2*sqrt(a))` with respect to a.

The quartic-radical references follow from the same beta substitution, producing
`Gamma(1/4)^2/(4*sqrt(pi))` on the positive half-line and
`Gamma(1/4)*sqrt(pi)/(4*Gamma(3/4))` on [0,1]. The logarithmic kernels follow
from convergent power series and give `pi^2/12` and `Zeta(3)`.
Differentiate the integral of `cosh(x)^(-s)` at s=1 for `pi*ln(2)`.
Substitute `t=atan(x)` in the inverse-trig moment and integrate `t*cot(t)` by
parts to obtain `pi*ln(2)/2`. The five indefinite references can be checked
directly by the product and chain rules.

## Explicit stress scales

Each stress case carries a machine-readable `scale` field and a short reference.

| Case | Scale | Reference basis |
| --- | --- | --- |
| `C3-stress-beta-polynomial-01` | Endpoint degree 255, expanded degree 510 | `2^512*255!^2/511!` |
| `C3-stress-binomial-substitution-01` | q=32, outer power 127, expanded degree 317 | `2^128*31!*127!/(2*159!)` |
| `C3-stress-beta-logit-01` | Logit moment 16 | `19391512145*pi^17` |
| `C3-stress-fermi-moment-01` | Degree 24, exponential scale 2 | `(1-2^(-24))*24!*Zeta(25)/2^25` |
| `C3-stress-harmonic-cancellation-01` | Largest frequency 31, cancellation order 3 | `-465*pi/2` |

The polynomial factors are normalized so the reference audit cannot pass merely
because both a tiny integral and zero fall below its absolute error tolerance.
The binomial stress reference follows from `t=x^2`. The logit identity follows
from the arcsine logit moment generating function `pi/cos(pi*t)`; the sixteenth
secant number is 19391512145. The Fermi reference follows from its decaying
exponential series. Twice integrating by parts gives

```text
integral((sin(a*x)-a*sin(x))/x^3, x, 0, +infinity)
  = pi*a*(1-a)/4, a>0.
```

Stress labels specify the inputs, **not evidence of exhausted resources**.
An unevaluated result or timeout alone does not demonstrate insufficient ROM,
heap, stack, or processor capacity. Those claims require measured resource data.
This host corpus does not claim to measure CG50 runtime limits.

## Domains and independent audit

Every ordinary definite integral in this round converges. In particular, the
harmonic stress numerator is O(x^3) at zero and its tail is O(1/x^3), so the
integral is absolutely convergent. No Cauchy principal value is substituted for
a divergent ordinary integral. Positive-domain fractional powers retain their
principal-power meaning; no branch is silently changed. The exterior-radical
primitive samples -3, -2, 2, and 3, in the two valid domain components.

Generate and check the frozen JSON using standard Python:

```sh
python3 tests/generate-coverage-cycle3.py
python3 tests/generate-coverage-cycle3.py --check
```

Independently audit it using SymPy and mpmath:

```sh
python3 tests/generate-coverage-cycle3.py --check --verify
```

SymPy parses expressions and creates numeric callables; it does not ask Giac or
another integrator for the reference. The audit uses 80 decimal digits and checks
`abs(observed-reference)/max(1,abs(reference)) <= 1e-20`.

All **30 references passed**: 23 ordinary quadratures, 1 explicitly transformed
quadrature, 1 oscillatory quadrature, and 5 sampled-derivative checks. The largest
scaled error was `5.383115802e-38`, for the third-logarithm Beta integral.
Two numerical details are recorded in each affected case's `verification` data:

* The phased Laplace numerator is evaluated through its exact trigonometric
  identity `((sin(3*x)-3*x)/2-sqrt(3)*sin(3*x/2)^2)`. This avoids subtracting
  nearly equal irrational constants before division by x^2. Its original engine
  input and its reference are unchanged.
* For the sixteenth logit moment, `x=1/(1+exp(-t))` turns the original integrand
  and Jacobian into `t^16/(2*cosh(t/2))` on the full line. This is numerical
  integration after a specified substitution, not evaluation of the answer
  formula. It avoids loss of accuracy close to the original endpoints.

Sampled differentiation is not a proof of global continuity, and numerical
agreement alone does not prove convergence. The derivations and explicit domains
provide the mathematical basis; the audit independently checks the references.
Run the actual engine separately and retain its exact/sampled/numeric/unevaluated/
timeout distinctions:

```sh
python3 tests/run-calculus.py --corpus tests/generalization-cycle3.json \
  --timeout 10 --report /tmp/khicas-generalization-cycle3.json
```
