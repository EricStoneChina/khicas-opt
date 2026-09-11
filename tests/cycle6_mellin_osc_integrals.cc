#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
namespace giac {bool cycle6_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
static int cases(){using namespace giac;context ctx;const context *c=&ctx;gen x(identificateur("x"));
struct C{const char *f,*expected;};const C tests[]={
 {"ln(x)/((1+x)*(1+x^2))","-pi^2/16"},
 {"x*ln(x)/((1+x)*(1+x^2))","pi^2/16"},
 {"ln(x)^2/((1+x)*(1+x^2))","pi^3/16"},
 {"1/((1+x)*(1+x^2))","pi/4"},
 {"x/((1+x)*(1+x^2))","pi/4"},
 {"ln(x)/((1+2*x)*(1+4*x^2))","-pi^2/32-pi*ln(2)/8"},
 {"ln(x)^2/((1+2*x)*(1+4*x^2))","pi^3/32+pi^2*ln(2)/16+pi*ln(2)^2/8"},
 {"x*ln(x)/((1+x^2)*(1+x^4))","-pi^2/64"},
 {"x*ln(x)^2/((1+x^2)*(1+x^4))","pi^3/128"},
 {"1/(sqrt(x)*(1+x)*(1+x^2))","pi/2"},
 {"x^(1/2)/((1+x)*(1+x^2))","pi*(sqrt(2)-1)/2"},
 {"(sin(x)-x*cos(x))/x^3","pi/4"},
 {"(sin(3*x)-3*x*cos(3*x))/x^3","9*pi/4"},
 {"(sin(-2*x)+2*x*cos(-2*x))/x^3","-pi"},
 {"(sin(x)-sin(2*x)/2)/x^3","pi/4"},
 {"(sin(x)-x)/x^3","-pi/4"},
 {"(1-cos(x))/x^2","pi/2"},
 {"(cos(2*x)-cos(3*x))/x^2","pi/2"},
 {"3*(sin(2*x)-2*x*cos(2*x))/x^3","3*pi"}
};
for(const C &t:tests)for(int raw=0;raw<2;++raw){gen f(t.f,c),r;if(!raw)f=f.eval(1,c);bool ok=cycle6_rule(f,x,0,plus_inf,r,c);gen ref=gen(t.expected,c).eval(1,c);
 if(!ok || !is_zero(_simplify(r-ref,c))){std::cerr<<t.f<<" matched="<<ok<<" result="<<r<<" expected="<<ref<<'\n';return 1;}}
for(const char *f:{"ln(x)/((1+x)*(1+2*x^2))","ln(x)^3/((1+x)*(1+x^2))","1/(x*(1+x)*(1+x^2))","x^2/((1+x)*(1+x^2))","ln(x)/((1-x)*(1+x^2))","(sin(x)-2*x*cos(x))/x^3","sin(x)/x^3","(1+cos(x))/x^2","(sin(x+1)-x*cos(x+1))/x^3","(sin(x)-x*cos(x))/x^4"}){gen r;assert(!cycle6_rule(gen(f,c).eval(1,c),x,0,plus_inf,r,c));}
for(const char *f:{"(sin(x)-x*cos(x))/x^3","(1-cos(x))/x^2"}){gen r,half;assert(cycle6_rule(gen(f,c).eval(1,c),x,0,plus_inf,half,c));assert(cycle6_rule(gen(f,c).eval(1,c),x,minus_inf,plus_inf,r,c));assert(r==2*half);assert(cycle6_rule(gen(f,c).eval(1,c),x,minus_inf,0,r,c));assert(r==half);}
std::cout<<"PASS: 19 exact mixed-Mellin/oscillatory variants raw/evaluated, 10 rejections, signed intervals\n";return 0;}
static void *worker(void *p){*static_cast<int *>(p)=cases();return 0;}
int main(int argc,char **argv){if(argc==2){using namespace giac;context c;gen r,x(identificateur("x"));if(!cycle6_rule(gen(argv[1],&c).eval(1,&c),x,0,plus_inf,r,&c))return 2;std::cout<<evalf_double(r,1,&c)<<"\n";return 0;}if(cases())return 1;pthread_attr_t a;assert(!pthread_attr_init(&a));assert(!pthread_attr_setstacksize(&a,65536));assert(!pthread_attr_setguardsize(&a,4096));pthread_t t;int r=1;assert(!pthread_create(&t,&a,worker,&r));pthread_attr_destroy(&a);assert(!pthread_join(t,0));return r;}
