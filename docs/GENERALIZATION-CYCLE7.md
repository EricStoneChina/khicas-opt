# Independent integration audit: cycle 7

This round was generated after **c977aa0**,
`checkpoint/integration-cycle6-2026a`, and frozen before any query or evaluation
of the tested integration engine. The eight cases comprise **two indefinite
integrals**, **six definite integrals**, and **two explicit expression-growth
probes**. Every reference has a proof; all definite integrals converge
absolutely in the ordinary sense. None requires a Cauchy principal value.

```text
tests/generalization-cycle7.json
SHA256 2fbf0b053156b3c9fd203b9664b8a55c8ac9b0443f1a919f2ca4b95e962beb2c
```

The generator is `tests/generate-coverage-cycle7.py`. Earlier corpora and
production code are unchanged. Seven references are elementary; the
log-sine-cosine sum uses a polygamma expression for Catalan's constant.

| Family | Coverage exercised |
| --- | --- |
| Quadratic radical–atan parts | Nonlinear substitution followed by integration by parts; all-real primitive |
| Nested-root degree boundary | Positive real primitive with two unrelated radical indices, 17 and 19 |
| Composed high-power beta boundary | Quadratic substitution and a power substitution avoid a degree-65,567 expansion |
| erf–erfc product | Integration by parts exposes a Gaussian weight absent from the original integrand |
| Radical Cauchy logarithm | Trigonometric/radical substitution reduces a logarithmic rational kernel |
| Damped sine cancellation | A trigonometric and polynomial difference must stay combined before dividing by x² |
| Möbius logit Cauchy | Möbius substitution and reciprocal symmetry combine with a Mellin second derivative |
| Logarithm of sine plus cosine | A positive trig sum inside a logarithm reveals a quarter-period special constant |

## Proofs and domains

### 1. Quadratic radical–atan primitive

Write `Q=x²+x+1=(x+1/2)²+3/4`, so Q is strictly positive for every real x.
Set `u=sqrt(Q)`. Then `du=(2*x+1)*dx/(2*sqrt(Q))` and

```text
integral (2*x+1)*atan(sqrt(Q))/sqrt(Q) dx
 = 2*integral atan(u) du
 = 2*u*atan(u)-ln(1+u²)+C.
```

This is the supplied primitive
`2*sqrt(Q)*atan(sqrt(Q))-ln(x²+x+2)`. It is smooth on the real line, including
x=-1/2 where the substitution's derivative vanishes. Monotonicity of u is not
required for the derivative identity. All roots and inverse tangents use their
principal real values; the logarithm argument is positive.

### 2. Nested-root resource boundary

For x>0, let `u=1+x^(1/17)`. Then
`du=x^(-16/17)*dx/17`, giving

```text
integral (1+x^(1/17))^(1/19)/x^(16/17) dx
 = 17*integral u^(1/19) du
 = (323/20)*u^(20/19)+C.
```

Both nested power bases are positive. The compact primitive has two power
nodes and a rational coefficient. A generic algebraic-extension strategy can
instead encounter degree `17*19=323`; that is the intended resource stress,
not a request to expand or manufacture a large output. Passing this case does
not prove a universal algebraic-extension degree limit for the calculator.

### 3. Composed high-power beta resource boundary

Let `u=x²+x`. On `[0,(sqrt(5)-1)/2]`, u increases from 0 to 1 and
`du=(2*x+1) dx`. A second substitution `v=u^16` gives

```text
integral (2*x+1)*(x²+x)^15*(1-(x²+x)^16)^2048 dx
 = (1/16)*integral_0^1 (1-v)^2048 dv
 = 1/(16*2049) = 1/32784.
```

The integrand is continuous and nonnegative. Fully expanding the input as a
polynomial in x would reach degree `1+30+32*2048=65567`; the mathematical
solution needs no expansion. A compact primitive is
`-(1-(x²+x)^16)^2049/32784`. This tests recognizing a composed Jacobian and
keeping high powers factored, not merely changing a coefficient in a single
monomial beta integral.

### 4. Product of erf and erfc

Let `I=integral_0^infinity erf(x)*erfc(x) dx`. Integration by parts, with x
as the antiderivative of 1, gives

```text
I = -(2/sqrt(pi))*integral_0^infinity x*exp(-x²)*(1-2*erf(x)) dx.
```

The boundary term is zero at both ends. For
`J=integral_0^infinity x*exp(-x²)*erf(x) dx`, another integration by parts
uses `erf'(x)=2*exp(-x²)/sqrt(pi)` to obtain

```text
J = (1/sqrt(pi))*integral_0^infinity exp(-2*x²) dx
  = 1/(2*sqrt(2)).
```

