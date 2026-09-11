#include "giacPCH.h"
#include <iostream>
namespace giac {bool user_log_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
int main(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
struct Case{const char *f,*hi,*answer;};
const Case yes[]={
 {"(atan(2*x)-atan(3*x))*ln(x)/x","+infinity","pi*(ln(3)^2-ln(2)^2)/4"},
 {"(atan(2*x)-atan(x))*ln(x)^2/x","+infinity","pi^3*ln(2)/8+pi*ln(2)^3/6"},
 {"(atan(pi*x)-atan(x))*ln(x)/x","+infinity","-pi*ln(pi)^2/4"},
 {"(atan(2*x)+atan(3*x)-2*atan(x))*ln(x)^2/x","+infinity","pi^3*ln(6)/8+pi*(ln(2)^3+ln(3)^3)/6"},
 {"(x-x^3)*ln(-ln(x))/(-ln(x))","1","-3*ln(2)^2/2-euler_gamma*ln(2)"},
 {"(1-x)*ln(-ln(x))/(-ln(x))","1","-ln(2)^2/2-euler_gamma*ln(2)"},
 {"(1-x)/(-ln(x))","1","ln(2)"},
 {"(1-x)*ln(-ln(x))^2/(-ln(x))","1","ln(2)^3/3+euler_gamma*ln(2)^2+(euler_gamma^2+pi^2/6)*ln(2)"},
 {"(1-x)*ln(-2*ln(x))/(-ln(x))","1","ln(2)^2/2-euler_gamma*ln(2)"},
 {"(1-x)*ln(-ln(x))/ln(x)","1","ln(2)^2/2+euler_gamma*ln(2)"},
 {"(x-x^3)*ln(-ln(x))/(-2*ln(x))","1","-3*ln(2)^2/4-euler_gamma*ln(2)/2"},
 {"x*(1-x^2)*ln(-ln(x))/(-ln(x))","1","-3*ln(2)^2/2-euler_gamma*ln(2)"},
 {"(1-2*x+x^3)*ln(-ln(x))/(-ln(x))","1","ln(2)^2"}
};
for(unsigned evaluated=0;evaluated<2;++evaluated)for(const Case&t:yes){gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);bool ok=user_log_rule(f,x,0,gen(t.hi,ctx).eval(1,ctx),r,ctx);gen want=gen(t.answer,ctx).eval(1,ctx);if(!ok||!is_zero(_simplify(r-want,ctx))){std::cerr<<t.f<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}}
const Case no[]={
 {"(atan(2*x)+atan(3*x))*ln(x)/x","+infinity",0},
 {"(atan(-2*x)-atan(x))*ln(x)^2/x","+infinity",0},
 {"(atan(2*x)-atan(x))*ln(x)^3/x","+infinity",0},
 {"(1+x)*ln(-ln(x))/(-ln(x))","1",0},
 {"(1/x-x)*ln(-ln(x))/(-ln(x))","1",0},
 {"(1-x)*ln(ln(x))/(-ln(x))","1",0},
 {"(1-x)*ln(-ln(x))^3/(-ln(x))","1",0},
 {"(1-x)*ln(-ln(x))/(-ln(x))^2","1",0},
 {"(1-x)*ln(-ln(2*x))/(-ln(x))","1",0},
 {"(1-x)*ln(-ln(x))/(a*ln(x))","1",0}
};
for(const Case&t:no)if(user_log_rule(gen(t.f,ctx),x,0,gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"incorrect acceptance "<<t.f<<" -> "<<r<<'\n';return 1;}
std::cout<<"PASS: 13 weighted/loglog Frullani identities in both syntaxes and 10 rejection cases\n";
}
