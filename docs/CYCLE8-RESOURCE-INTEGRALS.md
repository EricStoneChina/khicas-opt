# Cycle 8: bounded polynomial pullbacks and exponential Beta integrals

These are host tests against repository integration and the actual FXCG simplification entry points. A guarded 64 KiB pthread tests stack pressure; it is not CG50 hardware validation and does not establish that the reported hardware TLB errors are fixed.

## Frozen baseline and result

The `4c97dc5` baseline failures are recorded in `docs/benchmarks/cycle8-stack-before-2026a.json`: the composed polynomial integral exhausted the GMP memory limit in normal/64 KiB runs, both plain and under `simplify`; the exponential Beta integral timed out at 30 seconds on the ordinary host stack and crashed on the 64 KiB stack.

The candidate full pipeline now returns, with one parser invocation:

- `integrate((2*x+1)*(x^2+x)^63*(1+(x^2+x)^32)^1024,x)` → `(-(1+(x^2+x)^32)^1025/1025+(1+(x^2+x)^32)^1026/1026)/32`, 55 characters.
- `integrate(exp(-2*x)*(1-exp(-x))^4096,x,0,+infinity)` → `1/16789506`, 10 characters.

Both normal and 64 KiB stacks pass with and without outer `simplify`. A maximum supported eight-term pullback using `U=x^8+x^3+x+1` remains 269 characters; a rational endpoint-singularity Beta variant returns `8/3`. All 16 full-pipeline checks have exact independent reference checks, recorded in `docs/benchmarks/cycle8-resource-candidate-2026a.json`, including source and checker hashes. The measured few tenths of a millisecond are host timings, not calculator timings.

## General identity and bounded matching

For a rational polynomial `U`, integers `k>=1`, `m>=0`, `p>=1`, and integer `q=(m+1)/k`, substitute `t=U^k` and `v=a+b*t`:

```
∫ C U' U^m (a+b U^k)^p dx
 = C/(k b^q) * Σ[j=0..q-1] binomial(q-1,j) (-a)^(q-1-j)
                         * (a+b U^k)^(p+j+1)/(p+j+1).
```

This is a polynomial identity and extends to complex `x` without root or logarithm branch choices. The existing compact-primitive dispatcher still has its own real-mode restriction; standalone tests check the identity in both context modes. The new rule requires rational coefficients, `U` degree at most 8 and at most 4 terms, `k<=64`, `m<=32767`, `p<=8192`, `q<=8`, input at most 128 nodes and output at most 512 nodes. It compares sparse coefficients against `U'` directly, without expanding or differentiating a high power. Fractional/negative powers, mismatching derivatives, complex coefficients and oversized structures decline this optional rule.

The huge reference check replaces each *whole* integer power appearing in the answer and reference with the same independent atom, then proves their difference is exactly zero using a separate host-only checker. This is a stronger algebraic equality than sampled agreement. It does not claim that the ordinary high-degree derivative checker can safely expand degree 65663. Twenty smaller parameter cases independently differentiate and normalize the complete polynomial exactly. A raw nested-power fixture avoids relying on parser pre-evaluation to exercise the degree rejection.

For positive rational `a,b`, `r=a/b` an integer in `1..16`, and rational `-1<n<=8192`,

```
∫[0,∞] exp(-a*x) (1-exp(-b*x))^n dx = B(r,n+1)/b.
B(1,n+1) = 1/(n+1)
B(j,n+1) = (j-1)/(n+j) * B(j-1,n+1).
```

There are at most 16 rational steps regardless of the binomial exponent. No large factorial or Gamma ratio is constructed. The transformed base is positive in the open integration interval, so nested rational powers are flattened only after their final base has been proved positive; endpoint convergence requires `n>-1`. Nonpositive/complex rates, nonzero phases, shifted bounds, invalid endpoint orders, noninteger or oversized rate ratios decline this rule. Existing outer dispatch handles rational scalar factors and reversed bounds.

## Final integration and simplification checks

The final helpers additionally restrict each rational coefficient numerator and denominator to 256 bits before coefficient powers or recurrences. The current direct test uses production sources:

```
python3 tests/run-cycle8-resource.py --simplify-source ksubst.cc
```

Each normal/64 KiB configuration checks 2 compact references, 20 exact differentiated variants, 12 exact Beta variants, 25 domain/resource rejections and 12 simplifier checks. The earlier candidate report above predates the 256-bit tightening and the final simplifier; it is historical diagnostic evidence. Final frozen full-pipeline results are in [cycle8-stack-2026a.json](benchmarks/cycle8-stack-2026a.json).

The final main-agent implementation supersedes the candidate's polynomial-only guard. Its preflight occurs before `_simplify` preprocessing and covers large integer powers with symbolic bases even inside variable denominators, negative powers, square roots and trigonometric shells. It retains the exact compact expression instead of expanding it. Separate bounded unary-chain simplification preserves cheap leaf algebra. No real placeholders are introduced. See [simplification safety report](SIMPLIFY-RESOURCES-2026a.md) for tests and remaining limitations.

The high-degree formal corpus uses an independent SymPy proof of the generic substituted primitive, followed by exact equality between the printed result and that proved reference using whole-power atoms. This avoids requiring the independent validator to expand the same degree-65663 polynomial. Deliberately wrong coefficient and additive-remainder outputs are rejected. Ordinary lower-degree variants still have complete exact derivative checks.