Combining this with `integral_0^infinity x*exp(-x²) dx=1/2` proves
`I=(sqrt(2)-1)/sqrt(pi)`. At zero the original integrand is O(x); at infinity
erfc supplies Gaussian decay. Separate integrals of erf and erf² would
diverge and are not used in the proof.

### 5. Logarithmic radical Cauchy moment

Use `x=tan(theta)`, `0<=theta<pi/2`, and then `u=sin(theta)`:

```text
integral_0^infinity ln(1+x²)/(1+x²)^(3/2) dx
 = -2*integral_0^(pi/2) cos(theta)*ln(cos(theta)) dtheta
 = -integral_0^1 ln(1-u²) du.
```

Splitting the last logarithm is valid since `1-u` and `1+u` are positive in
the interior. Their elementary integrals are -1 and `2*ln(2)-1`, respectively,
so the result is `2-2*ln(2)`. The original endpoint behavior is O(x²) at zero
and O(ln(x)/x³) at infinity.

### 6. Damped sine minus its linear part

For real b, define the absolutely convergent function

```text
F(b)=integral_0^infinity exp(-x)*(sin(b*x)-b*x)/x² dx.
```

The numerator is O(x³) near zero. Differentiation in b under the combined
integral is valid on bounded parameter intervals and gives

```text
F'(b)=integral_0^infinity exp(-x)*(cos(b*x)-1)/x dx
     = -(1/2)*ln(1+b²).
```

The last equality follows by differentiating once more in b, evaluating
`-integral_0^infinity exp(-x)*sin(b*x) dx=-b/(1+b²)`, and using F'(0)=0.
Integrate in b and use F(0)=0:

```text
F(b)=b-atan(b)-(b/2)*ln(1+b²).
```

At b=2 this is `2-atan(2)-ln(5)`. The original difference remains combined;
neither its separately singular zero-endpoint terms nor a principal value is
used as a substitute for ordinary convergence.

### 7. Möbius logit under the Cauchy measure

Set `u=(1+x)/(1-x)`. On `[0,1)`, u runs from 1 to infinity, and
`dx/(1+x²)=du/(1+u²)`. Thus the integral is

```text
integral_1^infinity ln(u)^2/(1+u²) du.
```

The reciprocal substitution `u=1/t` shows that this is half the corresponding
integral on `(0,infinity)`. The standard positive beta integral gives

```text
M(s)=integral_0^infinity t^(s-1)/(1+t²) dt
    = pi/(2*sin(pi*s/2)),  0<Re(s)<2.
```

Differentiating twice at s=1 yields `M''(1)=pi³/8`; the required half is
`pi³/16`. Dominated convergence applies in a neighborhood of s=1. The
original x=1 singularity grows only as a squared logarithm and is integrable.

### 8. Logarithm of a positive sine/cosine sum

On `[0,pi/2]`, `sin(x)+cos(x)>=1`, and
`(sin(x)+cos(x))²=1+sin(2*x)`. Consequently

```text
integral_0^(pi/2) ln(sin(x)+cos(x)) dx
 = (1/4)*integral_0^pi ln(1+sin(u)) du
 = G-pi*ln(2)/4.
```

The last equality follows by reflection about pi/2 and the quarter-period
logarithm identity proved independently in `GENERALIZATION-CYCLE6.md`.
Writing Catalan's constant as

```text
G = sum_{k>=0}(-1)^k/(2*k+1)²
  = (Psi(1/4,1)-Psi(3/4,1))/16
```

produces the supplied reference using the actual Giac `Psi(z,1)` spelling.
All logarithms in the derivation have positive interior arguments.

## Independent audit and reproducibility

```sh
python3 tests/generate-coverage-cycle7.py
python3 tests/generate-coverage-cycle7.py --check --verify
```

Both primitives pass symbolic differentiation with their positive-power
domain assumptions. Separately, mpmath checks their numerical derivatives at
five stated points each, at **80 and 110 decimal digits**, for **20 numerical
derivative checks**. The all-real quadratic primitive includes negative x
and its stationary point x=-1/2 in those samples.

The six definite integrals pass at both precisions. Three also have separate
transformed-integrand checks, for **18 quadratures** in total. The largest
scaled discrepancy across all numerical checks is `2.17601318379e-81`; the
threshold is `1e-45` for `abs(observed-reference)/max(1,abs(reference))`.
For the damped sine difference, a convergent Taylor series near zero avoids
floating-point subtraction of nearly equal terms; the tested input is not
changed. The series truncation error is far below the working precision.

SymPy is used for expression parsing, differentiation, simplification of the
derivative difference, and creation of numerical callables. No symbolic
integration routine or tested engine supplies any reference. The symbolic
proofs and numerical audit establish the references, not engine success,
firmware speed, or a CG50 hardware resource measurement. Engine outcomes
belong in separate reports so this corpus remains frozen.
