#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <cmath>
namespace giac {bool user_arc_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
int main(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
struct Case{const char *f,*lo,*hi,*want;};
const Case yes[]={
{"acos(cos(x)/(1+2*cos(x)))","0","pi/2","5*pi^2/24"},
{"acos((-cos(x))/(-1-2*cos(x)))","0","pi/2","5*pi^2/24"},
{"acos((cos(x)-2)/(1+4*cos(x)))","0","pi/3","7*pi^2/9-2*pi*atan(sqrt(5/3))"},
{"acos((cos(x)-1)/(1+3*cos(x)))","0","2*atan(1/sqrt(2))","pi^2/2-2*pi*atan(sqrt(2))+2*pi*atan(sqrt(2)/2)-2*atan(sqrt(2)/2)^2"},
{"acos((1+2*cos(x))/(2+3*cos(x)))","0","2*atan(sqrt(2))","2*pi*atan(sqrt(2))-2*atan(sqrt(2))^2-2*pi*atan(sqrt(5))+pi^2/2"},
{"acos((2+3*cos(x))/(3+4*cos(x)))","0","2*pi/3","17*pi^2/18-2*pi*atan(sqrt(7))"},
{"acos((3+4*cos(x))/(4+5*cos(x)))","0","2*atan(2)","4*pi*atan(2)-2*atan(2)^2-pi^2"},
{"acos(cos(3*x-pi/2)/(1+2*cos(3*x-pi/2)))","pi/6","pi/3","5*pi^2/72"},
{"acos(cos(pi/2-2*x)/(1+2*cos(pi/2-2*x)))","0","pi/4","5*pi^2/48"},
{"acos(cos(x)/(1+2*cos(x)))","pi/2","0","-5*pi^2/24"},
{"acos(cos(x-1)/(1+2*cos(x-1)))","1","1+pi/2","5*pi^2/24"}
};
for(const Case&t:yes){bool ok=user_arc_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx);gen want=gen(t.want,ctx).eval(1,ctx);
if(!ok||!is_zero(_simplify(r-want,ctx))){std::cerr<<t.f<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}}
// Independently evaluated 80-digit direct acos quadrature for q=1/3,1/2,2,3,4.
const char *numeric[]={"1.9477383448188285192062041322589440240827257675133082526588233471",
"2.0419133806552511017615339510419853106322709043998520595182506356",
"1.8846646130507738953404351611065391807867983431347479073561873355",
"1.7222252478122960926550936054146040701685973698022217582417752855",
"1.5916801452975397508075432005943749797420743373635061381587922236"};
for(int i=0;i<5;++i){const Case&t=yes[i+2];assert(user_arc_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx));
  gen error=evalf_double(r-gen(numeric[i],ctx),1,ctx);assert(error.type==_DOUBLE_ && std::abs(error._DOUBLE_val)<1e-12);
}
const Case no[]={
{"acos(cos(x)/(1+2*cos(x)))","0","1",0},
{"acos(cos(x)/(1+2*cos(x)))","0","-pi/2",0},
{"acos(cos(x)/(1+2*cos(x)))","0","5*pi/2",0},
{"acos(cos(x)/(1+2*cos(x)))","0","pi",0},
{"acos(cos(x)/(1+2*cos(x)))","pi/4","pi/2",0},
{"acos(cos(x)/(1+3*cos(x)))","0","pi/2",0},
{"acos(cos(x)/(-1+2*cos(x)))","0","pi/2",0},
{"acos((2+cos(x))/1)","0","pi/2",0},
{"acos(cos(x)/(1+2*cos(2*x)))","0","pi/2",0},
{"acos(cos(x^2)/(1+2*cos(x^2)))","0","sqrt(pi/2)",0},
{"acos(cos(x)/(1+cos(x)))","0","pi/2",0},
{"acos(cos(x)/(1+2*cos(x))^2)","0","pi/2",0},
{"asin(cos(x)/(1+2*cos(x)))","0","pi/2",0}
};
for(const Case&t:no)if(user_arc_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"incorrect acceptance: "<<t.f<<'\n';return 1;}
angle_radian(false,ctx);assert(!user_arc_rule(gen(yes[0].f,ctx),x,0,cst_pi/2,r,ctx));
std::cout<<"PASS: 11 acos circle identities, 5 independent numeric references, and 14 rejected conditions\n";
}
