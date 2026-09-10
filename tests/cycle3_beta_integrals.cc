#include "giacPCH.h"
#include <cassert>
#include <iostream>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_mellin_log(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_beta_log(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
}
using namespace giac;
int main(){
 context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
 struct Case{const char *f,*hi,*answer;};
 const Case yes[]={
 // On the unit interval: integral(log(x)^2)=2 and
 // integral(log(x)*log(1-x))=2-pi^2/6; expansion gives the first identity.
 // Expanding log(1-x) as -sum(x^n/n) gives the second via zeta(2),zeta(3).
 {"ln(x/(1-x))*ln(x^2/(1-x))","1","pi^2/2"},
 {"ln(x)^2*ln(1-x)","1","-6+pi^2/3+2*Zeta(3)"},
 {"ln(x/(1-x))^2/sqrt(x*(1-x))","1","pi^3"},
 {"sqrt(x/(1-x))*ln(x/(1-x))","1","pi"},
 {"ln(x)^3/sqrt(x*(1-x))","1","-pi*(8*ln(2)^3+2*pi^2*ln(2)+12*Zeta(3))"},
 {"sqrt(x*(1-x))*ln(x)*ln(1-x)","1","pi/8*((1/2-2*ln(2))^2-pi^2/6+5/4)"},
 {"ln(x/(1-x))^16/sqrt(x*(1-x))","1","19391512145*pi^17"},
 {"ln(x/(1-x))^5/sqrt(x*(1-x))","1","0"},
 {"ln(x^2/(1-x)^2)^2/sqrt(x*(1-x))","1","4*pi^3"},
 {"x*(1-x)^2*ln(x/(1-x))","1","-1/24"},
 {"x*(1-x)^2*ln(x/(1-x))^2","1","pi^2/36-1/6"},
 {"ln(x^2/(1-x))^2","1","2*pi^2/3+2"},
 {"ln(x/(1-x))^4","1","7*pi^4/15"},
 {"ln(x)^4","1","24"},
 {"ln(1-x)^3","1","-6"},
 {"ln(x)*ln(1-x)","1","2-pi^2/6"},
 {"ln(x)^2/sqrt(x*(1-x))","1","pi*(4*ln(2)^2+pi^2/3)"},
 {"x^(-1/2)/(1+x^(3/2))^2","+infinity","8*pi/(9*sqrt(3))"},
 {"x^(3/2)/(1+x^2)^3","+infinity","Gamma(5/4)*Gamma(7/4)/4"},
 {"ln(x)/(1+x^2)^2","+infinity","-pi/4"},
 {"ln(x)^2/(1+x^2)^2","+infinity","pi^3/16"},
 {"1/sqrt(1+x^4)","+infinity","Gamma(1/4)^2/(4*sqrt(pi))"},
 {"1/(1+x)^3","+infinity","1/2"},
 {"1/(4+x^2)^2","+infinity","pi/32"},
 {"ln(x)/(4+x^2)","+infinity","pi*ln(2)/4"},
 {"ln(x)^4/(1+x^2)","+infinity","5*pi^5/32"},
 {"ln(x)/(4+x^2)^2","+infinity","pi*(ln(2)-1)/32"}
 };
 for(unsigned evaluated=0;evaluated<2;++evaluated)for(const Case &t:yes){
  gen f(t.f,ctx),hi=gen(t.hi,ctx).eval(1,ctx);if(evaluated)f=f.eval(1,ctx);f=integration_syntax(f,ctx);
  bool ok=(hi==1?integrate_beta_log(f,x,0,hi,res,ctx):integrate_mellin_log(f,x,0,hi,res,ctx));
  gen want=gen(t.answer,ctx).eval(1,ctx);
  if(!ok || (res!=want && !is_zero(_simplify(res-want,ctx)))){std::cerr<<"FAIL "<<t.f<<" parsed "<<f<<" got "<<res<<" expected "<<want<<" matched "<<ok<<'\n';return 1;}
 }
 const Case no[]={
 {"sqrt(ln(x)^2)","1",0}, {"sqrt(ln(x)^2)/(1+x^2)","+infinity",0},
 {"ln(x)^2*ln(1-x)^2","1",0},
 {"ln(x)^17","1",0}, {"ln(x)/x","1",0},
 {"ln(x)/(1-x)","1",0}, {"ln(x/(x-1))^2/sqrt(x*(1-x))","1",0},
 {"ln(x)^17/(1+x^2)","+infinity",0},
 {"x^3/(1+x^2)^2","+infinity",0},
 {"1/x/(1+x^2)^2","+infinity",0},
 {"1/sqrt(1-x^4)","+infinity",0},
 {"1/(1+x^2)^17","+infinity",0}
 };
 for(const Case &t:no){gen f=gen(t.f,ctx).eval(1,ctx),hi=gen(t.hi,ctx).eval(1,ctx);
  bool ok=(hi==1?integrate_beta_log(f,x,0,hi,res,ctx):integrate_mellin_log(f,x,0,hi,res,ctx));
  if(ok){std::cerr<<"UNEXPECTED "<<t.f<<" => "<<res<<'\n';return 1;}}
 std::cout<<"PASS: 27 beta/logit/Mellin identities in raw and evaluated syntax and 12 rejected conditions\n";
}
