#include "giacPCH.h"
#include <iostream>
#include <cmath>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_beta_log(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
using namespace giac;
int main(){
 context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
 // Independent 70-digit quadrature after x=exp(t)/(1+exp(t)), using stable
 // formulas on each half-line; the Bell recurrence was not the reference.
 struct Case{const char *f;double answer;size_t maximum_chars;};
 const Case cases[]={
 {"x^(-2/3)*(1-x)^(-4/5)*ln(x*(1-x))^4",80868.8924781454071680841178709442,1000},
 {"x^(-2/3)*(1-x)^(-4/5)*ln(x*(1-x))^8",79543639977.5713592004485845099944,3000},
 {"x^(-2/3)*(1-x)^(-4/5)*ln(x*(1-x))^12",585482433375912658.2589322788936374,10000},
 {"x^(-2/3)*(1-x)^(-4/5)*ln(x*(1-x))^16",15965523847498647219663144.3957650846,40000}
 };
 for(const Case &t:cases){
  gen f=integration_syntax(gen(t.f,ctx),ctx);
  if(!integrate_beta_log(f,x,0,1,res,ctx)){std::cerr<<"No result: "<<t.f<<'\n';return 1;}
  const size_t characters=res.print(ctx).size();
  if(characters>t.maximum_chars){std::cerr<<"Expression growth: "<<characters<<'\n';return 1;}
  gen value=evalf_double(res,1,ctx);
  if(value.type!=_DOUBLE_ || !std::isfinite(value._DOUBLE_val) || std::abs(value._DOUBLE_val/t.answer-1)>1e-11){std::cerr<<"Numerical mismatch: "<<value<<'\n';return 1;}
 }
 std::cout<<"PASS: 4 asymmetric Beta moments through degree16, independent quadrature agreement and bounded expression size\n";
}
