#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
namespace giac {bool user_period_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
static int run_cases(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
struct Case{const char *f,*lo,*hi,*want;};
const Case yes[]={
{"1/(5-3*cos(x))","0","2*pi","pi/2"},
{"1/(5-3*cos(x))","2*pi","0","-pi/2"},
{"7/(5-3*cos(x))","0","2*pi","7*pi/2"},
{"-2/(5-3*cos(x))","0","2*pi","-pi"},
{"1/(-5+3*cos(x))","0","2*pi","-pi/2"},
{"1/(6+3*cos(x)+4*sin(x))","0","2*pi","2*pi/sqrt(11)"},
{"3/(-6-3*cos(2*x+1)-4*sin(2*x+1))","1","1+3*pi","-9*pi/sqrt(11)"},
{"1/(5+3*sin(x))","1/3","1/3+2*pi","pi/2"},
{"1/(5-3*cos(3*x+pi/7))","-pi/3","pi/3","pi/6"},
{"1/(5-3*cos(-2*x+1+pi/4))","pi/3","pi/3+pi","pi/4"},
{"1/(1+cos(x)/2+sin(x)/3)","0","2*pi","12*pi/sqrt(23)"},
{"1/(2*(5-3*cos(x)))","0","2*pi","pi/4"},
{"1/(-2*(5-3*cos(x)))","0","2*pi","-pi/4"},
{"1/(5-3*cos(x))","0","2000000*pi","500000*pi"},
{"1/(5-3*cos(x))","1","1","0"}
};
for(const Case&t:yes){bool ok=user_period_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx);gen want=gen(t.want,ctx).eval(1,ctx);
if(!ok||!is_zero(_simplify(r-want,ctx))){std::cerr<<t.f<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}}
const Case no[]={
{"1/(1-cos(x))","0","2*pi",0},
{"1/(1-2*cos(x))","0","2*pi",0},
{"1/(-1+cos(x))","0","2*pi",0},
{"1/(5+3*cos(x)+4*sin(x))","0","2*pi",0},
{"1/(4+3*cos(x)+4*sin(x))","0","2*pi",0},
{"1/(5-3*cos(x))","0","pi",0},
{"1/(5-3*cos(x))","0","1",0},
{"1/(5-3*cos(x))","i","i+2*pi",0},
{"1/(5-3*cos(x))","y","y+2*pi",0},
{"1/(6+3*cos(x)+4*sin(2*x))","0","2*pi",0},
{"1/(6+3*cos(x)+4*sin(x+1))","0","2*pi",0},
{"1/(5-3*cos(x^2))","0","2*pi",0},
{"1/(5-3*cos(x+i))","0","2*pi",0},
{"1/(5-3*cos(x+y))","0","2*pi",0},
{"x/(5-3*cos(x))","0","2*pi",0},
{"1/(5-3*cos(x))^2","0","2*pi",0}
};
for(const Case&t:no)if(user_period_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"incorrect acceptance: "<<t.f<<'\n';return 1;}
angle_radian(false,ctx);assert(!user_period_rule(gen(yes[0].f,ctx),x,0,2*cst_pi,r,ctx));
std::cout<<"PASS: 15 complete-period identities and 17 rejected conditions\n";return 0;
}

static void *worker(void *p){*static_cast<int *>(p)=run_cases();return 0;}
int main(){
  if(!std::getenv("KHICAS_TEST_STACK_KIB")) return run_cases();
  pthread_attr_t attr;assert(!pthread_attr_init(&attr));
  assert(!pthread_attr_setstacksize(&attr,64*1024));assert(!pthread_attr_setguardsize(&attr,4096));
  pthread_t thread;int result=1;assert(!pthread_create(&thread,&attr,worker,&result));
  pthread_attr_destroy(&attr);assert(!pthread_join(thread,0));return result;
}
