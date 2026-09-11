#include "giacPCH.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
namespace giac {bool cycle4_mellin_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
static int run_cases(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
struct Case{const char *f,*lo,*hi,*want;};
const Case yes[]={
{"x^(-1/3)/((1+x^(2/3))*(1+4*x^(2/3)))","0","+infinity","ln(2)"},
{"x^(-1/3)/((1+4*x^(2/3))*(1+x^(2/3)))","0","+infinity","ln(2)"},
{"1/((1+x)*(1+4*x))","0","+infinity","2*ln(2)/3"},
{"x/((1+x^2)*(1+4*x^2))","0","+infinity","ln(2)/3"},
{"1/(sqrt(x)*(1+sqrt(x))*(1+4*sqrt(x)))","0","+infinity","4*ln(2)/3"},
{"1/((1+x^2)*(1+4*x^2))","0","+infinity","pi/6"},
{"x^2/((1+x^2)*(1+4*x^2))","0","+infinity","pi/12"},
{"1/(sqrt(x)*(1+x)*(1+4*x))","0","+infinity","pi/3"},
{"sqrt(x)/((1+x)*(1+4*x))","0","+infinity","pi/6"},
{"1/((2+2*x^2)*(3+12*x^2))","0","+infinity","pi/36"},
{"7/((2+2*x^2)*(3+12*x^2))","0","+infinity","7*pi/36"},
{"-2*x/((1+x^2)*(1+4*x^2))","0","+infinity","-2*ln(2)/3"},
{"1/((1+x^2)*(1+4*x^2))","+infinity","0","-pi/6"},
{"x^(-1/4)/((1+x)*(1+16*x))","0","+infinity","sqrt(2)*pi/15"},
{"x^(1/4)/((1+x)*(1+16*x))","0","+infinity","sqrt(2)*pi/30"}
};
for(const Case&t:yes){bool ok=cycle4_mellin_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx);gen want=gen(t.want,ctx).eval(1,ctx);
if(!ok||!is_zero(_simplify(r-want,ctx))){std::cerr<<t.f<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}}
const Case no[]={
{"1/(x*(1+x)*(1+4*x))","0","+infinity",0},
{"x/((1+x)*(1+4*x))","0","+infinity",0},
{"x^2/((1+x)*(1+4*x))","0","+infinity",0},
{"1/((1-x)*(1+4*x))","0","+infinity",0},
{"1/((-1+x)*(1+4*x))","0","+infinity",0},
{"1/((1+x)*(1+x))","0","+infinity",0},
{"1/((1+x)*(1+4*x^2))","0","+infinity",0},
{"1/((1+1/x)*(1+4/x))","0","+infinity",0},
{"1/((1+x)*(1+4*x)*(1+2*x))","0","+infinity",0},
{"ln(x)/((1+x)*(1+4*x))","0","+infinity",0},
{"1/((1+x)*(1+4*x))","0","1",0},
{"1/((1+x)*(1+4*x))","1","+infinity",0},
{"1/((1+i*x)*(1+4*x))","0","+infinity",0},
{"1/((1+a*x)*(1+4*x))","0","+infinity",0}
};
for(const Case&t:no)if(cycle4_mellin_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"incorrect acceptance: "<<t.f<<'\n';return 1;}
angle_radian(false,ctx);assert(!cycle4_mellin_rule(gen(yes[0].f,ctx),x,0,plus_inf,r,ctx));
std::cout<<"PASS: 15 distinct-binomial Mellin identities and 15 rejected conditions\n";return 0;
}
static void *worker(void *p){*static_cast<int *>(p)=run_cases();return 0;}
int main(){if(!std::getenv("KHICAS_TEST_STACK_KIB"))return run_cases();
pthread_attr_t attr;assert(!pthread_attr_init(&attr));assert(!pthread_attr_setstacksize(&attr,64*1024));assert(!pthread_attr_setguardsize(&attr,4096));
pthread_t thread;int result=1;assert(!pthread_create(&thread,&attr,worker,&result));pthread_attr_destroy(&attr);assert(!pthread_join(thread,0));return result;
}
