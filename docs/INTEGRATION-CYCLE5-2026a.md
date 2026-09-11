# Integration cycle 5

This integration checkpoint follows the separate equation checkpoint `0593c33`.
The independent eight-case corpus was frozen after integration cycle 4 `f8af451`,
with SHA256 `a5f42f94c3405ad6dc1abbb9a153c7539c66aa023d45e0a9d3fd8ee1de6ea8a6`.
Its proofs and independent quadrature audit are in [GENERALIZATION-CYCLE5.md](GENERALIZATION-CYCLE5.md).
The calculator still runs `895c848`; nothing in this round was installed.

## Added methods

- Positive-domain affine Beta/logit substitutions and monomial pullbacks of
  weighted-circle inverse-angle integrals. Three existing/new uses now share
  one circle-value helper instead of repeating the formula.
- Frullani inverse-tangent differences with a common nonzero rational power of
  x, including negative powers and logarithmic weights through degree two.
  Endpoint reversal and Jacobian factors depend on the power's sign.
- First and second logarithmic moments of convergent exponential differences
  divided by x or x², evaluated before splitting into divergent terms.
- Derivative-weighted erf powers with bounded rational Laurent inner functions;
  the inner function need not be monotone because its primitive is used.
- Paired rectangle inverse-tangent kernels with exchanged side lengths,
  and the Gaussian derivative multiplied by an exponential of an affine erf.

Recognition checks convergence, signs, matching derivative weights and bounded
syntax. It does not look up corpus answers. The erf/rectangle proofs, parser
limits and additional independent quadratures are in
[CYCLE5-GAUSSIAN-RECTANGLE-VALIDATION.md](CYCLE5-GAUSSIAN-RECTANGLE-VALIDATION.md).

For the exponential-difference moments, differentiate
`Gamma(s)*sum(c_j*a_j^(-s))` at s=0 or s=-1 after applying
`sum(c_j)=0` and, for division by x², `sum(c_j*a_j)=0`. These cancellations
remove the Gamma pole before evaluation. Positive rates give decay at infinity;
Taylor cancellation gives integrability at zero even with up to two log factors.
Writing `S_k=sum(c_j*ln(a_j)^k)`, the x⁻¹ moments of orders 0,1,2 are
`-S_1`, `S_2/2+EulerGamma*S_1`, and
`-S_3/3-EulerGamma*S_2-(EulerGamma²+pi²/6)*S_1`.
For x⁻² replace S_k with `sum(c_j*a_j*ln(a_j)^k)`; the moments are
`S_1`, `(1-EulerGamma)*S_1-S_2/2`, and
`S_3/3+(EulerGamma-1)*S_2+(2-2*EulerGamma+EulerGamma²+pi²/6)*S_1`.

## Validation

The ordinary host baseline solved 2 of the 8 new cases exactly and left 6
unevaluated. All 8 now return exact answers, including outer FXCG `simplify`
and a guarded 64 KiB computation stack: **32/32 checks**, with printed results
validated in a separate host-Giac process. The Gaussian-erf exponential had
previously crashed on that restricted host stack despite its normal-stack
success; its bounded rule now avoids the deep general path.

Seven direct-family test runners pass, including old exact identities and
rejection conditions. Fourteen additional 70-digit quadratures independently
check erf/rectangle variants. Reports:

- [cycle5-stack-2026a.json](benchmarks/cycle5-stack-2026a.json)
- [cycle5-direct-2026a.json](benchmarks/cycle5-direct-2026a.json)
- [generalization-cycle5-before-2026a.json](benchmarks/generalization-cycle5-before-2026a.json)

Host timings, RSS and the guarded pthread stack are not CG50 performance, heap
measurements or an SH4/MMU emulator. Target sources are hashed in the reports.

## Target capacity

[SH4 map/binary report](benchmarks/resources-cycle5-2026a.json):

| Region | Used bytes | Free bytes |
| --- | ---: | ---: |
| ROM | 2,053,452 | 11,700 |
| Static RAM | 424,620 | 17,748 |
| AC2 | 2,373,020 | 186,976 |

Relative to the equation checkpoint this adds 6,084 ROM bytes and 9,920 AC2
bytes, with unchanged static RAM and the same 1,572,864-byte configured CAS heap.
All 42 selected integration helper patterns and six conversion entries are
verified in AC2; the Gaussian-erf pattern covers both existing and new entries.
There is still code space. These figures do not establish a physical resource
limit or guarantee that arbitrary integrals can be solved.

## Historical regressions on the final sources

| Corpus entries (overlap retained) | Exact | Sampled derivative | Numerical constant |
| --- | ---: | ---: | ---: |
| MIT / Princeton 168 | 160 | 7 | 1 |
| Independent 152 | 150 | 2 | 0 |
| Cycle 2: 61 | 61 | 0 | 0 |
| Cycle 3: 30 | 30 | 0 | 0 |
| Cycle 4: 16 | 16 | 0 | 0 |
| Cycle 5: 8 | 8 | 0 | 0 |
| Original user 8 | 8 | 0 | 0 |
| Extra user 2 | 2 | 0 | 0 |
| New user 5 | 5 | 0 | 0 |
| Total 450 | 440 | 9 | 1 |

No unresolved or failed checks remain in these corpus runs. These are not 450
distinct problems. Sampled derivative checks and the numerical constant are
not counted as exact symbolic proofs. The original eight cases also pass their
restricted-stack regression; the five user challenges pass all 20
normal/restricted-stack and direct/outer-simplify combinations.

The separate crash-safety suite intentionally retains eight non-elementary
indefinite integrals rather than claiming they were solved; the remaining eight
checks cancel matching integral terms exactly. All 16 checks pass.

Reports `docs/benchmarks/*cycle5*2026a.json` identify the final source hashes,
except the explicitly named before report which retains the older snapshot.
