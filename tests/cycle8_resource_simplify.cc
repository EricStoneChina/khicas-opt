#include "giacPCH.h"
#include <cassert>
#include <iostream>
#include <pthread.h>
static int cases(){using namespace giac;context ctx;const context *c=&ctx;
 const char *compact[]={"(-(1+(x^2+x)^32)^1025/1025+(1+(x^2+x)^32)^1026/1026)/32","(x+i)^2048+(x-i)^2048","(x^2-2*x-1)^8192/3"};
 for(const char *s:compact){gen e=gen(s,c).eval(1,c),r=_simplify(e,c);assert(r==e);}
 const char *zero[]={"(x+1)^2048-(x+1)^2048","(x-x)^2048","(x+1)^2-x^2-2*x-1","((x+1)^2048-(x+1)^2048)/(x-2)","sin(x)^2+cos(x)^2-1"};
 for(const char *s:zero)assert(is_zero(_simplify(gen(s,c).eval(1,c),c)));
 for(int mode=0;mode<2;++mode){complex_mode(mode,c);
 assert(is_zero(_simplify(gen("sqrt(-4)+sqrt(-9)-5*i",c).eval(1,c),c)));
 assert(is_zero(_simplify(gen("sqrt((1+i)^2)-(1+i)",c).eval(1,c),c)));
 }
 std::cout<<"PASS: 3 compact large-polynomial forms, 5 cancellation/low-degree identities, 4 complex-root checks\n";return 0;
}
static void *worker(void *p){*static_cast<int *>(p)=cases();return 0;}
int main(){if(cases())return 1;pthread_attr_t a;assert(!pthread_attr_init(&a));assert(!pthread_attr_setstacksize(&a,65536));assert(!pthread_attr_setguardsize(&a,4096));pthread_t t;int r=1;assert(!pthread_create(&t,&a,worker,&r));pthread_attr_destroy(&a);assert(!pthread_join(t,0));return r;}
