# Cycle 7: logarithmic Cauchy moments and sine/cosine sums

These rules implement two families from the frozen Cycle 7 corpus, whose
baseline is checkpoint `c977aa0`. They use bounded structural matching and
proved identities. The corpus and its independently generated references
remain unchanged.

## Möbius logarithms under a Cauchy measure

Complete the rational quadratic denominator as
`A*w^2*(1+u^2)`, where `u=(x-h)/w`, `h=-B/(2*A)`, and
`w=sqrt(C/A-h^2)>0`. This rule requires rational h and w and verifies the
logarithm's affine numerator and denominator have opposite zeros at
`h-w,h+w`. Thus its argument has the exact form
`k*((1+u)/(1-u))^s`, with k positive rational and s equal to +1 or -1.
The accepted transformed intervals are [-1,0], [0,1] and [-1,1], with either
orientation. The quadratic has no real pole and the logarithm is real in
the open interval. Endpoint logarithmic singularities are integrable.

Set `t=(1+u)/(1-u)` on [0,1]. Direct differentiation gives
`du/(1+u^2)=dt/(1+t^2)`. Consequently

```text
J_n = integral_1^infinity ln(t)^n/(1+t^2) dt
    = integral_0^infinity v^n/(2*cosh(v)) dv
    = n! * beta(n+1).
```

Here beta is the Dirichlet beta function. Expand
`1/(2*cosh(v))=sum_{j>=0}(-1)^j exp(-(2*j+1)*v)` and integrate;
for n>0 the integrated series converges absolutely. The n=0 result is
pi/4 directly. A small Euler-number recurrence gives the even moments,
while odd moments keep an exact polygamma difference:

```text
J_(2m) = abs(E_(2m))*pi^(2m+1)/2^(2m+2),
J_n = (Psi(1/4,n)-Psi(3/4,n))/4^(n+1),  n odd.
```

Expand only the bounded binomial `(ln(k)+s*v)^n`. Reflection u -> -u
multiplies each j-th moment by (-1)^j. This proves left-half and full-interval
answers, including all odd cancellations. The original Jacobian contributes
`1/(A*w)` and any extracted scalar weight is retained. The baseline n=2,
k=1 result is the compact `pi^3/16`.

The matcher accepts integer orders 1 through 8 and at most 128 expression
nodes. It rejects unmatched quadratic widths, different affine zero/pole
locations, nonpositive k, additional variable weights, invalid bounds and
orders beyond the finite budget. It does not claim arbitrary Cauchy-log
moments or complex logarithm identities.

## Equal-amplitude sine/cosine logarithms

For `A*sin(theta)+B*cos(theta)` with nonzero rational A and A=+B or A=-B,
write the sum as `sqrt(2)*abs(A)*sin(theta+phase)`. The phase is one of
pi/4, -pi/4, 3*pi/4 or -3*pi/4, according to the two coefficient signs.
Theta may be a rational affine function a*x+b with either slope sign.

The existing quarter-period Fourier identity is

```text
H(n*pi/4) = integral_0^(n*pi/4) ln(abs(sin(v))) dv
          = -n*pi*ln(2)/4-G*sigma(n)/2,
sigma(n mod 4) = [0,1,0,-1],
G = (Psi(1/4,1)-Psi(3/4,1))/16.
```

Therefore, after checking the endpoints, the answer is

```text
(hi-lo)*(ln(abs(A))-ln(2)/2)
- G*(sigma(n_hi)-sigma(n_lo))/(2*a),
```

multiplied by the original scalar weight. Without abs, the entire shifted
interval must lie in a positive sine half-wave. With abs, negative lobes
and interior isolated logarithmic zeros are valid; no branch expansion of
a complex logarithm occurs. Reversed intervals and negative affine slopes
retain orientation. Unshifted endpoint angles are exact quarter-period
multiples with indices between -64 and 64. Different sine/cosine arguments,
unequal amplitudes, and arbitrary endpoint angles are outside this rule.

## Companion scalar normalization

The `integrate_real_definite` dispatcher must arithmetically normalize its
extracted constant with `integration_syntax` before `integration_rational`.
For raw syntax, extracting a denominator factor 3 can return symbolic
`inv(3)`; treating that node as a nonrational constant previously skipped the
bounded rules and entered the deep generic path. The normalization converts
it to exact 1/3 without expanding variable powers or evaluating functions.
This applies to all dispatch families, not just these two rules.

## Reproduction and scope

```sh
python3 tests/validate-cycle7-log.py
python3 tests/run-cycle7-log.py --full --report /tmp/cycle7-log.json
```

Configure the same host Giac headers/libraries and `GIAC_NUMERIC_LIBS` as the
other integration tests. Before production integration, add
`--helpers /tmp/khicas-cycle7-log-functions.cc`; the runner builds a temporary
source file and applies the companion scalar fix there only.

The owned unit test covers 29 exact variants in both raw and evaluated
syntax, 14 domain/structure rejections and two nonradian-mode rejections.
It runs on normal and guarded 64 KiB pthread stacks. Full pipeline testing
runs all 29 variants in four modes: normal/64 KiB, each plain and with actual
FXCG+NO_STDEXCEPT outer simplify. Mathematical validation of each printed
answer occurs separately outside the constrained computation stack.

The independent Python audit uses no engine calls: it checks nine base
moments, four shifted/signed combinations (including order eight), and five
direct sine/cosine quadratures at 80 and 110 decimal digits. These checks
supplement the symbolic derivations and frozen corpus reference audit.
Host guarded-stack results do not measure real CG50 wall time or peak RAM.

Final candidate audit: all 116 full-pipeline rows returned exit code 0 and
`exact=true`; both unit modes passed. All 36 independent high-precision
quadratures passed, with maximum scaled error `5.06025501275065e-80`.

```text
helper SHA256:
2e8ae03f96d76bc99d6081673752ff3a219575c2b8d48186e3f172f8d7bde998
/tmp/khicas-cycle7-log-full.json SHA256:
95891c23146ffa29986548d04e29d993e32bfdd145e2a9cda076ec1cee240615
```
