#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_reciprocal_cosh(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
using namespace giac;
static int run_cases(){
 context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
 struct Case{const char *f,*lo,*answer;};
 const Case yes[]={
 {"1/(2+cosh(x))","0","ln(2+sqrt(3))/sqrt(3)"},
 {"1/(2+cosh(-3*x))","0","ln(2+sqrt(3))/(3*sqrt(3))"},
 {"1/(5+3*cosh(2*x))","0","ln(3)/8"},
 {"1/(1+cosh(x))","0","1"},
 {"1/(3+3*cosh(-2*x))","0","1/6"},
 {"1/cosh(x)","0","pi/2"},
 {"1/(-1+2*cosh(x))","0","2*pi/(3*sqrt(3))"},
 {"1/(1+2*cosh(x))","0","pi/(3*sqrt(3))"},
 {"1/(1/2+cosh(x))","0","2*pi/(3*sqrt(3))"},
 {"-3/(2+cosh(x))","0","-sqrt(3)*ln(2+sqrt(3))"},
 {"1/(2+cosh(3*x+7))","-infinity","2*ln(2+sqrt(3))/(3*sqrt(3))"},
 {"1/(2+cosh(-3*x-7))","-infinity","2*ln(2+sqrt(3))/(3*sqrt(3))"},
 {"1/(1+cosh(x+2))","-infinity","2"}
 };
 for(unsigned evaluated=0;evaluated<2;++evaluated)for(const Case &t:yes){
  gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);f=integration_syntax(f,ctx);
  bool ok=integrate_reciprocal_cosh(f,x,gen(t.lo,ctx).eval(1,ctx),plus_inf,res,ctx);gen expected=gen(t.answer,ctx).eval(1,ctx);
  if(!ok || !is_zero(_simplify(res-expected,ctx))){std::cerr<<"FAIL "<<f<<" matched "<<ok<<" got "<<res<<'\n';return 1;}
 }
 const char *no[]={"1/(-1+cosh(x))","1/(-2+cosh(x))","1/(2-cosh(x))","1/(2+i*cosh(x))","1/(2+cosh(x+1))","1/(2+cosh(x^2))","1/(2+cosh(x)+cosh(2*x))","1/(2+cosh(x))^2","1/(a+cosh(x))","1/(2+cosh(i*x))"};
 for(const char *s:no)if(integrate_reciprocal_cosh(integration_syntax(gen(s,ctx),ctx),x,0,plus_inf,res,ctx)){std::cerr<<"UNEXPECTED "<<s<<'\n';return 1;}
 gen f=integration_syntax(gen("1/(2+cosh(x))",ctx),ctx);
 if(integrate_reciprocal_cosh(f,x,0,1,res,ctx))return 1;
 angle_radian(false,ctx);if(integrate_reciprocal_cosh(f,x,0,plus_inf,res,ctx))return 1;
 std::cout<<"PASS: 13 reciprocal-cosh exact identities raw/evaluated, 12 rejected conditions\n";return 0;
}
static void *worker(void *p){*static_cast<int *>(p)=run_cases();return 0;}
int main(){if(run_cases())return 1;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);int result=1;pthread_t t;int error=pthread_create(&t,&a,worker,&result);pthread_attr_destroy(&a);if(error || pthread_join(t,0))return 2;if(!result)std::cout<<"PASS: guarded64 KiB stack\n";return result;}
