// Host-only instrumentation. This executable links the actual yintg.cc.
#include "giacPCH.h"
#include <cassert>
#include <chrono>
#include <cmath>
#include <dlfcn.h>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <sys/resource.h>
static unsigned parser_calls;
namespace giac {
int giac_yyparse(void *p){
  static auto parse=reinterpret_cast<int(*)(void*)>(dlsym(RTLD_NEXT,"_ZN4giac12giac_yyparseEPv"));
  assert(parse);++parser_calls;return parse(p);
}
// The old host Giac ABI lacks this newer vector-field overload. Fail loudly
// if a test accidentally enters it; scalar integration uses the real module.
bool is_potential(const vecteur &,const vecteur &,gen &,GIAC_CONTEXT){
  throw std::runtime_error("Host ABI does not support vector-field integration");
}
}
static int probe_main(int argc,char **argv){
  using namespace giac;
  assert(argc>=2);
  rlimit mem={768*1024*1024,768*1024*1024};setrlimit(RLIMIT_AS,&mem);
  rlimit core={0,0};setrlimit(RLIMIT_CORE,&core);
  context ctx;const context *contextptr=&ctx;
  angle_radian(true,contextptr);
  auto start=std::chrono::steady_clock::now();
  gen input(argv[1],contextptr);
  gen result=input.eval(1,contextptr);
  std::cout<<result.print(contextptr)<<std::endl;
  std::cerr<<"PARSER_CALLS "<<parser_calls<<'\n';
  std::cerr<<"SECONDS "<<std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()<<'\n';
  rusage usage;getrusage(RUSAGE_SELF,&usage);
  std::cerr<<"MAX_RSS_KB "<<usage.ru_maxrss<<'\n';
  if (is_undef(result)) return 3;
  if (contains(result,*at_integrate)) return 2;
  if (argc<4) return 0;
  // Validation happens after measurement. Exact zero is distinguished from
  // sampled agreement; a sampled result is not a symbolic proof.
  gen expected=gen(argv[2],contextptr).eval(1,contextptr);
  gen x(identificateur("x"));
  bool indefinite=std::string(argv[3])=="indefinite";
  if (!indefinite && result==expected){std::cerr<<"CHECK exact\n";return 0;}
  gen actual=indefinite?derive(result,x,contextptr):result;
  gen target=expected,reference=expected;
  if (indefinite){
    assert(argc>=5);
    target=gen(argv[4],contextptr).eval(1,contextptr);
    reference=derive(expected,x,contextptr);
  }
  gen delta=_simplify(actual-target,contextptr);
  if (!indefinite && !is_zero(delta)) delta=_simplify(_evalc(delta,contextptr),contextptr);
  gen reference_delta=indefinite?_simplify(reference-target,contextptr):gen(0);
  if (is_zero(delta) && is_zero(reference_delta)) {std::cerr<<"CHECK exact\n";return 0;}
  if (!indefinite) {
    gen a=evalf_double(actual,1,contextptr),b=evalf_double(target,1,contextptr);
    if (a.type==_DOUBLE_ && b.type==_DOUBLE_ && std::isfinite(a._DOUBLE_val)
        && std::isfinite(b._DOUBLE_val)
        && std::abs(a._DOUBLE_val-b._DOUBLE_val)<=1e-10*(1+std::abs(b._DOUBLE_val))){
      std::cerr<<"CHECK numeric_constant\n";return 0;
    }
    std::cerr<<"CHECK unresolved\n";return 4;
  }
  for (int i=5;i<argc;++i){
    gen point=gen(argv[i],contextptr).eval(1,contextptr);
    gen a=evalf_double(subst(actual,x,point,false,contextptr),1,contextptr);
    gen b=evalf_double(subst(target,x,point,false,contextptr),1,contextptr);
    gen ref=evalf_double(subst(reference,x,point,false,contextptr),1,contextptr);
    if (ref.type!=_DOUBLE_ || b.type!=_DOUBLE_ || !std::isfinite(ref._DOUBLE_val)
        || !std::isfinite(b._DOUBLE_val)
        || std::abs(ref._DOUBLE_val-b._DOUBLE_val)>1e-8*(1+std::abs(b._DOUBLE_val))){
      std::cerr<<"CHECK reference_mismatch at "<<point<<": "<<ref<<" vs "<<b<<'\n';return 6;
    }
    if (a.type!=_DOUBLE_ || b.type!=_DOUBLE_ || !std::isfinite(a._DOUBLE_val)
        || !std::isfinite(b._DOUBLE_val)
        || std::abs(a._DOUBLE_val-b._DOUBLE_val)>1e-8*(1+std::abs(b._DOUBLE_val))){
      std::cerr<<"CHECK mismatch at "<<point<<": "<<a<<" vs "<<b<<'\n';return 5;
    }
  }
  if (argc<8) {std::cerr<<"CHECK insufficient samples\n";return 4;}
  std::cerr<<"CHECK sampled\n";
  return 0;
}

struct probe_arguments {int argc;char **argv;int result;};
static void *run_probe(void *v){
  probe_arguments *p=static_cast<probe_arguments *>(v);
  p->result=probe_main(p->argc,p->argv);
  return 0;
}
int main(int argc,char **argv){
  const char *size=std::getenv("KHICAS_TEST_STACK_KIB");
  if (!size) return probe_main(argc,argv);
  // Limit only the computation stack, after process/library startup. Setting
  // RLIMIT_STACK before exec can crash the loader depending on ASLR instead.
  char *end=0;
  unsigned long kib=std::strtoul(size,&end,10);
  if (!end || *end || kib<16 || kib>8192) return 7;
  pthread_attr_t attr;
  if (pthread_attr_init(&attr)) return 7;
  if (pthread_attr_setstacksize(&attr,kib*1024) ||
      pthread_attr_setguardsize(&attr,4096)) {pthread_attr_destroy(&attr);return 7;}
  probe_arguments p={argc,argv,0};
  pthread_t thread;
  int status=pthread_create(&thread,&attr,run_probe,&p);
  pthread_attr_destroy(&attr);
  if (status || pthread_join(thread,0)) return 7;
  return p.result;
}
