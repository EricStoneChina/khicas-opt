# Independent generalization audit: cycle 8

Baseline: `4c97dc5`, tag `checkpoint/integration-cycle7-2026a`.

This corpus is generated and mathematically checked before querying the
engine under test. It contains eight cases: two indefinite integrals, six
ordinary absolutely convergent definite integrals, and two expression-growth
probes. All references have finite proved expressions. No divergent
principal-value problems or unknown-closed-form placeholders are included.

## Cases and proofs

### 1. Quadratic composition with inverse hyperbolic sine

Let `q=x^2+x+1`. Integration by parts gives

```text
integral (2*x+1)*asinh(q) dx = q*asinh(q)-sqrt(1+q^2).
```

Differentiating with respect to q cancels the two q/sqrt(1+q^2) terms.
The expression is real for every real x; all square-root arguments are
strictly positive. This probes composition followed by integration by parts.

### 2. High-degree binomial composition with a two-term primitive

Set `q=x^2+x` and `t=q^32`. Since `dt=32*q^31*(2*x+1) dx`,

```text
integral (2*x+1)*q^63*(1+q^32)^1024 dx
 = integral t*(1+t)^1024 dt/32
 = (1+t)^1026/32832-(1+t)^1025/32800.
```

The identity is valid for all real x. It follows by writing t=(1+t)-1;
no polynomial expansion is needed. The expanded integrand has degree 65663,
whereas the exact primitive has two powers. Unlike a bare derivative-times-
power test, this requires a residual linear factor after substitution.
The independent symbolic proof uses a generic exponent to avoid expanding
the large polynomial in the verifier itself.

### 3. Exponential beta moment with a large binomial exponent

Use `t=exp(-x)`:

```text
integral_0^infinity exp(-2*x)*(1-exp(-x))^4096 dx
 = integral_0^1 t*(1-t)^4096 dt
 = B(2,4097) = 1/(4097*4098).
```

The original integrand behaves as x^4096 at zero and exp(-2*x) at infinity.
Direct binomial expansion would produce 4097 exponential terms. The
substitution gives a single beta moment with a short rational value.

### 4. Laplace transform of an error function with square-root argument

The integrand is nonnegative, so Tonelli's theorem permits swapping the
integrals in the defining representation of erf:

```text
integral_0^infinity exp(-x)*erf(sqrt(x)) dx
 = (2/sqrt(pi))*integral_0^infinity exp(-t^2)
                         *integral_(t^2)^infinity exp(-x) dx dt
 = (2/sqrt(pi))*integral_0^infinity exp(-2*t^2) dt
 = 1/sqrt(2).
```

This is ordinary absolute convergence. The square root is the positive real
root. The transformed Gaussian integral is also checked independently.

### 5. Inverse tangent times a logarithm under a Cauchy measure

Set `t=atan(x)` to obtain `integral_0^(pi/4) t*ln(tan(t)) dt`.
On the open interval, the Fourier expansion is

```text
ln(tan(t)) = -2*sum_(n odd, n>=1) cos(2*n*t)/n.
```

Integrating `t*cos(2*n*t)` and evaluating at pi/4 gives

```text
-(pi/4)*sum_(n odd) sin(n*pi/2)/n^2
+ (1/2)*sum_(n odd) 1/n^3
= -pi*G/4+7*Zeta(3)/16,
G = (Psi(1/4,1)-Psi(3/4,1))/16.
```

Termwise integration can be justified first for Abel-damped series and then
by dominated convergence; the resulting coefficient series are absolutely
convergent. The original integrand is O(x*ln(x)) at zero.

### 6. Product of opposite trigonometric logarithms

By symmetry and the half-angle formulas, the requested integral equals

```text
integral_0^(pi/2) (ln(2)+2*ln(sin(t)))
                  *(ln(2)+2*ln(cos(t))) dt.
```

Use the beta integral
`F(a,b)=integral_0^(pi/2) sin(t)^(a-1)*cos(t)^(b-1) dt
       = B(a/2,b/2)/2` and its mixed derivative at a=b=1:

```text
integral ln(sin(t)) dt = integral ln(cos(t)) dt = -pi*ln(2)/2,
integral ln(sin(t))*ln(cos(t)) dt = pi*ln(2)^2/2-pi^3/48.
```

Expanding the product therefore gives `pi*ln(2)^2/2-pi^3/12`.
Both original logarithm arguments are positive in the interior. The zero of
1-sin(x) at pi/2 gives an integrable logarithmic singularity. For numerical
validation, `ln(1-sin(x))=2*ln(abs(cos(x)))-ln(1+sin(x))` avoids subtractive
cancellation near that endpoint.

### 7. Squared inverse tangent with a squared Cauchy denominator

Set `t=atan(x)` to obtain `integral_0^(pi/2) t^2*cos(t)^2 dt`.
Using `cos(t)^2=(1+cos(2*t))/2` and two integrations by parts gives

```text
integral_0^(pi/2) t^2*cos(t)^2 dt = pi^3/48-pi/8.
```

The original integrand is O(x^2) at zero and O(x^-4) at infinity. This
combines an angle moment with a noninvariant rational measure.

### 8. Second logarithmic moment of a Gaussian

For a>0,

```text
M(a) = integral_0^infinity x^(a-1)*exp(-x^2) dx = Gamma(a/2)/2.
```

Two derivatives at a=1 yield

```text
integral_0^infinity exp(-x^2)*ln(x)^2 dx
 = sqrt(pi)*(Psi(1/2)^2+Psi(1/2,1))/8.
```

Differentiation under the integral is justified by an integrable logarithmic
bound near zero and Gaussian decay at infinity, uniformly for a near 1.
This is a finite special-function reference, not an unevaluated integral.

## Independent validation and reproduction

```sh
python3 tests/generate-coverage-cycle8.py --check --verify
```

The generator only uses Python, SymPy for explicit derivative checks, and
mpmath for numerical quadrature and derivative checks. It does not import,
launch or query the modified integration engine. Reference expressions are
written from the proofs above; no CAS antiderivative is requested.

The audit checks two symbolic derivative identities, ten sampled derivatives
at each of 80 and 110 digits, and both original and independently transformed
quadratures where recorded in the JSON. The two resource probes keep their
powers factored throughout verification. The JSON schema is the same as
previous cycles; the cases have explicit domains and resource notes.

Frozen corpus SHA256:
`976c08a687455e7fad76e7755de91b0e72c1db23dbf14c8b3e492222edb945cd`.

Independent validation passed: 2 symbolic derivative proofs, 20 sampled
numerical derivatives, and 22 quadratures across 80/110 decimal digits.
Maximum scaled error was `7.49242173532e-80`. Validation transcript:
`/tmp/khicas-cycle8-independent-validation.txt`. The generator's `--check`
mode confirms the frozen JSON exactly matches its deterministic output.
