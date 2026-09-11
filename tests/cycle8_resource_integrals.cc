#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
namespace giac {bool resource_rule(const gen &,const gen &,gen &,bool,GIAC_CONTEXT);}
static int cases(){using namespace giac;context ctx;const context *c=&ctx;gen x(identificateur("x")),r;
 const char *f="(2*x+1)*(x^2+x)^63*(1+(x^2+x)^32)^1024";
 for(int evaluated=0;evaluated<2;++evaluated){gen input(f,c);if(evaluated)input=input.eval(1,c);assert(resource_rule(input,x,r,false,c));
 std::cerr<<"COMPOSED "<<r<<"\n";assert(r.print(c).size()<150);
 gen expected=gen("(1+(x^2+x)^32)^1026/32832-(1+(x^2+x)^32)^1025/32800",c).eval(1,c);
 gen p1=gen("(1+(x^2+x)^32)^1025",c).eval(1,c),p2=gen("(1+(x^2+x)^32)^1026",c).eval(1,c);
 gen a(identificateur("A")),b(identificateur("B"));
 gen difference=quotesubst(quotesubst(r-expected,p1,a,c),p2,b,c);assert(is_zero(ratnormal(difference,c)));
 }
 for(int mode=0;mode<2;++mode){complex_mode(mode,c);
 for(int n=1;n<=5;++n)for(int k=2;k<=3;++k){
 std::string u="(x^2-3*x+1)",m=print_INT_(2*k-1);
 gen input("-2/3*(2*x-3)*"+u+"^"+m+"*(2-3*"+u+"^"+print_INT_(k)+")^"+print_INT_(n),c);
 if(!resource_rule(input,x,r,false,c)){std::cerr<<"MISSED composed "<<input<<"\n";return 1;}assert(is_zero(ratnormal(derive(r,x,c)-input.eval(1,c),c)));
 }}
 complex_mode(false,c);
 // Raw syntax prevents parser evaluation from hiding huge nested-power budgets.
 gen u=gen("x^2+x",c),huge=symbolic(at_pow,makesequence(u,gen(1048576)));
 gen raw=symbolic(at_prod,makevecteur(gen("2*x+1",c),symbolic(at_pow,makesequence(huge,63)),symbolic(at_pow,makesequence(1+symbolic(at_pow,makesequence(huge,32)),1024))));
 assert(!resource_rule(raw,x,r,false,c));
 const char *bad[]={"(2*x+2)*(x^2+x)^63*(1+(x^2+x)^32)^1024","(2*x+1)*(x^2+x)^62*(1+(x^2+x)^32)^1024","(2*x+1)*(x^2+x)^63*(1+(x^2+x)^32)^(1/2)","(2*x+1)*(x^2+x)^63*(1+(x^2+x)^32)^(-2)","(2*x+1)*(x^2+x)^(-1)*(1+(x^2+x)^32)^1024","(2*x+1)*(x^2+x)^63*(1+i*(x^2+x)^32)^1024","(9*x^8+1)*(x^9+x)^63*(1+(x^9+x)^32)^1024","(2*x+1)*(x^2+x)^63*(1+(x^2+x)^32)^8193","(2*x+1)*(x^2+x)^129*(1+(x^2+x)^65)^1024"};
 for(const char *s:bad){if(resource_rule(gen(s,c),x,r,false,c)){std::cerr<<"UNEXPECTED "<<s<<"\n";return 1;}}
 struct Case{const char *f,*value;};const Case cases[]={
 {"exp(-2*x)*(1-exp(-x))^4096","1/(4097*4098)"},
 {"exp(-x)*(1-exp(-x))^8192","1/8193"},
 {"-3*exp(-6*x)*(1-exp(-2*x))^7","-1/240"},
 {"exp(-x)*(1-exp(-x/2))^(-1/2)","8/3"},
 {"exp(-3*x/2)*(1-exp(-x/2))^(1/2)","32/105"},
 {"exp(-16*x)*(1-exp(-x))^2","1/2448"}
 };
 for(const Case &t:cases)for(int evaluated=0;evaluated<2;++evaluated){gen input(t.f,c);if(evaluated)input=input.eval(1,c);
 if(!resource_rule(input,x,r,true,c)){std::cerr<<"MISSED "<<t.f<<" evaluated="<<evaluated<<" input="<<input<<"\n";return 1;}
 if(!is_zero(ratnormal(r-gen(t.value,c).eval(1,c),c))){std::cerr<<t.f<<" = "<<r<<" != "<<t.value<<"\n";return 1;}
 }
 const char *reject[]={"exp(2*x)*(1-exp(-x))^4096","exp(-2*x)*(1-exp(x))^(1/2)","exp(-2*x)*(1-exp(-x))^(-1)","exp(-2*x)*(1-exp(-x))^8193","exp(-17*x)*(1-exp(-x))^2","exp(-x/2)*(1-exp(-x))^2","exp(-2*x+i)*(1-exp(-x))^2","exp(-2*x)*(1-exp(-x+i))^2","exp(-2*i*x)*(1-exp(-i*x))^2","exp(-2*x)*(exp(-x)-1)^(1/2)","exp(-2*x)*(1-exp(-x))^((2^1000)^1000)"};
 for(const char *s:reject)assert(!resource_rule(gen(s,c),x,r,true,c));
 // Already materialized large integers exercise bit budgets, independently
 // of syntax-node/exponent limits. Both numerator and denominator matter.
 gen big=pow(gen(2),300),u2=gen("x^2+x",c),u32=symbolic(at_pow,makesequence(u2,32));
 for(int denominator=0;denominator<2;++denominator){
  gen coefficient=denominator?gen(1)/big:big;
  gen composed=gen("2*x+1",c)*symbolic(at_pow,makesequence(u2,63))*symbolic(at_pow,makesequence(coefficient+u32,1024));
  assert(!resource_rule(composed,x,r,false,c));
  gen beta=symbolic(at_exp,-gen(2)*gen(identificateur("x")))*symbolic(at_pow,makesequence(1-symbolic(at_exp,-x),coefficient));
  assert(!resource_rule(beta,x,r,true,c));
 }
 std::cout<<"PASS compact exact reference, 20 exact differentiated polynomial variants, 12 exact beta variants, 25 guards\n";return 0;
}
static void *worker(void *p){*static_cast<int *>(p)=cases();return 0;}
int main(){if(cases())return 1;pthread_attr_t a;assert(!pthread_attr_init(&a));assert(!pthread_attr_setstacksize(&a,65536));assert(!pthread_attr_setguardsize(&a,4096));pthread_t t;int r=1;assert(!pthread_create(&t,&a,worker,&r));pthread_attr_destroy(&a);assert(!pthread_join(t,0));return r;}
