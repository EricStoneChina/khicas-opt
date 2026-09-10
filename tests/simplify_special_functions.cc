#include "giacPCH.h"
#include <cassert>
#include <iostream>

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
 std::cout<<"PASS: low-cost Psi algebra simplifies; large Psi powers stay factored; ordinary algebra unchanged\n";
}
