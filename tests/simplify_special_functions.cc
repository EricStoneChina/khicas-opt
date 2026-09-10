#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <cmath>

int main(){using namespace giac;context ctx;const context *contextptr=&ctx;
 gen p=symbolic(at_Psi,makesequence(gen(1)/3,0));
 sto(17,gen(identificateur(" simplify_Psi_0")),contextptr);
 sto(23,gen(identificateur(" nested_power_0")),contextptr);
 gen expr=pow(p+1,2)-pow(p-1,2);
 gen actual=simplify(expr,contextptr);
 assert(actual==4*p);
 gen big=symbolic(at_pow,makesequence(p+1,8));
 assert(simplify(big,contextptr)==big);
 gen nested=symbolic(at_Gamma,big);assert(simplify(nested,contextptr)==nested);
 gen deep=p;
 for(unsigned i=0;i<40;++i)deep=symbolic(at_sin,deep);
 assert(simplify(deep,contextptr)==deep);
 gen many(vecteur(2100,1));
 many._VECTptr->push_back(big);
 assert(simplify(many,contextptr)==many);
 gen x(identificateur("x"));
 assert(simplify(pow(x+1,2)-pow(x-1,2),contextptr)==4*x);
 gen h=atan(sqrt(gen(2),contextptr),contextptr);
 gen circle=2*cst_pi*h-2*h*h-2*cst_pi*atan(sqrt(gen(5),contextptr),contextptr)+cst_pi*cst_pi/2;
 gen compact=simplify(circle,contextptr);
 assert(is_zero(ratnormal(compact-circle,contextptr)) && compact.print(contextptr).size()<150);
 assert(!contains(compact,cst_i));
 assert(simplify(gen("tan(atan(x))",contextptr),contextptr)==x);
 for(const char *s:{"atan(2)+atan(1/2)","asin(1/3)+acos(1/3)"})
   {gen v=simplify(gen(s,contextptr).eval(1,contextptr),contextptr);assert(is_zero(ratnormal(v-cst_pi/2,contextptr)));}
 assert(simplify(gen("sin(atan(2)+atan(1/2))",contextptr).eval(1,contextptr),contextptr)==1);
 // A closed inverse angle or Psi value can still be complex. Masking it as a real identifier
 // would incorrectly replace sqrt(P^2) by abs(P).
 vecteur complex_angles=makevecteur(symbolic(at_asin,2),symbolic(at_acos,2),symbolic(at_atan,gen(1,1)),symbolic(at_Psi,makesequence(gen(1,1),0)));
 for(unsigned j=0;j<complex_angles.size();++j){
   gen square=symbolic(at_pow,makesequence(complex_angles[j],2));
   gen original=square+symbolic(at_pow,makesequence(square,gen(1)/2));
   gen reduced=simplify(original,contextptr);
   gen error=abs(evalf_double(reduced,1,contextptr)-evalf_double(original,1,contextptr),contextptr);
   assert(error.type==_DOUBLE_ && std::isfinite(error._DOUBLE_val) && error._DOUBLE_val<1e-10);
 }
 gen complex_big=symbolic(at_pow,makesequence(complex_angles.back()+1,8));
 assert(simplify(complex_big,contextptr)==complex_big);
 std::cout<<"PASS: low-cost Psi algebra simplifies; large Psi powers stay factored; ordinary algebra unchanged\n";
}
