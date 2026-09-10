#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <cmath>
namespace giac {bool user_log_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
int main(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
struct Case{const char *f,*lo,*hi,*want;};
const Case yes[]={
 {"(atan(2*x)-atan(x))/x","0","+infinity","pi*ln(2)/2"},
 {"(atan(pi*x)-atan(x))/x","0","+infinity","pi*ln(pi)/2"},
 {"(atan(x)-atan(3*x))/x","0","+infinity","-pi*ln(3)/2"},
 {"(atan(sqrt(2)*x)-atan(x))/x","0","+infinity","pi*ln(2)/4"},
 {"(atan(2*x)+atan(3*x)-2*atan(x))/x","0","+infinity","pi*ln(6)/2"},
 {"(atan(exp(1)*x)-atan(x))/x","0","+infinity","pi/2"},
 {"ln(ln(1/x))/(1+x^2)","0","1","pi*ln(2*pi)/4+pi*ln(Gamma(3/4)/Gamma(1/4))/2"},
 {"ln(-ln(x))/(1+x^2)","0","1","pi*ln(2*pi)/4+pi*ln(Gamma(3/4)/Gamma(1/4))/2"},
 {"x*ln(-ln(x^2))/(1+x^4)","0","1","pi*ln(2*pi)/8+pi*ln(Gamma(3/4)/Gamma(1/4))/4"},
 {"x^2*ln(-ln(x^3)/2)/(1+x^6)","0","1","pi*ln(pi)/12+pi*ln(Gamma(3/4)/Gamma(1/4))/6"},
 {"x^(-1/2)*ln(ln(1/sqrt(x)))/(1+x)","0","1","pi*ln(2*pi)/2+pi*ln(Gamma(3/4)/Gamma(1/4))"},
 {"x^16*ln(-ln(x^17))/(1+x^34)","0","1","pi*ln(2*pi)/68+pi*ln(Gamma(3/4)/Gamma(1/4))/34"},
 {"ln(-ln(x)/pi)/(1+x^2)","0","1","pi*ln(2)/4+pi*ln(Gamma(3/4)/Gamma(1/4))/2"},
 {"ln(-2*ln(x))/(1+x^2)","0","1","pi*ln(4*pi)/4+pi*ln(Gamma(3/4)/Gamma(1/4))/2"}
};
for(unsigned evaluated=0;evaluated<2;++evaluated)for(const Case&t:yes){gen f(t.f,ctx);if(evaluated)f=f.eval(1,ctx);bool ok=user_log_rule(f,x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx);gen want=gen(t.want,ctx).eval(1,ctx);if(!ok||!is_zero(_simplify(r-want,ctx))){std::cerr<<t.f<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}}
const Case no[]={
 {"(atan(2*x)+atan(x))/x","0","+infinity",0},
 {"(atan(-2*x)-atan(x))/x","0","+infinity",0},
 {"(atan(2*x+1)-atan(x))/x","0","+infinity",0},
 {"(atan(2*i*x)-atan(x))/x","0","+infinity",0},
 {"(atan(a*x)-atan(x))/x","0","+infinity",0},
 {"(atan(2*x)-atan(x))/x","0","1",0},
 {"ln(ln(x))/(1+x^2)","0","1",0},
 {"ln(ln(1/x))/(1-x^2)","0","1",0},
 {"ln(ln(1/x))/(1+x^2)","0","2",0},
 {"x*ln(ln(1/x))/(1+x^2)","0","1",0},
 {"ln(ln(2/x))/(1+x^2)","0","1",0},
 {"ln(i*ln(x))/(1+x^2)","0","1",0},
 {"x^(-2)*ln(-ln(x^(-1)))/(1+x^(-2))","0","1",0}
};
for(const Case&t:no)if(user_log_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"incorrect acceptance: "<<t.f<<'\n';return 1;}
assert(user_log_rule(gen(yes[6].f,ctx),x,0,1,r,ctx));
gen value=evalf_double(r,1,ctx);assert(value.type==_DOUBLE_ && std::abs(value._DOUBLE_val+0.26044280630098844554)<1e-13);
std::cout<<"PASS: 14 Frullani/log-log identities in raw and evaluated syntax, independent quadrature reference and 13 domain rejections\n";
}
