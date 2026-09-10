#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
namespace giac {
bool integrate_real_definite(const gen &,const gen &,gen,gen,gen &,GIAC_CONTEXT);
bool integrate_parameter_kernel(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
gen integration_syntax(const gen &,GIAC_CONTEXT);
}
static void checks(){using namespace giac;context ctx;const context *c=&ctx;gen x=identificateur("x"),res;
 for(const char *a:{"assume(a,complex)","assume(b,complex)","assume(s,complex)"})gen(a,c).eval(1,c);
 const char *v[][4]={
 {"exp(-a*x)","0","+infinity","when(re(a)>0,1/a,undef)"},
 {"x^(s-1)*exp(-x)","0","+infinity","when(re(s)>0,Gamma(s),undef)"},
 {"x^(a-1)*(1-x)^(b-1)","0","1","when((re(a)>0) and (re(b)>0),Gamma(a)*Gamma(b)/Gamma(a+b),undef)"},
 {"x^(s-1)/(1+x)","0","+infinity","when((re(s)>0) and (1-re(s)>0),pi/sin(pi*s),undef)"}};
 for(const auto &r:v){
  assert(integrate_real_definite(gen(r[0],c),x,gen(r[1],c).eval(1,c),gen(r[2],c).eval(1,c),res,c));
  gen expected=gen(r[3],c).eval(1,c);assert(res.is_symb_of_sommet(at_when) && expected.is_symb_of_sommet(at_when));
  const vecteur &got=*res._SYMBptr->feuille._VECTptr,&want=*expected._SYMBptr->feuille._VECTptr;
  if(got[0]!=want[0]){std::cerr<<"Condition mismatch "<<got[0]<<" != "<<want[0]<<'\n';assert(false);}
  assert(is_undef(got[2]));assert(is_zero(recursive_ratnormal(got[1]-want[1],c)));
 }
 for(const char *f:{"x^(i-1)*exp(-x)","x^i/(1+x)","exp(i*x)","exp((1+i)*x)"}){
  assert(integrate_real_definite(gen(f,c),x,0,plus_inf,res,c));assert(is_undef(res));
 }
 // Uniform sufficient conditions for complex scaled Gamma remain explicit;
 // the purely imaginary boundary may converge, so it is NOT called divergent.
 gen("assume(c,complex)",c).eval(1,c);
 assert(integrate_real_definite(gen("x^(s-1)*exp(-c*x)",c),x,0,plus_inf,res,c));
 assert(res.is_symb_of_sommet(at_when));assert(res._SYMBptr->feuille[2].is_symb_of_sommet(at_quote));
 assert(contains(res._SYMBptr->feuille[2],*at_integrate));
 for(const char *f:{"exp(-a*x+1)","x^(s-1)/(1-x)","x^(s-1)*exp(-x^2)","x^(s-1)*exp(-x)*ln(x)"})
  assert(!integrate_parameter_kernel(integration_syntax(gen(f,c),c),x,0,plus_inf,res,c));
 std::cout<<"PASS: 4 exact complex condition+answer contracts, 4 necessary-boundary divergences, Gamma boundary deferral and 4 unsupported structures\n";
}
static void *worker(void *){checks();return 0;}
int main(){checks();pthread_attr_t a;assert(!pthread_attr_init(&a));assert(!pthread_attr_setstacksize(&a,65536));assert(!pthread_attr_setguardsize(&a,4096));pthread_t t;assert(!pthread_create(&t,&a,worker,0));pthread_attr_destroy(&a);assert(!pthread_join(t,0));}
