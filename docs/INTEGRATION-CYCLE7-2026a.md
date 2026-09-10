# Integration cycle 7

This checkpoint follows `c977aa0`, `checkpoint/integration-cycle6-2026a`.
The requested standalone [passed-integral list](PASSED-INTEGRALS.md) now has
**495 entries: 490 exact, 5 sampled derivative checks**, with overlap.
Whitespace-normalized inputs have 486 distinct strings, not necessarily
486 distinct mathematical problems. Unresolved safety cases are excluded.

## Frozen new corpus and solutions

The independent eight-case corpus was frozen before engine evaluation, SHA256
`2fbf0b053156b3c9fd203b9664b8a55c8ac9b0443f1a919f2ca4b95e962beb2c`.
References and convergence proofs: [GENERALIZATION-CYCLE7.md](GENERALIZATION-CYCLE7.md).
The [baseline](benchmarks/cycle7-stack-before-2026a.json) has 18/32 exact runs;
three cases fail in all modes and one works normally but fails on 64 KiB.
The [final run](benchmarks/cycle7-stack-2026a.json) is **32/32 exact**.

The nested 17th/19th-root primitive, quadratic-radical atan primitive,
damped sine cancellation and composed degree-65567 Beta substitution already
worked in the baseline. They are coverage evidence, not new implementations.
New generalized paths cover:

- Two erf/erfc tails after a positive monomial substitution, including signed
  erf arguments and evaluated `1-erf` syntax.
- Logarithms of a positive binomial denominator through order four, reusing
  the existing Beta recurrence and its convergence guards.
- Möbius-logarithm moments against affine Cauchy kernels through order eight,
  with signed/scaled intervals and positive logarithm arguments.
- Equal-amplitude sine/cosine sums inside a logarithm, using quarter-period
  moments and explicit positive-wave checks unless an absolute value is given.

The external constant extracted by the dispatcher is now arithmetically
normalized before its rational check, so spellings containing `inv(3)` use the
same path as exact `1/3`. This was found by a scaled variant, not by changing
the frozen test references. Proofs, limits and rejection tests:
[Gaussian tails/Mellin](CYCLE7-TAIL-MELLIN.md),
[Möbius and log-trig](CYCLE7-LOG-VALIDATION.md).

## Basic finite integrals and host stack diagnosis

A supplementary 14-case elementary audit found 24 failures in its 56 baseline
attempts. The normal stack solved all cases. For `x^2`, the same 64 KiB failure
occurs in Cycle5, with a backtrace into glibc lazy symbol resolution from the
generic endpoint limit/parser path. Diagnostic eager binding removes that
host trigger; this does not establish an equivalent SH4 device crash.
Normal tests keep the default linker environment.

Bounded sparse polynomial endpoint evaluation now avoids symbolic primitives,
interval assumptions and generic limits. Finite affine elementary functions,
positive quadratic reciprocal kernels and domain-checked affine powers use
small endpoint formulas. The polynomial recognizer permits at most degree64,
32 terms and 512 recognition/arithmetic steps. It preserves variable poles
by refusing to cancel variable denominators. Fractional powers and logarithms
have explicit interval-domain checks; `sqrt(x^2)` is never blindly replaced
by `x` on a negative interval.

The [final basic audit](benchmarks/basic-finite-cycle7-2026a.json) is **56/56 exact**.
`x^2` parsing calls decrease from three to one, and the seven elementary cases
also use one parse. These are work reductions, not device speed percentages.
See [baseline diagnosis](BASELINE-FINITE-INTEGRALS.md) and
[elementary formulas](FINITE-ELEMENTARY-2026a.md).

## Constant simplification without expression expansion

Complex closed expressions containing two through eight proved-real
`sin(q*pi)`/`cos(q*pi)` atoms can use bounded rational normalization with
fresh temporary identifiers. Only explicitly bounded rational-angle syntax
is admitted. Expressions with variables, small identities, unsupported angles
and prior special-function guards retain their existing paths. The result is
used only when it does not exceed the input size.

