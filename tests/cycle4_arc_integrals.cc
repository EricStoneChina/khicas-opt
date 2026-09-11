#include "giacPCH.h"
#include <cassert>
#include <cmath>
#include <iostream>
namespace giac {bool cycle4_arc_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
int main(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
struct Case{const char *f,*lo,*hi,*want;};
const Case yes[]={
{"2*asin((3-x^2)/(5-x^2))/(1+x^2)","0","sqrt(2)","-pi*atan(sqrt(2))+2*atan(sqrt(2))^2+2*pi*atan(sqrt(5))-pi^2/2"},
{"2*acos((3-x^2)/(5-x^2))/(1+x^2)","0","sqrt(2)","2*pi*atan(sqrt(2))-2*atan(sqrt(2))^2-2*pi*atan(sqrt(5))+pi^2/2"},
{"asin((1-x^2)/(3-x^2))/(1+x^2)","0","1","pi^2/48"},
{"asin((x^2-1)/(x^2-3))/(1+x^2)","0","1","pi^2/48"},
{"acos((1-x^2)/(3-x^2))/(1+x^2)","0","1","5*pi^2/48"},
{"3*asin((1-x^2)/(3-x^2))/(2+2*x^2)","0","1","pi^2/32"},
{"acos((7-x^2)/(9-x^2))/(1+x^2)","0","2","2*pi*atan(2)-atan(2)^2-pi^2/2"},
{"acos((7-x^2)/(9-x^2))/(1+x^2)","2","0","-2*pi*atan(2)+atan(2)^2+pi^2/2"},
{"atan(2/sqrt(x^2+2))/((1+x^2)*sqrt(x^2+2))","0","2","atan(2)^2/2"},
{"atan(1/sqrt(x^2+2))/((1+x^2)*sqrt(x^2+2))","0","1","pi^2/32"},
{"atan(2/sqrt(x^2+8))/((4+x^2)*sqrt(x^2+8))","0","2","pi^2/128"},
{"atan(6/sqrt(9*x^2+72))/((8+2*x^2)*sqrt(9*x^2+72))","0","2","pi^2/768"},
{"atan(sqrt(x^2+2))/((1+x^2)*sqrt(x^2+2))","0","1","5*pi^2/96"},
{"atan(sqrt(9*x^2+72)/6)/((8+2*x^2)*sqrt(9*x^2+72))","0","2","5*pi^2/2304"},
{"atan(sqrt(x^2+2)/2)/((1+x^2)*sqrt(x^2+2))","0","2","(pi*atan(2/sqrt(6))-atan(2)^2)/2"}
};
for(const Case&t:yes){bool ok=cycle4_arc_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx);gen want=gen(t.want,ctx).eval(1,ctx);
if(!ok||!is_zero(_simplify(r-want,ctx))){std::cerr<<t.f<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}}
const Case no[]={
{"asin((1-x^2)/(3-x^2))/(1+x^2)","0","2",0},
{"asin((1-x^2)/(3-x^2))/(1+x^2)","0","sqrt(3)",0},
{"asin((1-x^2)/(3-x^2))/(1+x^2)","1/2","1",0},
{"asin((1-x^2)/(4-x^2))/(1+x^2)","0","1",0},
{"asin((1-x^2)/(3-x^2))/(2+x^2)","0","1",0},
{"asin((-3-x^2)/(-1-x^2))/(1+x^2)","0","1",0},
{"asin((1-x^2)/(3-x^2))/(1+x^2)","0","i",0},
{"atan(2/sqrt(x^2+2))/((1+x^2)*sqrt(x^2+2))","0","1",0},
{"atan(2/sqrt(x^2+3))/((1+x^2)*sqrt(x^2+3))","0","2",0},
{"atan(2/sqrt(x^2+2))/((1+x^2)*sqrt(x^2+3))","0","2",0},
{"atan(-2/sqrt(x^2+2))/((1+x^2)*sqrt(x^2+2))","0","2",0},
{"atan(2/sqrt(x^2-2))/((x^2-1)*sqrt(x^2-2))","0","2",0}
};
for(const Case&t:no)if(cycle4_arc_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"incorrect acceptance: "<<t.f<<'\n';return 1;}
angle_radian(false,ctx);assert(!cycle4_arc_rule(gen(yes[0].f,ctx),x,0,sqrt(gen(2),ctx),r,ctx));
std::cout<<"PASS: 15 arc/square identities and 13 rejected conditions\n";
}
