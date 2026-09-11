#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
#include <cstdlib>
namespace giac {bool cycle7_log_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
using namespace giac;
struct Case{const char*f,*lo,*hi,*want;};
static const Case yes[]={
{"ln((1+x)/(1-x))^2/(1+x^2)","0","1","pi^3/16"},
{"ln((1+x)/(1-x))/(1+x^2)","0","1","(Psi(1/4,1)-Psi(3/4,1))/16"},
{"ln((1+x)/(1-x))^3/(1+x^2)","0","1","(Psi(1/4,3)-Psi(3/4,3))/256"},
{"ln((1+x)/(1-x))^4/(1+x^2)","0","1","5*pi^5/64"},
{"ln((1+x)/(1-x))^8/(1+x^2)","0","1","1385*pi^9/1024"},
{"ln((1+x)/(1-x))/(1+x^2)","-1","0","-(Psi(1/4,1)-Psi(3/4,1))/16"},
{"ln((1+x)/(1-x))/(1+x^2)","-1","1","0"},
{"ln((1+x)/(1-x))^2/(1+x^2)","-1","1","pi^3/8"},
{"ln((1+2*x)/(1-2*x))^2/(1+4*x^2)","0","1/2","pi^3/32"},
{"ln((2+2*x)/(-2*x))^2/(1+(2*x+1)^2)","-1/2","0","pi^3/32"},
{"ln((2*x)/(2-2*x))^2/(3*(1+(2*x-1)^2))","1/2","1","pi^3/96"},
{"ln(2*(1+x)/(1-x))^2/(1+x^2)","0","1","pi^3/16+ln(2)*(Psi(1/4,1)-Psi(3/4,1))/8+pi*ln(2)^2/4"},
{"ln((1-x)/(1+x))^3/(1+x^2)","0","1","-(Psi(1/4,3)-Psi(3/4,3))/256"},
{"ln(2*(1+x)/(1-x))/(1+x^2)","-1","1","pi*ln(2)/2"},
{"ln(sin(x)+cos(x))","0","pi/2","(Psi(1/4,1)-Psi(3/4,1))/16-pi*ln(2)/4"},
{"ln(2*sin(x)+2*cos(x))","0","pi/2","(Psi(1/4,1)-Psi(3/4,1))/16+pi*ln(2)/4"},
{"ln(sin(2*x+1)+cos(2*x+1))","-1/2","pi/4-1/2","(Psi(1/4,1)-Psi(3/4,1))/32-pi*ln(2)/8"},
{"ln(cos(x)-sin(x))","0","pi/4","-(Psi(1/4,1)-Psi(3/4,1))/32-pi*ln(2)/8"},
{"ln(sin(x)-cos(x))","pi/4","pi/2","-(Psi(1/4,1)-Psi(3/4,1))/32-pi*ln(2)/8"},
{"ln(-sin(x)-cos(x))","pi","3*pi/2","(Psi(1/4,1)-Psi(3/4,1))/16-pi*ln(2)/4"},
{"ln(abs(sin(x)+cos(x)))","0","2*pi","-pi*ln(2)"},
{"ln(sin(x)+cos(x))","-pi/4","3*pi/4","-pi*ln(2)/2"},
{"ln(sin(-2*x)+cos(-2*x))","-pi/4","0","(Psi(1/4,1)-Psi(3/4,1))/32-pi*ln(2)/8"},
{"ln(sin(x+pi/4)+cos(x+pi/4))","-pi/4","pi/4","(Psi(1/4,1)-Psi(3/4,1))/16-pi*ln(2)/4"},
{"ln(abs(2*cos(x)-2*sin(x)))","0","pi","pi*ln(2)/2"},
{"ln((1+x)/(1-x))^2/(1+x^2)","1","0","-pi^3/16"},
{"ln((1+x)/(1-x))^2/(-1-x^2)","0","1","-pi^3/16"},
{"ln((sin(x)+cos(x))/3)","0","pi/2","(Psi(1/4,1)-Psi(3/4,1))/16-pi*ln(2)/4-pi*ln(3)/2"},
{"ln(sin(x)+cos(x))/3","pi/2","0","-(Psi(1/4,1)-Psi(3/4,1))/48+pi*ln(2)/12"}
};
static int run(){context c;const context*ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
 for(int evaluated=0;evaluated<2;++evaluated)for(const Case&t:yes){
  gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);
  bool ok=cycle7_log_rule(f,x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx);
  gen want=gen(t.want,ctx).eval(1,ctx);
  if(!ok || !is_zero(ratnormal(r-want,ctx))){std::cerr<<"FAIL "<<t.f<<" evaluated="<<evaluated<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}
 }
 const Case no[]={
 {"ln((1+x)/(1-x))^2/(1+2*x^2)","0","1",0},
 {"ln((1+x)/(1-2*x))^2/(1+x^2)","0","1",0},
 {"ln(-(1+x)/(1-x))^2/(1+x^2)","0","1",0},
 {"ln((1+x)/(1-x))^2/(1+x^2)","0","2",0},
 {"ln((1+x)/(1-x))^9/(1+x^2)","0","1",0},
 {"x*ln((1+x)/(1-x))^2/(1+x^2)","0","1",0},
 {"ln((1+x)/(1-x))^2/(1-x^2)","0","1",0},
 {"ln(sin(x)+cos(2*x))","0","pi/2",0},
 {"ln(2*sin(x)+3*cos(x))","0","pi/2",0},
 {"ln(sin(x)+cos(x))","0","2*pi",0},
 {"ln(cos(x)-sin(x))","0","pi/2",0},
 {"ln(-sin(x)-cos(x))","0","pi/2",0},
 {"ln(sin(x)+cos(x))","0","pi/3",0},
 {"ln(sin(x)+cos(x))","0","+infinity",0}
 };
 for(const Case&t:no)if(cycle7_log_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"WRONG ACCEPT "<<t.f<<'\n';return 1;}
 angle_radian(false,ctx);
 for(unsigned i:{0u,14u})if(cycle7_log_rule(gen(yes[i].f,ctx),x,gen(yes[i].lo,ctx).eval(1,ctx),gen(yes[i].hi,ctx).eval(1,ctx),r,ctx))return 1;
 std::cout<<"PASS:29 identities raw/evaluated,14 domain/structure guards,2 angle guards\n";return 0;
}
static void*worker(void*p){*static_cast<int*>(p)=run();return 0;}
int main(int argc,char**argv){if(argc>1){for(const Case&t:yes)std::cout<<t.f<<'\t'<<t.lo<<'\t'<<t.hi<<'\t'<<t.want<<'\n';return 0;}
 if(!std::getenv("KHICAS_TEST_STACK_KIB"))return run();
 pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);pthread_t t;int result=1;
 int err=pthread_create(&t,&a,worker,&result);pthread_attr_destroy(&a);if(err||pthread_join(t,0))return 2;return result;
}
