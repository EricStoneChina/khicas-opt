#include "giacPCH.h"
#include <iostream>
#include <cassert>
namespace giac {bool integrate_real_definite(const gen &,const gen &,gen,gen,gen &,GIAC_CONTEXT);}
int main(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),res;
struct Case{const char *f,*lo,*hi,*answer;};
const Case yes[]={
{"atan(sqrt(x^2+2))/((x^2+1)*sqrt(x^2+2))","0","1","5*pi^2/96"},
{"atan(sqrt(x^2+8)/2)/((x^2+4)*sqrt(x^2+8))","0","2","5*pi^2/384"},
{"atan(sqrt(9*x^2+72)/6)/((2*x^2+8)*sqrt(9*x^2+72))","0","2","5*pi^2/2304"},
{"atan(sqrt(x^2+2)/2)/((x^2+1)*sqrt(x^2+2))","0","2","(pi*atan(2/sqrt(6))-atan(2)^2)/2"},
{"atan(sqrt(x^2+2))/((x^2+1)*sqrt(x^2+2))","1","0","-5*pi^2/96"},
{"x*ln(sin(x))","0","pi","-pi^2*ln(2)/2"},
{"(2*x+3)*ln(sin(2*x))","0","pi/2","-(pi/2+3)*pi*ln(2)/2"},
{"x*ln(sin(x))","pi","0","pi^2*ln(2)/2"},
{"(3*x+2)*ln(cos(x))","-pi/2","pi/2","-2*pi*ln(2)"},
{"x*ln(abs(sin(x)))","0","2*pi","-2*pi^2*ln(2)"}
};
for(const Case &t:yes){bool ok=integrate_real_definite(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx);gen want=gen(t.answer,ctx).eval(1,ctx);
if(!ok || !is_zero(_simplify(res-want,ctx))){std::cerr<<t.f<<" matched="<<ok<<" result="<<res<<" want="<<want<<'\n';return 1;}}
const Case no[]={
{"atan(sqrt(x^2+3))/((x^2+1)*sqrt(x^2+3))","0","1",0},
{"atan(sqrt(x^2+2))/((x^2+1)*sqrt(x^2+3))","0","1",0},
{"atan(sqrt(x^2+2))/((x^2+1)*sqrt(x^2+2))","0","2",0},
{"atan(sqrt(x^2-2))/((x^2-1)*sqrt(x^2-2))","0","1",0},
{"x*ln(sin(x))","0","pi/2",0},
{"x^2*ln(sin(x))","0","pi",0},
{"x*ln(abs(sin(x)))","1","1+pi",0},
{"x*ln(cos(x))","0","pi",0}
};
for(const Case&t:no)assert(!integrate_real_definite(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx));
angle_radian(false,ctx);assert(!integrate_real_definite(gen(yes[0].f,ctx),x,0,1,res,ctx));
std::cout<<"PASS: 10 symmetry identities and 9 rejected conditions\n";
}
