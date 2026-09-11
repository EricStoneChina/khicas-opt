#include "giacPCH.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <pthread.h>
namespace giac {extern unsigned closed_trig_mask_calls;gen closed_trig_baseline(const gen &,GIAC_CONTEXT);}
static giac::gen rawpow(const giac::gen &g,int n){return giac::symbolic(giac::at_pow,giac::makesequence(g,n));}
static giac::gen cancel(const giac::gen &s,const giac::gen &t){using namespace giac;
 const int coefficients[]={1,5,10,10,5,1};gen expanded=0;
 for(int i=0;i<=5;++i)expanded+=coefficients[i]*pow(s,i)*pow(t,5-i);
 return symbolic(at_plus,makesequence(rawpow(s+t,5),-expanded));
}
static void numeric_same(const giac::gen &a,const giac::gen &b,const giac::context *c){using namespace giac;
 gen v=evalf_double(a,1,c),w=evalf_double(b,1,c);
 gen error=evalf_double(abs(v-w,c),1,c),scale=evalf_double(abs(w,c),1,c);
 assert(error.type==_DOUBLE_ && scale.type==_DOUBLE_ && std::isfinite(error._DOUBLE_val));
 assert(error._DOUBLE_val<1e-10*(1+scale._DOUBLE_val));
}
struct arguments{int argc;char **argv;int result;};
static int cases(int argc,char **argv){using namespace giac;context ctx;const context *c=&ctx;angle_radian(true,c);
 unsigned verified=0;
 for(int i=1;i<argc;++i){
  gen input=gen(argv[i],c).eval(1,c);closed_trig_mask_calls=0;
  gen result=_simplify(input,c);
  assert(closed_trig_mask_calls>0 && result.print(c).size()<450);
  assert(is_zero(ratnormal(result-input,c)));numeric_same(result,input,c);++verified;
 }
 for(const char *s:{"sin(pi/48)^2+cos(pi/48)^2-1", "sin(pi/11)^2+cos(pi/11)^2-1", "sin(pi/12)-cos(5*pi/12)"}){
  closed_trig_mask_calls=0;assert(is_zero(_simplify(gen(s,c).eval(1,c),c)));
  assert(!closed_trig_mask_calls);++verified;
 }
 // Some nonspecial complementary angles already stay unevaluated in C6.
 // Retain the exact baseline output; do not relabel sampled agreement exact.
 for(const char *s:{"sin(pi/48)-cos(23*pi/48)", "cos(pi/24)-sin(11*pi/24)", "sin(pi/48)/cos(pi/48)-tan(pi/48)"}){
  gen input=gen(s,c).eval(1,c);closed_trig_mask_calls=0;
  gen low=_simplify(input,c),baseline=closed_trig_baseline(input,c);
  assert(low==baseline && !closed_trig_mask_calls);numeric_same(low,0,c);++verified;
 }

 gen s=sin(cst_pi/48,c),t=cos(cst_pi/48,c);
 gen zero=symbolic(at_prod,makesequence(cancel(s,t),symbolic(at_prod,gen(vecteur(12,s),_SEQ__VECT))));
 assert(taille(zero,513)>128 && taille(zero,513)<513);
 closed_trig_mask_calls=0;assert(is_zero(_simplify(zero,c)));assert(closed_trig_mask_calls>0);++verified;
 gen near=symbolic(at_plus,makesequence(zero,2*s*t));
 closed_trig_mask_calls=0;gen reduced=_simplify(near,c);
 assert(closed_trig_mask_calls>0 && is_zero(ratnormal(reduced-2*s*t,c)));++verified;
 // Large real constant arithmetic may coexist with a complex trig root.
 // The complex atom must not be replaced by a real identifier.
 for(int mode=0;mode<2;++mode){complex_mode(mode,c);
  gen complex_s=sin(gen(1,1)*cst_pi/48,c);
  const gen roots[]={symbolic(at_sqrt,rawpow(complex_s,2)),symbolic(at_sqrt,-rawpow(complex_s,2)),
                     symbolic(at_sqrt,rawpow(-s,2)),symbolic(at_sqrt,-rawpow(-s,2))};
  for(const gen &root:roots){
   gen input=symbolic(at_plus,makesequence(zero,root,complex_s));closed_trig_mask_calls=0;
   gen result=_simplify(input,c);assert(closed_trig_mask_calls>0);
   numeric_same(result,root+complex_s,c);++verified;
  }
 }
 complex_mode(false,c);
 // Variables make this a near miss, not a closed-constant rewrite.
 gen variable(identificateur("x")),with_variable=symbolic(at_plus,makesequence(zero,variable));
 closed_trig_mask_calls=0;assert(_simplify(with_variable,c)==variable);assert(!closed_trig_mask_calls);++verified;
 std::cout<<"PASS: "<<verified<<" extreme/identity/cancellation/complex-root/guard checks\n";return 0;
}
static void *worker(void *p){arguments *a=static_cast<arguments *>(p);a->result=cases(a->argc,a->argv);return 0;}
int main(int argc,char **argv){if(cases(argc,argv))return 1;pthread_attr_t attr;assert(!pthread_attr_init(&attr));
 assert(!pthread_attr_setstacksize(&attr,64*1024));assert(!pthread_attr_setguardsize(&attr,4096));
 arguments a={argc,argv,1};pthread_t thread;assert(!pthread_create(&thread,&attr,worker,&a));
 pthread_attr_destroy(&attr);assert(!pthread_join(thread,0));return a.result;}
