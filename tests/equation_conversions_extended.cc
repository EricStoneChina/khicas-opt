#include "giacPCH.h"
#include <cassert>
#include <cmath>
#include <iostream>
namespace giac {gen _cart2param(const gen &,GIAC_CONTEXT);gen _polar2param(const gen &,GIAC_CONTEXT);bool curve_polynomial_test(const gen &,const gen &,const gen &,GIAC_CONTEXT);}
using namespace giac;
static gen parse(const char *s,GIAC_CONTEXT){return gen(s,contextptr).eval(1,contextptr);}
static gen difference(const gen &g){return is_equal(g)?g._SYMBptr->feuille[0]-g._SYMBptr->feuille[1]:g;}
static void expect_equivalent(const gen &a,const gen &b,GIAC_CONTEXT){gen d=_simplify(a-b,contextptr);if(!is_zero(d)){std::cerr<<"Mismatch "<<a<<" != "<<b<<" : "<<d<<'\n';std::abort();}}
int main(){context c;const context *ctx=&c;angle_radian(true,ctx);gen x(identificateur("x")),y(identificateur("y")),t(identificateur("t"));
const char *cart[]={
"(x^3+y^3-3*x*y=0,[x,y],t)",
"(-27*x^3+81*x*y-27*y^3=0,[x,y],t)",
"(x^3+y^3=6*x*y,[x,y],t)",
"((x^2+y^2)^2=2*(x^2-y^2),[x,y],t)",
"((x^2+y^2)^2=9*(x^2-y^2),[x,y],t)",
"(x+y^5+y=0,[x,y],t)",
"(y^2*x+y^3+1=0,[x,y],t)",
"(x^3+y^3+x^2+y^2=0,[x,y],t)",
"(x^3+x^2+y^2=0,[x,y],t)",
"(x*(x^3+y^3-3*x*y)=0,[x,y],t)",
"((y-1)*(x*y+1)=0,[x,y],t)",
"(x^4+y^4=1,[x,y],t)",
"(x^2=4,[x,y],t)",
"(v^3+u^3=3*u*v,[u,v],s)",
"(x^3+y^3=0,[x,y],t)",
"((x^2+y^2)^2=-2*(x^2-y^2),[x,y],t)",
"((x^2+y^2)^2=0,[x,y],t)",
"((x^2+y^2)^2/4=x^2-y^2,[x,y],t)",
"((x^3+y^3)/2=3*x*y,[x,y],t)"
};
for(unsigned i=0;i<sizeof(cart)/sizeof(*cart);++i){gen args=parse(cart[i],ctx),out=_cart2param(args,ctx);assert(out.type==_VECT && !out._VECTptr->empty());
  gen f=difference(args._VECTptr->front());const vecteur &xy=*(*args._VECTptr)[1]._VECTptr;
  for(unsigned j=0;j<out._VECTptr->size();++j){const gen &branch=(*out._VECTptr)[j];assert(branch.type==_VECT && branch._VECTptr->size()==2);
    gen d=subst(f,xy,*branch._VECTptr,false,ctx);
    if(!lop(d,at_sin).empty() || !lop(d,at_cos).empty())d=_trigsin(d,ctx);
    expect_equivalent(d,0,ctx);
  }
  if(i==0||i==1)expect_equivalent(out,parse("[[3*t/(1+t^3),3*t^2/(1+t^3)]]",ctx),ctx);
  if(i==3||i==4){assert(out._VECTptr->size()==1);assert(taille(out,81)<81);}
  if(i==7){assert(equalposcomp(*out._VECTptr,parse("[0,-1]",ctx)));assert(equalposcomp(*out._VECTptr,parse("[0,0]",ctx)));}
  if(i==8)assert(equalposcomp(*out._VECTptr,parse("[0,0]",ctx)));
  if(i==9)assert(equalposcomp(*out._VECTptr,parse("[0,t]",ctx)));
  if(i==10)assert(equalposcomp(*out._VECTptr,parse("[t,1]",ctx)));
}
// Check the actual division AST, rather than depending on parser canonicalization.
gen quartic=pow(pow(x,2)+pow(y,2),2);
gen divided=symbolic(at_division,makesequence(quartic,4));
assert(curve_polynomial_test(divided,x,y,ctx));
assert(!curve_polynomial_test(symbolic(at_division,makesequence(quartic,x+1)),x,y,ctx));
assert(!curve_polynomial_test(symbolic(at_division,makesequence(quartic,0)),x,y,ctx));
gen raw_equation=symb_equal(divided,pow(x,2)-pow(y,2));
gen raw_result=_cart2param(makesequence(raw_equation,makevecteur(x,y),t),ctx);
for(const gen &branch:*raw_result._VECTptr)expect_equivalent(_trigsin(subst(difference(raw_equation),makevecteur(x,y),*branch._VECTptr,false,ctx),ctx),0,ctx);
// Symbolic folium scale is used only after a nonzero real sign is proved.
parse("assume(a>0)",ctx);
for(const char *expr:{"(x^3+y^3=3*a*x*y,[x,y],t)","((x^2+y^2)^2=4*a^2*(x^2-y^2),[x,y],t)","(a*(x-y)=0,[x,y],t)"}){
 gen args=parse(expr,ctx),out=_cart2param(args,ctx);assert(out.type==_VECT&&out._VECTptr->size()==1);
 for(const gen &branch:*out._VECTptr)expect_equivalent(_trigsin(subst(difference(args._VECTptr->front()),makevecteur(x,y),*branch._VECTptr,false,ctx),ctx),0,ctx);
}
parse("purge(a)",ctx);
const char *polar[]={"(r^3=sin(theta),[r,theta],t)","(r^5=1-2*cos(theta),[r,theta],t)",
"(r^3/2+sin(theta)=0,[r,theta],t)","(r^9=sin(theta),[r,theta],t)",
"(r^3=-8,[r,theta],t)","(r^5=0,[r,theta],t)"};
const char *angles[]={"-pi","-pi/2","-pi/3","0","pi/6","pi/2","2*pi/3","pi"};
for(unsigned i=0;i<sizeof(polar)/sizeof(*polar);++i){gen args=parse(polar[i],ctx),out=_polar2param(args,ctx);assert(out.type==_VECT && out._VECTptr->size()==1);
  const vecteur &rt=*(*args._VECTptr)[1]._VECTptr;gen f=difference(args._VECTptr->front());
  for(unsigned j=0;j<out._VECTptr->size();++j){const vecteur &branch=*(*out._VECTptr)[j]._VECTptr;
    for(const char *angle:angles){gen a=parse(angle,ctx);gen X=subst(branch[0],t,a,false,ctx).eval(1,ctx),Y=subst(branch[1],t,a,false,ctx).eval(1,ctx);
      if(!is_zero(im(X,ctx)) || !is_zero(im(Y,ctx))){std::cerr<<"Nonreal "<<polar[i]<<" at "<<angle<<" X="<<X<<" Y="<<Y<<" branch="<<out<<'\n';return 1;}
      gen radius=X*cos(a,ctx)+Y*sin(a,ctx);
      gen residual=subst(f,rt,makevecteur(radius,a),false,ctx);
      gen error=evalf_double(residual,1,ctx);
      if(error.type!=_DOUBLE_ || !std::isfinite(error._DOUBLE_val) || std::abs(error._DOUBLE_val)>1e-9){std::cerr<<polar[i]<<" at "<<angle<<" residual="<<error<<'\n';return 1;}
    }
  }
}
// Collision and unsupported whole-plane inputs must remain errors.
for(const char *bad:{"(x^3+y^3=3*x*y,[x,y],x)","(x^3+y^3+t=0,[x,y],t)","(0=0,[x,y],t)","(a*(x-y)=0,[x,y],t)","(a*(x*y)=0,[x,y],t)"}){
 bool failed=false;try{failed=is_undef(_cart2param(parse(bad,ctx),ctx));}catch(const std::runtime_error&){failed=true;}assert(failed);
}
std::cout<<"PASS: 23 Cartesian exact branch checks, 48 real polar residual samples, and 8 guard cases\n";
}
