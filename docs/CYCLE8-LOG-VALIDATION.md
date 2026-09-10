# Cycle 8: bounded inverse-angle and opposite-trigonometric log moments

The independently frozen Cycle 8 proofs are in `GENERALIZATION-CYCLE8.md`.
An initial current-source test confirmed both assigned cases returned an
unevaluated integral on the ordinary host stack, and SIGSEGV on a guarded
64 KiB stack, both directly and with outer simplify. The baseline record is
`/tmp/khicas-cycle8-log-baseline.json`. These two general rules bypass that
unsupported generic route.

## Inverse tangent times a logarithm under a Cauchy measure

The rule recognizes a rational scalar times

```text
atan(u)*(shift+power*ln(u))*du/(1+u^2),
```

through its original expression, without constructing a transformed large
expression. The coordinate u is either a rational affine function, or a
positive rational multiple `c*x^q` with rational `0<q<=32`. For the monomial
case the original x interval must be nonnegative. The weight must match the
coordinate derivative, and the quadratic/binomial denominator must equal a
nonzero rational multiple of `1+u^2`. These checks prove there is no real
pole. A logarithm of a positive rational monomial `k*x^r` becomes
`ln(k)-(r/q)*ln(c)+(r/q)*ln(u)` only after proving u>0 in the interior.
For affine u, the logarithm argument must be a positive multiple of u.

The accepted transformed intervals are [0,1], [1,infinity), and
[0,infinity), in either orientation. Put t=atan(u). The two required moments
on each interval are:

| u interval | Integral atan(u)/(1+u^2) | Integral atan(u)*ln(u)/(1+u^2) |
|---|---|---|
| 0 to 1 | pi^2/32 | 7*Zeta(3)/16-pi*G/4 |
| 1 to infinity | 3*pi^2/32 | 7*Zeta(3)/16+pi*G/4 |
| 0 to infinity | pi^2/8 | 7*Zeta(3)/8 |

Here `G=(Psi(1/4,1)-Psi(3/4,1))/16`. The first column follows by integrating
t on the corresponding angle interval. For the logarithmic column, integrate
the Fourier series `ln(tan(t))=-2*sum_(n odd) cos(2*n*t)/n` against t.
The lower-quarter evaluation is proved in the frozen reference document;
the full interval gives `sum_(n odd)1/n^3=7*Zeta(3)/8`. Subtraction gives
the upper quarter. Both endpoint logarithms are ordinarily integrable.
The final result is a linear combination of these moments, with the exact
coordinate Jacobian and original scalar weight retained.

Matching has a 128-node input bound; `abs(power)<=32`, and denominators have
two monomial terms or a bounded rational quadratic representation. Missing
Jacobian factors, nonpositive logarithm scales, mismatched denominators,
negative monomial-domain intervals and other endpoints are rejected. This
rule does not claim arbitrary logarithmic powers or complex branches.

## Products of opposite trigonometric logarithms

Let `T=sin(theta)` or `cos(theta)`, with `theta=a*x+b` a rational affine
angle (a rational multiple of pi is also allowed for b). Recognize

```text
ln(A*(1+s*T))*ln(B*(1-s*T)),
```

where A and B are positive rational numbers and s is either sign. The
trigonometric functions and their angle syntax must agree. All logarithm
arguments are positive apart from isolated zeros, whose logarithmic
singularities are integrable; no complex log expansion is needed.

The unscaled product is even, pi-periodic, and symmetric about every
quarter-period midpoint. Its integral over each quarter of length pi/2 is

```text
pi*ln(2)^2/2-pi^3/12.
```

The beta mixed-derivative proof appears in the frozen reference document.
Independent scales add `ln(A)*ln(B)` and two single-log moments. For an
exact half-period grid endpoint `theta=n*pi/2`, these moments use

```text
sigma(n mod 4) = [0,1,0,-1],
integral ln(1+s*T) dx = -(hi-lo)*ln(2)-2*G*delta_sigma/a.
```

The offset in sigma is 2 or 0 for positive/negative cosine, and 1 or 3 for
positive/negative sine. These follow directly from `1+s*T=2*sin(u)^2`
and the existing quarter-period logarithmic-sine identity. The final answer
has a constant interval-width term and at most one Catalan coefficient;
equal scales or full-period cancellation omit the latter automatically.

Endpoint indices must be integers between -64 and 64. The rule supports
negative affine slopes, reversed intervals, sine/cosine shifts, multiple
periods and independent positive scales. It rejects unequal constant/trig
amplitudes, different angles, same-sign log pairs, nonpositive overall
arguments and endpoints outside its exact grid. Input size is bounded by
128 expression nodes.

## Reproduction and evidence

```sh
python3 tests/validate-cycle8-log.py
python3 tests/run-cycle8-log.py --full --report /tmp/cycle8-log.json
```

Use the normal host Giac environment and `GIAC_NUMERIC_LIBS`. Before
production integration, pass `--helpers /tmp/khicas-cycle8-log-functions.cc`.
The candidate runner stages a temporary yintg source; it does not edit
production files. Both helpers dispatch at the start of
`integrate_compact_definite`. Dependencies are existing bounded arithmetic,
monomial and quadratic readers plus `integration_quarter_sigma`.

The final candidate passed 26 exact variants in raw and evaluated syntax,
16 domain/structure rejection cases and two nonradian rejection cases, on
normal and guarded 64 KiB pthread stacks. Its full 104-run matrix (26 cases,
normal/64 KiB, direct/actual FXCG+NO_STDEXCEPT outer simplify) had no failed
rows: every computation returned exit code 0 and was independently checked
as exact outside the constrained stack.

```text
helper SHA256:
44ad407b6af66b10d3e9e89a4852772f8c56fcd4eba91e71edcbbcd5be12a74f
staged yintg SHA256:
bee8e24c976c4f84386a9f6340784a09c6d48d44b0282d07f736f0b918f48722
pipeline report: /tmp/khicas-cycle8-log-full.json
```

These are host bounded-stack measurements, not real CG50 timing or RAM
measurements. The full firmware build and hardware testing remain separate.

The independent audit additionally passed all 52 numerical variant
quadratures at 80 and 110 decimal digits. It reads only the owned test data,
uses SymPy to parse explicit expressions and mpmath to quadrature them, and
never calls Giac or requests an antiderivative. Exact half-angle rewrites
prevent catastrophic subtraction near trigonometric logarithm endpoints.
