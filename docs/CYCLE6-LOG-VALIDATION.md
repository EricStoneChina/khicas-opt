# Cycle 6: four bounded logarithmic integration families

The frozen references and their independent 80/110-digit audit are in
`GENERALIZATION-CYCLE6.md`. This implementation adds four general rules for
those references. It does not change the frozen corpus, use input strings as
lookup keys, or substitute principal values for divergent integrals.

## General identities and matching conditions

### Affine-weighted logarithmic sine/cosine quarters

For a real affine argument `u=a*x+b`, a rational affine weight `A*x+B`, and a
quarter or full positive sine half-wave, the change of variable reduces the
answer to the zeroth and first moments of `ln(sin(u))`. On `[0,pi/2]`,

```text
M0 = -pi*ln(2)/2,
M1 = -pi^2*ln(2)/8 + 7*Zeta(3)/16.
```

Reflection `u -> pi-u` gives the first moment on `[pi/2,pi]` as
`-pi^2*ln(2)/2-M1`. Adding both quarters gives the full half-wave. The affine
weight and Jacobian are applied symbolically; negative slopes and reversed
transformed bounds retain the correct orientation. A cosine is shifted to a
sine by adding pi/2 to its argument. With `abs`, the same moments apply on
other half-waves after translating the affine weight by an integer multiple
of pi. Without `abs`, odd-numbered negative sine half-waves are rejected.

The Fourier-series proof of M1 is recorded in the frozen reference document.
Unweighted cases bypass construction of the zeta moment. Supported weights
are affine, not arbitrary powers. Endpoint locations are checked exactly,
with a bounded half-wave index; arbitrary-angle intervals are not claimed.

### Products and squares of opposite logarithms

On `0<x<L`, let `u=b*x^q`, with positive rational b and q, `q<=32`, and the
checked endpoint `b*L^q=1`. The factor `dx/x` becomes `du/(q*u)`. Thus the
three exact constants are

```text
integral_0^1 ln(1+u)^2/u du = Zeta(3)/4,
integral_0^1 ln(1-u)^2/u du = 2*Zeta(3),
integral_0^1 ln(1-u)*ln(1+u)/u du = -5*Zeta(3)/8.
```

The zeta-series, polarization and trilogarithm-reduction proofs appear in
`GENERALIZATION-CYCLE6.md`. The matcher accepts two logarithmic factors or a
single squared logarithm, but requires the same positive monomial inside
both factors. It preserves the dx/x measure and verifies the finite endpoint
before applying any result. Mismatched coefficients, exponents, endpoints,
third powers, and negative/zero q are rejected by this rule. Positive-domain
fractional powers include the square-root spelling.

### Logarithms of trigonometric sums

For positive rational A, an affine argument theta, and either sign, rewrite
`A*(1 +/- sin(theta))` or `A*(1 +/- cos(theta))` as `2*A*sin(u)^2`, where u
is theta/2 plus a fixed quarter-period offset. This is an equality of real
nonnegative functions; isolated zeros produce integrable logarithmic
singularities. No complex logarithm expansion is used.

For integer n,

```text
H(n*pi/4) = integral_0^(n*pi/4) ln(abs(sin(u))) du
          = -n*pi*ln(2)/4-G*sigma(n)/2,
sigma(n mod 4) = [0,1,0,-1],
G = (Psi(1/4,1)-Psi(3/4,1))/16.
```

The Fourier-series proof and the absolutely convergent polygamma difference
are in the frozen reference document. Affine endpoint angles must be exact
multiples of pi/2, within a bounded grid. Their sine-square half-angle
arguments then lie on the required quarter grid. The final result consists
only of the interval width times `ln(A/2)` and a bounded multiple of G divided
by the affine slope. Full-period cancellation omits G entirely. Unequal
constant/trig amplitudes, negative overall log arguments, and arbitrary
endpoints are outside this rule. Positive overall rational scaling inside
the logarithm is retained exactly.

### Inverse tangent under a logarithmic measure

The involution `u -> (1-u)/(1+u)` on `[0,1]` preserves `du/(1+u)` and replaces
`atan(u)` by `pi/4-atan(u)`. Therefore

```text
integral_0^1 atan(u)/(1+u) du = pi*ln(2)/8.
```

The matcher supports either a rational affine u or a positive rational
monomial `c*x^q`, `0<q<=32`. It checks that the remaining numerator is
proportional to u', that the denominator is proportional to `1+u`, and that
the transformed endpoints are exactly 0 and 1 in either orientation. For
fractional monomials the original interval must be nonnegative. These checks
prove the real principal-atan domain, avoid denominator poles, and reject
missing Jacobian factors, altered denominators, or wrong bounds.

## Validation and limits

Owned tests:

```text
tests/cycle6_log_integrals.cc
tests/run-cycle6-log.py
```

Reproduce with the host Giac headers/libraries configured as for the other
integration tests, including `GIAC_NUMERIC_LIBS`:

```sh
python3 tests/run-cycle6-log.py
python3 tests/run-cycle6-log.py --full --report /tmp/cycle6-log-full.json
```

Before integration into production, `--helpers
/tmp/khicas-cycle6-log-functions.cc` supplies the independently reviewed
candidate definitions. The runner defaults to the integrated repository
source after the rules are merged.

The helper suite checks **22 identities**, each in raw parsed and evaluated
syntax, plus **16 structural/domain rejections** and **3 angle-mode
rejections**, both normally and on a guarded **64 KiB** computation stack.
The variants include shifted and negative affine arguments, affine weights,
absolute-value half-waves, square roots, cubic substitutions, matched scaled
endpoints, both trig signs, a full trig period and affine/monomial inverse
atan substitutions.

The complete candidate pipeline passed **66/66 exact computations**:
22 variants under ordinary `_integrate`, the same 22 under a 64 KiB guarded
stack, and the same 22 with outer `_simplify` compiled from the actual FXCG
and NO_STDEXCEPT branches. Each printed result was checked by an independent
host Giac validator outside the constrained computation stack; sampled-only
agreement does not count as an exact pass. Report:
`/tmp/khicas-cycle6-log-full.json`.

Candidate helper SHA256:

```text
cf9466277caa640620bd3b64b4fa9d59aee5f83edb7e9fbe162f1405f7287420
```

The complete staged yintg source used for that report has SHA256
`40e6633af13df8cf42e4661fec0d4c5f90b6f959738078e6950a38b86e4b4727`.
The SH4 compiler accepted the extracted candidate. Its reported static stack
frames are 276 bytes (weighted quarters), 144 bytes (log products), 304 bytes
(trig-log sums), and 228 bytes (atan measure). These are individual static
frames, not upper bounds on all library-call stack usage. The extracted
object is 10,744 bytes including reused dependency code; that is not an
incremental firmware-size measurement. Host stack tests are not true CG50
hardware emulation, and the general solver fallback remains outside these
bounded rule guarantees.
