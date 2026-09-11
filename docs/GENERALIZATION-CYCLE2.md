# Independent integration coverage: cycle 2

This second set contains **61 new scalar integrals**, generated after checkpoint
`3f5fbde` (`checkpoint/integration-cycle1-2026a`). It preserves the first corpus
and its generator. The new files are:

* `tests/generalization-cycle2.json`
* `tests/generate-coverage-cycle2.py`
* this document

There are **49 definite and 12 indefinite integrals**. References are derived
independently and checked without asking the tested Giac engine for an answer.
The intent labels identify **11 near misses, 8 parameter variants, and 42
coverage extensions**. These are diagnostic labels: a nearby expression can
still be solved by the general integrator even when a particular rule is invalid.

## Breadth and scope

The 24 detailed `family` labels are organized below into 12 broader coverage
groups. They are not 24 independent new integration methods, and some deliberately
extend mathematical families already present in cycle 1.

| Broader group | Cases | What changes |
| --- | ---: | --- |
| Gaussian and Cauchy convolutions | 9 | Different centers and scales, normal moments through degree 12, odd Cauchy numerator without parity |
| Mellin rational integrals | 8 | Rational powers in numerator and denominator; first/second logarithmic weights |
| Beta endpoint integrals | 7 | Fractional exponents at both endpoints, single and mixed logarithmic weights |
| Bose/Fermi moments | 6 | Frequency scaling, degrees 2 through 11, both signs of the denominator constant |
| Logistic moments | 4 | Degrees 12 and 14, scaled/shifted moments of degrees 4 and 6 |
| Laplace phase and cosine cancellation | 5 | Nonzero phases, negative frequency, cancellation needed at the origin |
| Conditional cosine differences and cross-sinc | 4 | Ordinary conditional convergence and unequal-frequency products |
| Cauchy Fourier transform | 4 | Scale, oscillation frequency, and translations |
| Rational-power and real-root primitives | 4 | Fractional substitutions, real odd roots on both sides of zero |
| Shifted Gaussian primitive | 1 | Linear term in the Gaussian exponent and erf reference |
| Other composed primitives | 7 | Hyperbolic parts, exponential trig, nested logs, fractional log parts, absolute exponential |
| Inverse-Gaussian kernels | 2 | Exponential suppression at both zero and infinity |

The set was chosen to exercise both new methods and failures of tempting
shortcuts. A shifted odd numerator is not automatically odd; a shifted density
does not retain its centered moments; a nonzero trigonometric phase changes the
Laplace result; a logarithmic Mellin weight changes the value and sign.

## What “reference answer” means

Every case in this set has a known closed-form reference. `answer_class` is:

* `elementary-closed-form`: 50 reference expressions;
* `special-function-closed-form`: 11 expressions using Gamma, Zeta, or erf.

These labels describe the supplied expression, not a proof that it cannot be
simplified further. For example, some Gamma ratios or even zeta values reduce to
elementary constants. The corresponding spellings occur in the host parser's
`static_lexer_full.h`; SymPy maps them to its own numeric functions during the
independent audit.

There are **no numeric-only, unknown-closed-form, or divergent cases** silently
assigned exact answers. Such cases would need a separately specified numerical
reference/error protocol instead of being mixed with this runner's exact
reference comparisons. This deliberate restriction keeps the second round
small enough to audit completely; it does not assert that all integrals possess
closed forms or that the tested engine can find these ones.

## Independent derivations

### Convolutions

For the Gaussian product, complete the square with
`s=a+b`, `mu=(a*u+b*v)/s`, and `d=a*b*(u-v)^2/s`.
The integral of `x^n*exp(-a*(x-u)^2-b*(x-v)^2)` is

```text
exp(-d)*sqrt(pi/s)*sum(
  binomial(n,2*k)*mu^(n-2*k)*(2*k)!/(4^k*k!*s^k),
  k=0..floor(n/2))
```

The generator evaluates these coefficients as exact rational numbers. The
Cauchy convolution identity, obtainable by partial fractions or residues, is

```text
integral(1/((x^2+a^2)*((x-c)^2+b^2)), x, -infinity, +infinity)
  = pi*(a+b)/(a*b*((a+b)^2+c^2))
```

With an extra numerator x, the value is
`pi*c/(b*((a+b)^2+c^2))`. Both integrals converge absolutely for positive a,b;
the shifted cases are not evaluated by an invalid parity argument.

### Mellin and endpoint identities

For `0<p<q`, substitution followed by the beta reflection identity gives

```text
I(p,q) = integral(x^(p-1)/(1+x^q), x, 0, +infinity)
       = pi/(q*sin(pi*p/q))
```

Differentiating with respect to p once or twice gives the supplied logarithmic
weights. The assumptions ensure convergence and justify differentiation on a
compact parameter neighborhood.

Fractional endpoint references are `Gamma(a)*Gamma(b)/Gamma(a+b)`, with a,b>0.
For the arcsine weight `1/sqrt(x*(1-x))`, beta differentiation gives, respectively,

