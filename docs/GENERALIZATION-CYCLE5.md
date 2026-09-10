# Independent integration audit: cycle 5

This bounded round contains **eight exact, ordinarily convergent definite
integrals**. It was generated after `f8af451`
(`checkpoint/integration-cycle4-2026a`), without querying or running the tested
engine. The corpus was frozen after independent reference validation and before
engine evaluation:

```text
tests/generalization-cycle5.json
SHA256 a5f42f94c3405ad6dc1abbb9a153c7539c66aa023d45e0a9d3fd8ee1de6ea8a6
```

The generator is `tests/generate-coverage-cycle5.py`. It preserves all earlier
corpora and production code. Six supplied references are elementary expressions;
two contain Euler's constant and are marked `special-function-closed-form`.
Some integrands contain erf even when their final reference is elementary.
There are no guessed closed forms, numerical-only answer placeholders, or
Cauchy-principal-value substitutions for divergent integrals.

The scope is composition and structural changes to existing families, not eight
claims of unrelated new integration methods:

| Case family | Change exercised |
| --- | --- |
| `gaussian-erf-exponential` | Exponential of erf under its Gaussian derivative |
| `beta-affine-logit` | Logit and radical simultaneously pulled back to [-1,1] |
| `frullani-root-log` | Square-root substitution with a logarithmic weight |
| `loglog-frullani-second` | Second iterated-log moment of a cancelling difference |
| `laplace-second-log` | Logarithmic weight after second-order exponential cancellation |
| `reciprocal-gaussian-erf` | Nonlinear whole-line substitution combining reciprocal Gaussian and erf squared |
| `arc-quartic-pullback` | Quartic rational angle obtained through a non-affine substitution |
| `atan-rectangle-pair` | Two asymmetric pieces combine by rectangle symmetry |

## Exact reference proofs

### Gaussian composed with an exponential

Set `t=erf(x)`. Then `dt=2*exp(-x^2)*dx/sqrt(pi)` and the endpoints are -1
and 1. Thus

```text
integral(exp(-x^2+erf(x)), x, -infinity, +infinity)
  = sqrt(pi)/2 * integral(exp(t), t, -1, 1)
  = sqrt(pi)*sinh(1).
```

### Affine beta-logit moment

Set `t=(1+x)/2`. The differential `dx/sqrt(1-x^2)` becomes
`dt/sqrt(t*(1-t))`, and the logarithm becomes `ln(t/(1-t))`.
The beta identity
`Beta(1/2+s,1/2-s)=pi/cos(pi*s)` has second derivative `pi^3` at zero,
which proves the supplied second-logit reference.

### Root-composed inverse-tangent Frullani

First put `t=sqrt(x)`. The logarithm contributes a factor 2 and dx/x another
factor 2, so the integral is four times the first-logarithm Frullani moment
with frequencies 2 and 3. For positive a,b, differentiation in a gives

```text
d/da integral((atan(a*t)-atan(b*t))*ln(t)/t, t, 0, +infinity)
  = integral(ln(t)/(1+a^2*t^2), t, 0, +infinity)
  = -pi*ln(a)/(2*a).
```

Integrating from b to a yields `pi*(ln(b)^2-ln(a)^2)/4`. Multiplying by the
substitution's factor 4 proves the reference `pi*(ln(3)^2-ln(2)^2)`.
The unsplit original difference converges absolutely; separate inverse-tangent
terms would not be valid convergent integrals.

### Second loglog-Frullani moment

Put `t=-ln(x)`, giving
`(exp(-2*t)-exp(-4*t))*ln(t)^2/t` on the positive half-line. Near s=0,

```text
H(s)=Gamma(s)*(2^(-s)-4^(-s))
    = integral((exp(-2*t)-exp(-4*t))*t^(s-1), t, 0, +infinity).
```

The cancellation makes H analytic there. With `L=ln(2)` and
`g=euler_gamma`, use

```text
Gamma(s)=1/s-g+(g^2/2+pi^2/12)*s+O(s^2),
2^(-s)-4^(-s)=L*s-3*L^2*s^2/2+7*L^3*s^3/6+O(s^4).
```

Twice the s^2 coefficient, namely
`7*L^3/3+3*g*L^2+(g^2+pi^2/6)*L`, is H''(0) and the exact reference.

