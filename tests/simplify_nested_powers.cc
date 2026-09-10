#include "giacPCH.h"
#include <cassert>
#include <cmath>
#include <iostream>
int main(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x"));
const gen primitive=gen("4*(-3*(1+x^(1/4))^(4/3)/4+3*(1+x^(1/4))^(7/3)/7)",ctx).eval(1,ctx);
gen reduced=_simplify(primitive,ctx);
assert(lop(reduced,at_rootof).empty());assert(taille(reduced,257)<257);
assert(is_zero(_simplify(derive(reduced,x,ctx)-gen("(1+x^(1/4))^(1/3)/sqrt(x)",ctx).eval(1,ctx),ctx)));
const char *expressions[]={
"(1+x^(1/4))^(1/3)+sin(x)^2+cos(x)^2",
"((1+x^(1/4))^(1/3))^3-1-x^(1/4)",
"1/((1+x^(1/4))^(1/3))*(1+x^(1/4))^(1/3)",
"(1+sqrt(x))^(1/3)+(1+sqrt(x))^(1/3)",
"(1+(1+x^(1/4))^(1/3))^(1/5)",
"(1+x^(1/4))^(4/3)/(1+x^(1/4))",
"(1+x^(1/4))^(-2/3)*(1+x^(1/4))",
"abs((1+x^(1/4))^(1/3))"
};
const char *points[]={"1/16","1","16","-16","1+i","-1+i","-16-i"};
for(int mode=0;mode<2;++mode){complex_mode(mode,ctx);
  for(const char *expr:expressions){gen f=gen(expr,ctx).eval(1,ctx),g=_simplify(f,ctx);
    assert(lop(g,at_rootof).empty());assert(taille(g,513)<513);
    for(const char *point:points){gen p=gen(point,ctx).eval(1,ctx);
      gen value=subst(f,x,p,false,ctx).eval(1,ctx),actual=subst(g,x,p,false,ctx).eval(1,ctx);
      gen error=evalf_double(abs(evalf_double(actual,1,ctx)-evalf_double(value,1,ctx),ctx),1,ctx),scale=evalf_double(abs(evalf_double(value,1,ctx),ctx),1,ctx);
      if(error.type!=_DOUBLE_ || scale.type!=_DOUBLE_ || !std::isfinite(error._DOUBLE_val) || error._DOUBLE_val>1e-10*(1+scale._DOUBLE_val)){
        std::cerr<<"branch mismatch: "<<expr<<" at "<<point<<" complex_mode="<<mode<<" result="<<g<<" error="<<error<<'\n';return 1;
      }
    }
  }
}
complex_mode(false,ctx);
gen collision(identificateur(" nested_power_0"));
gen f=gen("(1+x^(1/4))^(1/3)",ctx).eval(1,ctx)+collision;
gen g=_simplify(f,ctx);
assert(contains(g,collision));assert(is_zero(_simplify(g-f,ctx)));
gen many=0;
for(int i=1;i<=33;++i) many+=pow(gen(i)+pow(x,gen(1)/4,ctx),gen(1)/3,ctx);
g=_simplify(many,ctx);
assert(lop(g,at_rootof).empty());assert(lop(g,at_pow).size()==lop(many,at_pow).size());
assert(g==simplifier(many,ctx));
std::cout<<"PASS: exact primitive derivative, identifier collision, 33-atom bound, 112 real/complex branch samples\n";
}
