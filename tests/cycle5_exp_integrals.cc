#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_exp_difference(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
using namespace giac;
static int run_cases(){
 context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
 struct Case{const char *f,*answer;};
 const Case yes[]={
 {"(exp(-x)-2*exp(-2*x)+exp(-3*x))*ln(x)/x^2","(1-euler_gamma)*(3*ln(3)-4*ln(2))-3*ln(3)^2/2+2*ln(2)^2"},
 {"(exp(-x)-exp(-2*x))*ln(x)/x","-ln(2)^2/2-euler_gamma*ln(2)"},
 {"(exp(-x)-exp(-2*x))*ln(x)^2/x","ln(2)^3/3+euler_gamma*ln(2)^2+(euler_gamma^2+pi^2/6)*ln(2)"},
 {"(exp(-x)-2*exp(-2*x)+exp(-3*x))*ln(x)^2/x^2","(3*ln(3)^3-4*ln(2)^3)/3+(euler_gamma-1)*(3*ln(3)^2-4*ln(2)^2)+(2-2*euler_gamma+euler_gamma^2+pi^2/6)*(3*ln(3)-4*ln(2))"},
 {"(exp(1-2*x)-2*exp(1-4*x)+exp(1-6*x))*ln(x)/x^2","2*exp(1)*((1-euler_gamma-ln(2))*(3*ln(3)-4*ln(2))-(3*ln(3)^2-4*ln(2)^2)/2)"}
 };
 for(unsigned evaluated=0;evaluated<2;++evaluated)for(const Case &t:yes){
  gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);f=integration_syntax(f,ctx);
  bool ok=integrate_exp_difference(f,x,0,plus_inf,res,ctx);
  gen expected=gen(t.answer,ctx).eval(1,ctx);
  if(!ok || (res!=expected && !is_zero(_simplify(res-expected,ctx)))){std::cerr<<"FAIL "<<t.f<<" parsed "<<f<<" got "<<res<<" matched "<<ok<<'\n';return 1;}
 }
 // A common affine intercept is an exact outside exponential factor.
 gen shifted=integration_syntax(gen("(exp(2-x)-2*exp(2-2*x)+exp(2-3*x))/x^2",ctx),ctx);
 if(!integrate_exp_difference(shifted,x,0,plus_inf,res,ctx) || !is_zero(_simplify(res-gen("exp(2)*(3*ln(3)-4*ln(2))",ctx).eval(1,ctx),ctx)))return 1;
 const char *no[]={
 "(exp(-x)-exp(-2*x))*ln(x)/x^2",
 "(exp(-x)-2*exp(-2*x))*ln(x)/x",
 "(exp(-x)-exp(-2*x))*ln(x)^3/x",
 "(exp(-x)-2*exp(-2*x)+exp(-3*x))*ln(-x)/x^2",
 "(exp(x)-2*exp(-x)+exp(-3*x))*ln(x)/x^2"
 };
 for(const char *input:no){
  if(integrate_exp_difference(integration_syntax(gen(input,ctx),ctx),x,0,plus_inf,res,ctx)){std::cerr<<"UNEXPECTED "<<input<<" -> "<<res<<'\n';return 1;}
 }
 std::cout<<"PASS: 5 logarithmic exponential-difference identities raw/evaluated, common intercept, 5 rejected conditions\n";
 return 0;
}
static void *worker(void *output){*static_cast<int *>(output)=run_cases();return 0;}
int main(){
 if(run_cases())return 1;
 pthread_attr_t attributes;if(pthread_attr_init(&attributes))return 2;
 if(pthread_attr_setstacksize(&attributes,64*1024) || pthread_attr_setguardsize(&attributes,4096))return 2;
 int result=1;pthread_t thread;
 int error=pthread_create(&thread,&attributes,worker,&result);pthread_attr_destroy(&attributes);
 if(error || pthread_join(thread,0))return 2;
 if(!result)std::cout<<"PASS: same cases on a guarded 64 KiB computation stack\n";
 return result;
}
