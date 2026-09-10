#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
static int cases(){using namespace giac;context ctx;const context *c=&ctx;angle_radian(true,c);
 const char *inputs[]={
  "sin(100*x)*cos(x)^100/100", "sin(100*x)*sin(x)^100/100",
  "pi*sin(100*x)*cos(x)^100/100", "sin(32*x)*cos(x)^32",
  "sin(100*x+i)*cos(x+i)^100/100", "sin(x^2)*cos(ln(x))^100"
 };
 for(int mode=0;mode<2;++mode){complex_mode(mode,c);
  for(const char *s:inputs){
   gen input=gen(s,c).eval(1,c),actual=_simplify(input,c);
   if(actual.print(c).size()>=100){std::cerr<<s<<" expanded to "<<actual.print(c).size()<<" characters\n";return 1;}
   assert(is_zero(ratnormal(actual-input,c)));
  }
  assert(is_one(_simplify(gen("sin(x)^2+cos(x)^2",c),c)));
 }
 std::cout<<"PASS: 12 compact real/complex trig products, ordinary Pythagorean identity\n";return 0;
}
static void *worker(void *p){*static_cast<int *>(p)=cases();return 0;}
int main(){if(cases())return 1;pthread_attr_t attr;assert(!pthread_attr_init(&attr));
 assert(!pthread_attr_setstacksize(&attr,64*1024));assert(!pthread_attr_setguardsize(&attr,4096));
 int result=1;pthread_t thread;assert(!pthread_create(&thread,&attr,worker,&result));
 pthread_attr_destroy(&attr);assert(!pthread_join(thread,0));return result;}
