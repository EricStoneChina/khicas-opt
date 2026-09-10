#include "giacPCH.h"
#include <cassert>
#include <iostream>
namespace giac {
bool integrate_real_definite(const gen &,const gen &,gen,gen,gen &,GIAC_CONTEXT);
gen integration_syntax(const gen &,GIAC_CONTEXT);
}
using namespace giac;
int main(){
 context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
 struct Case {const char *f,*lo,*hi,*answer;};
 const Case yes[]={
 {"x^(1/2-1)*ln(x)^1/(1+x^2)","0","+infinity","-pi^2/(2*sqrt(2))"},
 {"x^(1-1)*ln(x)^2/(1+x^4)","0","+infinity","3*sqrt(2)*pi^3/64"},
 {"ln(x)/(4+x^2)","0","+infinity","pi*ln(2)/4"},
 {"ln(x)^2/(1+x^2)","0","+infinity","pi^3/8"},
 {"x^(-1/3)*ln(x)/(1+x^(4/3))","0","+infinity","0"},
 {"3*ln(x)^2/(1+x^2)","+infinity","0","-3*pi^3/8"},
 {"ln(x)/sqrt(x*(1-x))","0","1","-2*pi*ln(2)"},
 {"ln(x)^2/sqrt(x*(1-x))","0","1","pi*(4*ln(2)^2+pi^2/3)"},
 {"ln(x)*ln(1-x)/sqrt(x*(1-x))","0","1","pi*(4*ln(2)^2-pi^2/6)"},
 {"ln(1-x)^2/sqrt(x*(1-x))","0","1","pi*(4*ln(2)^2+pi^2/3)"},
 {"ln(x)*x^(-1/2)*(1-x)^(-1/2)","0","1","-2*pi*ln(2)"},
 {"x*ln(x)","0","1","-1/4"},
 {"ln(x)*ln(1-x)","0","1","2-pi^2/6"},
 {"exp(-3*x)*(sin(2*x+pi/3)-sin(pi/3))/x","0","+infinity","atan(2/3)/2-sqrt(3)*ln(13/9)/4"},
 {"exp(-x)*(cos(2*x)-1)/x","0","+infinity","-ln(5)/2"},
 {"exp(-2*x)*(cos(x)-cos(3*x))/x","0","+infinity","ln(13/5)/2"},
 {"exp(-2*x)*(cos(x)-cos(3*x))/x","+infinity","0","-ln(13/5)/2"},
 {"exp(1-x)*(sin(x+pi/2)-1)/x","0","+infinity","-exp(1)*ln(2)/2"},
 {"exp(-2*x-3/x)/sqrt(x)","0","+infinity","sqrt(pi/2)*exp(-2*sqrt(6))"},
 {"exp(-2*x-3/x)*x^(-3/2)","0","+infinity","sqrt(pi/3)*exp(-2*sqrt(6))"},
 {"exp(2-3*x-2/x)/sqrt(x)","+infinity","0","-sqrt(pi/3)*exp(2-2*sqrt(6))"}
 };
 for(const Case &t:yes){
  bool ok=integrate_real_definite(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx);
  gen want=gen(t.answer,ctx).eval(1,ctx);
  if(!ok || (res!=want && !is_zero(_simplify(res-want,ctx)))){std::cerr<<"FAIL "<<t.f<<" got "<<res<<" expected "<<want<<" matched "<<ok<<'\n';return 1;}
 }
 const Case no[]={
 {"ln(x)/(x^2-1)","0","+infinity",0},
 {"ln(x)/x/(1+x^2)","0","+infinity",0},
 {"x*ln(x)/(1+x^2)","0","+infinity",0},
 {"ln(x)^3/(1+x^2)","0","+infinity",0},
 {"ln(x)/(1+x^2)","-infinity","+infinity",0},
 {"ln(x)/sqrt(x*(x-1))","0","1",0},
 {"ln(x)/(x*(1-x))","0","1",0},
 {"ln(x)^3/sqrt(x*(1-x))","0","1",0},
 {"ln(x)/sqrt(x*(1-x))","0","2",0},
 {"exp(x)*(cos(2*x)-1)/x","0","+infinity",0},
 {"exp(-x)*(sin(2*x+1)-1)/x","0","+infinity",0},
 {"exp(-x)*(cos(2*x)-2)/x","0","+infinity",0},
 {"exp(-x)*(cos(x^2)-1)/x","0","+infinity",0},
 {"exp(-x)*(sin(x+i)-sin(i))/x","0","+infinity",0},
 {"exp(-x)*(cos(x)-1)/x","-infinity","+infinity",0},
 {"exp(2*x-3/x)/sqrt(x)","0","+infinity",0},
 {"exp(-2*x+3/x)/sqrt(x)","0","+infinity",0},
 {"exp(-2*x-3/x)/x","0","+infinity",0},
 {"exp(-2*x-3/x)/sqrt(x)","-infinity","+infinity",0}
 };
 for(const Case &t:no){
  if(integrate_real_definite(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx)){
   std::cerr<<"Unexpected match: "<<t.f<<" -> "<<res<<'\n';return 1;
  }
 }
 assert(integration_syntax(gen("x^0*ln(x)^1",ctx),ctx)==gen("ln(x)",ctx));
 assert(is_undef(integration_syntax(gen("0^0",ctx),ctx)));
 angle_radian(false,ctx);
 assert(!integrate_real_definite(gen("ln(x)^2/(1+x^2)",ctx),x,0,plus_inf,res,ctx));
 std::cout<<"PASS: 21 Mellin/Beta/Laplace identities, 20 rejected conditions, canonical power identities\n";
}
