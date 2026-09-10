# Gaussian tails and positive-binomial logarithms

For `a,b>0`, integration by parts and the Gaussian integral give

```
Integral_0^infinity erfc(a*u)*erfc(b*u) du
  = (a+b-sqrt(a^2+b^2))/(a*b*sqrt(pi)).
```
Subtracting this from `Integral erfc(b*u) du=1/(b*sqrt(pi))` gives

```
Integral_0^infinity erf(a*u)*erfc(b*u) du
  = (sqrt(a^2+b^2)-b)/(a*b*sqrt(pi)).
```
The second identity extends to any nonzero real `a` by oddness of erf;
`b` must remain positive. With `u=x^q`, `q>0`, the weight `x^(q-1)`
contributes exactly `1/q`. The implementation matches at most two tails and
six factors, with a shared 48-step monomial-recognition budget. It accepts
both erfc syntax and the evaluated identity `1-erf(u)`. It rejects unweighted
mixed exponents, shifted arguments, complex slopes, non-decaying erfc tails,
more than two tails and erf-only products.

For `A,B,q>0`, `a=(m+1)/q>0`, `b=r-a>0`, define

```
F(r) = Integral_0^infinity x^m/(A+B*x^q)^r dx
     = A^(-r)*(A/B)^a*Beta(a,b)/q.
```
Absolute convergence, locally uniformly for `r>a`, permits differentiation:

```
Integral x^m*ln(A+B*x^q)^n/(A+B*x^q)^r dx = (-1)^n F^(n)(r).
```
This reuses the existing Beta moment recurrence. Its first cumulant is
`ln(A)+Psi(r)-Psi(b)`; for order `j>=2`, the cumulant is
`(-1)^j*(Psi(b,j-1)-Psi(r,j-1))`. The new denominator-log mode accepts orders
1 through 4, retains the existing `r<=16` bound and rejects mixed log bases.
The existing `ln(x)` mode retains its original bounds. Identical denominator
factors may be combined after positivity is proved, including the evaluated
spelling `D^(-3/2)=D^(-1)/sqrt(D)`.

These are recognition/work bounds, not proofs of CG50 physical exhaustion.
Exact large-integer arithmetic still costs according to operand size.

`tests/run-cycle7-tail-mellin.py --verify` checks 15 exact identities in raw
and evaluated form and 17 rejected domains on normal and guarded 64 KiB
stacks. It additionally checks 15 independent 80-digit quadratures against
Gaussian-tail formulas or differentiation of mpmath's Beta function;
maximum absolute discrepancy was `1.06e-81`. The actual integration and outer
simplification entry points are checked separately using
`tests/cycle7-tail-mellin.json`; direct helper successes alone do not establish
whole-pipeline behavior. Host stack and timing checks are not CG50 measurements.
