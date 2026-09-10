// Probe one conversion per process, with process/library startup outside the
// guarded computation stack. No post-conversion symbolic proof runs on it.
#include "giacPCH.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <sys/resource.h>
namespace giac {gen _cart2param(const gen &,GIAC_CONTEXT);gen _cart2polar(const gen &,GIAC_CONTEXT);gen _polar2param(const gen &,GIAC_CONTEXT);}
struct arguments{int argc;char **argv;int result;};
static void *worker(void *raw){using namespace giac;arguments &a=*static_cast<arguments *>(raw);context c;const context *ctx=&c;angle_radian(true,ctx);
 try{
  if(a.argc>3)gen(a.argv[3],ctx).eval(1,ctx);
  gen in=gen(a.argv[2],ctx).eval(1,ctx),out;
  std::string mode=a.argv[1];
  if(mode=="cart")out=_cart2param(in,ctx);
  else if(mode=="polar")out=_polar2param(in,ctx);
  else out=_cart2polar(in,ctx);
  if(std::getenv("KHICAS_OUTER_SIMPLIFY"))out=_simplify(out,ctx);
  std::cout<<out<<'\n';a.result=is_undef(out)?2:0;
 }catch(const std::runtime_error&e){std::cerr<<e.what()<<'\n';a.result=2;}
 return 0;
}
int main(int argc,char **argv){assert(argc>=3);rlimit core={0,0};setrlimit(RLIMIT_CORE,&core);rlimit memory={256*1024*1024,256*1024*1024};setrlimit(RLIMIT_AS,&memory);
 pthread_attr_t attr;assert(!pthread_attr_init(&attr));assert(!pthread_attr_setstacksize(&attr,64*1024));assert(!pthread_attr_setguardsize(&attr,4096));
 pthread_t thread;arguments args={argc,argv,1};assert(!pthread_create(&thread,&attr,worker,&args));pthread_attr_destroy(&attr);assert(!pthread_join(thread,0));return args.result;
}
