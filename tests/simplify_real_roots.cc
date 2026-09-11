#include "giacPCH.h"
#include <cassert>
#include <cmath>
#include <iostream>
int main(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen t(identificateur("t"));
const char *arguments[]={"sin(t)","-2*sin(t)","1-2*cos(t)","t"};
const char *points[]={"0","pi","-pi","pi/2","-pi/2","1/3","-1/3"};
unsigned samples=0;
for(int mode=0;mode<2;++mode){complex_mode(mode,ctx);
 for(const char *arg:arguments)for(int n=3;n<=9;n+=2){
  gen a=gen(arg,ctx).eval(1,ctx);
  gen root=symbolic(at_pow,makesequence(gen(symbolic(at_abs,a)),gen(1)/n));
  for(int signed_root=0;signed_root<2;++signed_root){
   gen f=signed_root?gen(symbolic(at_sign,a))*root:root;
   gen g=_simplify(f,ctx);
   for(const char *point:points){gen p=gen(point,ctx).eval(1,ctx);
    gen before=subst(f,t,p,false,ctx).eval(1,ctx),after=subst(g,t,p,false,ctx).eval(1,ctx);
    if(is_zero(before))assert(is_zero(after));
    gen error=evalf_double(abs(evalf_double(before,1,ctx)-evalf_double(after,1,ctx),ctx),1,ctx);
    assert(error.type==_DOUBLE_&&std::isfinite(error._DOUBLE_val)&&error._DOUBLE_val<1e-10);++samples;
   }
  }
 }
}
complex_mode(false,ctx);
gen a=sin(t,ctx),root=symbolic(at_pow,makesequence(gen(symbolic(at_abs,a)),gen(1)/3));
gen collision(identificateur(" simplify_real_root_0"));
gen f=root+collision,g=_simplify(f,ctx);assert(contains(g,collision));assert(is_zero(ratnormal(g-f,ctx)));
// The real-root protection must preserve ordinary trigonometric arithmetic.
gen identity=root*(pow(sin(t,ctx),2)+pow(cos(t,ctx),2));
assert(is_zero(ratnormal(_simplify(identity,ctx)-root,ctx)));
gen many=0;
for(int i=0;i<33;++i)many+=gen(symbolic(at_pow,makesequence(gen(symbolic(at_abs,t+i)),gen(1)/3)));
assert(_simplify(many,ctx)==many);
// Explicitly complex arguments must keep their original sign convention.
for(const char *arg:{"1+i","-1+i","i"}){
 gen z=gen(arg,ctx).eval(1,ctx);
 gen f=gen(symbolic(at_sign,z))*gen(symbolic(at_pow,makesequence(gen(symbolic(at_abs,z)),gen(1)/3)));
 gen g=_simplify(f,ctx);
 gen error=evalf_double(abs(evalf_double(f,1,ctx)-evalf_double(g,1,ctx),ctx),1,ctx);
 assert(error.type==_DOUBLE_&&std::isfinite(error._DOUBLE_val)&&error._DOUBLE_val<1e-10);
}
std::cout<<"PASS: "<<samples<<" real odd-root samples in both complex modes, exact zeros, collisions, trig coefficients and complex constants\n";
}
