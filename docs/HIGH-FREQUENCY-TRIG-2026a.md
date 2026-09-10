# Compact integer trigonometric powers

Let u=a*x+b, a a nonzero rational constant, and n an integer in 1..32765.
The product rule and addition formulas give

    d[sin((n+1)*u)*sin(u)^(n+1)/(a*(n+1))]/dx
      = sin(u)^n*(cos((n+1)*u)*sin(u)+sin((n+1)*u)*cos(u))
      = sin((n+2)*u)*sin(u)^n.

    d[sin((n+1)*u)*cos(u)^(n+1)/(a*(n+1))]/dx
      = cos(u)^n*(cos((n+1)*u)*cos(u)-sin((n+1)*u)*sin(u))
      = cos((n+2)*u)*cos(u)^n.

These are entire-function identities for integer n and thus also hold at complex u. A negative carrier phase changes the sine rule's sign and leaves the cosine rule unchanged. Both affine slope and phase are checked; equality of frequencies alone is insufficient. Noninteger/negative/out-of-budget n and non-affine arguments fall through. n=0 remains handled by the existing elementary path. The existing compact dispatcher still retains its real-mode restriction.

The recognizer bounds input node count and never expands the integer trig power or allocates a degree-indexed table. It reuses the engine's affine coefficient matching. Rational and other already extracted finite constant multipliers are restored after matching. This new branch goes before the old rational-coefficient-only check: a raw trailing /2 may be represented as symbolic inv(2), and is mathematically a valid scalar here.

For the cosine primitive the old outer simplifier changes cos(u)^(2m) to (1-sin(u)^2)^m and expands. The proposed simplify budget keeps an already compact product (<=4 factors, <=64 nodes, one positive integer trig power >=32) unchanged. Its other factors must be scalar atoms/rational inverses or direct sin/cos nodes. It introduces no real placeholders and hence no additional complex-branch assumptions. Low-order trig identities continue through the existing simplifier.

Validation keeps two separate conclusions:
- The candidate is exactly equal to the compact reference primitive; the general identity above is independently verified symbolically, and n=1..8 are differentiated and trig-expanded to exact zero in the unit tests.
- The existing generic high-order derivative checker still reports sampled. That status is not promoted or weakened.

C5 2011-Q22's prior output had 6188 characters. The new integrate and outer simplify outputs both have 25 characters (99.596% smaller). Full candidate tests cover eight inputs, each with/without outer simplify and on normal/guarded 64 KiB stacks (32 runs). Timings are host measurements with repository FXCG entrypoints and host Giac dependencies, not CG50 hardware measurements.

## Final validation

- `tests/run-high-frequency-trig.py --helpers /tmp/khicas-highfreq-trig-functions.cc --simplify-source /tmp/khicas-highfreq-ksubst.cc`: 56 exact primitive cases and 13 rejection guards, plus 12 real/complex compact-product cases; repeated on normal and guarded 64 KiB stacks. Each test executable has a 60-second timeout.
- `tests/run-simplify-nested.py --source /tmp/khicas-highfreq-ksubst.cc`: the previous nested-power, Psi, closed inverse-angle, real-root and equation cases pass, together with the new trig-product test.
- `/tmp/khicas-highfreq-results.json`: eight integrands times two simplify modes times two stack sizes; all 32 computations finish with exact equality to the reference primitive and matching normal/64 KiB outputs. Each computation/reference-identity check has a 25-second timeout. The independent generic derivative check has a 30-second timeout: six large-order cases remain `sampled`; the small shifted case and n=0 elementary fallback are `exact`. No validation status was weakened.

| Expression family | integrate output characters | outer simplify output characters |
|---|---:|---:|
| Original sine n=99 | 25 | 25 |
| Cosine n=99 | 25 | 25 |
| Affine phase, sine n=99 | 35 | 35 |
| Negative carrier, sine n=99 | 36 | 36 |
| Rational scalar, sine n=99 | 29 | 29 |
| Pi scalar, sine n=99 | 28 | 28 |

Before the proposed simplify guard, the cosine primitive's outer simplification grows from 25 to 1609 characters. The guard prevents that optional expansion. The small shifted n=5 case still uses ordinary simplification (35 to 114 characters), demonstrating that the guard is restricted to large powers.

## Frozen candidate SHA256

These hashes identify the reviewed candidate artifacts before root integration. The full ksubst snapshot is provided for testing; merge the insertion block into current production rather than replacing unrelated concurrent changes.

- `/tmp/khicas-highfreq-trig-functions.cc`: `1ac869db5546d3d9b7a1406b02724527f73cac030c8ef53e8e33598fc957f5a5`
- `/tmp/khicas-highfreq-compact-primitive.cc`: `4d96b55e7e8809176336b1ee311ed6305b0cceccf2e0ca9c6dbe9ed3adaeb410`
- `/tmp/khicas-highfreq-simplify-guard.txt`: `bedb6afa24509012b3c3f1216c47a0fb75037ba5b11d5caec8550af40c32d8f8`
- `/tmp/khicas-highfreq-ksubst.cc`: `a13ba5f43cd5ebd9eb979a639f7147d8ce81af0fa54d7f238e7389ed33c32eb8`
