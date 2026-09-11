// Domain restrictions are as important as the answers: these direct helper
// tests assert that superficially similar invalid identities are rejected.
#include "giacPCH.h"
#include <cassert>
#include <iostream>
namespace giac {
bool integrate_real_definite(const gen &,const gen &,gen,gen,gen &,GIAC_CONTEXT);
bool integrate_real_root(const gen &,const gen &,gen &,int,GIAC_CONTEXT);
}
using namespace giac;
int main(){
  context c;const context *ctx=&c;angle_radian(true,ctx);
  gen x(identificateur("x"));
  struct test {const char *f,*lo,*hi,*answer;};
  const test accepted[]={
    {"exp(-x^2)/(1+exp(2*x))","-infinity","+infinity","sqrt(pi)/2"},
    {"3*exp(-4*x^2)/(1+exp(-7*x))","-infinity","+infinity","3*sqrt(pi)/4"},
    {"exp(-2*x^2)/(1+exp(3*x))","+infinity","-infinity","-sqrt(pi/2)/2"},
    {"1/(1+(2/(x+3)+5/(x-1))^(-2))","-infinity","+infinity","7*pi"},
    {"1/(1+(-2/(x+3)-5/(x-1))^(-2))","-infinity","+infinity","7*pi"},
    {"-3/(1+(2/(2*x+3)+5/(x-1))^(-2))","-infinity","+infinity","-18*pi"},
    {"1/(1+(2/(x-1)+5/(x-1))^(-2))","+infinity","-infinity","-7*pi"},
    {"1/(1+(3/(x-7))^(-2))","-infinity","+infinity","3*pi"},
    {"exp(-9/x^2)/x^2","0","+infinity","sqrt(pi)/6"},
    {"-2*exp(-4/x^2)/x^2","+infinity","0","sqrt(pi)/2"},
    {"exp(-x^2)/x^2","0","+infinity","+infinity"},
    {"-2*exp(3*x^2)/x^2","0","+infinity","-infinity"},
    {"exp(-2*x^2)/x^2","+infinity","0","-infinity"},
    {"1/(x^2*ln(x))","1","+infinity","+infinity"},
    {"-2/(x^2*ln(x))","1","3","-infinity"},
    {"1/(x^2*ln(x))","2","1","-infinity"},
    {"1/(1+tan(x)^sqrt(2020))","0","pi/2","pi/4"},
    {"3/(1+tan(2*x)^sqrt(2))","0","pi/4","3*pi/8"},
    {"1/(1+tan(x)^(-3))","pi/2","0","-pi/4"},
    {"1/(1+tan(-x)^3)","-pi/2","0","pi/4"},
    {"ln(1+x)/(1+x^2)","0","1","pi*ln(2)/8"},
    {"3*ln(2*(1+2*x))/(1+(2*x)^2)","0","1/2","9*pi*ln(2)/16"},
    {"ln(1+(2*x+1))/(1+(2*x+1)^2)","-1/2","0","pi*ln(2)/16"},
    {"ln(1+x)/(1+x^2)","1","0","-pi*ln(2)/8"},
    {"ln(1-x)/(1+(-x)^2)","-1","0","pi*ln(2)/8"}
  };
  unsigned passed=0;
  for (const test &t:accepted){
    gen f(t.f,ctx),lo=gen(t.lo,ctx).eval(1,ctx),hi=gen(t.hi,ctx).eval(1,ctx),res;
    bool matched=integrate_real_definite(f,x,lo,hi,res,ctx);
    gen expected=gen(t.answer,ctx).eval(1,ctx);
    if (!matched || (res!=expected && !is_zero(_simplify(res-expected,ctx)))){
      std::cerr<<"Failed "<<f<<": matched="<<matched<<" result="<<res<<" expected="<<expected<<'\n';return 1;
    }
    ++passed;
  }
  const test rejected[]={
    {"exp(x^2)/(1+exp(2*x))","-infinity","+infinity",0},
    {"exp(-x^2)/(1+exp(2*x+1))","-infinity","+infinity",0},
    {"exp(-x^2+x)/(1+exp(2*x))","-infinity","+infinity",0},
    {"exp(-x^2)/(1+exp(i*x))","-infinity","+infinity",0},
    {"exp(-x^2)/(1+exp(2*x))","0","+infinity",0},
    {"1/(1+(2/(x+3)-5/(x-1))^(-2))","-infinity","+infinity",0},
    {"1/(1+(1+2/(x+3)+5/(x-1))^(-2))","-infinity","+infinity",0},
    {"1/(1+(2/(x+i)+5/(x-1))^(-2))","-infinity","+infinity",0},
    {"1/(1+(2/(x^2+1)+5/(x-1))^(-2))","-infinity","+infinity",0},
    {"1/(1+(2/(x+3)+5/(x-1))^(-2))","0","+infinity",0},
    {"exp(9/x^2)/x^2","0","+infinity",0},
    {"exp(-9/x^2)/x^3","0","+infinity",0},
    {"exp(-x^2)/x","-infinity","+infinity",0},
    {"1/(x^2*ln(x))","0","2",0},
    {"1/(x^2*ln(x))","2","+infinity",0},
    {"1/(x^2*ln(x))","0","1",0},
    {"1/(1+tan(x)^sqrt(2))","-pi/2","pi/2",0},
    {"1/(1+tan(x)^sqrt(2))","0","pi",0},
    {"1/(1+tan(x)^i)","0","pi/2",0},
    {"1/(1+tan(x)^a)","0","pi/2",0},
    {"ln(1+x)/(1+x^2)","0","2",0},
    {"ln(1-x)/(1+x^2)","0","1",0},
    {"ln(-1-x)/(1+x^2)","0","1",0},
    {"ln(1+x)/(1-x^2)","0","1",0}
  };
  for(const test &t:rejected){
    gen res;
    if(integrate_real_definite(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx)){
      std::cerr<<"Incorrectly accepted "<<t.f<<'\n';return 2;
    }
  }
  angle_radian(false,ctx);
  gen res;
  assert(!integrate_real_definite(gen("1/(1+tan(x)^sqrt(2))",ctx),x,0,cst_pi/2,res,ctx));
  angle_radian(true,ctx);
  const char *roots[]={"cos(surd(x,3))/surd(x,3)",
    "cos(surd(2*x+1,3))/surd(2*x+1,3)",
    "sin(surd(x,5))/surd(x,5)^3", "cos(surd(x,3))*surd(x,-3)"};
  for(const char *f:roots){
    gen e=gen(f,ctx).eval(1,ctx),primitive;
    if(!integrate_real_root(e,x,primitive,0,ctx)){std::cerr<<"Root substitution rejected "<<e<<'\n';return 3;}
    gen d=derive(primitive,x,ctx);
    for(int point=-4;point<=4;++point){
      if(!point)continue;
      gen a=evalf_double(subst(d,x,point,false,ctx),1,ctx),b=evalf_double(subst(e,x,point,false,ctx),1,ctx);
      if(a.type!=_DOUBLE_ || b.type!=_DOUBLE_ || !(std::abs(a._DOUBLE_val-b._DOUBLE_val)<1e-8)){
        std::cerr<<"Root derivative mismatch "<<e<<" at "<<point<<": "<<a<<" vs "<<b<<'\n';return 4;
      }
    }
  }
  const char *badroots[]={"cos(surd(x,2))/surd(x,2)","cos(surd(x^2,3))/surd(x^2,3)",
    "cos(surd(x,3))/surd(x+1,3)","cos(surd(x,3))*x^1000","cos(surd(i*x,3))/surd(i*x,3)"};
  for(const char *f:badroots)assert(!integrate_real_root(gen(f,ctx).eval(1,ctx),x,res,0,ctx));
  complex_mode(true,ctx);
  assert(!integrate_real_root(gen(roots[0],ctx).eval(1,ctx),x,res,0,ctx));
  std::cout<<"PASS: "<<passed<<" definite identities, "<<sizeof(rejected)/sizeof(test)
           <<" rejected domains, 32 real-root derivative samples and 6 rejected root cases\n";
}
