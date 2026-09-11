#include "giacPCH.h"
#include "integration_guard.h"
#include <cassert>
#include <iostream>
namespace giac {
void baseline_convolutionpower(const vecteur &,int,vecteur &,GIAC_CONTEXT);
void baseline_tconvolutionpower(const vecteur &,int,vecteur &,GIAC_CONTEXT);
int continuous_parity(const gen &,const gen &,unsigned &);
bool integrate_large_power(const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_trig_periods(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_sparse_atan(const gen &,const gen &,gen &,GIAC_CONTEXT);
integration_guard *integration_guard::active_=0;
}
using namespace giac;
static void depth_test(unsigned n,const gen &x,const context *ctx){
  integration_guard guard(x+int(n),x,0,ctx);
  assert(guard.allowed==(n<=24));
  if(n<25)depth_test(n+1,x,ctx);
}
int main(){
  context c;const context *ctx=&c;angle_radian(true,ctx);
  const char *samples[]={"exp(x)+2*exp(-x)","1+exp(x)+exp(2*x)",
    "sin(x)+cos(x)","2*sin(x)+cos(2*x)+1","sin(x)-sin(x)","3"};
  unsigned checks=0;
  for(const char *sample:samples){
    gen e(sample,ctx);e=e.eval(1,ctx);
    for(int trig=0;trig<2;++trig){
      vecteur terms;if(trig)tlin(e,terms,ctx);else lin(e,terms,ctx);
      const vecteur preserved(terms);
      for(int power=0;power<=12;++power){
        vecteur original,optimized;
        if(trig){baseline_tconvolutionpower(terms,power,original,ctx);tconvolutionpower(terms,power,optimized,ctx);}
        else {baseline_convolutionpower(terms,power,original,ctx);convolutionpower(terms,power,optimized,ctx);}
        assert(original==optimized);assert(terms==preserved);++checks;
      }
    }
  }
  gen x(identificateur("x"));
  {integration_guard a(x,x,0,ctx);assert(a.allowed);
   integration_guard b(x,x,0,ctx);assert(!b.allowed);}
  {integration_guard a(x,x,0,ctx);assert(a.allowed);
   integration_guard b(x,x,1,ctx);assert(b.allowed);}
  depth_test(1,x,ctx);
  depth_test(1,x,ctx); // state restored after every return
  unsigned powers=0;
  for (int m=2;m<=5;++m) for (int n=32;n<=34;++n)
    for (int a=-2;a<=2;++a){
      if (!a) continue;
      gen u=a*pow(x,m)+3;
      gen e=(derive(u,x,ctx)*pow(u,n)).eval(1,ctx),primitive;
      assert(integrate_large_power(e,x,primitive,ctx));
      // Independent CAS differentiation/normalization of manageable degrees.
      assert(is_zero(ratnormal(derive(primitive,x,ctx)-e,ctx)));
      ++powers;
    }
  const char *rejected[]={"x^2023*(1-x^2025)^2025", "x*(1-x^2)^(-32)",
    "x*(1-x^2)^(1/3)", "x*(1/x+1)^32", "cos(x)*sin(x)^32",
    "x*(x^2+a)^32", "x*(x^2+1)^n"};
  for (const char *s:rejected){
    gen e=gen(s,ctx).eval(1,ctx),primitive;
    assert(!integrate_large_power(e,x,primitive,ctx));
  }
  struct parity_case {const char *s;int expected;};
  const parity_case parity[]={
    {"sin(x)^2025*cos(x)^2026",1}, {"x^5+x^3",1},
    {"sin(x^2)",0}, {"cos(x^3)",0}, {"sin(sin(x))",1},
    {"cos(2*x)*sin(3*x)",1}, {"sin(x)^2*cos(x)^2",0},
    {"1/x",-1}, {"tan(x)",-1}, {"x/(x^2-1)",-1},
    {"sin(x)/x^2",-1}, {"sqrt(x)",-1}, {"x+1",-1},
    {"sin(x+1)",-1}, {"asin(x)",-1}};
  for (const parity_case &p:parity){
    unsigned budget=64;
    assert(continuous_parity(gen(p.s,ctx).eval(1,ctx),x,budget)==p.expected);
  }
  const char *periods[]={"sin(x)^32", "cos(x)^34", "2*cos(2*x+1)^32",
    "abs(sin(x))", "abs(cos(2*x+1))", "sin(-2*x)^32"};
  for (const char *s:periods){
    gen e=gen(s,ctx).eval(1,ctx),result;
    assert(integrate_trig_periods(e,x,0,2*cst_pi,result,ctx));
    gen reference=_integrate(makesequence(e,x,0,2*cst_pi),ctx);
    assert(is_zero(_simplify(result-reference,ctx)));
    gen reverse;
    assert(integrate_trig_periods(e,x,2*cst_pi,0,reverse,ctx));
    assert(is_zero(result+reverse));
  }
  const char *not_periods[]={"sin(x)^33", "sin(x)^(33/2)", "tan(x)^32",
    "abs(sin(x^2))", "sin(x+i)^32", "sin(x)^1000000"};
  for (const char *s:not_periods){
    gen result;
    assert(!integrate_trig_periods(gen(s,ctx).eval(1,ctx),x,0,2*cst_pi,result,ctx));
  }
  gen periodic=gen("cos(x)^32",ctx).eval(1,ctx),unused;
  assert(!integrate_trig_periods(periodic,x,0,1,unused,ctx));
  assert(!integrate_trig_periods(periodic,x,0,plus_inf,unused,ctx));
  angle_radian(false,ctx);
  assert(!integrate_trig_periods(periodic,x,0,2*cst_pi,unused,ctx));
  angle_radian(true,ctx);
  unsigned sparse_checks=0;
  for (int m=33;m<=35;++m) for (int a=1;a<=2;++a) for (int b=1;b<=2;++b){
    gen u=a*pow(x,m)+b*pow(x,m+1),num=derive(u,x,ctx);
    gen den=expand(3+u*u,ctx),primitive;
    gen e=gen("("+num.print(ctx)+")/("+den.print(ctx)+")",ctx).eval(1,ctx);
    assert(integrate_sparse_atan(e,x,primitive,ctx));
    gen residual=recursive_ratnormal(derive(primitive,x,ctx)-e,ctx);
    if (!is_zero(residual)) std::cerr<<"Sparse residual: "<<residual<<'\n';
    assert(is_zero(residual));
    ++sparse_checks;
    gen bad=gen("("+num.print(ctx)+")/("+(den+x).print(ctx)+")",ctx).eval(1,ctx);
    assert(!integrate_sparse_atan(bad,x,primitive,ctx));
    bad=gen("("+(num+1).print(ctx)+")/("+den.print(ctx)+")",ctx).eval(1,ctx);
    assert(!integrate_sparse_atan(bad,x,primitive,ctx));
  }
  std::cout<<"PASS: "<<sparse_checks<<" sparse rational derivative checks and 24 rejected near misses\n";
  std::cout<<"PASS: 12 period integrals and 9 rejected period shortcuts\n";
  std::cout<<"PASS: "<<powers<<" large-power derivative checks, 7 rejected shortcuts, 15 parity cases\n";
  std::cout<<"PASS: "<<checks<<" convolution powers and integration recursion guards\n";
}
