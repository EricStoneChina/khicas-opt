# Integration cycle 6

This round follows `dff6974` (`checkpoint/integration-cycle5-2026a`).
The requested standalone list is [PASSED-INTEGRALS.md](PASSED-INTEGRALS.md).
No changes were installed on the calculator, which still runs `895c848`.

## Failures reproduced and resolved

The independent eight-case corpus was frozen before engine use, SHA256
`541d969e545046f4c1c1e03fa58ab749ce198c166c58615ddc68b3b73ee0f345`.
Its exact references, convergence proofs and 18 independent 80/110-digit
quadratures are in [GENERALIZATION-CYCLE6.md](GENERALIZATION-CYCLE6.md).

Normal-stack baseline integration solved only two cases; the other six retained
unevaluated integrals. All eight crashed when the computation stack was limited
to a guarded 64 KiB. The saved [before report](benchmarks/cycle6-stack-before-2026a.json)
contains all 32 plain/outer-simplify and normal/restricted-stack attempts.

After the new bounded paths, all eight return exact results in all four modes:
**32/32 exact**, with independent verification of printed results, in the
[final report](benchmarks/cycle6-stack-2026a.json). Added families include:

- Gaussian–atan integration by parts, with a bounded recurrence for odd
  moments through x^17 and a single shared erfc term.
- Fermi/Bose logarithmic moments after a positive monomial substitution,
  with integer Mellin order through 12.
- Affine-weighted quarter-wave logarithmic sine/cosine moments, opposite
  logarithm products and squares, quarter-grid logarithms of 1±sin/cos,
  and a Möbius inverse-tangent measure identity.
- Mixed linear/quadratic Mellin kernels with logarithmic weights through
  degree two, and second/third-order oscillatory cancellation before splitting
  terms which would diverge individually.

Proofs and explicit limits: [Gaussian/Fermi](CYCLE6-GAUSSIAN-FERMI.md),
[logarithmic families](CYCLE6-LOG-VALIDATION.md),
[Mellin/oscillatory families](CYCLE6-MELLIN-OSCILLATORY.md).

## Performance, expression size and numerical stability

A general product-rule/addition-formula path returns
`sin(100*x)*sin(x)^100/100` for `sin(101*x)*sin(x)^99`. The result shrinks
from 6188 to 25 characters, a **99.6% expression-size reduction**. A bounded
simplification guard also preserves analogous cosine products. This is not
a measured CG50 peak-memory or timing percentage. The general derivative
checker still reports sampled verification for high orders; separate exact
reference equality and the general identity proof are recorded in
[HIGH-FREQUENCY-TRIG-2026a.md](HIGH-FREQUENCY-TRIG-2026a.md).

The reciprocal-cosh rule now gives `ln(2+sqrt(3))/sqrt(3)` for the historical
2016-Q20 input, avoiding cancelling complex logarithms. Parameter domains and
all three real branches are in [RECIPROCAL-COSH-2026a.md](RECIPROCAL-COSH-2026a.md).

An independent audit found numerical cancellation in the first Mellin formula
near its removable internal poles. The implementation now uses a positive
sine-product formula and shares its value and derivatives, avoiding up to
three repeated trigonometric evaluations for a scaled second log moment.
A pure-power parser handles evaluation spellings such as `(x^(1/18))^17`
without exponentiating coefficients or widening the general coefficient-power
budget. All 42 audited boundary cases now pass, with maximum scaled error
about 2.85e-12 against independent 80-digit quadrature.

Eight additional extreme-parameter full-pipeline runs pass on 64 KiB stacks.
Their outer `simplify` still increases output from 385–409 to 1659–1868
characters and takes about 40–46 ms in that host audit. This is a remaining
optimization opportunity, not a demonstrated physical-resource limit. Details:
[extreme-parameter report](benchmarks/cycle6-mellin-extremes-2026a.json).

## Final verification

| Corpus entries (with overlap) | Exact | Sampled derivative |
| --- | ---: | ---: |
| MIT / Princeton 168 | 164 | 4 |
| Independent 152 | 151 | 1 |
| Cycle 2: 61 | 61 | 0 |
| Cycle 3: 30 | 30 | 0 |
| Cycle 4: 16 | 16 | 0 |
| Cycle 5: 8 | 8 | 0 |
| Cycle 6: 8 | 8 | 0 |
| Original user 8 | 8 | 0 |
| Extra user 2 | 2 | 0 |
| New user 5 | 5 | 0 |
| Total 458 | 453 | 5 |

There are no unresolved/failed cases in these formal corpus runs. The table
does not claim 458 distinct mathematical problems. The separate crash-safety
suite still retains non-elementary logarithmic-trigonometric indefinite
integrals; those are explicitly excluded from the passed list.

Four historical sampled checks became exact because the independent verifier
now tries the exact hyperbolic-to-exponential identity on bounded expressions.
Both the candidate and reference derivatives must equal the integrand exactly.
Four deliberately perturbed primitives are rejected. This is stronger
verification, not four new solver implementations; all new reports record
the verifier source SHA separately.

The prior Cycle 5 corpus passes all 32 direct/outer and normal/64 KiB runs.
All 40 equation-conversion/outer-simplification stack checks and the named
parameter-display adapter/plotparam compatibility check also pass after the
new simplification guard.
Original user cases, nested powers, special functions, real roots and high
trigonometric powers pass their separate safety suites. Five final direct
family runners pass, including 39 additional independent quadratures; see
[cycle6-direct-2026a.json](benchmarks/cycle6-direct-2026a.json).
The six legacy direct-rule suites pass after replacing one obsolete rejection
expectation for the newly solved half-period weighted log-sine integral.

All final report source/verifier hashes match the final working sources. These
are host tests of actual repository integration/FXCG-simplification entry
points with host Giac dependencies, not SH4/MMU emulation or device timings.

## CG50 binary capacity

[Final SH4 linker/binary report](benchmarks/resources-cycle6-2026a.json):

| Region | Used bytes | Remaining bytes |
| --- | ---: | ---: |
| ROM | 2,055,452 | 9,700 |
| Static RAM | 424,620 | 17,748 |
| AC2 | 2,391,760 | 168,236 |

Relative to Cycle 5: +2000 ROM bytes, +18740 AC2 bytes, unchanged static RAM.
All 52 selected integration patterns and six conversion entries are checked
in AC2. CAS heap configuration remains 1,572,864 bytes. There is still
code capacity; these data do not prove the calculator has reached its physical
limit, or that arbitrary symbolic integration is supported.
