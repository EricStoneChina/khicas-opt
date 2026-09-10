#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <cmath>
namespace giac {bool cycle3_log_rule(const gen &,const gen &,const gen &,const gen &,gen &,GIAC_CONTEXT);}
int main(){using namespace giac;context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),r;
struct Case{const char *f,*lo,*hi,*want;};
const Case yes[]={
{"ln(1+x)/x","0","1","pi^2/12"},
{"ln(x)*ln(1-x)/x","0","1","Zeta(3)"},
{"ln(1-x^3)/x","0","1","-pi^2/18"},
{"ln(x^2)^2*ln(1+x^2)/x","0","1","7*pi^4/720"},
{"ln(4*x^2)*ln(1-4*x^2)/x","0","1/2","Zeta(3)/2"},
{"ln(x)^8*ln(1-x)/x","0","1","-factorial(8)*Zeta(10)"},
{"ln(cosh(x))/cosh(x)","-infinity","+infinity","pi*ln(2)"},
{"ln(cosh(2*x+3))/cosh(2*x+3)","-infinity","+infinity","pi*ln(2)/2"},
{"ln(cosh(2*x+3))/cosh(2*x+3)","-3/2","+infinity","pi*ln(2)/4"},
{"ln(cosh(-3*x+1))/cosh(-3*x+1)^2","-infinity","+infinity","2*(1-ln(2))/3"},
{"ln(cosh(x))^2/cosh(x)","-infinity","+infinity","pi*(ln(2)^2+pi^2/12)"},
{"ln(cosh(x))/cosh(x)^3","-infinity","+infinity","pi*(2*ln(2)-1)/4"},
{"atan(x)/(x*(1+x^2))","0","+infinity","pi*ln(2)/2"},
{"atan(2*x)/(x*(3+12*x^2))","0","+infinity","pi*ln(2)/6"},
{"atan(-2*x)/(x*(3+12*x^2))","0","+infinity","-pi*ln(2)/6"},
{"atan(x)^2/(x*(1+x^2))","0","+infinity","pi^2*ln(2)/4-7*Zeta(3)/8"},
{"atan(x)^3/(x*(1+x^2))","0","+infinity","pi^3*ln(2)/8-9*pi*Zeta(3)/16"},
{"atan(x)^4/(x*(1+x^2))","0","+infinity","pi^4*ln(2)/16-9*pi^2*Zeta(3)/16+93*Zeta(5)/32"}
};
for(const Case&t:yes){bool ok=cycle3_log_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx);gen want=gen(t.want,ctx).eval(1,ctx);if(!ok||!is_zero(_simplify(r-want,ctx))){std::cerr<<t.f<<" accepted="<<ok<<" got="<<r<<" want="<<want<<'\n';return 1;}}
const Case no[]={
{"ln(1+x)/x","0","2",0},
{"ln(1-x)/x","0","2",0},
{"ln(1+2*x)/x","0","1",0},
{"ln(x)*ln(1-x^2)/x","0","1",0},
{"ln(x)^9*ln(1-x)/x","0","1",0},
{"ln(cosh(x))/cosh(x)","0","1",0},
{"ln(cosh(x))*cosh(x)","-infinity","+infinity",0},
{"ln(cosh(x))/cosh(2*x)","-infinity","+infinity",0},
{"ln(cosh(x))/cosh(x)^17","-infinity","+infinity",0},
{"atan(x)/(x*(1+2*x^2))","0","+infinity",0},
{"atan(x)/(x*(1+x^2))","-infinity","+infinity",0},
{"atan(x)^9/(x*(1+x^2))","0","+infinity",0},
{"atan(x+1)/(x*(1+x^2))","0","+infinity",0}
};
for(const Case&t:no)if(cycle3_log_rule(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),r,ctx)){std::cerr<<"incorrect acceptance: "<<t.f<<'\n';return 1;}
// Independent 60-digit quadrature of t^n*cot(t), rather than the Fourier recurrence.
const char *numeric[]={"0.61023647942964360816630148141147644072284360252846",
"0.70545500443399489237457440769488550600498633988977",
"0.85078001432382704513941348879315417262171489842284",
"1.0592214436492161073943101817700667056298269406411"};
for(int n=5;n<=8;++n){
  std::string f="atan(x)^"+std::to_string(n)+"/(x*(1+x^2))";
  assert(cycle3_log_rule(gen(f,ctx),x,0,plus_inf,r,ctx));
  gen error=evalf_double(r-gen(numeric[n-5],ctx),1,ctx);
  if(error.type!=_DOUBLE_ || std::abs(error._DOUBLE_val)>2e-12){std::cerr<<"high atan moment n="<<n<<" error="<<error<<'\n';return 1;}
}
angle_radian(false,ctx);assert(!cycle3_log_rule(gen(yes[12].f,ctx),x,0,plus_inf,r,ctx));
std::cout<<"PASS: 18 exact logarithmic moments, 4 independent numeric moments, and 14 rejected conditions\n";
}
