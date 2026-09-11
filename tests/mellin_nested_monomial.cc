#include "giacPCH.h"
#include <iostream>
#include <pthread.h>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integration_mellin_monomial(const gen &,const gen &,gen &,gen &,unsigned &,GIAC_CONTEXT);
}
using namespace giac;
static int cases(){context ctx;const context *p=&ctx;gen x(identificateur("x")),c,n;
 const char *powers[]={"-17/18","17/18","-31/33","31/33"};
 for(const char *power:powers)for(int eval=0;eval<2;++eval){
  gen q=gen(power,p).eval(1,p),g=symbolic(at_pow,makevecteur(x,q));if(eval)g=g.eval(1,p);g=integration_syntax(g,p);unsigned budget=4;
  if(!integration_mellin_monomial(g,x,c,n,budget,p)||c!=1||n!=q||budget!=3){std::cerr<<"FAIL "<<g<<" c="<<c<<" n="<<n<<" budget="<<budget<<'\n';return 1;}
 }
 struct C{const char *expression,*coefficient,*power;};
 for(const C &t:{C{"3*x^(-17/18)*x^(31/33)","3","-1/198"},C{"(2*x)^3","8","3"},C{"(x^(1/64))^64","1","1"},C{"(x^(1/64))^(-64)","1","-1"}}){
  gen g=integration_syntax(gen(t.expression,p),p);unsigned budget=8;
  if(!integration_mellin_monomial(g,x,c,n,budget,p)||c!=gen(t.coefficient,p).eval(1,p)||n!=gen(t.power,p).eval(1,p)){std::cerr<<"FAIL "<<g<<" c="<<c<<" n="<<n<<'\n';return 1;}
 }
 for(const char *input:{"((2*x)^(1/18))^17","(2*x^(1/18))^17","(x^(1/65))^17","(x^(1/18))^65","(x^(1/18))^(17/2)"}){
  unsigned budget=8;gen g=integration_syntax(gen(input,p),p);
  if(integration_mellin_monomial(g,x,c,n,budget,p)){std::cerr<<"UNEXPECTED "<<g<<'\n';return 1;}
 }
 unsigned budget=0;gen g=gen("x^(17/18)",p).eval(1,p);
 if(integration_mellin_monomial(g,x,c,n,budget,p)||budget)return 1;
 std::cout<<"PASS:4 fractional weights raw/evaluated,4 product/boundary cases,6 rejects/budget checks\n";return 0;}
static void *worker(void *p){*static_cast<int *>(p)=cases();return 0;}
int main(){if(cases())return 1;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,65536);pthread_attr_setguardsize(&a,4096);int result=1;pthread_t t;if(pthread_create(&t,&a,worker,&result)||pthread_join(t,0))return 2;pthread_attr_destroy(&a);return result;}
