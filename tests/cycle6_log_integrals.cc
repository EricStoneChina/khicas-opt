#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
#include <cstdlib>
namespace giac {bool cycle6_log_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
using namespace giac;
struct Case {const char *f,*lo,*hi,*want;};
static const Case yes[]={
{"x*ln(sin(x))","0","pi/2","7*Zeta(3)/16-pi^2*ln(2)/8"},
{"(2*x+3)*ln(sin(2*x+1))","-1/2","pi/4-1/2","7*Zeta(3)/32-pi^2*ln(2)/16-pi*ln(2)/2"},
{"x*ln(cos(-2*x))","0","pi/4","-pi^2*ln(2)/32-7*Zeta(3)/64"},
{"x*ln(abs(sin(x)))","pi","3*pi/2","7*Zeta(3)/16-5*pi^2*ln(2)/8"},
{"x*ln(sin(x))","pi/2","pi","-3*pi^2*ln(2)/8-7*Zeta(3)/16"},
{"3*ln(cos(x))","0","pi/2","-3*pi*ln(2)/2"},
{"ln(1-x)*ln(1+x)/x","0","1","-5*Zeta(3)/8"},
{"ln(1-4*x^2)*ln(1+4*x^2)/x","0","1/2","-5*Zeta(3)/16"},
{"ln(1+x^3)^2/x","0","1","Zeta(3)/12"},
{"ln(1-sqrt(x))^2/x","0","1","4*Zeta(3)"},
{"-3*ln(1-x)*ln(1+x)/x","0","1","15*Zeta(3)/8"},
{"ln(1+sin(x))","0","pi/2","(Psi(1/4,1)-Psi(3/4,1))/8-pi*ln(2)/2"},
{"ln(2+2*sin(x))","0","pi/2","(Psi(1/4,1)-Psi(3/4,1))/8"},
{"ln(3-3*cos(2*x+1))","-1/2","pi/4-1/2","pi*ln(3/2)/4-(Psi(1/4,1)-Psi(3/4,1))/16"},
{"ln(1+sin(-x))","0","pi/2","-(Psi(1/4,1)-Psi(3/4,1))/8-pi*ln(2)/2"},
{"ln(1+sin(x))","0","2*pi","-2*pi*ln(2)"},
{"atan(x)/(1+x)","0","1","pi*ln(2)/8"},
{"atan(2*x)/(1+2*x)","0","1/2","pi*ln(2)/16"},
{"atan(2*x+1)/(3+3*x)","-1/2","0","pi*ln(2)/24"},
{"atan(1-x)/(2-x)","0","1","pi*ln(2)/8"},
{"x*atan(x^2)/(1+x^2)","0","1","pi*ln(2)/16"},
{"atan(sqrt(x))/(sqrt(x)*(1+sqrt(x)))","0","1","pi*ln(2)/4"}
};
static int run(){context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
 for(int evaluated=0;evaluated<2;++evaluated)for(const Case &t:yes){
  gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);
  bool ok=cycle6_log_rule(f,x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx);
  gen want=gen(t.want,ctx).eval(1,ctx);
  if(!ok || !is_zero(ratnormal(r-want,ctx))){std::cerr<<"FAIL "<<t.f<<" evaluated="<<evaluated<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}
 }
 const Case no[]={
 {"x*ln(sin(x))","0","pi/3",0},
 {"x^2*ln(sin(x))","0","pi/2",0},
 {"x*ln(sin(x))","pi","3*pi/2",0},
 {"ln(1-2*x)*ln(1+2*x)/x","0","1",0},
 {"ln(1-x)*ln(1+2*x)/x","0","1",0},
 {"ln(1-x)*ln(1+x^2)/x","0","1",0},
 {"ln(1-x)^3/x","0","1",0},
 {"ln(1-x^33)^2/x","0","1",0},
 {"ln(1+2*sin(x))","0","pi/2",0},
 {"ln(-1-sin(x))","0","pi/2",0},
 {"ln(1+sin(x))","0","pi/3",0},
 {"atan(2*x)/(1+x)","0","1/2",0},
 {"atan(x)/(1+x)","0","2",0},
 {"atan(x)/(1-x)","0","1",0},
 {"atan(x^2)/(1+x^2)","0","1",0},
 {"atan(x^(-1))/(1+x^(-1))","0","1",0}
 };
 for(const Case &t:no)if(cycle6_log_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"WRONG ACCEPT "<<t.f<<'\n';return 1;}
 angle_radian(false,ctx);
 for(unsigned i:{0u,11u,16u})if(cycle6_log_rule(gen(yes[i].f,ctx),x,gen(yes[i].lo,ctx).eval(1,ctx),gen(yes[i].hi,ctx).eval(1,ctx),r,ctx))return 1;
 std::cout<<"PASS:22 identities raw/evaluated,16 domain/structure guards and3 angle-mode guards\n";return 0;
}
static void *worker(void *p){*static_cast<int *>(p)=run();return 0;}
int main(int argc,char **argv){
 if(argc>1){for(const Case &t:yes)std::cout<<t.f<<'\t'<<t.lo<<'\t'<<t.hi<<'\t'<<t.want<<'\n';return 0;}
 if(!std::getenv("KHICAS_TEST_STACK_KIB"))return run();
 pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);pthread_t t;int result=1;
 int err=pthread_create(&t,&a,worker,&result);pthread_attr_destroy(&a);if(err || pthread_join(t,0))return 2;return result;
}