### Logarithm after second-order Laplace cancellation

Define `A=3*ln(3)-4*ln(2)` and `B=3*ln(3)^2-4*ln(2)^2`. The Mellin transform
of the exponential difference divided by x^2 is

```text
G(s)=Gamma(s-1)*(1-2*2^(1-s)+3^(1-s)).
```

The two cancellations make the integral analytic near s=0. Its second factor
is `-A*s+B*s^2/2+O(s^3)`, while
`Gamma(s-1)=-1/s+euler_gamma-1+O(s)`.
The coefficient of s, or G'(0), is
`(1-euler_gamma)*A-B/2`, proving the logarithmically weighted reference.

### Reciprocal Gaussian with erf squared

Let `t=sqrt(x)-1/sqrt(x)`. This is a strictly increasing bijection from
(0,infinity) to the real line and
`dt=(x+1)*dx/(2*x^(3/2))`. The integral therefore becomes
`2*integral(exp(-t^2)*erf(t)^2, t, -infinity, +infinity)`.
Now put `u=erf(t)` to get
`sqrt(pi)*integral(u^2,u,-1,1)=2*sqrt(pi)/3`.

### Quartic pullback of the arc identity

The independently proved weighted-disk identity in
`docs/GENERALIZATION-CYCLE4.md` is

```text
F(q)=integral_0^(2h) acos((q-1+q*cos(u))/(q+(q+1)*cos(u))) du
    = 2*pi*h-2*h^2-2*pi*atan(sqrt(1+2*q))+pi^2/2,
h=atan(sqrt(q)), q>0.
```

Its proof reflects the positive weight
`1/((1+t^2)*(1+y^2))` across the quarter-disk diagonal; the union is the
quarter-disk and the intersection the square. Polar coordinates evaluate the
disk integral. In this case, `u=2*atan(x^2)` gives
`du=4*x*dx/(1+x^4)`, sends the endpoints to 0 and `2*atan(sqrt(2))`, and
transforms the q=2 acos argument to `(3-x^4)/(5-x^4)`. The reference is F(2).
The argument stays in [1/3,3/5], preserving the principal real branch.

### Asymmetric inverse-tangent rectangle

For positive A,B, the first scaled summand integrates to

```text
J(A,B)=integral_0^A integral_0^B
       1/((1+u^2)*(2+u^2+v^2)) dv du.
```

The second summand is J(B,A). Reflect its coordinates to the same rectangle.
The sum of the two kernels is `1/((1+u^2)*(1+v^2))`, since their numerator
sum cancels `2+u^2+v^2`. Thus
`J(A,B)+J(B,A)=atan(A)*atan(B)`; use A=2,B=3. The proof does not assert that
the two asymmetric pieces are individually half the answer.

## Convergence and independent numerical audit

All infinite tails decay sufficiently for ordinary absolute convergence.
In particular, the root-Frullani integrand is O(ln(x)/sqrt(x)) at zero and
O(ln(x)/x^(3/2)) at infinity. The Laplace numerator is O(x^2), leaving an
integrable logarithm at zero. The loglog expression has only an integrable
squared-logarithm endpoint singularity at one. The reciprocal-Gaussian change
of variables proves convergence at both of its endpoints.

The generator supplies positive-domain principal square roots, real logarithms,
and principal real inverse trig values. Every domain and bound is explicit in
the JSON. It uses standard Python for generation and byte-for-byte checks:

```sh
python3 tests/generate-coverage-cycle5.py
python3 tests/generate-coverage-cycle5.py --check
```

Independent validation requires SymPy and mpmath:

```sh
python3 tests/generate-coverage-cycle5.py --check --verify
```

SymPy parses and constructs callables; neither SymPy integration nor the tested
engine supplies the references. At **80 decimal digits**, all eight cases passed
**nine quadratures**, including a separate substituted check of the loglog
moment. The maximum scaled error was `1.69805056492e-39`, for the affine beta
logit. The threshold is
`abs(observed-reference)/max(1,abs(reference)) <= 1e-25`.

Exact equivalent formulas avoid numerical cancellation in the inverse-tangent
difference and the three-exponential numerator; these are recorded as
`verification.stable_f`, leaving the tested input unchanged. This numerical
audit supplements the proofs; it is not a certified interval computation or
a hardware measurement. No engine outcomes are asserted here.
