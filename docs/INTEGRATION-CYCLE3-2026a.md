# Integration cycle 3 and simplification safety

This round starts from `895c848` (`checkpoint/integration-cycle2-2026a`).
The user requested continued host testing before another calculator installation.
No changes from this round have been transferred to the CG50.

## Independent corpus

The frozen 30-case corpus and independently verified references are described in
[GENERALIZATION-CYCLE3.md](GENERALIZATION-CYCLE3.md). Before this round, 15 cases
were exact and 15 retained an unresolved integral. The first integrated run of
the new rules returned 30 exact results, including the previously unresolved
16th logit moment and 24th Fermi moment. Final regression/build evidence is
recorded below.

The changes implement families, not expression-to-answer tables:

- Mellin integrals with a positive rational denominator power and logarithmic
  moments; endpoint convergence is checked before applying the Beta identity.
- Beta log directions, including asymmetric logit weights, using cumulants and
  a complete Bell recurrence through degree 16.
- Exponentially damped sine/cosine differences divided by `x^2`, requiring both
  constant and linear cancellation at the origin.
- Reciprocal-exponential integrals after a positive monomial substitution,
  using a finite polynomial for half-integer Bessel orders (at most 17 terms).
- Logarithmic Zeta kernels, hyperbolic log moments and inverse-tangent moments.
- Fermi/Bose integer moments through degree 64.

Two additional user examples now have exact symmetry-based results:

```text
integrate(atan(sqrt(x^2+2))/((x^2+1)*sqrt(x^2+2)),x,0,1)
  = 5*pi^2/96
integrate(x*ln(sin(x)),x,0,pi)
  = -pi^2*ln(2)/2
```

## Reported TLB error

The installed `895c848` firmware was reported to crash on
`simplify(integrate(3*ln(sin(2*x)),x))`. A host computation using a guarded
64 KiB pthread stack also crashed on this composition while the integral alone
returned a partial result. This is evidence for a costly stack path, not proof
of the exact CG50 fault address.

The new integration path keeps bare affine log-sine/log-cosine primitives
unevaluated instead of replacing them by larger unresolved integrals through
integration by parts. Simplification treats an unresolved integral as opaque.
The result is still marked **unevaluated**, never counted as a solved primitive.
Corresponding supported definite integrals continue to be evaluated exactly.

Two concrete bounds errors were also corrected:

- FXCG `tsimplify_noexpln` indexed a fresh trig-function vector using counts
  taken before protected atoms were substituted. Empty or sole-tangent vectors
  could be indexed out of bounds. Counts are refreshed and indexing is guarded.
- Integral substitution compared the fourth argument when only three arguments
  existed. Equality of two bounds is now checked only with four arguments.

The parser's preprocessing buffer no longer occupies 8192 automatic bytes or
leaves `currently_scanned` pointing to a dead local array. Flex owns the scanned
copy until scanner deletion. The SH4 compiler's individual `set_lexer_string`
frame decreased from 8320 to 128 bytes; this is **not** a measurement of the
whole computation's maximum stack usage.

The strengthened target-branch tests also exposed excessive algebraic-extension
construction in the original fifth input. Before protection,
`simplify(integrate((1+x^(1/4))^(1/3)/sqrt(x),x))` grew from a short primitive
into a 7255-character `rootof` expression. Nested fractional powers are now
temporarily treated as opaque atoms while their rational coefficient expressions
are simplified. The result is 77 characters and passes exact derivative
validation; 112 negative-real/complex branch samples, a placeholder-name collision
and the conservative atom-count boundary are also covered by tests.

The Beta recurrence needed a separate output-growth guard. The accepted input
`x^(-2/3)*(1-x)^(-4/5)*ln(x*(1-x))^8` previously generated 10,691,952 characters
inside the candidate helper (host peak RSS 425,776 KiB). Keeping non-half-integer
`Psi` values symbolic and switching to a factored integer-partition formula before
excessive expansion reduced that output to 1885 characters. The order-16 variant
has 28,939 characters. These are host observations and expression sizes, **not**
CG50 heap measurements. Four asymmetric moments have independent 70-digit
quadrature references in `tests/beta_resource_integrals.cc`.

