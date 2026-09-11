#include "giacPCH.h"
#include <iostream>
namespace giac {bool user_log_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
int main(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
struct Case{const char *f,*lo,*hi,*answer;};
const Case yes[]={
 {"(atan(2/sqrt(x))-atan(3/sqrt(x)))*ln(x)/x","0","+infinity","pi*(ln(2)^2-ln(3)^2)"},
 {"4*x*acos((3-x^4)/(5-x^4))/(2+2*x^4)","0","2^(1/4)","pi*atan(sqrt(2))-atan(sqrt(2))^2-pi*atan(sqrt(5))+pi^2/4"},
 {"ln((1+x)/(1-x))^2/sqrt(1-x^2)","-1","1","pi^3"},
 {"ln((x-2)/(6-x))^2/sqrt((x-2)*(6-x))","2","6","pi^3"},
 {"ln((x-2)/(6-x))^2/sqrt(-x^2+8*x-12)","2","6","pi^3"},
 {"ln((x-2)/(6-x))^2","2","6","4*pi^2/3"},
 {"(x-2)*ln((x-2)/(6-x))","2","6","8"},
 {"sqrt((x-2)/(6-x))*ln((x-2)/(6-x))^3","2","6","12*pi^3"},
 {"ln((1+x)/(1-x))^3/sqrt(1-x^2)","-1","1","0"},
 {"(atan(2*sqrt(x))-atan(3*sqrt(x)))*ln(x)/x","0","+infinity","pi*(ln(3)^2-ln(2)^2)"},
 {"(atan(2*x^3)-atan(x^3))*ln(x)^2/x","0","+infinity","(pi^3*ln(2)/8+pi*ln(2)^3/6)/27"},
 {"(atan(pi*sqrt(x))-atan(sqrt(x)))/x","0","+infinity","pi*ln(pi)"},
 {"4*x*acos((3-x^4)/(5-x^4))/(1+x^4)","0","2^(1/4)","2*pi*atan(sqrt(2))-2*atan(sqrt(2))^2-2*pi*atan(sqrt(5))+pi^2/2"},
 {"2*acos((3-x^2)/(5-x^2))/(1+x^2)","0","sqrt(2)","2*pi*atan(sqrt(2))-2*atan(sqrt(2))^2-2*pi*atan(sqrt(5))+pi^2/2"},
 {"4*acos((1-4*x^2)/(3-4*x^2))/(1+4*x^2)","0","1/2","7*pi^2/8-2*pi*atan(sqrt(3))"},
 {"6*x^2*acos((1-x^6)/(3-x^6))/(1+x^6)","0","1","7*pi^2/8-2*pi*atan(sqrt(3))"}
};
for(const Case&t:yes){bool ok=user_log_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx);gen want=gen(t.answer,ctx).eval(1,ctx);if(!ok||!is_zero(_simplify(r-want,ctx))){std::cerr<<t.f<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}}
const Case no[]={
 {"ln(((1+x)/(1-x))^1000000)^2/sqrt(1-x^2)","-1","1",0},
 {"ln((1+x)/(1-x))^2/sqrt(1-x^2)","-1","2",0},
 {"ln((1+x)/(1-x))^2/(1-x^2)","-1","1",0},
 {"ln((1+x)/(x-1))^2/sqrt(1-x^2)","-1","1",0},
 {"sqrt(ln((1+x)/(1-x))^2)/sqrt(1-x^2)","-1","1",0},
 {"(atan(2*sqrt(x))-atan(3*x))*ln(x)/x","0","+infinity",0},
 {"(atan(2*sqrt(x))+atan(3*sqrt(x)))*ln(x)/x","0","+infinity",0},
 {"4*x*acos((3-x^4)/(5-x^4))/(1+x^4)","0","2",0},
 {"4*x*acos((3-x^4)/(5-x^4))/(1+2*x^4)","0","2^(1/4)",0},
 {"4*x^2*acos((3-x^4)/(5-x^4))/(1+x^4)","0","2^(1/4)",0},
 {"4*x*acos((3+x^4)/(5-x^4))/(1+x^4)","0","2^(1/4)",0}
};
for(const Case&t:no)if(user_log_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"incorrect acceptance "<<t.f<<" -> "<<r<<'\n';return 1;}
std::cout<<"PASS: 16 affine/monomial substitution identities and 11 rejected conditions\n";
}
