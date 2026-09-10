# Cycle 6: mixed Mellin moments and oscillatory cancellation

These rules are bounded families, derived independently of the selected corpus
answers. All integrals are ordinary improper integrals, not principal values.

## Mixed degrees

For `0<s<3`, let

```
F(s) = integral_0^infinity u^(s-1)/((1+u)*(1+u^2)) du.
F(s) = pi/4*(2*csc(pi*s)+csc(pi*s/2)-sec(pi*s/2)).
```

Partial fractions prove the identity first on `0<s<1`; both sides continue
analytically throughout `0<s<3` after the removable poles are resolved.
At `s=1,2`, use these finite values before evaluating any trigonometric pole:

```
F(1)=F(2)=pi/4
F'(1)=-pi^2/16; F'(2)=pi^2/16
F''(1)=F''(2)=pi^3/16
```

At zero the integrand is `O(u^(s-1))`, and at infinity it is `O(u^(s-4))`.
The same estimates permit two logarithmic derivatives uniformly on compact
subintervals of `0<s<3`. Thus `F^(m)` is the integral with `ln(u)^m` for m=0,1,2.

The substitution `u=a*x^k`, with `a,k>0`, handles
`x^q*ln(x)^m/((1+a*x^k)*(1+a^2*x^(2*k)))` on the positive axis. Write
`s=(q+1)/k`, `L=ln(a)`. The answers are `a^(-s)/k^(m+1)` times
`F(s)`, `F'(s)-L*F(s)`, or `F''(s)-2*L*F'(s)+L^2*F(s)`.
For evaluation the implementation uses the equivalent stable identity
`F(s)=pi/(4*sqrt(2)*sin(pi*s/4)*sin(pi*(s+1)/4))`.
Both sine factors are positive inside the entire convergence interval. With
`D=cot(pi*s/4)+cot(pi*(s+1)/4)`, its logarithmic derivatives give
`F'=-pi*F*D/4` and
`F''=pi²*F*(D²+csc²(pi*s/4)+csc²(pi*(s+1)/4))/16`.
This avoids cancellation of large terms near s=1,2. An independent boundary
audit reduced the maximum observed scaled error of printed floating answers
from about 2.73e-10 to 2.07e-12; this is numerical evaluation accuracy in the
host audit, not a CG50 speed measurement.

A rational outside coefficient is retained. Convergence is checked before
forming the answer. The recognizer limits input size, syntax visits, factor
count and exact trigonometric denominators (at most 12); these are algorithm
budgets, not proofs that larger inputs exceed CG50 physical resources.

## Oscillatory cancellation

For real rational frequencies, two families use finite sums rather than
integrating divergent terms separately:

```
integral_0^infinity sum(c_j*cos(k_j*x))/x^2 dx
 = -pi/2*sum(c_j*abs(k_j)), provided sum(c_j)=0.

N(x)=sum(c_j*sin(k_j*x)+d_j*x*cos(k_j*x))
integral_0^infinity N(x)/x^3 dx
 = -pi/4*sum((c_j*k_j+2*d_j)*abs(k_j)),
provided sum(c_j*k_j+d_j)=0.
```

Different frequencies may be present; zero frequencies represent constant
cosine terms or a linear numerator term. Taylor expansion shows bounded
integrands at zero under exactly the stated cancellations. At infinity the
terms are O(x^-2) or smaller, so both integrals converge absolutely.

For the second identity integrate by parts on a finite interval. The boundary
term `-N(x)/(2*x^2)` vanishes at both ends. The remaining cosine differences
use the first identity, while `integral sin(k*x)/x = pi*sign(k)/2` is the
ordinary Dirichlet integral. Limits are taken after cancellation, without
assigning a value to the individual divergent terms. Negative frequencies
preserve the correct signs. Both original integrands are even; the negative
half-line has the same value and the full line has twice the value.

The parser checks at most 128 nodes, 16 numerator terms, and four factors per
term. It rejects nonzero phase shifts, unsupported powers, and unmatched
cancellations.

## Checks

`tests/run-cycle6-mellin-osc.py --verify` exercises 19 exact variants with raw
and evaluated syntax, ten rejected conditions, both signed intervals and a
64 KiB guarded computation stack. Eighteen independent 70-digit quadratures
also check fractional Mellin parameters and all three log orders against
printed floating results. This numerical audit is distinct from the exact
family proofs above. The two corpus cases pass all eight combinations of
normal/64 KiB stack and direct/outer-simplify with an independent exact checker.
