#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_monomial_gaussian_erf(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_atan_cauchy_power(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
using namespace giac;
using Rule=bool (*)(const gen &,const gen &,const gen &,const gen &,gen &,const context *);
static int cases(){context ctx;const context *p=&ctx;gen x(identificateur("x")),r;
struct C{const char *kind,*f,*answer;};const C yes[]={
 {"erf","1*x^(0)*exp(-(1)*x^(1)+(0))*erf((1)*x^(1/2))","sqrt(2)/2"},
 {"erf","1*x^(1)*exp(-(1)*x^(1)+(0))*erf((1)*x^(1/2))","5*sqrt(2)/8"},
 {"erf","1*x^(2)*exp(-(2)*x^(1)+(0))*erf((3)*x^(1/2))","801*sqrt(11)/10648"},
 {"erf","1*x^(1)*exp(-(1)*x^(2)+(0))*erf((1)*x^(1))","sqrt(2)/4"},
 {"erf","1*x^(5)*exp(-(2)*x^(2)+(0))*erf((-1)*x^(1))","-sqrt(3)/16"},
 {"erf","1/3*x^(1/3)*exp(-(3/2)*x^(2/3)+(0))*erf((2/3)*x^(1/3))","194*sqrt(70)/11025"},
 {"erf","1*x^(-1/2)*exp(-(2)*x^(1/2)+(0))*erf((-3)*x^(1/4))","-3*sqrt(11)/11"},
 {"erf","1*x^(5)*exp(-(3)*x^(3)+(1))*erf((2)*x^(3/2))","17*sqrt(7)*exp(1)/1323"},
 {"erf","1*x^(15)*exp(-(1)*x^(2)+(0))*erf((1)*x^(1))","116658675*sqrt(2)/65536"},
 {"erf","1*x^(3)*exp(-(2)*x^(4)+(0))*erf((1)*x^(2))","sqrt(3)/24"},
 {"atan","1*x^(0)*atan((1)*x^(1))^2/((1)+(1)*x^(2))^2","pi*(-6 + pi^2)/48"},
 {"atan","1*x^(0)*atan((1)*x^(1))^1/((1)+(1)*x^(2))^2","-1/4 + pi^2/16"},
 {"atan","1*x^(0)*atan((-1)*x^(1))^3/((1)+(1)*x^(2))^2","-pi^4/128 - 3/8 + 3*pi^2/32"},
 {"atan","1*x^(0)*atan((3)*x^(1))^4/((2)+(18)*x^(2))^3","pi*(-50*pi^2 + 2*pi^4 + 315)/20480"},
 {"atan","1/3*x^(-1/3)*atan((2)*x^(2/3))^2/((3)+(12)*x^(4/3))^2","pi*(-6 + pi^2)/1728"},
 {"atan","1*x^(1)*atan((-2)*x^(2))^1/((1)+(4)*x^(4))^1","-pi^2/32"},
 {"atan","2*x^(2)*atan((2/3)*x^(3))^3/((1/2)+(2/9)*x^(6))^3","-45*pi^2/64 + 3 + 3*pi^4/64"},
 {"atan","1*x^(0)*atan((1)*x^(1))^8/((1)+(1)*x^(2))^4","5*pi*(-14606130*pi^2 - 15876*pi^6 + 162*pi^8 + 717066*pi^4 + 88042955)/11943936"},
 {"atan","1*x^(0)*atan((1)*x^(1))^2/((1)+(1)*x^(2))^1","pi^3/24"},
 {"atan","1*x^(-1/2)*atan((-3)*x^(1/2))^1/((2)+(18)*x^(1))^4","17/1728 - 5*pi^2/3072"},
};
for(const C &t:yes)for(int eval=0;eval<2;++eval){gen f(t.f,p);if(eval)f=f.eval(1,p);f=integration_syntax(f,p);Rule rule=t.kind[0]=='e'?integrate_monomial_gaussian_erf:integrate_atan_cauchy_power;
bool ok=rule(f,x,0,plus_inf,r,p);gen expected=gen(t.answer,p).eval(1,p);if(!ok||!is_zero(normal(r-expected,p))){std::cerr<<"FAIL "<<t.f<<" eval="<<eval<<" matched="<<ok<<" result="<<r<<" expected="<<expected<<'\n';return 1;}}
const char *no_erf[]={"exp(x)*erf(sqrt(x))","exp(-x)*erf(x)","exp(-x)*erf(sqrt(x)+1)","x^(-1)*exp(-x)*erf(sqrt(x))","x^8*exp(-x)*erf(sqrt(x))","exp(-a*x)*erf(sqrt(x))","exp(-x)*erf(b*sqrt(x))","exp(-x)*erf(sqrt(x))^2","exp(-x^17)*erf(x^(17/2))","exp(-x)*erf(sqrt(x))*sin(x)"};
const char *no_atan[]={"atan(x)^2/(1-x^2)^2","atan(x)^2/(2+x^2)^2","atan(x)^2/(1+x^2)^5","atan(x)^9/(1+x^2)^2","atan(x+1)^2/(1+x^2)^2","atan(a*x)^2/(1+x^2)^2","atan(x)^2/x^4","atan(x)^2/(a+x^2)^2","atan(x^2)^2/(1+x^4)^2","sqrt(atan(-x)^2)/(1+x^2)^2"};
for(const char *s:no_erf)if(integrate_monomial_gaussian_erf(integration_syntax(gen(s,p),p),x,0,plus_inf,r,p)){std::cerr<<"unexpected erf "<<s<<'\n';return 1;}
for(const char *s:no_atan)if(integrate_atan_cauchy_power(integration_syntax(gen(s,p),p),x,0,plus_inf,r,p)){std::cerr<<"unexpected atan "<<s<<'\n';return 1;}
gen f=integration_syntax(gen("exp(-x)*erf(sqrt(x))",p),p),g=integration_syntax(gen("atan(x)^2/(1+x^2)^2",p),p);
for(Rule rule:{integrate_monomial_gaussian_erf,integrate_atan_cauchy_power}){gen e=rule==integrate_monomial_gaussian_erf?f:g;if(rule(e,x,minus_inf,plus_inf,r,p)||rule(e,x,1,plus_inf,r,p)||rule(e,x,0,1,r,p))return 1;}
angle_radian(false,p);if(integrate_atan_cauchy_power(g,x,0,plus_inf,r,p))return 1;
std::cout<<"PASS:20 exact families raw/evaluated;27 domain/resource rejections\n";return 0;}
static void *worker(void *p){*static_cast<int *>(p)=cases();return 0;}
int main(){if(cases())return 1;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);pthread_t t;int result=1;if(pthread_create(&t,&a,worker,&result)||pthread_join(t,0))return 2;pthread_attr_destroy(&a);return result;}
