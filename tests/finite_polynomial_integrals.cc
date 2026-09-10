#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_finite_polynomial(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
using namespace giac;
static int cases(){context ctx;const context *p=&ctx;gen x(identificateur("x")),r;
 struct C{const char *f,*lo,*hi,*answer;};
 const C yes[]={
 {"x^2","0","1","1/3"},{"0","-1","3","0"},{"7","-2","3","35"},{"1/3","-1/2","3/4","5/12"},
 {"x","-1","3","4"},{"2*x+3","1/2","3/2","5"},{"x^2","1","0","-1/3"},
 {"x^5-3*x^2+2","-2","3","515/6"},{"x^4/3-x/2+1/7","-2/3","5/4","5395961/26127360"},
 {"(x+1)^2","0","1","7/3"},{"(2*x-1)^4","0","1","1/5"},{"(x+1)*(x-1)*(x+2)","0","1","-19/12"},
 {"(x+1)^8","0","1","511/9"},{"x^64","0","1","1/65"},{"x^63","-1","1","0"},{"(x+1)^8","2","2","0"}
 };
 for(const C &t:yes)for(int eval=0;eval<2;++eval){gen f(t.f,p);if(eval)f=f.eval(1,p);f=integration_syntax(f,p);bool ok=integrate_finite_polynomial(f,x,gen(t.lo,p).eval(1,p),gen(t.hi,p).eval(1,p),r,p);gen expected=gen(t.answer,p).eval(1,p);
  if(!ok||!is_zero(r-expected)){std::cerr<<"FAIL "<<t.f<<" got="<<r<<" expected="<<expected<<" matched="<<ok<<'\n';return 1;}}
 for(const char *s:{"1/x","sqrt(x)","(x+1)^(1/2)","sin(x)","x^65","(x+1)^64","a*x^2","1/(x+1)","(x^2)^40"})if(integrate_finite_polynomial(integration_syntax(gen(s,p),p),x,0,1,r,p)){std::cerr<<"UNEXPECTED "<<s<<'\n';return 1;}
 if(integrate_finite_polynomial(x,x,0,plus_inf,r,p)||integrate_finite_polynomial(x,x,0,cst_pi,r,p)||integrate_finite_polynomial(x,x,0,cst_i,r,p))return 1;
 std::cout<<"PASS:16 finite rational polynomial cases raw/evaluated;12 rejection/resource cases\n";return 0;}
static void *worker(void *p){*static_cast<int *>(p)=cases();return 0;}
int main(){if(cases())return 1;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);pthread_t t;int result=1;if(pthread_create(&t,&a,worker,&result)||pthread_join(t,0))return 2;pthread_attr_destroy(&a);return result;}
