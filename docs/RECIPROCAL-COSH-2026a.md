# Reciprocal cosh: real definite output

For rational `A,B,a`, with `a != 0`, `B>0`, and `A+B>0`, write
`F(A,B)=integral(1/(A+B*cosh(u)),u,0,+infinity)`.
The substitution `t=tanh(u/2)` gives
`F=2*integral(1/(A+B+(B-A)*t^2),t,0,1)`.
The denominator is strictly positive on `[0,1]`: its endpoint values are
`A+B>0` and `2B>0`, and it is affine in `t^2`. This proves ordinary convergence
and selects real principal logarithm/arctangent branches.

* `A>B`: `F=ln((A+sqrt(A^2-B^2))/B)/sqrt(A^2-B^2)`.
* `A=B`: `F=1/B`.
* `-B<A<B`: `F=2*atan(sqrt((B-A)/(B+A)))/sqrt(B^2-A^2)`.

The integral with argument `a*x` on `[0,+infinity)` is `F/abs(a)`.
On the whole real line, an arbitrary rational shift `b` is permitted and the
answer is `2*F/abs(a)`, by translation and evenness after scaling.
A rational outside multiplier is also preserved. Half-line nonzero shifts,
finite intervals, nonrational/unknown parameters and radian mode disabled are
outside this bounded recognizer. `A+B<=0` and `B<=0` are rejected, so it never
assigns an ordinary convergent answer to a pole-containing member.

The parser permits at most 64 input nodes, four denominator terms and one affine
cosh, with no polynomial expansion or root search. `integration_syntax` only
canonicalizes arithmetic, including inverse-integer constants introduced by
coefficient extraction. The entry is explicitly `noinline,optimize("Os")`.

Integration instructions:

1. Insert `/tmp/khicas-real-cosh-reciprocal-functions.cc` before the attribute
   preceding `integrate_compact_definite` (preserve that existing attribute).
2. Add `integrate_reciprocal_cosh(e,x,lo,hi,res,contextptr)` to early dispatch.
3. Add `yintg.o(.text.*integrate_reciprocal_cosh*)` to the AC2 selectors in both
   linker scripts.
4. Add `'  static bool integrate_reciprocal_cosh('` before the compact dispatcher
   in the direct-rule extraction catalog.

Tests: `tests/run-real-cosh-reciprocal.py --helpers
/tmp/khicas-real-cosh-reciprocal-functions.cc --verify`. Thirteen exact cases
(raw and evaluated syntax), twelve rejections, repeated on guarded 64 KiB;
seven independent 70-digit quadratures, max scaled error <1.5e-71.
The actual integration/FXCG-simplify pipeline is recorded separately in
`/tmp/khicas-cosh-pipeline.json`: thirteen cases times four configurations,
with a separate host validator on each printed result.
