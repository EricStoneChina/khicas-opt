# Gamma logarithmic moments

For positive real `a,q`, `s=(m+1)/q>0`, and positive `B`, substitution
`u=a*x^q` gives the analytic generating function

```
Integral_0^infinity x^m*exp(-a*x^q)*(B*x^k)^t dx
  = B^t*Gamma(s+k*t/q)/(q*a^(s+k*t/q)).
```

Differentiation at `t=0` gives the integral with `ln(B*x^k)^n`. Dominated
convergence holds in a neighborhood of zero because `s>0` and the exponential
decays at infinity. The first logarithmic derivative is
`ln(B)+(k/q)*(Psi(s)-ln(a))`; subsequent derivatives are
`(k/q)^j*Psi(s,j-1)`. Their moment recurrence is the same Bell/cumulant
recurrence already used by Beta moments. The implementation factors that
existing recurrence into `integration_cumulant_moment` and shares it; its
64-term growth check and compact partition fallback are retained.

The new recognizer permits `0<s<=16`, rational s with denominator at most16,
logarithmic order through4, at most8 factors and a 64-step monomial budget.
It accepts only positive exponential decay rate and positive log scale,
on the positive half-line. It requires radian mode consistently with the
existing special-function shortcuts. Shifted logarithms, divergent endpoints,
complex scales, mixed log atoms and larger orders reject to existing paths.
Evaluated spellings such as `(-ln(x))^3` retain their sign. Fractional wrappers
such as `sqrt((-ln(x))^2)` are not flattened into a signed logarithm.

`tests/run-cycle8-gamma.py --verify` checks 15 exact identities in raw and
evaluated forms, 15 rejected domains/resources, and repeats on guarded64 KiB
stacks. Twelve independent80-digit quadratures are compared with numerical
differentiation of the complete mpmath Gamma generating function, without
repeating the implementation's recurrence. Maximum scaled error was
`1.07e-78`. The separate corpus `tests/cycle8-gamma-log.json` checks all15
through actual repository integration and FXCG outer simplification on both
normal and64 KiB stacks, with printed results independently validated.

These are bounded host checks, not CG50 runtime memory/timing measurements.
The existing Beta/logit/high-order recurrence tests must also pass after the
shared-code extraction; the new Gamma identities alone do not validate that
refactor.
