#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {gen integration_syntax(const gen &,GIAC_CONTEXT);bool integrate_gamma_log_moment(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
using namespace giac;
static int run(){context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
struct Case{const char *f,*want;};const Case yes[]={
{"exp(-x^2)*ln(x)^2","sqrt(pi)*(Psi(1/2)^2+Psi(1/2,1))/8"},
{"exp(-x)","1"},
{"exp(-2*x^2)","sqrt(pi)/(2*sqrt(2))"},
{"exp(-x)*ln(x)","Psi(1)"},
{"exp(-x)*ln(x)^2","Psi(1)^2+pi^2/6"},
{"exp(-x)*ln(x)^3","Psi(1)^3+pi^2*Psi(1)/2-2*Zeta(3)"},
{"exp(-x)*ln(x)^4","Psi(1)^4+pi^2*Psi(1)^2-8*Psi(1)*Zeta(3)+3*pi^4/20"},
{"x^3*exp(-x^2)*ln(x)","Psi(2)/4"},
{"exp(-2*x^2)*ln(3*x^2)^2","sqrt(pi)*((Psi(1/2)+ln(3/2))^2+Psi(1/2,1))/(2*sqrt(2))"},
{"exp(-sqrt(x))*ln(x)^2","8*(Psi(2)^2+Psi(2,1))"},
{"x^(1/3)*exp(-x^(2/3))*ln(x)","9*Psi(2)/4"},
{"exp(-x^2)*ln(1/x)^3","-sqrt(pi)*(Psi(1/2)^3+3*Psi(1/2)*Psi(1/2,1)+Psi(1/2,2))/16"},
{"ln(x)^2/exp(2*x)","((Psi(1)-ln(2))^2+pi^2/6)/2"},
{"exp(-x)*ln(x)/3","Psi(1)/3"},
{"x^15*exp(-x)","factorial(15)"},};
for(unsigned evaluated=0;evaluated<2;++evaluated)for(const Case &t:yes){gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);f=integration_syntax(f,ctx);bool ok=integrate_gamma_log_moment(f,x,0,plus_inf,res,ctx);if(!ok || !is_zero(_simplify(res-gen(t.want,ctx).eval(1,ctx),ctx))){std::cerr<<"FAIL "<<t.f<<" evaluated="<<evaluated<<" matched="<<ok<<" -> "<<res<<'\n';return 1;}}
const char *no[]={
"exp(x)*ln(x)","exp(-i*x)*ln(x)","exp(-x^2)*sqrt(ln(x)^2)","exp(-x)*ln(-x)","exp(-x)*ln(x+1)","exp(-x)*ln(x)*ln(2*x)","exp(-x)*ln(x)^5","exp(-x)*sqrt((-ln(x))^2)","exp(-x)*(-ln(x))^(1/2)","exp(-x)/x","exp(-x^(-2))*ln(x)","exp(-x+1)*ln(x)","x^16*exp(-x)","x^(-16/17)*exp(-x)"};
for(const char *t:no)if(integrate_gamma_log_moment(integration_syntax(gen(t,ctx),ctx),x,0,plus_inf,res,ctx)){std::cerr<<"UNEXPECTED "<<t<<'\n';return 1;}
if(integrate_gamma_log_moment(integration_syntax(gen("exp(-x)",ctx),ctx),x,-1,plus_inf,res,ctx))return 1;
std::cout<<"PASS:15 Gamma/Mellin identities raw/evaluated,15 domain/resource rejections\n";return 0;}
static void *worker(void *p){*static_cast<int *>(p)=run();return 0;}
int main(){if(run())return 1;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);int result=1;pthread_t t;int error=pthread_create(&t,&a,worker,&result);pthread_attr_destroy(&a);if(error || pthread_join(t,0))return 2;return result;}
