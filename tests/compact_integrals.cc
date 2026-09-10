#include "giacPCH.h"
#include <cassert>
#include <iostream>
namespace giac {
gen integration_syntax(const gen &,GIAC_CONTEXT);
bool integrate_compact_primitive(const gen &,const gen &,gen &,GIAC_CONTEXT);
bool integrate_binomial_chain(const gen &,const gen &,gen &,bool,GIAC_CONTEXT);
bool integrate_real_definite(const gen &,const gen &,gen,gen,gen &,GIAC_CONTEXT);
}
using namespace giac;
static void derivative(const gen &f,const gen &p,const gen &x,const context *ctx,bool positive=false){
  gen d=derive(p,x,ctx);
  for(int i=-4;i<=4;++i){
    if (!i || (positive && i<0)) continue;
    gen point=gen(i)/3;
    gen a=evalf_double(subst(d,x,point,false,ctx),1,ctx),b=evalf_double(subst(f,x,point,false,ctx),1,ctx);
    if(a.type!=_DOUBLE_ || b.type!=_DOUBLE_ || !(std::abs(a._DOUBLE_val-b._DOUBLE_val)<1e-8*(1+std::abs(b._DOUBLE_val)))){
      std::cerr<<"Derivative mismatch "<<f<<" primitive "<<p<<" at "<<point<<": "<<a<<" vs "<<b<<'\n';assert(false);
    }
  }
}
int main(){
  context ctx_;const context *ctx=&ctx_;angle_radian(true,ctx);gen x(identificateur("x")),res;
  unsigned powers=0,quartics=0,trigs=0;
  for(int k=1;k<=4;++k)for(int q=1;q<=4;++q)for(int n=2;n<=7;n+=5){
    gen f=pow(x,k*q-1)*pow(2-pow(x,k),n);
    bool ok=integrate_compact_primitive(f,x,res,ctx);
    // A zero monomial exponent drops the product; that trivial case already
    // has the ordinary chain integrator and need not match this shortcut.
    if(k==1 && q==1)continue;
    assert(ok);assert(is_zero(ratnormal(derive(res,x,ctx)-f,ctx)));++powers;
  }
  const char *fractional[]={"(1+x^(1/4))^(1/3)/sqrt(x)","x^(1/2)*(2+3*x^(1/2))^(2/3)","x^(-1/3)*(2-x^(1/3))^(1/2)"};
  for(const char *s:fractional){gen f=gen(s,ctx).eval(1,ctx);if(!integrate_compact_primitive(f,x,res,ctx)){std::cerr<<"Fractional rejected "<<f<<"\n";assert(false);}derivative(f,res,x,ctx,true);
    assert(!integrate_binomial_chain(integration_syntax(f,ctx),x,res,true,ctx));}
  for(int d=1;d<=3;++d)for(int j=-1;j<=3;++j){
    gen f=(pow(x,2)-d)/((pow(x,2)+d)*sqrt(pow(x,4)+gen(j*d)*pow(x,2)+d*d,ctx));
    assert(integrate_compact_primitive(f,x,res,ctx));derivative(f,res,x,ctx);++quartics;
  }
  for(int a=-2;a<=2;++a){if(!a)continue;
    gen u=gen(a)*x+gen(1)/3,f=gen(3)/(pow(sin(u,ctx),4)+pow(cos(u,ctx),4));
    assert(integrate_compact_primitive(f,x,res,ctx));derivative(f,res,x,ctx);++trigs;
    // Continuity at old tan/atan branch boundaries, using high-precision
    // differences rather than treating local differentiation as a proof.
    gen delta=gen(1)/10000000,point=(cst_pi/4-gen(1)/3)/a;
    gen jump=evalf_double(subst(res,x,point+delta,false,ctx)-subst(res,x,point-delta,false,ctx),1,ctx);
    assert(jump.type==_DOUBLE_ && std::abs(jump._DOUBLE_val)<1e-5);
  }
  struct definite {const char *f,*lo,*hi,*expected;};
  const definite good[]={
    {"x^5*(1-x^3)^50","0","1","1/7956"},
    {"x^5*(1-x^3)^50","1","0","-1/7956"},
    {"sin(3*x)^3/x","0","+infinity","pi/4"},
    {"sin(-2*x)^5/x","0","+infinity","-3*pi/16"},
    {"sin(2*x)^7/x","-infinity","+infinity","5*pi/16"},
    {"sin(2*x)^3/x","0","-infinity","-pi/4"},
    {"1/(exp(2*x)+exp(-2*x)+2)","-infinity","+infinity","1/2"},
    {"x^2/(exp(2*x)+exp(-2*x)+2)","-infinity","+infinity","pi^2/24"},
    {"x^4/(exp(2*x)+exp(-2*x)+2)","-infinity","+infinity","7*pi^4/480"},
    {"x^6/(exp(x)+exp(-x)+2)","-infinity","+infinity","31*pi^6/21"},
    {"x^8/(exp(x)+exp(-x)+2)","-infinity","+infinity","127*pi^8/15"},
    {"x^3/(exp(x)+exp(-x)+2)","-infinity","+infinity","0"},
    {"x^2/(exp(x)+exp(-x)+2)","+infinity","-infinity","-pi^2/3"},
    {"x*sin(x)/(1+cos(x)^2)","0","pi","pi^2/4"},
    {"(3*x+2)*sin(2*x)/(1+cos(2*x)^2)","0","pi/2","3*pi^2/16+pi/2"},
    {"x*sin(x)/(1+cos(x)^2)","pi","0","-pi^2/4"},
    {"x*sin(-x)/(1+cos(-x)^2)","-pi","0","-pi^2/4"},
    {"x*sin(x)/(2+2*cos(x)^2)","0","pi","pi^2/8"}
  };
  for(const definite &t:good){
    gen f(t.f,ctx),lo=gen(t.lo,ctx).eval(1,ctx),hi=gen(t.hi,ctx).eval(1,ctx);
    bool ok=integrate_real_definite(f,x,lo,hi,res,ctx);
    gen expected=gen(t.expected,ctx).eval(1,ctx);
    if(!ok || !is_zero(_simplify(res-expected,ctx))){std::cerr<<"Definite failed "<<f<<" got "<<res<<" expected "<<expected<<" matched "<<ok<<'\n';assert(false);}
  }
  const definite bad[]={
    {"sin(x)^2/x","0","+infinity",0},{"sin(x+1)^3/x","0","+infinity",0},
    {"sin(i*x)^3/x","0","+infinity",0},{"sin(x)^3/x^2","0","+infinity",0},
    {"sin(x)^3/x","1","+infinity",0},{"sin(x)^33/x","0","+infinity",0},
    {"x^2/(exp(x)+exp(-x)-2)","-infinity","+infinity",0},
    {"x^2/(exp(x)+exp(-2*x)+2)","-infinity","+infinity",0},
    {"x^2/(exp(i*x)+exp(-i*x)+2)","-infinity","+infinity",0},
    {"x^2/(exp(x)+exp(-x)+2)","0","+infinity",0},
    {"x^18/(exp(x)+exp(-x)+2)","-infinity","+infinity",0},
    {"x*sin(x)/(1-cos(x)^2)","0","pi",0},{"x^2*sin(x)/(1+cos(x)^2)","0","pi",0},
    {"x*sin(x)/(1+cos(2*x)^2)","0","pi",0},{"x*sin(x)/(1+cos(x)^2)","0","2*pi",0}
  };
  for(const definite &t:bad)assert(!integrate_real_definite(gen(t.f,ctx),x,gen(t.lo,ctx).eval(1,ctx),gen(t.hi,ctx).eval(1,ctx),res,ctx));
  const char *badprimitive[]={"x^4*(1-x^3)^50","x^26*(1-x^3)^50","x^5*(1-x^3)^(-1)","x^5*(a-x^3)^50",
    "(x^2+1)/((x^2+1)*sqrt(x^4+1))","(x^2-1)/((x^2+1)*sqrt(x^4-2*x^2+1))",
    "(x^2+1)/((x^2-1)*sqrt(x^4-3*x^2+1))","1/(sin(x)^4-cos(x)^4)","1/(sin(x)^4+cos(2*x)^4)"};
  for(const char *s:badprimitive)assert(!integrate_compact_primitive(gen(s,ctx).eval(1,ctx),x,res,ctx));
  angle_radian(false,ctx);
  assert(!integrate_compact_primitive(gen("1/(sin(x)^4+cos(x)^4)",ctx),x,res,ctx));
  assert(!integrate_compact_primitive(gen("(x^2-1)/((x^2+1)*sqrt(x^4+1))",ctx),x,res,ctx));
  assert(!integrate_real_definite(gen("sin(x)^3/x",ctx),x,0,plus_inf,res,ctx));
  std::cout<<"PASS: "<<powers<<" exact binomial derivatives, 3 fractional chains, "<<quartics<<" quartic radicals, "<<trigs
           <<" continuous trig primitives, 18 definite identities and 27 rejected cases\n";
}
