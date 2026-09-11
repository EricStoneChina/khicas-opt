# Independent integration audit: cycle 6

This round freezes eight definite integrals after **dff6974**,
`checkpoint/integration-cycle5-2026a`, before running or querying the tested
engine. All eight have proved exact references and converge absolutely as
ordinary integrals. Earlier corpora and production sources are unchanged.

```text
tests/generalization-cycle6.json
SHA256 541d969e545046f4c1c1e03fa58ab749ce198c166c58615ddc68b3b73ee0f345
```

The generator is `tests/generate-coverage-cycle6.py`. It uses Python to build the
JSON, SymPy only to parse expressions and create numerical callables, and
mpmath for independent quadrature. Neither Giac nor symbolic integration
supplies a reference. This is a coverage probe, not a statement of engine
success, hardware speed, or universal integration completeness.

| Family | New combination relative to cycle 5 |
| --- | --- |
| Gaussian–atan parts | Integration by parts leaves a Gaussian rational kernel and erfc |
| Half-period log-sine moment | A linear weight on a half-period retains an odd zeta contribution |
| Opposite logarithms | Product of two different endpoint logarithms and a zeta reduction |
| Mixed-degree Mellin log | Linear and quadratic denominator factors with a logarithmic moment |
| Trig-log quarter-period | A sum inside a logarithm exposes a quarter-period Catalan contribution |
| Möbius atan measure | Nonlinear interval involution preserves a logarithmic integration measure |
| Third-order oscillatory cancellation | Trigonometric cancellation makes a whole integrand absolutely integrable |
| Fermi logarithm moment | A logarithm of a decaying exponential sum, rather than a Fermi rational kernel |

Five references are marked `special-function-closed-form` (zeta, erfc, or Psi);
three are elementary. There are no numerical-only reference placeholders.

## Proofs of the references

### 1. Gaussian times inverse tangent

Let

```text
I = integral_0^infinity x*exp(-x^2)*atan(x) dx.
```

Integration by parts gives `I=J/2`, where
`J=integral_0^infinity exp(-x^2)/(1+x^2) dx`; both boundary terms vanish.
Use the positive Laplace representation
`1/(1+x^2)=integral_0^infinity exp(-(1+x^2)*t) dt` and Tonelli's theorem:

```text
J = sqrt(pi)/2 * integral_0^infinity exp(-t)/sqrt(1+t) dt
  = sqrt(pi)*exp(1) * integral_1^infinity exp(-u^2) du
  = pi*exp(1)*erfc(1)/2.
```

Therefore `I=pi*exp(1)*erfc(1)/4`. The original integrand is O(x²) at zero
and has Gaussian decay.

### 2. Half-period logarithmic sine moment

On `(0,pi)`, use the Abel-regularized Fourier series
`ln(sin(x))=-ln(2)-sum_{n>=1} cos(2*n*x)/n`. Integrate against x on
`[0,pi/2]`; the resulting series is absolutely summable because

```text
integral_0^(pi/2) x*cos(2*n*x) dx = ((-1)^n-1)/(4*n^2).
```

The odd-index reciprocal cubes sum to `(1-2^(-3))*zeta(3)`, hence

```text
integral_0^(pi/2) x*ln(sin(x)) dx
  = -pi^2*ln(2)/8 + 7*zeta(3)/16.
```

The endpoint behavior `x*ln(x)` is integrable; the Abel limit agrees with the
ordinary integral. The half-period upper bound is essential: full-period
symmetry would remove this zeta term.

### 3. Opposite endpoint logarithms

Write

```text
A = integral_0^1 ln(1+x)^2/x dx,
B = integral_0^1 ln(1-x)^2/x dx,
C = integral_0^1 ln(1-x)*ln(1+x)/x dx.
```

Expanding `1/(1-y)` and integrating `y^n*ln(y)^2` gives `B=2*zeta(3)`.
Also `ln(1-x^2)^2=ln(1-x)^2+ln(1+x)^2+2*ln(1-x)*ln(1+x)`, so the substitution
`t=x^2` yields `B/2=B+A+2*C`.

For completeness, the required reduction of A can be verified without an
integration oracle. Put `t=1/(1+x)` and use the differentiable primitive

```text
integral ln(t)^2/(1-t) dt
 = -ln(t)^2*ln(1-t)-2*ln(t)*Li_2(t)+2*Li_3(t).
```

At the bounds one obtains, with `L=ln(2)`,

```text
A = L^3/3-pi^2*L/6+2*zeta(3)-2*Li_3(1/2).
```

The classical reduction used here follows directly from differentiating

```text
Li_3(z)+Li_3(-z/(1-z))+Li_3(1-z)
 = zeta(3)+(pi^2/6)*ln(1-z)
   -(1/2)*ln(z)*ln(1-z)^2+(1/6)*ln(1-z)^3,  0<z<1.
```

Both sides have the same derivative by the two dilogarithm reflection
identities, themselves obtained by differentiating and fixing their endpoint
constants. Their common limit at z=0 is zeta(3). Setting z=1/2 and using the
absolutely convergent series `Li_3(-1)=-3*zeta(3)/4` gives
`Li_3(1/2)=7*zeta(3)/8-pi^2*L/12+L^3/6`. Thus `A=zeta(3)/4` and
`C=-5*zeta(3)/8`. No polylogarithm evaluation is required in the supplied final
reference. C is O(x) at zero and O(ln(1-x)) at one.