Four previously audited Mellin extremes shrink from **1659–1868 characters
to 380–393** after outer simplify. Their final host measurements are about
0.66–0.71 ms, compared with the earlier 40–46 ms host audit; these are separate
host runs and are not a CG50 speedup measurement. All 16 direct/outer and
normal/64 KiB runs are exactly equivalent to the frozen original closed
results, and match the original mp80 quadrature references without changing
the tolerance. [Final constant report](benchmarks/closed-trig-constants-cycle7-2026a.json).

Checks cover algebraic cancellation, nonzero remainders, temporary-name
collisions, variables, complex parameters and negative roots in both complex
modes. Three low-order checks retain an existing baseline simplification gap;
they are checked against the baseline rather than mislabeled new exact solves.

## Final verification

All reports below use the same integration/simplification source hashes and
independent printed-result validator. The full suite reused one frozen target
binary and a separate validator binary; no baseline-source substitution was
used to produce a final report.

| Corpus | Exact | Sampled |
| --- | ---: | ---: |
| MIT / Princeton | 164 | 4 |
| Independent generalization | 151 | 1 |
| Cycles 2, 3, 4 | 107 | 0 |
| Cycles 5, 6, 7 | 24 | 0 |
| Original/extra user cases | 15 | 0 |
| Basic finite integrals | 14 | 0 |
| Gaussian-tail / denominator-log variants | 15 | 0 |
| Total, with overlap | 490 | 5 |

The 29 additional Möbius/log-trig variants pass 116 full-pipeline runs.
Direct tests include 16 polynomial and 29 elementary identities in raw and
evaluated forms, with 12 and 21 rejection cases respectively. Tail/Mellin
checks include 15 identities and 17 rejections. Numerical checks use independent
80/110-digit quadratures. The original definite-rule suites pass.
[Direct and regression outputs](benchmarks/cycle7-direct-2026a.json).

All 40 equation-conversion direct/outer stack cases pass, as do named
parameter views, selection of original coordinate data and plotparam
compatibility. Nested roots, equalities, high-frequency trigonometry and
special-function simplification checks pass. These are host adapter checks,
not rendered CG50 screen measurements. The separate 16-run log-trig safety
suite passes while explicitly retaining four non-elementary indefinite
integrals, each tested on two stacks; retained integrals are not counted solved.

## Actual linked capacity and remaining gaps

| Region | Used bytes | Capacity | Remaining |
| --- | ---: | ---: | ---: |
| Main ROM | 2,057,940 | 2,065,152 | 7,212 |
| Static RAM | 424,620 | 442,368 | 17,748 |
| AC2 | 2,406,692 | 2,559,996 | 153,304 |

The configured CAS heap remains **1,572,864 bytes**. Relative to Cycle6, this
round uses 2488 more ROM bytes and 14932 more AC2 bytes, with unchanged static
RAM. Polynomial auxiliary functions moved to AC2; every selected function and
compiler clone is checked by symbol name and section. The final build verifies
61 integration helper selections and all six conversion entries.
[SH4 binary/map/frame report](benchmarks/resources-cycle7-2026a.json).

**The physical resource limit has not been reached.** Free linked capacity
alone does not prove an arbitrary symbolic problem fits the runtime heap or
stack. This checkpoint does not claim universal integration or globally
optimal performance. Specific remaining gaps are:

1. Arbitrary logarithmic-trigonometric indefinite integrals, including the
   reported `3*ln(sin(2*x))`, are retained safely in host tests; no new closed
   special-function primitive is provided for that family.
2. Five historical derivative checks remain sampled: the real cube-root
   trigonometric example, variable-exponent power, a branch-sensitive inverse
   trigonometric radical, the high-frequency product and a quartic real-root
   sign case. Their domains and references remain in the passed list.
3. Every bounded recognizer has documented degree, logarithmic-order, syntax
   or domain limits. Exceeding one is an algorithmic gap, not proof of a
   hardware impossibility. Generic integration is still not universally bounded.
4. General implicit curves need not have textbook elementary parametrizations.
   Generic conversion fallback and the existing bare-variable/program-result
   parameter-label limitation remain; see the equation checkpoint reports.
5. New C3–C7 code, crash guards and named equation rendering have not been
   installed or measured on the calculator. It still runs `895c848` (Cycle2).
   No claim is made that the reported TLB failure is confirmed fixed on-device.