Outer `simplify` now also protects this representation. A bounded scan estimates
arithmetic expansion before normalization. Low-cost expressions mask `Psi` as
atoms and simplify surrounding algebra; expensive expressions retain their exact
factored form. Scan and placeholder limits are explicit, with tests for existing
context bindings, deep expressions and exhaustion before a later `Psi` node.

## Script loading and host-test scope

The FXCG `read` branch previously returned constant zero without loading a
file. It now reads through `c_load_script`, checks parser size/errors and
evaluates the loaded program once. The file loader checks allocation failures
and incomplete reads. Direct tests cover the actual FXCG branch and file-loader
implementation; host `_read` is no longer used as evidence for target loading.

The optional `--target-simplify` benchmark mode includes the repository's
`simplify`, `_simplify` and FXCG `tsimplify_noexpln` entry points. Other library
dependencies remain host Giac. It is not an SH4/MMU/display emulator.

Corpus and original-user tests in this mode validate the printed target result
in a separate host-Giac process. This avoids using the target simplifier as its
own mathematical checker. Target compute time and validation time are recorded
separately. Earlier direct-target-checker results are retained in the
`*-target-validation-2026a.json` reports; their extra sampled results and one
reference-check failure must not be confused with failures of the target's
computed primitive.

## Completed regression evidence

| Suite | Exact | Sampled derivatives | Numerical constant agreement | Unresolved |
| --- | ---: | ---: | ---: | ---: |
| MIT / Princeton 168 | 160 | 7 | 1 | 0 |
| Independent 152 | 150 | 2 | 0 | 0 |
| Independent cycle 2, 61 | 61 | 0 | 0 | 0 |
| Independent cycle 3, 30 | 30 | 0 | 0 | 0 |
| Original user 8 | 8 | 0 | 0 | 0 |
| Two additional user integrals | 2 | 0 | 0 | 0 |

These are 421 test cases, with overlap between suites, not 421 distinct
integration methods. The original 8 also return identical outputs with a guarded
64 KiB computation stack. Reports are under `docs/benchmarks`, named
`calculus-cycle3`, `generalization-cycle3`, `cycle2-cycle3`,
`generalization-cycle3-after`, `user-eight-cycle3`, and `user-extra-cycle3`.

The SH4 build passes with 27 selected integration helpers verified in AC2:

| Region | Used bytes | Capacity bytes | Free bytes |
| --- | ---: | ---: | ---: |
| Main code | 2,041,864 | 2,065,152 | 23,288 |
| Static RAM | 424,620 | 442,368 | 17,748 |
| AC2 code | 2,327,252 | 2,559,996 | 232,744 |

The configured CAS heap remains 1,572,864 bytes. Binary/source hashes and
compiler-frame measurements are recorded in
[resources-cycle3-2026a.json](benchmarks/resources-cycle3-2026a.json).

The user's subsequent five-problem challenge is a separate follow-up batch:
its frozen baseline has 2 exact and 3 unresolved cases. It is not included in
the completed cycle-3 table above.

Stack tests now run CAS computation in a pthread with a fixed stack and a guard
page. Earlier pre-exec `RLIMIT_STACK` tests could fail in the dynamic loader due
to address-layout variation; those failures must not be attributed to the CAS
or used as evidence of CG50 resource exhaustion.

## Remaining limits

- Non-elementary affine log-sine/log-cosine primitives remain unresolved;
  Clausen/dilogarithm support has not been added.
- Bounded helper orders describe implementation budgets, not a proof that the
  physical calculator cannot handle larger orders or another representation.
- No real-device latency or peak CAS heap measurements exist for this round.
- The user's previous real-device report for `integrate(x^2,x,0,1)` was
  `0.0703125` seconds, with no allocation/fragmentation failures. It measures
  installed checkpoint `895c848`, not these uninstalled changes. Raw evidence:
  [cg50-device-first-2026a.json](benchmarks/cg50-device-first-2026a.json).

There was still code-region headroom at the preceding checkpoint. This round
must not be described as having reached the CG50's actual resource limit.
