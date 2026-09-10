# Cycle6 Gaussian-atan and exponential logarithm families

## Gaussian-atan odd moments

For `a>0`, real nonzero `b`, and integer `n>=0`, define

```
I_n = integral_0^infinity x^(2n+1)*exp(-a*x^2)*atan(b*x) dx
J_n = integral_0^infinity x^(2n)*exp(-a*x^2)/(1+b^2*x^2) dx
G_n = integral_0^infinity x^(2n)*exp(-a*x^2) dx.
```

Positive Laplace representation of the rational denominator gives

```
J_0 = pi*exp(a/b^2)*erfc(sqrt(a)/abs(b))/(2*abs(b)).
```

Polynomial division and integration by parts then give

```
J_n = (G_(n-1)-J_(n-1))/b^2,
I_0 = b*J_0/(2*a),
I_n = n*I_(n-1)/a+b*J_n/(2*a).
```

All parts boundary terms vanish, and each integral converges ordinarily.
The implementation stores coefficients of just `J_0` and `sqrt(pi)/sqrt(a)`;
these coefficients are rational. It computes no successive symbolic
expressions for `J_n`, no generic derivatives and no erfc expansions.
The Gaussian moment coefficients satisfy `g_0=1/2` and
`g_n=(2n-1)*g_(n-1)/(2a)`.

The recognizer permits odd weights through degree17 (`n<=8`), rational Gaussian
and atan scales, a rational outside multiplier, and a rational exponent offset
when preserved in the exponential syntax. A full-real-line input is twice the
half-line result by parity. Nonzero shifts in the atan or Gaussian, complex
parameters, growing Gaussian tails and unsupported powers are rejected.
Input size is capped at96 nodes, factor count at5 and monomial parsing at32
visits. The recurrence has at most8 steps and retains one erfc term.

## Exponential logarithm moments

For rational `a,q>0`, `N=(m+1)/q>0`, and `s=+1` or `-1`, set `t=a*x^q`:

```
integral_0^infinity x^m*ln(1+s*exp(-a*x^q)) dx
  = Gamma(N)/(q*a^N) *
      ((1-2^(-N))*zeta(N+1) if s=+1 else -zeta(N+1)).
```

This follows by integrating the logarithm series. The sum of absolute
term integrals is proportional to `sum(k^(-N-1))`, which converges for `N>0`.
For the negative sign, `0<1-exp(-a*x^q)<1` on the entire open positive axis;
the logarithmic endpoint singularity is integrable. These are ordinary real
integrals and contain no principal-value assumption.

The implementation restricts `N` to integers1..12 and `q<=16`, giving finite
factorial and integer-zeta output. It accepts rational monomial scales and
fractional positive q, preserves an outside rational multiplier, and rejects
unknown signs, a nonunit exponential amplitude, an exponential offset,
unsupported Mellin order and other interval endpoints. It reuses existing
bounded monomial and arithmetic parsers instead of creating another transform
expression for the generic engine to evaluate.

## Verification and insertion

`tests/run-cycle6-gaussian-fermi.py --helpers
/tmp/khicas-cycle6-gaussian-fermi-functions.cc --verify` checks19 exact cases
with raw and evaluated syntax,15 rejected conditions, and repeats the direct
checks on a guarded64 KiB stack. Fourteen independent70-digit quadratures
include maximum moment orders. Gaussian reference moments use derivatives of
the independently proved zeroth-moment formula, not the implemented IBP
recurrence. The degree17 exact test was independently obtained by the eighth
symbolic derivative with respect to a. Negative-sign logarithm quadrature uses
`log(-expm1(-t))` to avoid subtraction loss near zero.

`/tmp/khicas-cycle6-gaussian-fermi-pipeline.json` records all19 cases with and
without outer simplify on normal and guarded64 KiB stacks (76 target runs),
using actual repository integration and FXCG simplify and a separate host
validator on each printed result. Host timings are not SH4 measurements.

