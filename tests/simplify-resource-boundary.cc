#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
using namespace giac;
struct work {gen input,result;context ctx;int mode,step;};
static void *compute(void *arg){
  work &w=*static_cast<work *>(arg);
  calc_mode(w.mode,&w.ctx);step_infolevel(w.step,&w.ctx);
  w.result=_simplify(w.input,&w.ctx);
  assert(calc_mode(&w.ctx)==w.mode && step_infolevel(&w.ctx)==w.step);
  return 0;
}
static void check(work &w,const gen &expected,bool small_stack){
  if(small_stack){
    pthread_attr_t attr;assert(!pthread_attr_init(&attr));
    assert(!pthread_attr_setstacksize(&attr,65536));assert(!pthread_attr_setguardsize(&attr,4096));
    pthread_t thread;assert(!pthread_create(&thread,&attr,compute,&w));
    pthread_attr_destroy(&attr);assert(!pthread_join(thread,0));
  }else compute(&w);
  // Compare outside the worker: raw 512-level construction/printing/destruction
  // are deliberately excluded when isolating the simplification call stack.
  assert(w.result==expected);
}
int main(){
  work w;gen x=identificateur("x");unsigned count=0;
  for(int stack=0;stack<2;++stack){
    w.mode=stack?38:0;w.step=2;
    for(unsigned n: {6u,14u,15u,16u,24u,33u,64u,128u,256u,512u}){
      for(unsigned kind=0;kind<3;++kind){
        gen input=symbolic(at_plus,makevecteur(x,x)),expected=2*x;
        for(unsigned j=0;j<n;++j){
          if(kind==2){input=gen(vecteur(1,input));expected=gen(vecteur(1,expected));}
          else {input=symbolic(kind?at_erf:at_sin,input);expected=symbolic(kind?at_erf:at_sin,expected);}
        }
        w.input=input;
        if(n>64)expected=input; // Beyond the bounded local rewrite, preserve syntax.
        std::cerr<<"CASE "<<stack<<" "<<n<<" "<<kind<<"\n";check(w,expected,stack);++count;
      }
    }
    // A refusal must not poison later ordinary work or calculator modes.
    for(const char *s: {"x+x-2*x","sin(x)^2+cos(x)^2-1","sqrt(-4)+sqrt(-9)-5*i"}){
      w.input=gen(s,&w.ctx).eval(1,&w.ctx);check(w,0,stack);++count;
    }
  }
  std::cout<<"PASS "<<count<<" isolated normal/64 KiB calls; raw nesting, simple leaf algebra, mode restoration and recovery\n";
}
