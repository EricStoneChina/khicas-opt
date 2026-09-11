#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_gaussian_erf_exp(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
using namespace giac;
static int run_cases(){
 context c;const context *ctx=&c;gen x(identificateur("x")),res;
 struct Case{const char *f,*answer;};
 const Case yes[]={
 {"exp(-x^2+erf(x))","sqrt(pi)*sinh(1)"},
 {"exp(-(2*x+3)^2+4*erf(2*x+3)+5)","exp(5)*sqrt(pi)*sinh(4)/8"},
 {"exp(-(-2*x+3)^2+4*erf(-2*x+3)+5)","exp(5)*sqrt(pi)*sinh(4)/8"},
 {"exp(-(2*x+3)^2-4*erf(2*x+3)+5)","exp(5)*sqrt(pi)*sinh(4)/8"},
 {"exp(-(x/2+1/3)^2+erf(x/2+1/3)/3-2)","6*exp(-2)*sqrt(pi)*sinh(1/3)"},
 {"exp(-4*x^2-12*x+4*erf(2*x+3)-4)","exp(5)*sqrt(pi)*sinh(4)/8"},
 {"exp(-(2*x+3)^2+0*erf(2*x+3)+5)","exp(5)*sqrt(pi)/2"},
 {"exp(-4*x^2-12*x-4)","exp(5)*sqrt(pi)/2"}
 };
 for(unsigned evaluated=0;evaluated<2;++evaluated)for(const Case &t:yes){
  gen f=integration_syntax(gen(t.f,ctx),ctx);
  // Normalize the exponent only: host eval may turn exp(k+u) into a product.
  if(evaluated)f=symbolic(at_exp,f._SYMBptr->feuille.eval(1,ctx));
  bool ok=integrate_gaussian_erf_exp(f,x,minus_inf,plus_inf,res,ctx);gen expected=gen(t.answer,ctx).eval(1,ctx);
  if(!ok || !is_zero(_simplify(res-expected,ctx))){std::cerr<<"FAIL phase "<<evaluated<<" "<<f<<" matched "<<ok<<" got "<<res<<'\n';return 1;}
 }
 const char *no[]={"exp(-2*x^2+erf(x))","exp(-x^2+erf(2*x))","exp(-x^2+erf(x+1))","exp(x^2+erf(x))","exp(-x^4+erf(x^2))","exp(-x^2+i*erf(x))","exp(-x^2+erf(x)+erf(2*x))","exp(-x^2+erf(x)^2)","exp(-x^2+erf(x)+i)"};
 for(const char *s:no)if(integrate_gaussian_erf_exp(integration_syntax(gen(s,ctx),ctx),x,minus_inf,plus_inf,res,ctx)){std::cerr<<"UNEXPECTED "<<s<<'\n';return 1;}
 if(integrate_gaussian_erf_exp(integration_syntax(gen("exp(-x^2+erf(x))",ctx),ctx),x,0,plus_inf,res,ctx))return 1;
 std::cout<<"PASS: 8 exact Gaussian-erf exponentials, raw and normalized exponent; 10 rejection checks\n";return 0;
}
static void *worker(void *p){*static_cast<int *>(p)=run_cases();return 0;}
int main(){if(run_cases())return 1;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);int result=1;pthread_t t;int error=pthread_create(&t,&a,worker,&result);pthread_attr_destroy(&a);if(error || pthread_join(t,0))return 2;if(!result)std::cout<<"PASS: guarded64 KiB stack\n";return result;}
