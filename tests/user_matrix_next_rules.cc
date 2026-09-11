#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
namespace giac {
bool integrate_real_definite(const gen &,const gen &,gen,gen,gen &,GIAC_CONTEXT);
bool integrate_reciprocal_quartic(const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_log_trig_sum(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_log_trig_quarters(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_exp_difference(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_unit_log_arc(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_positive_cosine_kernel(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
static void checks(){using namespace giac;context ctx;const context *c=&ctx;gen x=identificateur("x"),r;
 const char *positive[][4]={
 {"sqrt((x-1/2)^2)","-1","3","17/4"},
 {"sqrt((2-3*x)^2)","-1","2","41/6"},
 {"sqrt(sin(x)^2)","0","2*pi","4"},
 {"-3*sqrt(cos(2*x+1)^2)","0","3*pi","-18"},
 {"ln(cos(x))","0","pi/4","-pi*ln(2)/4+(Psi(1/4,1)-Psi(3/4,1))/32"},
 {"ln(5-3*sin(2*x))","0","pi","pi*ln(9/2)"},
 {"ln(3*cos(x)^2+sin(x)^2)","0","pi/2","pi*ln((sqrt(3)+1)/2)"},
 {"(exp(-x)-exp(-3*x))*cos(x)/x","0","+infinity","ln(5)/2"}};
 for(const auto &v:positive){
  assert(integrate_real_definite(gen(v[0],c),x,gen(v[1],c).eval(1,c),gen(v[2],c).eval(1,c),r,c));
  gen d=_evalf(r-gen(v[3],c).eval(1,c),c);assert(is_zero(d) || (d.type==_DOUBLE_ && std::abs(d._DOUBLE_val)<1e-12));
 }
 for(const char *s:{"ln(1+2*cos(x))","ln(-2+cos(x))","ln(2+cos(x)^2+sin(2*x)^2)"})
  assert(!integrate_log_trig_sum(integration_syntax(gen(s,c),c),x,0,cst_pi,r,c));
 assert(!integrate_log_trig_sum(integration_syntax(gen("ln(2+cos(x))",c),c),x,0,cst_pi/2,r,c));
 assert(!integrate_log_trig_quarters(integration_syntax(gen("ln(sin(x))",c),c),x,cst_pi,gen(5)*cst_pi/4,r,c));
 assert(!integrate_log_trig_quarters(integration_syntax(gen("ln(cos(x))",c),c),x,0,cst_pi,r,c));
 for(const char *s:{"(exp(-x)+exp(-3*x))*cos(x)/x","(exp(x)-exp(-3*x))*cos(x)/x","(exp(-x)-exp(-3*x))*cos(x+1)/x","(exp(-x)-exp(-3*x))*cos(x)/x^2","(exp(-x)-exp(-3*x))*cos(x)*ln(x)/x"})
  assert(!integrate_exp_difference(integration_syntax(gen(s,c),c),x,0,plus_inf,r,c));
 for(const char *s:{"(1+x^2)/((x^2-1)*sqrt(x^4-3*x^2+1))","(2+x^2)/((x^2-1)*sqrt(x^4+1))","(1+x^2)/((x^2-1)*sqrt(x^4+2))"})
  assert(!integrate_reciprocal_quartic(integration_syntax(gen(s,c),c),x,r,c));
 for(const char *v:{"atan(x)/x^2","atan(x)/(x*sqrt(1+x^2))","atan(x)/(x*sqrt(1-2*x^2))","ln(-x)/(1+x^2)","ln(x)/(1+2*x^2)"})
  assert(!integrate_unit_log_arc(integration_syntax(gen(v,c),c),x,0,1,r,c));
 gen("assume(a>0)",c).eval(1,c);gen("assume(b>0)",c).eval(1,c);
 const char *parameter[][2]={
  {"(exp(-a*x)-exp(-b*x))/x","ln(b)-ln(a)"},
  {"exp(-a*x)*sin(b*x)/x","atan(b/a)"},
  {"exp(-a*x)*(1-cos(b*x))/x","ln(1+b^2/a^2)/2"}};
 for(const auto &v:parameter){assert(integrate_real_definite(gen(v[0],c),x,0,plus_inf,r,c));assert(is_zero(recursive_ratnormal(r-gen(v[1],c).eval(1,c),c)));}
 gen("assume(r>-1 and r<1)",c).eval(1,c);gen n=identificateur("n");
 gen("assume(n,integer)",c).eval(1,c);assert(is_assumed_integer(n,c));
 gen("additionally(n>=1)",c).eval(1,c);assert(is_assumed_integer(n,c));assert(is_strictly_positive(n,c));
 assert(integrate_real_definite(gen("cos(n*x)/(1-2*r*cos(x)+r^2)",c),x,0,2*cst_pi,r,c));
 // Use a distinct result identifier: the CAS parameter r remains unassigned.
 assert(is_zero(ratnormal(r-gen("2*pi*r^n/(1-r^2)",c).eval(1,c),c)));
 assert(integrate_real_definite(gen("ln(1-2*r*cos(x)+r^2)*cos(n*x)",c),x,0,2*cst_pi,r,c));
 assert(is_zero(ratnormal(r-gen("-2*pi*r^n/n",c).eval(1,c),c)));
 gen("purge(n)",c).eval(1,c);gen("assume(n>0)",c).eval(1,c);assert(!is_assumed_integer(n,c));
 assert(!integrate_positive_cosine_kernel(integration_syntax(gen("cos(n*x)/(1-2*r*cos(x)+r^2)",c),c),x,0,2*cst_pi,r,c));
 assert(!integrate_positive_cosine_kernel(integration_syntax(gen("cos(x/2)/(2+cos(x))",c),c),x,0,2*cst_pi,r,c));
 assert(!integrate_positive_cosine_kernel(integration_syntax(gen("cos(2*x)/(1+2*cos(x))",c),c),x,0,2*cst_pi,r,c));
 assert(!integrate_positive_cosine_kernel(integration_syntax(gen("cos(2*x+1)/(2+cos(x))",c),c),x,0,2*cst_pi,r,c));
 for(const char *v:{"cos(x)","sin(3*x+1)","-2*cos(4*x)"}){
  assert(integrate_real_definite(gen(v,c),x,0,plus_inf,r,c));assert(is_undef(r));
 }
 std::cout<<"PASS: 13 identities including symbolic parameters, 23 rejections, integer-assumption checks and 3 ordinary divergence checks\n";
}
static void *worker(void *){checks();return 0;}
int main(){checks();pthread_attr_t a;assert(!pthread_attr_init(&a));assert(!pthread_attr_setstacksize(&a,65536));assert(!pthread_attr_setguardsize(&a,4096));pthread_t t;assert(!pthread_create(&t,&a,worker,0));pthread_attr_destroy(&a);assert(!pthread_join(t,0));}
