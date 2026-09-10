#include "giacPCH.h"
#include "equation_normalize.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
static giac::gen residual(const giac::gen &g){
  assert(g.is_symb_of_sommet(giac::at_equal));
  const giac::vecteur &v=*g._SYMBptr->feuille._VECTptr;
  return v[0]-v[1];
}
static int run_cases(){using namespace giac;context ctx;const context *c=&ctx;
  struct Case{const char *input,*expected;};
  const Case cases[]={
    {"-27*x^3+81*x*y-27*y^3=0","x^3-3*x*y+y^3=0"},
    {"2*x+2=2*y+2","x-y=0"},
    {"x/2+y/3=0","3*x+2*y=0"},
    {"2*x*y=0","x*y=0"},
    {"2*x*(x-1)=0","x*(x-1)=0"},
    {"-2*(x-y)^2=0","(x-y)^2=0"},
    {"(2*x-2*y)^2=0","x^2-2*x*y+y^2=0"},
    {"9*u^4-18*u^2*v^2+9*v^4=0","u^4-2*u^2*v^2+v^4=0"},
    {"6*x*y=12*x","x*y-2*x=0"},
    {"3*x=6","x-2=0"}
  };
  for(const Case &t:cases){
    gen input(t.input,c),expected(t.expected,c);
    gen actual=_simplify(input.eval(1,c),c);
    if(!is_zero(ratnormal(residual(actual)-residual(expected),c))){std::cerr<<input<<" -> "<<actual<<" expected "<<expected<<'\n';return 1;}
  }
  gen ignored;
  for(const char *s:{"x*y=0","a*(x-y)=0","x=2","2*x/y=0","2*(x-1)/(x-1)=0","sin(x)=0","sqrt(x)^2=0","(x+y)^100=0"})
    assert(!equation_primitive(gen(s,c),ignored,c));
  // Construct raw nodes: the parser/evaluator would otherwise flatten nested powers
  // before the pre-expansion guard is tested.
  gen x(identificateur("x")),y(identificateur("y"));
  gen x32=symbolic(at_pow,makesequence(x,32));
  gen x64=symbolic(at_pow,makesequence(x32,2));
  unsigned budget=512;equation_polynomial_budget bound;
  assert(equation_polynomial_bound(x64,budget,0,bound) && bound.degree==64);
  assert(equation_primitive(symb_equal(symbolic(at_prod,makesequence(2,x64)),0),ignored,c));
  assert(is_zero(ratnormal(residual(ignored)-pow(x,64),c)));
  gen constant32=symbolic(at_pow,makesequence(2,32));
  gen constant1024=symbolic(at_pow,makesequence(constant32,32));
  gen rational32=symbolic(at_pow,makesequence(gen(2)/3,32));
  gen rational1024=symbolic(at_pow,makesequence(rational32,32));
  gen large=gen("2^1100",c).eval(1,c);
  gen d1=large+1,d2=large+3;
  gen large_sum=symbolic(at_plus,makesequence(
    symbolic(at_division,makesequence(x,d1)),symbolic(at_division,makesequence(y,d2))));
  gen wide=symbolic(at_plus,makesequence(symbolic(at_prod,makesequence(gen("2^100",c).eval(1,c),x)),y));
  const gen rejected[]={
    symbolic(at_pow,makesequence(x32,3)),
    symbolic(at_prod,makesequence(x32,x32,x)),
    symbolic(at_pow,makesequence(constant1024,3)),
    symbolic(at_pow,makesequence(rational1024,2)),
    symbolic(at_prod,makesequence(large,large,x)),
    large_sum,
    symbolic(at_pow,makesequence(wide,8)),
    gen("2^2100",c).eval(1,c),
    symbolic(at_division,makesequence(x,gen(1)/gen("2^2100",c).eval(1,c)))
  };
  for(const gen &g:rejected){
    budget=512;assert(!equation_polynomial_bound(g,budget,0,bound));
    assert(!equation_primitive(symb_equal(g,0),ignored,c));
  }
  // Individually bounded sides must also fit after clearing both denominators.
  gen lhs=symbolic(at_division,makesequence(x,d1));
  gen rhs=symbolic(at_division,makesequence(y,d2));
  budget=512;assert(equation_polynomial_bound(lhs,budget,0,bound));
  budget=512;assert(equation_polynomial_bound(rhs,budget,0,bound));
  assert(!equation_primitive(symb_equal(lhs,rhs),ignored,c));
  // Moderate rational powers and binomial expansion remain eligible.
  gen ordinary=symbolic(at_pow,makesequence(symbolic(at_plus,makesequence(x/2,y/3)),4));
  budget=512;assert(equation_polynomial_bound(ordinary,budget,0,bound));
  assert(bound.degree==4 && bound.terms==16);
  assert(_simplify(gen("x=2",c),c)==gen("x=2",c));
  gen factored=_simplify(gen("-2*(x-y)^2=0",c).eval(1,c),c);
  assert(factored.print(c).size()<20);
  std::cout<<"PASS: 10 primitive equalities, 8 domain guards, 10 growth guards, degree-64 boundary, rational powers and compact factors\n";
  return 0;
}

static void *worker(void *p){*static_cast<int *>(p)=run_cases();return 0;}
int main(){
  if(run_cases())return 1;
  pthread_attr_t attributes;assert(!pthread_attr_init(&attributes));
  assert(!pthread_attr_setstacksize(&attributes,64*1024));
  assert(!pthread_attr_setguardsize(&attributes,4096));
  int result=1;pthread_t thread;assert(!pthread_create(&thread,&attributes,worker,&result));
  pthread_attr_destroy(&attributes);assert(!pthread_join(thread,0));return result;
}
