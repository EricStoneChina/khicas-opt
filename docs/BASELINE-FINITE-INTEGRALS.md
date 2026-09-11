# Basic finite-integral audit

The independently elementary 14-case corpus is
[`basic-finite-integrals.json`](../tests/basic-finite-integrals.json).
Its references follow from `x^(n+1)/(n+1)`, `-cos(x)`, `sin(x)`, `exp(x)`,
`ln(x)` on positive arguments, `atan(x)`, `2*x^(3/2)/3` on nonnegative
arguments, and `x*ln(x)-x` on positive arguments. All bounds and orientations
are retained in the test data.

The [initial default-environment report](benchmarks/basic-finite-before-2026a.json)
has 24 failures among 56 runs: all 14 cases pass normally, but 12 fail both
plain and outer-simplified on the guarded 64 KiB host stack. The constant and
symmetric cubic cases pass in both modes. This supplementary audit was added
after Cycle6; those inputs were not part of the previously published formal
458-entry pass list.

## What the stack failure means

For `integrate(x^2,x,0,1)`, both `dff6974` (Cycle5) and `c977aa0` (Cycle6)
show the same default-environment behavior. GDB follows two nested
`_integrate_` calls into generic endpoint `limit`, an internal string parse,
`find_or_make_symbol`, and glibc `_dl_runtime_resolve_xsavec`. The diagnostic
setting `LD_BIND_NOW=1` allows both versions to finish on the guarded stack.
The [loader comparison](benchmarks/basic-finite-loader-diagnosis-2026a.json)
records both environments.

The immediate trigger therefore includes host lazy dynamic-linking stack
cost. CG50 uses a different SH4 execution environment and does not use this
host glibc loader. These crashes are neither proof of a Cycle6 regression
nor proof of the same device failure. Normal verification retains the default
linker environment; eager binding is used only for diagnosis.

## Finite polynomial shortcut

A rational polynomial is continuous at finite rational endpoints, so exact
endpoint powers can replace interval assumptions, symbolic primitive
construction and generic limits:

```
sum(c_k*(h^(k+1)-l^(k+1))/(k+1)).
```

The sparse recognizer accepts zero, constants, linear, factored and powered
polynomials, with degree at most 64, at most 32 retained terms, depth at most
8 and a shared 512-step arithmetic/recognition budget. It uses repeated
squaring and permits only nonzero rational constant denominators. It never
cancels a variable denominator to infer continuity. Reversed bounds preserve
the integral's orientation. Large exact integer costs remain dependent on
operand bit length; these are work budgets, not physical-capacity claims.

`tests/run-finite-polynomial.py` independently checks 16 exact raw/evaluated
cases, twelve rejected domains/resource cases, and repeats on 64 KiB.
The measured parser count for the quadratic falls from three to one by
removing the two internal endpoint parses. This is a reduction in work,
not a claimed device speedup percentage.
