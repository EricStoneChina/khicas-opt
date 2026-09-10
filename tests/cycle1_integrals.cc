#include "giacPCH.h"
#include <cassert>
#include <iostream>
namespace giac {bool integrate_real_definite(const gen &,const gen &,gen,gen,gen &,GIAC_CONTEXT);}
using namespace giac;
int main(){
 context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
 struct Case {const char *f,*lo,*hi,*answer;};
 const Case yes[]={
 {"x^10/(exp(x)+exp(-x)+2)","-infinity","+infinity","2555*pi^10/33"},
 {"1/(exp(2*x+3)+exp(-2*x-3)+2)","-infinity","+infinity","1/2"},
 {"x/(exp(2*x+3)+exp(-2*x-3)+2)","-infinity","+infinity","-3/4"},
 {"x^2/(exp(2*x+3)+exp(-2*x-3)+2)","-infinity","+infinity","9/8+pi^2/24"},
 {"x^3/(exp(x-2)+exp(-x+2)+2)","-infinity","+infinity","8+2*pi^2"},
 {"x^2/(exp(x-2)+exp(-x+2)+2)","+infinity","-infinity","-4-pi^2/3"},
 {"x*sin(x)/(2-cos(x)^2)","0","pi","pi*atanh(1/sqrt(2))/sqrt(2)"},
 {"exp(-2*x)*sin(3*x)/x","0","+infinity","atan(3/2)"},
 {"exp(-2*x)*sin(-3*x)/x","+infinity","0","atan(3/2)"},
 {"exp(1-2*x)*sin(3*x)/x","0","+infinity","exp(1)*atan(3/2)"},
 {"exp(-2*x^2)*cos(3*x)","-infinity","+infinity","sqrt(pi/2)*exp(-9/8)"},
 {"exp(-2*x^2+4*x+1)*cos(3*x+2)","-infinity","+infinity","sqrt(pi/2)*exp(15/8)*cos(5)"},
 {"exp(-x^2+2*x)*sin(2*x+1)","-infinity","+infinity","sqrt(pi)*sin(3)"},
 {"exp(-x^2)*sin(3*x)","-infinity","+infinity","0"},
 {"ln(sin(2*x))","0","pi/2","-pi*ln(2)/2"},
 {"ln(sin(x))","pi","0","pi*ln(2)"},
 {"ln(cos(2*x))","-pi/4","pi/4","-pi*ln(2)/2"},
 {"ln(cos(x))","0","pi/2","-pi*ln(2)/2"},
 {"ln(abs(sin(x)))","1","1+2*pi","-2*pi*ln(2)"},
 {"x/(exp(2*x)+1)","0","+infinity","pi^2/48"},
 {"x^2/(exp(x)+1)","0","+infinity","3*Zeta(3)/2"},
 {"x^3/(exp(x)-1)","0","+infinity","pi^4/15"},
 {"1/(exp(3*x)+1)","0","+infinity","ln(2)/3"},
 {"1/(exp(x)-1)","0","+infinity","+infinity"}
 };
 for(const Case &t:yes){
  bool ok=integrate_real_definite(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx);
  gen want=gen(t.answer,ctx).eval(1,ctx);
  if(!ok || (res!=want && !is_zero(_simplify(res-want,ctx)))){std::cerr<<"FAIL "<<t.f<<" got "<<res<<" expected "<<want<<" matched "<<ok<<'\n';return 1;}
 }
 const Case no[]={
 {"x^2/(exp(x+1)+exp(-x+1)+2)","-infinity","+infinity",0},
 {"x^2/(exp(x+i)+exp(-x-i)+2)","-infinity","+infinity",0},
 {"x*sin(x)/(1-2*cos(x)^2)","0","pi",0},
 {"exp(2*x)*sin(3*x)/x","0","+infinity",0},
 {"exp(-2*x)*cos(3*x)/x","0","+infinity",0},
 {"exp(-2*x)*sin(3*x+1)/x","0","+infinity",0},
 {"exp(-2*x)*sin(3*x)/x","-infinity","+infinity",0},
 {"exp(x^2)*cos(3*x)","-infinity","+infinity",0},
 {"exp(-x^2)*cos(3*x^2)","-infinity","+infinity",0},
 {"exp(-x^2)*cos(3*x)","0","+infinity",0},
 {"ln(sin(x))","-pi","0",0},{"ln(cos(x))","0","pi",0},
 {"ln(abs(sin(x)))","i","i+pi",0},
 {"x/(exp(-x)+1)","0","+infinity",0},
 {"x/(exp(x+1)+1)","0","+infinity",0},
 {"x^(-1)/(exp(x)+1)","0","+infinity",0},
 {"x/(exp(x)-2)","0","+infinity",0}
 };
 for(const Case &t:no)assert(!integrate_real_definite(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx));
 angle_radian(false,ctx);
 assert(!integrate_real_definite(gen("exp(-x^2)*cos(3*x)",ctx),x,minus_inf,plus_inf,res,ctx));
 std::cout<<"PASS: 24 convergence/transform identities, 18 rejected conditions\n";
}
