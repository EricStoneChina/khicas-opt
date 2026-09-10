// KhiCAS equation conversions. GPL-2.0-or-later, like the Giac engine.
#include "giacPCH.h"
#include "usual.h"
#include "subst.h"
#include "solve.h"
#include "sym2poly.h"
#include "series.h"
#include "rpn.h"
#include "derive.h"
#include "lin.h"

namespace giac {

gen _param2cart(const gen &args,GIAC_CONTEXT);

static bool pair(const gen &g) {
  return g.type==_VECT && g._VECTptr->size()==2;
}

static bool variables(const gen &g) {
  return pair(g) && g._VECTptr->front().type==_IDNT &&
    g._VECTptr->back().type==_IDNT &&
    g._VECTptr->front()!=g._VECTptr->back();
}

static bool depends(const gen &g,const gen &v) {
  return !lvarx(g,v).empty();
}

static bool disjoint(const gen &a,const gen &b) {
  const vecteur &v=*a._VECTptr;
  if (b.type==_IDNT)
    return v[0]!=b && v[1]!=b;
  const vecteur &w=*b._VECTptr;
  return v[0]!=w[0] && v[0]!=w[1] && v[1]!=w[0] && v[1]!=w[1];
}

static gen residual(const gen &g) {
  if (g.is_symb_of_sommet(at_equal) && pair(g._SYMBptr->feuille)) {
    const vecteur &v=*g._SYMBptr->feuille._VECTptr;
    return v[0]-v[1];
  }
  return g; // A scalar expression denotes g=0.
}

static bool scalar(const gen &g) {
  return g.type!=_VECT && g.type!=_STRNG && !is_undef(g);
}

static gen equation(const gen &g,GIAC_CONTEXT) {
  if (is_undef(g)) return g;
  gen n=normal(g,contextptr);
  return is_undef(n)?n:symb_equal(n,0);
}

// Substitution is simultaneous, so one coordinate cannot capture the other.
gen _cart2polar(const gen &args,GIAC_CONTEXT) {
  if (args.type==_STRNG && args.subtype==-1) return args;
  if (args.type!=_VECT || args._VECTptr->size()!=3)
    return gensizeerr("cart2polar(eq,[x,y],[r,theta])");
  const vecteur &v=*args._VECTptr;
  if (!variables(v[1]) || !variables(v[2]) || !disjoint(v[1],v[2]) || !scalar(v[0]))
    return gensizeerr("Use distinct unassigned coordinate names and a scalar equation");
  if (!angle_radian(contextptr)) return gensizeerr("Equation conversion requires radians");
  const vecteur &w=*v[2]._VECTptr;
  if (depends(v[0],w[0]) || depends(v[0],w[1]))
    return gensizeerr("Output coordinates already occur in input");
  gen converted=subst(residual(v[0]),*v[1]._VECTptr,
    makevecteur(w[0]*cos(w[1],contextptr),w[0]*sin(w[1],contextptr)),false,contextptr);
  // Product-to-sum identities preserve sin/cos domains. The general simplify
  // command may introduce tan(theta/2), which is undefined at odd multiples of pi.
  return equation(_tlin(converted,contextptr),contextptr);
}

gen _polar2cart(const gen &args,GIAC_CONTEXT) {
  if (args.type==_STRNG && args.subtype==-1) return args;
  if (args.type!=_VECT || args._VECTptr->size()!=3)
    return gensizeerr("polar2cart(eq,[r,theta],[x,y])");
  const vecteur &v=*args._VECTptr;
  if (!variables(v[1]) || !variables(v[2]) || !disjoint(v[1],v[2]) || !scalar(v[0]))
    return gensizeerr("Use distinct unassigned coordinate names and a scalar equation");
  if (!angle_radian(contextptr)) return gensizeerr("Equation conversion requires radians");
  const vecteur &w=*v[2]._VECTptr;
  if (depends(v[0],w[0]) || depends(v[0],w[1]))
    return gensizeerr("Output coordinates already occur in input");
  // Canonical chart: r>=0, principal argument, origin checked separately.
  // Do not square this relation: that would introduce extra branches.
  return equation(subst(residual(v[0]),*v[1]._VECTptr,
    makevecteur(sqrt(w[0]*w[0]+w[1]*w[1],contextptr),
                arg(w[0]+cst_i*w[1],contextptr)),false,contextptr),contextptr);
}

// Return all branches as [[X(t),Y(t)],...], suitable for plotparam.
gen _cart2param(const gen &args,GIAC_CONTEXT) {
  if (args.type==_STRNG && args.subtype==-1) return args;
  if (args.type!=_VECT || args._VECTptr->size()!=3)
    return gensizeerr("cart2param(eq,[x,y],t)");
  const vecteur &v=*args._VECTptr;
  if (!variables(v[1]) || v[2].type!=_IDNT || !disjoint(v[1],v[2]) || !scalar(v[0]))
    return gensizeerr("Use distinct unassigned coordinate and parameter names");
  if (depends(v[0],v[2])) return gensizeerr("Parameter already occurs in input");
  const vecteur &xy=*v[1]._VECTptr;
  gen f=residual(v[0]);
  if (!depends(f,xy[0]) && !depends(f,xy[1]))
    return gensizeerr("Equation does not define a curve in these coordinates");
  // An explicit graph needs only substitution, no equation solver.
  if (v[0].is_symb_of_sommet(at_equal) && pair(v[0]._SYMBptr->feuille)) {
    const vecteur &e=*v[0]._SYMBptr->feuille._VECTptr;
    for (int side=0;side<2;++side)
      for (int i=0;i<2;++i)
        if (e[side]==xy[i] && !depends(e[1-side],xy[i])) {
          gen value=subst(e[1-side],xy[1-i],v[2],false,contextptr);
          return vecteur(1,i?makevecteur(v[2],value):makevecteur(value,v[2]));
        }
  }
  // Split reducible curves before isolation, so x*y=0 retains the vertical
  // component x=0 as well as y=0. Multiplicities do not create new branches.
  gen factored=_factor(f,contextptr);
  if (is_undef(factored)) return factored;
  vecteur factors;
  if (factored.is_symb_of_sommet(at_prod) && factored._SYMBptr->feuille.type==_VECT)
    factors=*factored._SYMBptr->feuille._VECTptr;
  else factors.push_back(factored);
  vecteur components;
  for (unsigned i=0;i<factors.size();++i) {
    gen part=factors[i];
    if (part.is_symb_of_sommet(at_pow) && pair(part._SYMBptr->feuille)) {
      const vecteur &power=*part._SYMBptr->feuille._VECTptr;
      if (power[1].type==_INT_) {
        if (power[1].val<=0) continue; // denominator exclusions stay with the input
        part=power[0];
      }
    }
    if (depends(part,xy[0]) || depends(part,xy[1])) components.push_back(part);
  }
  if (components.size()>1) {
    vecteur out;
    for (unsigned i=0;i<components.size();++i) {
      gen branches=_cart2param(makesequence(symb_equal(components[i],0),v[1],v[2]),contextptr);
      if (is_undef(branches) || branches.type!=_VECT) return branches;
      const vecteur &b=*branches._VECTptr;
      out.reserve(out.size()+b.size());
      for (unsigned j=0;j<b.size();++j) out.push_back(b[j]);
    }
    return out;
  }
  if (components.size()==1) f=components[0];
  int solved=depends(f,xy[1])?1:0;
  gen roots=_solve(makesequence(symb_equal(f,0),xy[solved]),contextptr);
  if (is_undef(roots)) return roots;
  if (roots.type!=_VECT || roots._VECTptr->empty())
    return gensizeerr("Cannot isolate this curve; try another coordinate or parametrization");
  vecteur out;
  out.reserve(roots._VECTptr->size());
  for (unsigned i=0;i<roots._VECTptr->size();++i) {
    gen value=(*roots._VECTptr)[i];
    if (!scalar(value) || depends(value,xy[solved]) || value.is_symb_of_sommet(at_solve))
      return gensizeerr("Unresolved solution branch");
    value=subst(value,xy[1-solved],v[2],false,contextptr);
    out.push_back(solved?makevecteur(v[2],value):makevecteur(value,v[2]));
  }
  return out;
}

gen _polar2param(const gen &args,GIAC_CONTEXT) {
  if (args.type==_STRNG && args.subtype==-1) return args;
  if (args.type!=_VECT || args._VECTptr->size()!=3)
    return gensizeerr("polar2param(eq,[r,theta],t)");
  const vecteur &v=*args._VECTptr;
  if (!variables(v[1]) || v[2].type!=_IDNT || !disjoint(v[1],v[2]) || !scalar(v[0]))
    return gensizeerr("Use distinct unassigned coordinate and parameter names");
  if (!angle_radian(contextptr)) return gensizeerr("Equation conversion requires radians");
  // Reuse the all-branches solver with axes [theta,r]. This also handles rays.
  const vecteur &rt=*v[1]._VECTptr;
  gen branches=_cart2param(makesequence(v[0],makevecteur(rt[1],rt[0]),v[2]),contextptr);
  if (is_undef(branches) || branches.type!=_VECT) return branches;
  vecteur out;
  out.reserve(branches._VECTptr->size());
  for (unsigned i=0;i<branches._VECTptr->size();++i) {
    const vecteur &b=*(*branches._VECTptr)[i]._VECTptr;
    out.push_back(makevecteur(b[1]*cos(b[0],contextptr),b[1]*sin(b[0],contextptr)));
  }
  return out;
}

gen _param2polar(const gen &args,GIAC_CONTEXT) {
  if (args.type==_STRNG && args.subtype==-1) return args;
  if (args.type!=_VECT || (args._VECTptr->size()!=2 && args._VECTptr->size()!=3))
    return gensizeerr("param2polar([X(t),Y(t)],t,[r,theta]); omit [r,theta] for a polar pair");
  const vecteur &v=*args._VECTptr;
  if (!pair(v[0]) || v[1].type!=_IDNT)
    return gensizeerr("Expected two coordinate expressions and an unassigned parameter");
  if (!angle_radian(contextptr)) return gensizeerr("Equation conversion requires radians");
  const vecteur &p=*v[0]._VECTptr;
  if (!scalar(p[0]) || !scalar(p[1]) || is_equal(p[0]) || is_equal(p[1]))
    return gensizeerr("Use [X(t),Y(t)] expressions, not equations");
  if (v.size()==3) {
    if (!variables(v[2]) || !disjoint(v[2],v[1]))
      return gensizeerr("Use distinct unassigned polar coordinate names");
    const vecteur &rt=*v[2]._VECTptr;
    if (depends(v[0],rt[0]) || depends(v[0],rt[1]))
      return gensizeerr("Output coordinates already occur in input");
    gen xy=makevecteur(gen(identificateur(" khicas_curve_x")),gen(identificateur(" khicas_curve_y")));
    gen cart=_param2cart(makesequence(v[0],v[1],xy),contextptr);
    if (is_undef(cart)) return cart;
    if (cart.type==_VECT) { // A constant parametrization is a point: conjunction.
      vecteur out;
      for (unsigned i=0;i<cart._VECTptr->size();++i)
        out.push_back(_cart2polar(makesequence((*cart._VECTptr)[i],xy,v[2]),contextptr));
      return out;
    }
    return _cart2polar(makesequence(cart,xy,v[2]),contextptr);
  }
  return makevecteur(_simplify(sqrt(p[0]*p[0]+p[1]*p[1],contextptr),contextptr),
                     arg(p[0]+cst_i*p[1],contextptr));
}

static bool rational_in(const gen &g,const gen &t) {
  vecteur vars=lvarx(g,t);
  return vars.empty() || (vars.size()==1 && vars[0]==t);
}

gen _param2cart(const gen &args,GIAC_CONTEXT) {
  if (args.type==_STRNG && args.subtype==-1) return args;
  if (args.type!=_VECT || args._VECTptr->size()!=3)
    return gensizeerr("param2cart([X(t),Y(t)],t,[x,y])");
  const vecteur &v=*args._VECTptr;
  if (!pair(v[0]) || v[1].type!=_IDNT || !variables(v[2]) || !disjoint(v[2],v[1]))
    return gensizeerr("Use two expressions and distinct unassigned variable names");
  const vecteur &p=*v[0]._VECTptr, &xy=*v[2]._VECTptr;
  if (!scalar(p[0]) || !scalar(p[1]) || is_equal(p[0]) || is_equal(p[1]) ||
      depends(v[0],xy[0]) || depends(v[0],xy[1]))
    return gensizeerr("Use [X(t),Y(t)]; output coordinates must not occur in input");
  for (int i=0;i<2;++i)
    if (p[i]==v[1])
      return symb_equal(xy[1-i],subst(p[1-i],v[1],xy[i],false,contextptr));
  if (!depends(p[0],v[1]) && !depends(p[1],v[1]))
    return makevecteur(symb_equal(xy[0],p[0]),symb_equal(xy[1],p[1]));
  gen a=p[0],b=p[1],t=v[1];
  if (!rational_in(a,t) || !rational_in(b,t)) {
    if (!angle_radian(contextptr)) return gensizeerr("Trigonometric conversion requires radians");
    // Rationalize sin(t),cos(t) with the tangent half-angle substitution.
    // Reject residual transcendental dependence instead of returning a bogus resultant.
    gen u(identificateur(" khicas_curve_u"));
    if (depends(v[0],u)) return gensizeerr("Internal parameter collision");
    gen half=tan(t/2,contextptr);
    a=subst(_halftan(a,contextptr),half,u,false,contextptr);
    b=subst(_halftan(b,contextptr),half,u,false,contextptr);
    if (depends(a,t) || depends(b,t) || !rational_in(a,u) || !rational_in(b,u))
      return gensizeerr("Cannot eliminate this parametrization; keep the parametric form");
    t=u;
  }
  gen f=_numer(normal(a-xy[0],contextptr),contextptr);
  gen g=_numer(normal(b-xy[1],contextptr),contextptr);
  gen result=_resultant(makesequence(f,g,t),contextptr);
  if (is_undef(result)) return result;
  if (depends(result,t) || is_zero(result))
    return gensizeerr("Elimination was inconclusive");
  *logptr(contextptr) << "Algebraic closure: retain parameter range and denominator exclusions.\n";
  return equation(result,contextptr);
}

#define CURVE_COMMAND(name) \
  static const char name##_s[]=#name; \
  static define_unary_function_eval(__##name,&_##name,name##_s); \
  define_unary_function_ptr5(at_##name,alias_at_##name,&__##name,0,true)
CURVE_COMMAND(cart2param);
CURVE_COMMAND(cart2polar);
CURVE_COMMAND(param2cart);
CURVE_COMMAND(param2polar);
CURVE_COMMAND(polar2cart);
CURVE_COMMAND(polar2param);
#undef CURVE_COMMAND
}
