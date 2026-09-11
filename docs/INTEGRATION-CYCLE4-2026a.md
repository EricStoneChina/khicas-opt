# Integration cycle 4: exact results and bounded computation paths

This round starts at `0e5ed65` (`checkpoint/integration-cycle3-2026a`).
The calculator still has `895c848`; none of these changes were installed.
Host tests exercise the repository integration, normalization and FXCG
simplification entry points with host Giac dependencies. They are not an SH4
MMU/display emulator, and their timings and RSS are not CG50 measurements.

## Added coverage

The five user inputs and independent proofs are in
[USER-CHALLENGE-INTEGRALS.md](USER-CHALLENGE-INTEGRALS.md). Their exact answers
are `5*pi^2/96`, `pi/2`, `5*pi^2/24`, `pi*ln(pi)/2`, and
`pi*ln(2*pi)/4+pi*ln(Gamma(3/4)/Gamma(1/4))/2`.
Returning the original `integrate(...)` on installed firmware means that
firmware did not solve the integral; it does not contradict the newer source
results. In particular, the first rule was added after the installed checkpoint.

The independent 16-case corpus was generated after the preceding checkpoint
without querying the engine. Its references, convergence proofs and 80-digit
quadrature audit are in [GENERALIZATION-CYCLE4.md](GENERALIZATION-CYCLE4.md).
The ordinary host baseline solved 6 cases exactly and left 10 unevaluated.

New and extended methods include:

- Complete-period reciprocals of a positive or negative nonsingular affine
  sine/cosine denominator, without visiting each period separately.
- Weighted-circle inverse-angle identities, including affine arguments,
  rational half-angle coordinates and principal-angle complements.
- Positive-square symmetry for both `atan(k*sqrt(R))` and `atan(k/sqrt(R))`.
- Distinct positive-binomial Mellin kernels, including the removable logarithmic
  limit at Mellin exponent 1 and the full convergence interval `(0,2)`.
- Inverse-tangent Frullani differences with logarithmic weights through degree
  2; iterated-logarithm Mellin and Frullani moments through degree 2.
- Mixed Beta logarithmic directions through total degree 3 using joint
  cumulants; single directions retain the earlier degree-16 recurrence.
- Gaussian/erf convolution, translated centered erf products, and a bounded
  reciprocal-monomial square in the inverse-Gaussian exponent.
- Bounded trigonometric product-to-sum before Laplace integration, and direct
  exponential differences divided by `x` or `x^2`.

These are domain-checked families, not lookups of the corpus answers.
Ordinary convergence is required before differences are integrated; cancelling
terms are not evaluated as separate divergent integrals.

## Additional failures found by stronger tests

The first ordinary host run returned 16 exact answers, but the stronger runner
also composes `simplify` and uses a 64 KiB pthread computation stack with a 4 KiB
guard page. Its saved failure report is
[cycle4-stack-before-2026a.json](benchmarks/cycle4-stack-before-2026a.json).
Of 64 runs, 8 failed the exact-result requirement:

- Two cases crashed only with the restricted computation stack: distinct
  fractional-power denominator factors, and a three-exponential difference.
  Direct Mellin and cancellation rules now avoid those general search paths.
- Two inverse-angle answers became long complex-log expressions under outer
  `simplify`, with cancelling imaginary parts. Independent verification could
  establish numerical agreement but no longer report an exact symbolic check.
  Proven-real closed inverse-angle atoms are now protected in arithmetic with
  repeated angle factors or integer powers. Simple inverse-angle sum identities
  continue to simplify normally. Complex angles are not masked as real variables.

The user complete-period example also needed a direct rule after its initial
ordinary-host success failed under the restricted stack. These observations
show why an ordinary host result alone is insufficient evidence of suitability
for the calculator. They do not identify a real-device fault address.

Mixed Beta moments share the existing pre-expansion budget: if the estimated
arithmetic expansion exceeds 64 terms, retain exact factored output. A tested
near-boundary parameter example shrank from 8275 to 3154 characters, a 61.9%
reduction. This is expression size, not a measured reduction in CG50 peak heap.
The same result survives outer target simplification and the restricted host
stack; independent quadrature checks its value.

## Reproduction

