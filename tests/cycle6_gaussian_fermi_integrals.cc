#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_gaussian_atan_moment(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_exponential_log_moment(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
using namespace giac;
static int run_cases(){
 context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
 struct Case{const char *f,*lo,*answer;bool fermi;};
 const Case yes[]={
 {"x*exp(-x^2)*atan(x)","0","pi*exp(1)*erfc(1)/4",false},
 {"x*exp(-2*x^2)*atan(3*x)","0","pi*exp(2/9)*erfc(sqrt(2)/3)/8",false},
 {"x*exp(-x^2)*atan(-2*x)","0","-pi*exp(1/4)*erfc(1/2)/4",false},
 {"x^3*exp(-x^2)*atan(x)","0","sqrt(pi)/4",false},
 {"x^5*exp(-x^2)*atan(x)","0","3*sqrt(pi)/8+pi*exp(1)*erfc(1)/4",false},
 {"x^7*exp(-x^2)*atan(x)","0","23*sqrt(pi)/16+pi*exp(1)*erfc(1)/2",false},
 {"x^3*exp(-2*x^2)*atan(x)","0","sqrt(2*pi)/16-pi*exp(2)*erfc(sqrt(2))/16",false},
 {"x^17*exp(-x^2)*atan(x)","0","14833*exp(1)*pi*erfc(1)/4+5764325*sqrt(pi)/512",false},
 {"x*exp(-x^2)*atan(x)","-infinity","pi*exp(1)*erfc(1)/2",false},
 {"x*ln(1+exp(-x))","0","3*Zeta(3)/4",true},
 {"ln(1+exp(-x))","0","pi^2/12",true},
 {"x^2*ln(1+exp(-x))","0","7*pi^4/360",true},
 {"x*ln(1+exp(-2*x))","0","3*Zeta(3)/16",true},
 {"x^3*ln(1+exp(-x^2))","0","3*Zeta(3)/8",true},
 {"ln(1+exp(-sqrt(x)))","0","3*Zeta(3)/2",true},
 {"x*ln(1-exp(-x))","0","-Zeta(3)",true},
 {"x^3*ln(1-exp(-2*x^2))","0","-Zeta(3)/8",true},
 {"x^11*ln(1+exp(-x))","0","39916800*4095*Zeta(13)/4096",true},
 {"-2*x*ln(1+exp(-x))","0","-3*Zeta(3)/2",true}
 };
 for(unsigned evaluated=0;evaluated<2;++evaluated)for(const Case &t:yes){
  gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);f=integration_syntax(f,ctx);
  bool ok=(t.fermi?integrate_exponential_log_moment:integrate_gaussian_atan_moment)(f,x,gen(t.lo,ctx).eval(1,ctx),plus_inf,res,ctx);gen expected=gen(t.answer,ctx).eval(1,ctx);
  if(!ok || !is_zero(_simplify(res-expected,ctx))){std::cerr<<"FAIL evaluated="<<evaluated<<" "<<f<<" matched "<<ok<<" got "<<res<<'\n';return 1;}
 }
 const Case no[]={
 {"x*exp(x^2)*atan(x)","0","",false},
 {"x*exp(-x^2)*atan(x+1)","0","",false},
 {"x*exp(-x^2+x)*atan(x)","0","",false},
 {"x^2*exp(-x^2)*atan(x)","0","",false},
 {"x^19*exp(-x^2)*atan(x)","0","",false},
 {"x*exp(-x^2)*atan(i*x)","0","",false},
 {"x*ln(1+exp(x))","0","",true},
 {"x*ln(1-2*exp(-x))","0","",true},
 {"x*ln(1+exp(1-x))","0","",true},
 {"x^(-1)*ln(1+exp(-x))","0","",true},
 {"x^12*ln(1+exp(-x))","0","",true},
 {"ln(1+exp(-x^2))","0","",true},
 {"x*ln(1+exp(-x))^2","0","",true},
 {"x*ln(1+exp(-i*x))","0","",true},
 {"x*ln(1+exp(-x))","-infinity","",true}
 };
 for(const Case &t:no)if((t.fermi?integrate_exponential_log_moment:integrate_gaussian_atan_moment)(integration_syntax(gen(t.f,ctx),ctx),x,gen(t.lo,ctx).eval(1,ctx),plus_inf,res,ctx)){std::cerr<<"UNEXPECTED "<<t.f<<'\n';return 1;}
 std::cout<<"PASS:19 exact Gaussian-atan/log-exponential identities raw/evaluated,15 rejected conditions\n";return 0;
}
static void *worker(void *p){*static_cast<int *>(p)=run_cases();return 0;}
int main(){if(run_cases())return 1;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);int result=1;pthread_t t;int error=pthread_create(&t,&a,worker,&result);pthread_attr_destroy(&a);if(error || pthread_join(t,0))return 2;if(!result)std::cout<<"PASS:guarded64 KiB stack\n";return result;}
