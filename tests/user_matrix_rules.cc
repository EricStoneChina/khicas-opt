#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
namespace giac {
bool integrate_real_definite(const gen &,const gen &,gen,gen,gen &,GIAC_CONTEXT);
bool integrate_complementary_ratio(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_cauchy_fourier(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integration_rational_tail(const gen &,const gen &,int &,gen &,unsigned &,unsigned,GIAC_CONTEXT);
}
static void tests(){using namespace giac;context ctx;const context *c=&ctx;gen x=identificateur("x"),result;
 const char *reflection[][4]={
 {"sin(x)^7/(sin(x)^7+cos(x)^7)","0","pi/2","pi/4"},
 {"cos(x)^9/(sin(x)^9+cos(x)^9)","0","pi/2","pi/4"},
 {"sin(2*x+pi/4)^3/(sin(2*x+pi/4)^3+cos(2*x+pi/4)^3)","-pi/8","pi/8","pi/8"},
 {"sin(3*x)^64/(sin(3*x)^64+cos(3*x)^64)","0","pi/6","pi/12"},
 {"x^5/(x^5+(1-x)^5)","0","1","1/2"},
 {"(1-x)^12/(x^12+(1-x)^12)","0","1","1/2"},
 {"(3*x+1)^7/((3*x+1)^7+(11-3*x)^7)","-1/3","11/3","2"},
 {"x^3/(x^3+(1-x)^3)","1","0","-1/2"},
 {"-3*x^9/(x^9+(1-x)^9)","0","1","-3/2"}};
 for(const auto &v:reflection){assert(integrate_real_definite(gen(v[0],c).eval(1,c),x,gen(v[1],c).eval(1,c),gen(v[2],c).eval(1,c),result,c));assert(is_zero(ratnormal(result-gen(v[3],c).eval(1,c),c)));}
 const char *fourier[][4]={
 {"cos(2*x)/(x^2+9)","0","+infinity","pi*exp(-6)/6"},
 {"cos(-2*x)/(x^2+9)","0","+infinity","pi*exp(-6)/6"},
 {"cos(3*x)/(2*x^2+8)","0","+infinity","pi*exp(-6)/8"},
 {"cos(3*x)/(2*x^2+8)","-infinity","+infinity","pi*exp(-6)/4"},
 {"cos(3*x)/(2*x^2+8)","+infinity","0","-pi*exp(-6)/8"},
 {"1/(x^2+4)","0","+infinity","pi/4"},
 {"-5*cos(x)/(x^2+1)","0","+infinity","-5*pi*exp(-1)/2"}};
 for(const auto &v:fourier){assert(integrate_real_definite(gen(v[0],c).eval(1,c),x,gen(v[1],c).eval(1,c),gen(v[2],c).eval(1,c),result,c));assert(is_zero(ratnormal(result-gen(v[3],c).eval(1,c),c)));}
 gen("assume(a>0)",c).eval(1,c);
 assert(integrate_real_definite(gen("cos(b*x)/(x^2+a^2)",c).eval(1,c),x,0,plus_inf,result,c));
 assert(is_zero(ratnormal(result-gen("pi*exp(-a*abs(b))/(2*a)",c).eval(1,c),c)));
 for(const char *s:{"x/(1+x^2)","x^3/(x^4+1)","(x+2)/(x^2+4*x+5)","1/x","-3*x/(2*x^2+3)"}){
  assert(integrate_real_definite(gen(s,c).eval(1,c),x,minus_inf,plus_inf,result,c));assert(is_undef(result));
 }
 for(const char *s:{"sin(x)^3/(2*sin(x)^3+cos(x)^3)","sin(x)^3/(sin(x)^3-cos(x)^3)","sin(x)^65/(sin(x)^65+cos(x)^65)","sin(x)^(-3)/(sin(x)^(-3)+cos(x)^(-3))"})
  assert(!integrate_complementary_ratio(gen(s,c).eval(1,c),x,0,cst_pi/2,result,c));
 for(const char *s:{"cos(x+1)/(x^2+1)","sin(x)/(x^2+1)","cos(x)/(x^2-1)","cos(x)/x^2","cos(i*x)/(x^2+1)"})
  assert(!integrate_cauchy_fourier(gen(s,c).eval(1,c),x,0,plus_inf,result,c));
 for(const char *s:{"0","exp(x)/(1+x^2)","x^1000000","x^2-x^2"}){
  unsigned budget=128;int degree;gen leading;
  assert(!integration_rational_tail(gen(s,c).eval(1,c),x,degree,leading,budget,0,c));
 }
 std::cout<<"PASS: 9 reflection identities, 8 Fourier identities including symbolic frequency, 5 divergence rejections, 13 unsupported/resource cases\n";
}
static void *worker(void *){tests();return 0;}
int main(){tests();pthread_attr_t a;assert(!pthread_attr_init(&a));assert(!pthread_attr_setstacksize(&a,65536));assert(!pthread_attr_setguardsize(&a,4096));pthread_t t;assert(!pthread_create(&t,&a,worker,0));pthread_attr_destroy(&a);assert(!pthread_join(t,0));}
