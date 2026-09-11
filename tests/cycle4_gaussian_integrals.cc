#include "giacPCH.h"
#include <iostream>
namespace giac {bool integrate_real_definite(const gen &,const gen &,gen,gen,gen &,GIAC_CONTEXT);}
using namespace giac;
int main(){
 context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
 struct Case{const char *f,*lo,*hi,*answer;};
 const Case yes[]={
 {"exp(-x)*(1-cos(2*x))^2/x^2","0","+infinity","4*atan(2)-2*atan(4)-ln(5)+ln(17)/4"},
 {"exp(-x)*sin(x)^2/x^2","0","+infinity","atan(2)-ln(5)/4"},
 {"exp(-x)*sin(x)^4/x^2","0","+infinity","atan(2)-atan(4)/2-ln(5)/4+ln(17)/16"},
 {"exp(-x)*(1-cos(x))*sin(x)/x^2","0","+infinity","-pi/4+atan(2)/2-ln(2)/2+ln(5)/2"},
 {"exp(-x)*(sin(x+pi/2)-1)^2/x^2","0","+infinity","pi/2-atan(2)-ln(2)+ln(5)/4"},
 {"exp(1-x)*(1-cos(2*x))^2/x^2","+infinity","0","-exp(1)*(4*atan(2)-2*atan(4)-ln(5)+ln(17)/4)"},
 {"exp(-2*x^2)*erf(x+1)","-infinity","+infinity","sqrt(pi/2)*erf(sqrt(2/3))"},
 {"exp(-(x-1)^2)*erf(x-1)*erf(2*x-2)","-infinity","+infinity","2*asin(2/sqrt(10))/sqrt(pi)"},
 {"exp(-2*(x-3)^2+1)*erf(2*x-5)","-infinity","+infinity","exp(1)*sqrt(pi/2)*erf(1/sqrt(3))"},
 {"exp(-x^2+2*x-3)*erf(2*x-1)","-infinity","+infinity","exp(-2)*sqrt(pi)*erf(1/sqrt(5))"},
 {"exp(-2*x^2)*erf(1-x)","-infinity","+infinity","sqrt(pi/2)*erf(sqrt(2/3))"},
 {"exp(-(x-1)^2)*erf(1-x)*erf(2*x-2)","-infinity","+infinity","-2*asin(2/sqrt(10))/sqrt(pi)"},
 {"exp(-x^2)*erf(x)^2","-infinity","+infinity","sqrt(pi)/3"},
 {"exp(-x^2)*erf(x)^2","0","+infinity","sqrt(pi)/6"},
 {"exp(-x^2)*erf(x)","0","+infinity","sqrt(pi)/4"},
 {"exp(-x^2)*erf(-x)","0","+infinity","-sqrt(pi)/4"},
 {"exp(-x^2)*erf(x)","-infinity","+infinity","0"}
 };
 for(unsigned evaluated=0;evaluated<2;++evaluated)for(const Case &t:yes){
  gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);
  bool ok=integrate_real_definite(f,x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx);
  gen want=gen(t.answer,ctx).eval(1,ctx);
  if(!ok || (res!=want && !is_zero(_simplify(res-want,ctx)))){std::cerr<<"FAIL "<<t.f<<" parsed "<<f<<" got "<<res<<" expected "<<want<<" matched "<<ok<<'\n';return 1;}
 }
 const Case no[]={
 {"exp(-x)*cos(x)^2/x^2","0","+infinity",0},
 {"exp(-x)*(1-cos(x))^(1/2)/x^2","0","+infinity",0},
 {"exp(-x)*(1-cos(x))^(-2)/x^2","0","+infinity",0},
 {"exp(-x)*(1-cos(x^2))^2/x^2","0","+infinity",0},
 {"exp(x)*(1-cos(x))^2/x^2","0","+infinity",0},
 {"exp(-x)*(1-cos(x))^8/x^2","0","+infinity",0},
 {"exp(x^2)*erf(x+1)","-infinity","+infinity",0},
 {"exp(-x^2)*erf(x+i)","-infinity","+infinity",0},
 {"exp(-x^2)*erf(x^2)","-infinity","+infinity",0},
 {"exp(-x^2)*erf(x+1)*erf(2*x)","-infinity","+infinity",0},
 {"exp(-x^2)*erf(x)*erf(2*x)*erf(3*x)","-infinity","+infinity",0},
 {"exp(-x^2)*erf(x+1)","0","+infinity",0},
 {"exp(-(x-1)^2)*erf(x-1)","0","+infinity",0},
 {"exp(-x^4)*erf(x+1)","-infinity","+infinity",0}
 };
 for(const Case &t:no){
  if(integrate_real_definite(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx)){std::cerr<<"UNEXPECTED "<<t.f<<" -> "<<res<<'\n';return 1;}
 }
 std::cout<<"PASS: 17 harmonic/Gaussian identities in raw and evaluated syntax, 14 rejected conditions\n";
}
