#include "giacPCH.h"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace giac;

int main(int argc,char **argv) {
  context ctx; const context *contextptr=&ctx;
  angle_radian(true,contextptr);
  const gen x(identificateur("x"));
  const char *functions[]={"sin(x)","exp(x)","x^7-2*x+3","1/(x^2+1)",
    "ln(x)","x*exp(x^2)","x*sin(x)","ln(x)^3","1/sqrt(1-x^2)","exp(-x^2)"};
  for (unsigned i=0;i<sizeof(functions)/sizeof(*functions);++i) {
    gen f=gen(functions[i],contextptr).eval(1,contextptr);
    gen antiderivative=_integrate(makesequence(f,x),contextptr);
    assert(!is_undef(antiderivative));
    assert(!contains(antiderivative,*at_integrate));
    gen error=_simplify(derive(antiderivative,x,contextptr)-f,contextptr);
    if (!is_zero(error)) {
      std::cerr << functions[i] << " derivative mismatch: " << error << '\n';
      return 1;
    }
  }
  struct sample { const char *f; double expected; } numerical[]={
    {"x^2",1.0/3}, {"exp(x)",std::exp(1.0)-1},
    {"exp(-x^2)",0.746824132812427}, {"1/(1+x^2)",0.7853981633974483}
  };
  for (unsigned i=0;i<sizeof(numerical)/sizeof(*numerical);++i) {
    gen f=gen(numerical[i].f,contextptr).eval(1,contextptr);
    gen result=_gaussquad(makesequence(f,x,0,1),contextptr);
    result=evalf_double(result,1,contextptr);
    assert(result.type==_DOUBLE_);
    assert(std::fabs(result._DOUBLE_val-numerical[i].expected)<1e-8);
  }
  for (int i=1;i<argc;++i)
    assert(!is_undef(_read(string2gen(argv[i],false),contextptr)));
  std::cout << "PASS: 10 symbolic and 4 numerical integration cases; benchmark scripts\n";
}