```text
integral(ln(x)*weight)             = -2*pi*ln(2)
integral(ln(x)^2*weight)           = pi*(4*ln(2)^2+pi^2/3)
integral(ln(x)*ln(1-x)*weight)     = pi*(4*ln(2)^2-pi^2/6)
```

### Exponential-density moments

Expanding the Bose/Fermi denominator into decaying exponentials and integrating
the terms gives, for the positive scales and integer n used here,

```text
integral(x^n/(exp(a*x)-1), x, 0, +infinity) = n!*Zeta(n+1)/a^(n+1)
integral(x^n/(exp(a*x)+1), x, 0, +infinity) = (1-2^(-n))*n!*Zeta(n+1)/a^(n+1)
```

The logistic density's even moments are
`(2^n-2)*abs(B_n)*pi^n/a^(n+1)` for n>=2. The generator uses the exact Bernoulli
values `abs(B_12)=691/2730` and `abs(B_14)=7/6`. Shifted moments use the binomial
expansion around the density's mean; odd centered moments vanish, while the
zeroth centered integral is `1/a`.

### Oscillatory identities

For b>0, splitting the shifted sine and enforcing cancellation at zero gives

```text
integral(exp(-b*x)*(sin(a*x+phi)-sin(phi))/x, x, 0, +infinity)
  = cos(phi)*atan(a/b)-sin(phi)*ln(1+(a/b)^2)/2
```

The damped cosine difference is
`ln((b^2+c^2)/(b^2+a^2))/2`. The undamped difference
`(cos(a*x)-cos(b*x))/x` has integral `ln(b/a)` for positive a,b.
For positive a,b, `sin(a*x)*sin(b*x)/x^2` integrates to
`pi*min(a,b)/2` on the half-line. These identities follow by differentiating
with respect to frequency or by the elementary product-to-sum formulas with
the Dirichlet integral.

The translated Cauchy Fourier transform is
`pi*exp(-a*abs(b))*cos(b*c)/a` for
`cos(b*x)/((x-c)^2+a^2)` on the full line. The sine component of the translated
centered kernel is zero by parity; the phase factor remains in the result.

### Remaining references

The inverse-Gaussian identity is

```text
integral(exp(-a*x-b/x)/sqrt(x), x, 0, +infinity)
  = sqrt(pi/a)*exp(-2*sqrt(a*b)),  a,b>0
```

It follows from the Gaussian integral after a square-root substitution and
completion of the square. The twelve primitives follow by direct chain/product
rules or integration by parts. In particular, the absolute-exponential
primitive includes the additive term inside
`sign(x-2)*((abs(x-2)-1)*exp(abs(x-2))+1)` so it is continuous at x=2.

## Domains, convergence, and validation

Fractional powers mean principal powers; `surd(x,3)` explicitly means a real
cube root. Positive-domain fractional examples sample x>0. The real-root
rational primitive samples `-8`, `-1/8`, `1/8`, and `8`, avoiding its singularities
at -1 and 0. Nested-log samples stay in their stated domain. Every indefinite
case has at least three samples.

The undamped cosine differences are ordinary conditionally convergent
half-line integrals. All full-line integrals in this round converge absolutely.
No Cauchy principal value of an otherwise divergent integral is used as an
ordinary definite-integral reference.

Generate or check reproducibility with standard Python:

```sh
python3 tests/generate-coverage-cycle2.py
python3 tests/generate-coverage-cycle2.py --check
```

Audit all references independently with SymPy and mpmath:

```sh
python3 tests/generate-coverage-cycle2.py --check --verify
```

The audit uses **80 decimal digits** and a scaled error limit of `1e-20`:
`abs(observed-reference)/max(1,abs(reference))`. SymPy only parses expressions
and constructs callables; it does not integrate the examples. mpmath evaluates
the original integrands by quadrature and differentiates the proposed primitives
at their declared samples. Oscillatory tails use `quadosc`; ordinary integrals
split around central peaks and finite endpoint singularities.

The initial audit passed **all 61 references**: 41 ordinary quadratures,
8 oscillatory quadratures, and 12 sampled-derivative checks. The largest scaled
error was `6.2183573e-22` for `C2-fractional-beta-03`, whose endpoint exponent is
-3/4. This records actual numerical accuracy, rather than assuming that 80-digit
working precision gives 80 accurate digits for a singular endpoint integral.

Run the tested engine separately:

```sh
python3 tests/run-calculus.py --corpus tests/generalization-cycle2.json \
  --timeout 10 --report /tmp/khicas-generalization-cycle2.json
```

Sampled derivatives are not formal proofs and can miss constants on disconnected
domain components. Numerical checks do not prove convergence; the derivations
and stated assumptions supply that justification. Engine outcomes must continue
to distinguish exact, sampled, numeric, unevaluated, and timeout results. Host
benchmarks do not establish CG50 runtime or resource limits.
