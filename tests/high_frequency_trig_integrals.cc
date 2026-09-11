#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
namespace giac {bool high_frequency_rule(const gen &,const gen &,gen &,GIAC_CONTEXT);}
static int cases(){using namespace giac;context ctx;const context *c=&ctx;angle_radian(true,c);
 gen x(identificateur("x")),actual;
 // Exact proof of the two addition identities used by the product rule,
 // with independent symbolic angles. No numerical samples are accepted.
 gen u(identificateur("u")),v(identificateur("v"));
 assert(is_zero(ratnormal(_texpand(sin(u+v,c),c)-sin(u,c)*cos(v,c)-cos(u,c)*sin(v,c),c)));
 assert(is_zero(ratnormal(_texpand(cos(u+v,c),c)-cos(u,c)*cos(v,c)+sin(u,c)*sin(v,c),c)));
 unsigned checked=0;
 for(int mode=0;mode<2;++mode){complex_mode(mode,c);
  for(const char *trig:{"sin","cos"})for(int n=1;n<=8;++n){
   std::string t=trig,k=print_INT_(n),high=print_INT_(n+2),middle=print_INT_(n+1);
   std::string f=t+"("+high+"*x)*"+t+"(x)^"+k;
   gen input=gen(f,c).eval(1,c);
   assert(high_frequency_rule(input,x,actual,c));
   // For low orders independently expand trig expressions and prove the
   // differentiated candidate equals the original input exactly.
   assert(is_zero(_simplify(_texpand(derive(actual,x,c)-input,c),c)));
   ++checked;
  }
 }
 complex_mode(false,c);
 struct Case {const char *input,*primitive;};
 const Case checks[]={
  {"sin(101*x)*sin(x)^99","sin(100*x)*sin(x)^100/100"},
  {"cos(101*x)*cos(x)^99","sin(100*x)*cos(x)^100/100"},
  {"-7/3*sin(21*x+7)*sin(3*x+1)^5","-7/54*sin(18*x+6)*sin(3*x+1)^6"},
  {"5*cos(14*x+7/2)*cos(2*x+1/2)^5","5/12*sin(12*x+3)*cos(2*x+1/2)^6"},
  {"sin(-14*x-7/2)*sin(2*x+1/2)^5","-sin(12*x+3)*sin(2*x+1/2)^6/12"},
  {"cos(-14*x-7/2)*cos(2*x+1/2)^5","sin(12*x+3)*cos(2*x+1/2)^6/12"},
  {"sin(-21*x+7)*sin(-3*x+1)^5","-sin(-18*x+6)*sin(-3*x+1)^6/18"},
  {"sin(7*x+7*pi/11)*sin(x+pi/11)^5","sin(6*x+6*pi/11)*sin(x+pi/11)^6/6"},
  {"pi*sin(5*x)*sin(x)^3","pi*sin(4*x)*sin(x)^4/4"},
  {"k*cos(5*x)*cos(x)^3","k*sin(4*x)*cos(x)^4/4"},
  {"sin(5*x+5*i)*sin(x+i)^3","sin(4*x+4*i)*sin(x+i)^4/4"},
  {"sin(32767*x)*sin(x)^32765","sin(32766*x)*sin(x)^32766/32766"}
 };
 for(const Case &t:checks)for(int evaluated=0;evaluated<2;++evaluated){
  gen input(t.input,c);if(evaluated)input=input.eval(1,c);
  assert(high_frequency_rule(input,x,actual,c));
  gen expected=gen(t.primitive,c).eval(1,c);
  if(!is_zero(recursive_ratnormal(actual-expected,c))){std::cerr<<t.input<<" -> "<<actual<<" != "<<expected<<"\n";return 1;}
  assert(actual.print(c).size()<160);
  ++checked;
 }
 const char *reject[]={
  "sin(100*x)*sin(x)^99", "sin(101*x+1)*sin(x)^99",
  "sin(-101*x+1)*sin(x)^99", "cos(101*x+1)*cos(x)^99",
  "sin(101*x)*cos(x)^99", "cos(101*x)*sin(x)^99",
  "sin(5*x)*sin(x)^(-3)", "sin(5*x/2)*sin(x)^(1/2)",
  "sin(32768*x)*sin(x)^32766", "x*sin(5*x)*sin(x)^3",
  "sin(5*x^2)*sin(x^2)^3", "sin(5*x)*sin(x+1)^3"
 };
 unsigned rejected=0;
 for(unsigned i=0;i<sizeof(reject)/sizeof(*reject);++i){
  assert(!high_frequency_rule(gen(reject[i],c),x,actual,c));++rejected;
 }
 assert(high_frequency_rule(gen("sin(5*x)*sin(x)^3/2",c),x,actual,c));
 angle_radian(false,c);assert(!high_frequency_rule(gen("sin(5*x)*sin(x)^3",c),x,actual,c));++rejected;
 std::cout<<"PASS: "<<checked<<" exact cases, general addition proofs, "<<rejected<<" guards\n";
 return 0;
}
static void *worker(void *p){*static_cast<int *>(p)=cases();return 0;}
int main(){if(cases())return 1;pthread_attr_t attr;assert(!pthread_attr_init(&attr));
 assert(!pthread_attr_setstacksize(&attr,64*1024));assert(!pthread_attr_setguardsize(&attr,4096));
 int result=1;pthread_t thread;assert(!pthread_create(&thread,&attr,worker,&result));
 pthread_attr_destroy(&attr);assert(!pthread_join(thread,0));return result;}
