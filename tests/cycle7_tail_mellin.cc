#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_erf_tail_product(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_mellin_log(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
using namespace giac;
static int run(){context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
struct Case{const char *f,*answer;bool mellin;};const Case yes[]={
{"erf(x)*erfc(x)","(sqrt(2)-1)/sqrt(pi)",false},
{"erf(2*x)*erfc(3*x)","(sqrt(13)-3)/(6*sqrt(pi))",false},
{"erf(-2*x)*erfc(3*x)","-(sqrt(13)-3)/(6*sqrt(pi))",false},
{"erfc(x)^2","(2-sqrt(2))/sqrt(pi)",false},
{"erfc(2*x)*erfc(3*x)","(5-sqrt(13))/(6*sqrt(pi))",false},
{"x*erf(2*x^2)*erfc(3*x^2)","(sqrt(13)-3)/(12*sqrt(pi))",false},
{"erf(sqrt(x))*erfc(2*sqrt(x))/sqrt(x)","(sqrt(5)-2)/sqrt(pi)",false},
{"ln(1+x^2)/(1+x^2)^(3/2)","2-2*ln(2)",true},
{"ln(1+4*x^2)/(1+4*x^2)^(3/2)","1-ln(2)",true},
{"x*ln(1+x^2)/(1+x^2)^2","1/2",true},
{"x*ln(1+x^2)^2/(1+x^2)^2","1",true},
{"ln(1+x)/(1+x)^2","1",true},
{"ln(2+2*x)/(2+2*x)^2","(1+ln(2))/4",true},
{"ln(1+x^2)^2/(1+x^2)^(3/2)","(2-2*ln(2))^2+4-pi^2/3",true},
{"x*ln(1+x^2)^4/(1+x^2)^2","12",true},};
for(int evaluated=0;evaluated<2;++evaluated)for(const Case &t:yes){gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);f=integration_syntax(f,ctx);bool ok=(t.mellin?integrate_mellin_log:integrate_erf_tail_product)(f,x,0,plus_inf,res,ctx);if(!ok || !is_zero(_simplify(res-gen(t.answer,ctx).eval(1,ctx),ctx))){std::cerr<<"FAIL "<<t.f<<" form "<<f<<" evaluated "<<evaluated<<" got "<<res<<" matched "<<ok<<'\n';return 1;}}
const Case no[]={
{"erf(x)^2","",false},
{"erf(x)*erfc(-x)","",false},
{"erfc(-x)^2","",false},
{"erf(i*x)*erfc(x)","",false},
{"erf(x+1)*erfc(x)","",false},
{"erf(x)*erfc(x^2)","",false},
{"x*erf(x)*erfc(x)","",false},
{"erfc(x)^3","",false},
{"erf(x^-1)*erfc(x^-1)/x^2","",false},
{"ln(1-x)/(1-x)^2","",true},
{"ln(1+x)/(1+x)","",true},
{"ln(1+x)^5/(1+x)^2","",true},
{"ln(2+x)/(1+x)^2","",true},
{"ln(1+x)*ln(x)/(1+x)^2","",true},
{"sqrt(ln(1+x)^2)/(1+x)^2","",true},
{"ln(1+x^2)/(1+x^2)^(1/2)","",true},
{"ln(1+i*x)/(1+i*x)^2","",true},};
for(const Case &t:no)if((t.mellin?integrate_mellin_log:integrate_erf_tail_product)(integration_syntax(gen(t.f,ctx),ctx),x,0,plus_inf,res,ctx)){std::cerr<<"UNEXPECTED "<<t.f<<'\n';return 1;}
std::cout<<"PASS:15 identities raw/evaluated,17 rejected domains\n";return 0;}
static void *worker(void *p){*static_cast<int *>(p)=run();return 0;}
int main(){if(run())return 1;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);int result=1;pthread_t t;int error=pthread_create(&t,&a,worker,&result);pthread_attr_destroy(&a);if(error || pthread_join(t,0))return 2;if(!result)std::cout<<"PASS:guarded64 KiB stack\n";return result;}