### 4. Logarithmic Mellin moment with mixed degrees

For `0<s<1`, partial fractions and the elementary beta integral give

```text
M(s) = integral_0^infinity x^(s-1)/((1+x)*(1+x^2)) dx
     = pi/(2*sin(pi*s)) + pi/(4*sin(pi*s/2))
       -pi/(4*sin(pi*(s+1)/2)).
```

The original integral is analytic on `0<Re(s)<3`. The poles of the separate
terms at s=1 cancel. With `s=1+h`, the first and third terms have expansions

```text
-1/(2*h)-pi^2*h/12+O(h^3),
+1/(2*h)+pi^2*h/48+O(h^3),
```

and the second term is `pi/4+O(h^2)`. Consequently `M'(1)=-pi^2/16`.
Differentiation under the original combined integral is valid by dominated
convergence near s=1. The argument never treats separately divergent terms
as convergent integrals or assigns a principal value.

### 5. Logarithm of a trigonometric sum

On the stated interval,
`1+sin(x)=2*cos(x/2-pi/4)^2`, and the cosine factor is positive. Therefore

```text
integral_0^(pi/2) ln(1+sin(x)) dx
 = (pi/2)*ln(2)+4*integral_0^(pi/4) ln(cos(u)) du
 = 2*G-(pi/2)*ln(2),
G = sum_{k>=0} (-1)^k/(2*k+1)^2.
```

The second equality follows by integrating the Abel-regularized log-cosine
Fourier series; the integrated series is absolutely convergent. To avoid
assuming a named Catalan constant in the input language, the reference uses

```text
Psi(1/4,1)-Psi(3/4,1)
 = 16*sum_{k>=0}(1/(4*k+1)^2-1/(4*k+3)^2)
 = 16*G.
```

Thus the reference is `(Psi(1/4,1)-Psi(3/4,1))/8-pi*ln(2)/2`.
The JSON uses Giac's `Zeta(3)` spelling for the mathematical zeta function.
Here `Psi(z,1)` denotes the first polygamma derivative. The integrand is
continuous on the entire closed interval.

### 6. Inverse tangent with a preserved logarithmic measure

The involution `x=(1-t)/(1+t)` exchanges 0 and 1 and satisfies

```text
dx/(1+x) = -dt/(1+t),
atan((1-t)/(1+t)) = pi/4-atan(t),  0<=t<=1.
```

Both angles are principal real values. If I denotes the requested integral,
these identities give `I=(pi/4)*ln(2)-I`, or `I=pi*ln(2)/8`.
The denominator has no zero, and no branch continuation is used.

### 7. Trigonometric cancellation of third order

The numerator `sin(x)-x*cos(x)=x^3/3+O(x^5)`, so the apparent singularity at
zero is removable. At infinity the whole integrand is O(1/x²), hence its
improper integral is absolutely convergent. Integration by parts gives

```text
integral_0^infinity (sin(x)-x*cos(x))/x^3 dx
 = (1/2)*integral_0^infinity sin(x)/x dx
 = pi/4.
```

The integration-by-parts boundary terms vanish at both endpoints. The
intermediate sine integral converges in the ordinary Dirichlet sense: its
exponentially damped version is `atan(1/epsilon)` and the Abel limit as
`epsilon -> 0+` is pi/2. The original integral needs no regularization or PV.

### 8. First moment of a Fermi logarithm

For x>0, expand
`ln(1+exp(-x))=sum_{n>=1}(-1)^(n+1)*exp(-n*x)/n`. The sum of the absolute
integrals after multiplication by x is `sum 1/n^3<infinity`, justifying
termwise integration. Hence

```text
integral_0^infinity x*ln(1+exp(-x)) dx
 = sum_{n>=1}(-1)^(n+1)/n^3
 = (1-2^(-2))*zeta(3) = 3*zeta(3)/4.
```

The independent substitution `t=exp(-x)` gives the finite-interval form
`integral_0^1 -ln(t)*ln(1+t)/t dt`, also recorded in the JSON for verification.

## Independent validation and reproducibility

```sh
python3 tests/generate-coverage-cycle6.py
python3 tests/generate-coverage-cycle6.py --check --verify
```

Before any engine evaluation, all eight references passed at **80 and 110
decimal digits**, for **18 quadrature checks** including the independent
finite-interval form of case 8. The largest scaled discrepancy was
`2.10843958865e-81`; the acceptance threshold was `1e-45`, measured as
`abs(value-reference)/max(1,abs(reference))`.

The oscillatory case uses `mpmath.quadosc` with frequency 1. Near zero it
uses the convergent Taylor series of the unchanged integrand to prevent
subtractive cancellation; its retained 41 terms at |x|<0.1 have truncation
error far below the requested precision. These choices are independent of
the engine and do not replace the tested input by an easier integral.

Numerical agreement supplements the proofs; it is not certified interval
arithmetic, a pixel/UI test, or a CG50 resource measurement. Subsequent engine
results should be stored separately so this frozen corpus remains unchanged.