`tests/run-calculus.py --target-simplify` records exact, sampled and numerical
checks separately. Mathematical verification runs in a separate host-Giac
process using the printed target result, not a recomputed input integral.

```sh
python3 tests/run-user-challenge.py --report REPORT.json
python3 tests/run-user-challenge.py --corpus tests/generalization-cycle4.json --report STACK.json
python3 tests/run-calculus.py --target-simplify --corpus tests/generalization-cycle4.json --strict --report CORPUS.json
python3 tests/generate-coverage-cycle4.py --check --verify
TOOLS_DIR=/tmp/khicas-toolchain python3 tools/build.py optimized khicas50.g3a khicas50.ac2
python3 tools/check-resources.py --report RESOURCES.json
```

Direct family tests additionally check parameter variants and rejected domains.
`run-cycle4-gaussian.py --verify` has 11 independent 80-digit Gaussian/erf
quadratures; the corpus generator has 18 independent quadratures for 16 cases.

`bench/CG50USER.xw` prepares short future device commands `ct(1)` to `ct(5)`.
The actual FXCG read branch is tested for loading both benchmark scripts and
recognizing their functions. The file has not been transferred or tested on
the calculator in this round.

## Completed verification and target capacity

All final reports use the same production sources. The following are corpus
entries, with overlap across corpora, not 442 distinct independent problems:

| Corpus | Exact | Sampled derivative | Numerical constant |
| --- | ---: | ---: | ---: |
| MIT/Princeton 168 | 160 | 7 | 1 |
| Independent 152 | 150 | 2 | 0 |
| Cycle 2, 61 | 61 | 0 | 0 |
| Cycle 3, 30 | 30 | 0 | 0 |
| Cycle 4, 16 | 16 | 0 | 0 |
| Original user 8 | 8 | 0 | 0 |
| Extra user 2 | 2 | 0 | 0 |
| New user 5 | 5 | 0 | 0 |
| Total 442 | 432 | 9 | 1 |

There are no unresolved results, timeouts or failed checks in these final
corpus runs. Sampled and numerical checks are not counted as exact results.
The 16 cycle-4 cases and five new user cases also pass all 84 combinations of
direct integration/outer simplification and normal/guarded 64 KiB host stacks.
The original eight inputs and the previously reported log-trig crash variants
pass their separate guarded-stack regression tests.

Reports are in `docs/benchmarks/*cycle4*2026a.json` and
`user-challenge-after-2026a.json`; `*-before-*` reports preserve earlier failures.
The new simplified-angle tests also cover ordinary complement identities and
complex `asin`, `acos`, `atan` and `Psi`. In particular, a complex special
function must not turn `sqrt(P^2)` into `abs(P)` through a real placeholder.

Both SH4 artifacts build successfully. The final linker report is
[resources-cycle4-2026a.json](benchmarks/resources-cycle4-2026a.json):

| Region | Used bytes | Remaining bytes |
| --- | ---: | ---: |
| ROM | 2,048,296 | 16,856 |
| Static RAM | 424,620 | 17,748 |
| AC2 code | 2,346,480 | 213,516 |

All 37 selected integration helpers are placed in AC2. Configured CAS heap
remains 1,572,864 bytes. Relative to cycle 3, the new functionality adds 6,432
ROM bytes and 19,228 AC2 bytes, with unchanged static RAM. This is a functional
coverage/robustness improvement with additional code cost, not a claim that
the whole executable became smaller or all integrals became uniformly faster.

## Remaining gaps

- Non-elementary affine log-sine/log-cosine primitives remain unevaluated. The
  earlier safety fix preserves the integral instead of expanding a partial
  integration-by-parts result; it is not a solved primitive.
- Mixed Beta orders above 3, general shifted two-erf products, unmatched
  inverse-angle endpoints, and kernels outside the bounded recognizers can
  still require the older solver or remain unevaluated.
- Term/order/scan limits are implementation budgets, not proofs of physical
  impossibility. A finite corpus cannot establish universal integration coverage.
- Actual-device latency, maximum stack, allocator peak and TLB behavior of
  this round remain unmeasured. Hardware testing is deferred as requested.
- Linker headroom must be reported from the built artifacts. Available code
  space means the CG50 physical resource limit has not been reached.
