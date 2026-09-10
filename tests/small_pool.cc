#include <cassert>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <random>
#include <vector>

static void exercise(unsigned size,unsigned capacity,unsigned char *base){
  std::vector<void *> live(capacity,nullptr);
  // Saturation preserves the original lowest-free-slot order.
  for (unsigned i=0;i<capacity;++i){
    live[i]=optimized::allocfast(size);
    assert(live[i]==base+i*size);
    std::memset(live[i],i&255,size);
  }
  void *fallback=optimized::allocfast(size);
  assert(fallback && (uintptr_t(fallback)<uintptr_t(base) || uintptr_t(fallback)>=uintptr_t(base)+capacity*size));
  optimized::deletefast(fallback);
  optimized::fail_alloc=true;
  assert(!optimized::allocfast(size));
  optimized::fail_alloc=false;
  std::mt19937 rng(size);
  for (unsigned n=0;n<40000;++n){
    unsigned i=rng()%capacity;
    if (live[i]){
      for(unsigned j=0;j<size;++j) assert(static_cast<unsigned char *>(live[i])[j]==(i&255));
      optimized::deletefast(live[i]);live[i]=nullptr;
    } else {
      unsigned first=0;while(live[first]) ++first;
      live[first]=optimized::allocfast(size);
      assert(live[first]==base+first*size);
      std::memset(live[first],first&255,size);
    }
  }
  for(void *p:live) if(p) optimized::deletefast(p);
  // Full exhaustion must not leave the cursor stuck after freeing slot zero.
  for(unsigned i=0;i<capacity;++i) live[i]=optimized::allocfast(size);
  optimized::deletefast(live[0]);
  assert(optimized::allocfast(size)==live[0]);
  for(void *p:live) optimized::deletefast(p);
}

int main(){
  exercise(16,optimized::ALLOC16,reinterpret_cast<unsigned char *>(optimized::tab16));
  exercise(24,optimized::ALLOC24,reinterpret_cast<unsigned char *>(optimized::tab24));
  exercise(48,ALLOC48,reinterpret_cast<unsigned char *>(optimized::tab48));
#ifdef ALLOC32
  exercise(32,ALLOC32,reinterpret_cast<unsigned char *>(optimized::tab32));
#endif
  // Sustained near-full symbolic pool: count bitmap words inspected, not timing.
  std::vector<void*> a,b;
  for(unsigned i=0;i<baseline::ALLOC16-1;++i){
    a.push_back(baseline::allocfast(16));b.push_back(optimized::allocfast(16));
  }
  baseline::probes=optimized::probes=0;
  for(unsigned i=0;i<100000;++i){
    void *x=baseline::allocfast(16),*y=optimized::allocfast(16);
    baseline::deletefast(x);optimized::deletefast(y);
  }
  assert(optimized::probes*8<baseline::probes);
  std::cout<<"PASS: pool allocation/reuse/exhaustion; bitmap probes "<<baseline::probes<<" -> "<<optimized::probes<<" per 100000 near-full allocations\n";
  for(void *p:a) baseline::deletefast(p);
  for(void *p:b) optimized::deletefast(p);
  optimized::deletefast(nullptr);
  for(unsigned n: {1u,8u,32u,128u}){
    void *p=optimized::allocfast(n);assert(p);optimized::deletefast(p);
  }
}
