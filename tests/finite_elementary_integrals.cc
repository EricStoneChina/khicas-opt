#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_finite_elementary(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
using namespace giac;
static int cases(){context ctx;const context *p=&ctx;gen x(identificateur("x")),r;
 struct C{const char *f,*lo,*hi,*answer;};
 const C yes[]={
 {"sin(x)","0","pi","2"},{"cos(x)","0","pi/2","1"},{"exp(x)","0","1","exp(1)-1"},
 {"ln(x)","1","exp(1)","1"},{"1/x","1","2","ln(2)"},{"1/(1+x^2)","0","1","pi/4"},{"sqrt(x)","0","1","2/3"},
 {"3*sin(2*x)","0","pi/2","3"},{"2*cos(3*x)","0","pi/6","2/3"},{"2*exp(3*x+1)","0","1","2*(exp(4)-exp(1))/3"},
 {"sin(-2*x+pi/2)","0","pi/4","1/2"},{"cos(-2*x+pi)","0","pi/2","0"},{"exp(-2*x+1)","0","1","(exp(1)-exp(-1))/2"},
 {"ln(2*x)","0","1/2","-1/2"},{"ln(1-x)","0","1","-1"},{"atan(x)","0","1","pi/4-ln(2)/2"},
 {"atan(-2*x)","0","1/2","-pi/8+ln(2)/4"},{"2/(4*x^2+1)","0","1/2","pi/4"},
 {"1/x","-2","-1","-ln(2)"},{"1/(2*x+1)","0","1","ln(3)/2"},{"1/x^2","-2","-1","1/2"},
 {"sqrt(1-x)","0","1","2/3"},{"1/sqrt(x)","0","1","2"},{"x^(-2/3)","0","1","3"},
 {"(2*x+1)^(1/3)","0","7/2","45/8"},{"sqrt(x^2)","0","1","1/2"},{"1/x","2","1","-ln(2)"},
 {"sin(x)","pi","0","-2"},{"abs(2*x-1)","0","1","1/2"}
 };
 for(const C &t:yes)for(int eval=0;eval<2;++eval){gen f(t.f,p);if(eval)f=f.eval(1,p);f=integration_syntax(f,p);bool ok=integrate_finite_elementary(f,x,gen(t.lo,p).eval(1,p),gen(t.hi,p).eval(1,p),r,p);gen expected=gen(t.answer,p).eval(1,p);
  if(!ok||!is_zero(normal(r-expected,p))){std::cerr<<"FAIL "<<t.f<<" got="<<r<<" expected="<<expected<<" matched="<<ok<<'\n';return 1;}}
 const C no[]={
 {"1/x","-1","1",""},{"1/x^2","-1","1",""},{"1/x","0","1",""},{"x^(-3/2)","0","1",""},
 {"sqrt(x)","-1","1",""},{"sqrt(x^2)","-1","1",""},{"ln(x)","-1","1",""},{"ln(-x)","0","1",""},
 {"1/(x^2-1)","0","2",""},{"1/(1-x^2)","0","2",""},{"sin(x^2)","0","1",""},{"exp(x^2)","0","1",""},
 {"ln(x^2)","0","1",""},{"sin(a*x)","0","1",""},{"a*sin(x)","0","1",""},{"x^(1/17)","0","1",""},
 {"x^65","0","1",""},{"sin(x)","0","+infinity",""},{"ln(x)","0","a",""},{"sin(x)","0","i",""}
 };
 for(const C &t:no)if(integrate_finite_elementary(integration_syntax(gen(t.f,p),p),x,gen(t.lo,p).eval(1,p),gen(t.hi,p).eval(1,p),r,p)){std::cerr<<"UNEXPECTED "<<t.f<<'\n';return 1;}
 angle_radian(false,p);
 if(integrate_finite_elementary(integration_syntax(gen("sin(x)",p),p),x,0,1,r,p))return 1;
 angle_radian(true,p);
 std::cout<<"PASS:29 finite elementary cases raw/evaluated;21 domain/resource rejections\n";return 0;}
static void *worker(void *p){*static_cast<int *>(p)=cases();return 0;}
int main(){if(cases())return 1;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);pthread_t t;int result=1;if(pthread_create(&t,&a,worker,&result)||pthread_join(t,0))return 2;pthread_attr_destroy(&a);return result;}
