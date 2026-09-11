#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integration_chain_add(vecteur &,vecteur &,const gen &,const gen &);
bool integration_chain_terms(const gen &,const gen &,vecteur &,vecteur &,unsigned &,GIAC_CONTEXT);
bool integrate_erf_chain(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_atan_rectangle_pair(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
using namespace giac;
static int run_cases(){
 context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
 struct Case{const char *f,*lo,*hi,*answer;bool rectangle;};
 const Case yes[]={
 {"(x+1)*exp(-(sqrt(x)-1/sqrt(x))^2)*erf(sqrt(x)-1/sqrt(x))^2/x^(3/2)","0","+infinity","2*sqrt(pi)/3",false},
 {"(1+2/x^2)*exp(-(x-2/x)^2)*erf(x-2/x)^4","0","+infinity","sqrt(pi)/5",false},
 {"(-1-2/x^2)*exp(-(-x+2/x)^2)*erf(-x+2/x)^4","0","+infinity","-sqrt(pi)/5",false},
 {"(1+2/x^2)*exp(-(x-2/x)^2)*erf(x-2/x)^3","0","+infinity","0",false},
 {"2*x*exp(-x^4)*erf(x^2)^2","0","+infinity","sqrt(pi)/6",false},
 {"(-1/x^2)*exp(-1/x^2)*erf(1/x)^2","0","+infinity","-sqrt(pi)/6",false},
 {"(2*x-1)*exp(-(x^2-x)^2)*erf(x^2-x)^2","0","1","0",false},
 {"exp(-(x+1)^2)*erf(x+1)^2","1","2","sqrt(pi)*(erf(3)^3-erf(2)^3)/6",false},
 {"exp(-x^2)*erf(x)^16","0","+infinity","sqrt(pi)/34",false},
 {"2*atan(3/sqrt(4*x^2+2))/((1+4*x^2)*sqrt(4*x^2+2))+3*atan(2/sqrt(9*x^2+2))/((1+9*x^2)*sqrt(9*x^2+2))","0","1","atan(2)*atan(3)",true},
 {"atan(3/sqrt(x^2+8))/((x^2+4)*sqrt(x^2+8))+(3/2)*atan(2/sqrt(9*x^2/4+8))/((9*x^2/4+4)*sqrt(9*x^2/4+8))","0","2","pi*atan(3/2)/16",true},
 {"-2*atan(3/sqrt(4*x^2+2))/((1+4*x^2)*sqrt(4*x^2+2))-3*atan(2/sqrt(9*x^2+2))/((1+9*x^2)*sqrt(9*x^2+2))","0","1","-atan(2)*atan(3)",true}
 };
 for(unsigned evaluated=0;evaluated<2;++evaluated)for(const Case &t:yes){
  gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);f=integration_syntax(f,ctx);
  bool ok=(t.rectangle?integrate_atan_rectangle_pair:integrate_erf_chain)(f,x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx);
  gen expected=gen(t.answer,ctx).eval(1,ctx);
  if(!ok || (res!=expected && !is_zero(_simplify(res-expected,ctx)))){std::cerr<<"FAIL "<<t.f<<" parsed "<<f<<" got "<<res<<" matched "<<ok<<'\n';return 1;}
 }
 const Case no[]={
 {"(1+1/x^2)*exp(-(x-2/x)^2)*erf(x-2/x)^2","0","+infinity","",false},
 {"exp(-2*x^2)*erf(x)^2","0","+infinity","",false},
 {"exp(-x^2)*erf(x)^(-1)","0","+infinity","",false},
 {"exp(-x^2)*erf(x)^(1/2)","0","+infinity","",false},
 {"exp(-x^2)*sqrt(erf(x)^2)","0","+infinity","",false},
 {"exp(-x^2)*erf(x)^17","0","+infinity","",false},
 {"exp(-x^2)*erf(x)^2","-1","1","",false},
 {"exp(-x^2)*erf(x)^2","1","0","",false},
 {"2*atan(3/sqrt(4*x^2+2))/((1+4*x^2)*sqrt(4*x^2+2))+3*atan(4/sqrt(9*x^2+2))/((1+9*x^2)*sqrt(9*x^2+2))","0","1","",true},
 {"2*atan(3/sqrt(4*x^2+2))/((1+4*x^2)*sqrt(4*x^2+2))+4*atan(2/sqrt(9*x^2+2))/((1+9*x^2)*sqrt(9*x^2+2))","0","1","",true},
 {"2*atan(-3/sqrt(4*x^2+2))/((1+4*x^2)*sqrt(4*x^2+2))+3*atan(2/sqrt(9*x^2+2))/((1+9*x^2)*sqrt(9*x^2+2))","0","1","",true},
 {"2*atan(3/sqrt(4*x^2+2))/((1+4*x^2)*sqrt(4*x^2+2))+3*atan(2/sqrt(9*x^2+2))/((1+9*x^2)*sqrt(9*x^2+2))","0","2","",true}
 };
 for(const Case &t:no){gen f=integration_syntax(gen(t.f,ctx),ctx);if((t.rectangle?integrate_atan_rectangle_pair:integrate_erf_chain)(f,x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx)){std::cerr<<"UNEXPECTED "<<f<<" -> "<<res<<'\n';return 1;}}
 // Exercise the accumulator and products directly, before syntax may combine
 // their powers. The exponent and derivative matchers use this same guard.
 for(const char *input:{"x^32*x^33","x^(-32)*x^(-33)","x^(1/3)*x^(1/5)"}){
  vecteur p,c;unsigned budget=64;gen product(input,ctx);
  vecteur factors=*product._SYMBptr->feuille._VECTptr;for(unsigned i=0;i<factors.size();++i)factors[i]=integration_syntax(factors[i],ctx);product=symbolic(at_prod,factors);
  if(integration_chain_terms(product,x,p,c,budget,ctx)){std::cerr<<"UNBOUNDED POWER "<<input<<'\n';return 1;}
 }
 for(const char *input:{"x^32*x^32","x^(1/4)*x^(1/8)"}){
  vecteur p,c;unsigned budget=64;gen product(input,ctx);
  vecteur factors=*product._SYMBptr->feuille._VECTptr;for(unsigned i=0;i<factors.size();++i)factors[i]=integration_syntax(factors[i],ctx);product=symbolic(at_prod,factors);
  if(!integration_chain_terms(product,x,p,c,budget,ctx)){std::cerr<<"BOUNDARY REJECTED "<<input<<'\n';return 1;}
 }
 for(const char *input:{"65","-65","1/9","8/15"}){
  vecteur p,c;if(integration_chain_add(p,c,gen(input,ctx).eval(1,ctx),1)){std::cerr<<"UNBOUNDED ACCUMULATOR "<<input<<'\n';return 1;}
 }
 std::cout<<"PASS: 9 accumulator/convolution boundary checks\n";
 std::cout<<"PASS: 12 exact chain/rectangle identities raw and evaluated; 12 rejection checks\n";return 0;
}
static void *worker(void *output){*static_cast<int *>(output)=run_cases();return 0;}
int main(){if(run_cases())return 1;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,64*1024);pthread_attr_setguardsize(&a,4096);int result=1;pthread_t t;int error=pthread_create(&t,&a,worker,&result);pthread_attr_destroy(&a);if(error || pthread_join(t,0))return 2;if(!result)std::cout<<"PASS: guarded 64 KiB stack\n";return result;}
