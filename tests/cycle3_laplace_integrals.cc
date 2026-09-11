#include "giacPCH.h"
#include <cassert>
#include <iostream>
namespace giac {
bool integrate_real_definite(const gen &,const gen &,gen,gen,gen &,GIAC_CONTEXT);
bool integrate_inverse_gaussian(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);
gen integration_syntax(const gen &,GIAC_CONTEXT);
}
using namespace giac;
int main(){
 context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
 struct Case {const char *f,*answer;};
 const Case yes[]={
 {"exp(-2*x)*(1-cos(3*x))/x^2","3*atan(3/2)-ln(13/4)"},
 {"exp(-3*x)*(sin(2*x)-2*x)/x^2","2-3*atan(2/3)-ln(13/9)"},
 {"exp(-2*x)*(sin(3*x+pi/3)-sin(pi/3)-3*x*cos(pi/3))/x^2","(3-2*atan(3/2)-3*ln(13/4)/2)/2-sqrt(3)*(3*atan(3/2)-ln(13/4))/2"},
 {"exp(-5*x)*(cos(-7*x)-1)/x^2","-7*atan(7/5)+5*ln(74/25)/2"},
 {"exp(1-4*x)*(sin(-3*x)+3*x)/x^2","exp(1)*(-3+4*atan(3/4)+3*ln(25/16)/2)"},
 {"exp(-2*x)*(sin(x)-sin(3*x)+2*x)/x^2","-2-2*atan(1/2)+2*atan(3/2)-ln(5/4)/2+3*ln(13/4)/2"},
 {"exp(-2*x)*(cos(3*x+pi/2)+3*x)/x^2","-3+2*atan(3/2)+3*ln(13/4)/2"},
 {"exp(-2*x)*(cos(3*x+pi/3)-cos(pi/3)+3*x*sin(pi/3))/x^2","(-3*atan(3/2)+ln(13/4))/2-sqrt(3)*(3-2*atan(3/2)-3*ln(13/4)/2)/2"},
 {"exp(-3*x^2-2/x^2)","sqrt(pi/3)*exp(-2*sqrt(6))/2"},
 {"sqrt(x)*exp(-2*x-3/x)","sqrt(pi)*exp(-2*sqrt(6))*(1/(4*sqrt(2))+sqrt(3)/2)"},
 {"x^2*exp(-3*x^2-2/x^2)","sqrt(pi)*exp(-2*sqrt(6))*(1/(12*sqrt(3))+sqrt(2)/6)"},
 {"x^(31/2)*exp(-x-1/x)","1329548327094606279*sqrt(pi)*exp(-2)/65536"},
 {"x^(3/2)*exp(-x-1/x)","13*sqrt(pi)*exp(-2)/4"},
 {"x^(-7/2)*exp(-x-1/x)","13*sqrt(pi)*exp(-2)/4"},
 {"x^(13/2)*exp(-x^3-1/x^3)","13*sqrt(pi)*exp(-2)/12"},
 {"x^(1/4)*exp(-sqrt(x)-1/sqrt(x))","13*sqrt(pi)*exp(-2)/2"},
 {"x^(-3/2)*exp(-2*x-3/x)","sqrt(pi/3)*exp(-2*sqrt(6))"}
 };
 for(const Case &t:yes){
  bool ok=integrate_real_definite(gen(t.f,ctx),x,0,plus_inf,res,ctx);
  gen want=gen(t.answer,ctx).eval(1,ctx);
  if(!ok || (res!=want && !is_zero(_simplify(res-want,ctx)))){std::cerr<<"FAIL "<<t.f<<" got "<<res<<" expected "<<want<<" matched "<<ok<<'\n';return 1;}
 }
 const char *no[]={
 "exp(-x)*(sin(2*x)-x)/x^2", // uncancelled first derivative
 "exp(-x)*(cos(2*x)-2)/x^2", // uncancelled constant
 "exp(x)*(1-cos(x))/x^2", // exponential growth
 "exp(-x)*(sin(x+i)-sin(i)-x*cos(i))/x^2", // complex phase
 "exp(-x)*(1-cos(x))/x^3", // logarithmically divergent
 "exp(-2*x+3/x)*sqrt(x)", // growth near zero
 "exp(2*x-3/x)*sqrt(x)", // growth near infinity
 "exp(-x^2-1/x)*x^2", // unmatched powers
 "exp(-x-1/x)/x", // non-half-integer Bessel order
 "exp(-x-1/x)*x^(33/2)" // bounded recurrence budget
 };
 for(const char *f:no){
  if(integrate_real_definite(gen(f,ctx),x,0,plus_inf,res,ctx)){std::cerr<<"Unexpected match: "<<f<<" -> "<<res<<'\n';return 1;}
 }
 std::cout<<"PASS: 17 second-order Laplace / half-integer Gaussian identities, 10 rejected conditions\n";
}
