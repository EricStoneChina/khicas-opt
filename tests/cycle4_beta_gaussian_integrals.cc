#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <cmath>
#include <pthread.h>
namespace giac {bool cycle4_beta_gaussian_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
static int check_cases(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
struct Case{const char *f,*hi,*answer;};
const Case yes[]={
 {"ln(x)*ln(1-x)*ln(x*(1-x))/sqrt(x*(1-x))","1","pi*(4*Zeta(3)-16*ln(2)^3)"},
 {"sqrt(x/(1-x))*ln(x/(1-x))^3","1","3*pi^3"},
 {"ln(x)^2*ln(1-x)","1","-6+pi^2/3+2*Zeta(3)"},
 {"ln(x)*ln(1-x)*ln(x*(1-x))","1","-12+2*pi^2/3+4*Zeta(3)"},
 {"ln(x)*ln(1-x)*ln(x/(1-x))","1","0"},
 {"ln(x^2/(1-x))*ln(x/(1-x)^2)*ln(x/(1-x))","1","0"},
 {"ln(x/(1-x))*ln(x^2/(1-x))","1","pi^2/2"},
 {"exp(-(sqrt(x)-2/sqrt(x))^2)/sqrt(x)","+infinity","sqrt(pi)"},
 {"exp(-(sqrt(x)+2/sqrt(x))^2)/sqrt(x)","+infinity","sqrt(pi)*exp(-8)"},
 {"exp(-3*(2*sqrt(x)-1/sqrt(x))^2)/sqrt(x)","+infinity","sqrt(pi/12)"},
 {"exp(-(x-2/x)^2)","+infinity","sqrt(pi)/2"},
 {"x*exp(-(x^2-1/x^2)^2)","+infinity","sqrt(pi)/4"},
 {"sqrt(x)*exp(-(sqrt(x)-2/sqrt(x))^2)","+infinity","5*sqrt(pi)/2"},
 {"exp(-(sqrt(x)-2/sqrt(x))^2)/x^(3/2)","+infinity","sqrt(pi)/2"},
 {"exp(-(sqrt(x)-2/sqrt(x))^2-x-1/x)/sqrt(x)","+infinity","sqrt(pi/2)*exp(4-2*sqrt(10))"},
 {"exp(-2*x-3/x)/sqrt(x)","+infinity","sqrt(pi/2)*exp(-2*sqrt(6))"}
};
for(const Case&t:yes){bool ok=cycle4_beta_gaussian_rule(gen(t.f,ctx),x,0,gen(t.hi,ctx).eval(1,ctx),r,ctx);gen want=gen(t.answer,ctx).eval(1,ctx);if(!ok||!is_zero(_simplify(r-want,ctx))){std::cerr<<t.f<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}}
// Independent 50-digit quadrature of the input, split at 1/2.
const char *numeric_f[]={"x*(1-x)^2*ln(x)^2*ln(1-x)","x^(1/3)*(1-x)^(1/5)*ln(x)^2*ln(1-x)","x^(44/3)*(1-x)^(74/5)*ln(x^2/(1-x))*ln(x*(1-x))*ln(x/(1-x)^2)"};
const double numeric[]={-0.0336158466628415495517834939184852683,-0.179755850997773059583822610980346128,8.42401725430293851766394161002215e-11};
for(unsigned i=0;i<3;++i){assert(cycle4_beta_gaussian_rule(gen(numeric_f[i],ctx),x,0,1,r,ctx));gen v=evalf_double(r,1,ctx);assert(v.type==_DOUBLE_ && std::abs(v._DOUBLE_val/numeric[i]-1)<1e-11);if(i==2){std::cout<<"Mixed Beta near parameter bounds: "<<r.print(ctx).size()<<" characters\n";assert(r.print(ctx).size()<4000);}}
const Case no[]={
 {"ln(x)^2*ln(1-x)^2","1",0},
 {"ln(x)*ln(1-x)/x","1",0},
 {"ln(x)*ln(x-1)","1",0},
 {"sqrt(ln(x)^2)*ln(1-x)","1",0},
 {"ln(x)*ln(1-x)*ln(x*(1-x))^15","1",0},
 {"exp(-(sqrt(x)-2/sqrt(x))^3)/sqrt(x)","+infinity",0},
 {"exp(-(sqrt(x)-2/sqrt(x)+1)^2)/sqrt(x)","+infinity",0},
 {"exp(-(sqrt(x)-2/x)^2)/sqrt(x)","+infinity",0},
 {"exp((sqrt(x)-2/sqrt(x))^2)/sqrt(x)","+infinity",0},
 {"exp(-(sqrt(x)-2/sqrt(x))^2)/x","+infinity",0},
 {"exp(-(sqrt(x)-2/sqrt(x))^2)/sqrt(x)","1",0}
};
for(const Case&t:no)if(cycle4_beta_gaussian_rule(gen(t.f,ctx),x,0,gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"incorrect acceptance "<<t.f<<" -> "<<r<<'\n';return 1;}
std::cout<<"PASS: 16 exact Beta/Gaussian identities, 3 independent quadratures and 11 rejected conditions on a guarded 64 KiB host stack\n";
return 0;
}

static int status;
static void *worker(void *){status=check_cases();return 0;}
int main(){
 pthread_attr_t attr;assert(pthread_attr_init(&attr)==0);
 assert(pthread_attr_setstacksize(&attr,64*1024)==0);
 assert(pthread_attr_setguardsize(&attr,4096)==0);
 pthread_t thread;assert(pthread_create(&thread,&attr,worker,0)==0);
 pthread_attr_destroy(&attr);assert(pthread_join(thread,0)==0);return status;
}
