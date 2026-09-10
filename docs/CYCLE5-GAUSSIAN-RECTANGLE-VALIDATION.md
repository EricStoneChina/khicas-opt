# Cycle 5: bounded erf chains and paired rectangles

The frozen Cycle 5 references are unchanged. These tests exercise families derived
from the identities below, independently of the integration engine.

## Derivative-weighted erf powers

For real differentiable `U` and an integer `n >= 0`,

```
∫ c U'(x) exp(-U(x)^2) erf(U(x))^n dx
  = c sqrt(pi) erf(U(x))^(n+1)/(2(n+1)).
```

This follows directly from `erf'(u)=2 exp(-u²)/sqrt(pi)`. Monotonicity of `U`
is unnecessary: the displayed expression is a primitive. On the positive axis,
a finite rational Laurent sum is real and smooth, and its limits at zero and
infinity follow from its lowest and highest nonzero powers. Its erf is bounded,
so these limits also prove convergence of each endpoint of the improper integral.

For the corpus case `U=sqrt(x)-1/sqrt(x)`, the supplied weight is `2 U'` and
`erf(U)` tends to `-1` and `1`, giving `2 sqrt(pi)/3`.

The bounded recognizer accepts at most four powers in `U`, powers of erf from
1 through 16, and positive finite rational bounds or the endpoints `0,+infinity`.
The input has at most 128 expression nodes. Laurent parsing shares a budget of
64 syntax visits, keeps at most 32 distinct powers, and permits at most 64
pairwise products in each convolution. Primitive evaluation uses two endpoint
values, without symbolic differentiation or expansion of an arbitrary function.
Powers of erf must be direct integer powers; nested fractional powers are not
flattened. Negative-axis intervals, unmatched derivative weights, and unsupported
powers are rejected by this recognizer and left to other integration methods.

## Paired rectangle identity

For positive `A,B,C`, define

```
J_C(A,B) = ∫₀ᴬ atan(B/sqrt(u²+2C))/((u²+C)*sqrt(u²+2C)) du.
```

Writing the arctangent quotient as an integral gives

```
J_C(A,B) = ∫₀ᴬ ∫₀ᴮ 1/((u²+C)*(u²+v²+2C)) dv du.
```

Reflect the rectangle in `J_C(B,A)`, then add the kernels on the same rectangle:

```
[1/(u²+C)+1/(v²+C)]/(u²+v²+2C) = 1/((u²+C)*(v²+C)).
```

Consequently,

```
J_C(A,B)+J_C(B,A) = atan(A/sqrt(C))*atan(B/sqrt(C))/C.
```

All kernels are continuous on the finite rectangle because `C>0`.
The two terms must remain paired: for unequal sides, either individual term is
generally different from half the answer. Recognition checks exchanged positive
side lengths, the same positive `C`, the same outside multiplier, and matching
integration bounds. The multiplier can have either sign. Each term has precisely
one atan, one inverse square root and one inverse quadratic, and each quadratic
uses the existing bounded rational polynomial parser.

## Reproduction

```
python3 tests/run-cycle5-gaussian-rectangle.py --verify
```

Before source integration, append
`--helpers /tmp/khicas-cycle5-gaussian-rectangle-functions.cc`.
The C++ tests check 12 exact identities with raw and evaluated syntax and 12
rejected conditions, then repeat them on a guarded 64 KiB computation stack.
The optional audit uses mpmath at 70 decimal digits for eight independent
quadratures, including reversed erf endpoints, an odd erf power, a nonmonotone
inner function, unequal rectangle sides, scaled denominators and negative
outside multipliers. Host stack tests do not measure SH4 execution time.

## Exponential of an affine erf

For rational `a,b,c,d`, with `a != 0`,

```
∫₋∞^∞ exp(-(a*x+b)^2+c*erf(a*x+b)+d) dx
  = exp(d)*sqrt(pi)*sinh(c)/(abs(a)*c).
```

At `c=0`, use the continuous value `exp(d)*sqrt(pi)/abs(a)`.
Set `t=a*x+b`, reversing the endpoints if `a<0`, then `u=erf(t)`.
The remaining integral is `exp(d)*sqrt(pi)/(2*abs(a))*∫₋₁¹ exp(c*u) du`.
This also proves ordinary convergence, since the erf exponential is bounded
and the remaining factor is Gaussian. Both signs of `a` and `c` are supported.

The shortcut examines at most 96 input nodes and eight exponent terms. It
requires one affine erf term and exactly the matching Gaussian quadratic;
constant offsets are permitted. The existing bounded quadratic parser handles
either a squared affine or its expanded polynomial. When `c=0` removes the erf
term, the same quadratic parser provides the Gaussian limit. Complex constants,
nonlinear erf arguments, mismatched quadratic coefficients, multiple different
erf terms and non-full-line bounds are rejected by this shortcut.

```
python3 tests/run-cycle5-erf-exponential.py --verify
```

The direct tests cover eight exact cases using both raw and normalized exponent
syntax and ten rejection checks, on normal and guarded 64 KiB stacks. Six
independent 70-digit quadratures have maximum scaled error below `2.1e-71`.
