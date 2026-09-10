#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "libbf.h"

typedef struct { size_t size; long double align; } block;
typedef struct { size_t live, peak, calls; int fail; } heap;
static void *allocate(void *opaque, void *ptr, size_t size) {
    heap *h=opaque;
    block *b=ptr?(block *)ptr-1:NULL;
    size_t old=b?b->size:0;
    if (!size) { h->live-=old; free(b); return NULL; }
    ++h->calls;
    if (h->fail) return NULL;
    b=realloc(b,sizeof(*b)+size);
    assert(b);
    b->size=size;
    h->live=h->live-old+size;
    if (h->live>h->peak) h->peak=h->live;
    return b+1;
}

static void same(const bf_t *a,const bf_t *b) {
    assert(a->expn==b->expn);
    assert(a->len==b->len);
    if (a->expn!=BF_EXP_NAN) assert(a->sign==b->sign);
    if (a->len) assert(!memcmp(a->tab,b->tab,a->len*sizeof(limb_t)));
}

static uint64_t rng=UINT64_C(0x532903a78656a9d1);
static uint64_t random64(void) {
    rng^=rng<<13; rng^=rng>>7; rng^=rng<<17; return rng;
}

static void check(bf_context_t *ctx,const bf_t *a,uint64_t bits,int operation,
                  limb_t precision,bf_flags_t flags,int alias) {
    bf_t b,r,expected;
    bf_init(ctx,&b); bf_init(ctx,&r); bf_init(ctx,&expected);
    int64_t signed_value;
    memcpy(&signed_value,&bits,sizeof(bits));
    if (!operation) assert(!bf_set_ui(&b,bits));
    else assert(!bf_set_si(&b,signed_value));
    if (alias) assert(!bf_set(&r,a));
    int ref=operation==2?bf_add(&expected,a,&b,precision,flags):
                         bf_mul(&expected,a,&b,precision,flags);
    const bf_t *input=alias?&r:a;
    int actual=operation==0?bf_mul_ui(&r,input,bits,precision,flags):
               operation==1?bf_mul_si(&r,input,signed_value,precision,flags):
                            bf_add_si(&r,input,signed_value,precision,flags);
    assert(actual==ref);
    same(&r,&expected);
    bf_delete(&b); bf_delete(&r); bf_delete(&expected);
}

static void benchmark(bf_context_t *ctx,heap *h) {
    bf_t a,r,b;
    bf_init(ctx,&a); bf_init(ctx,&r); bf_init(ctx,&b);
    assert(!bf_set_ui(&a,123456789));
    const int count=200000;
    size_t allocation_counts[2];
    for (int mode=0;mode<2;++mode) {
        size_t calls=h->calls;
        clock_t start=clock();
        for (int i=0;i<count;++i) {
            if (!mode) {
                assert(!bf_set_ui(&b,(unsigned)i+1));
                assert(!bf_mul(&r,&a,&b,BF_PREC_INF,BF_RNDZ));
                bf_delete(&b);
                bf_init(ctx,&b);
            } else assert(!bf_mul_ui(&r,&a,(unsigned)i+1,BF_PREC_INF,BF_RNDZ));
        }
        allocation_counts[mode]=h->calls-calls;
        printf("%s: %.6fs, %zu allocation calls (%d operations)\n",
               mode?"stack operand":"heap operand",(double)(clock()-start)/CLOCKS_PER_SEC,
               h->calls-calls,count);
    }
    assert(allocation_counts[0]>=allocation_counts[1]+count-1);
    bf_delete(&a); bf_delete(&r); bf_delete(&b);
}

int main(void) {
    heap h={0}; bf_context_t ctx; bf_t a,b;
    bf_context_init(&ctx,allocate,&h); bf_init(&ctx,&a); bf_init(&ctx,&b);
    const uint64_t values[]={0,1,2,UINT32_MAX,UINT64_C(0x100000000),
        UINT64_C(0x8000000000000000),UINT64_MAX,INT64_MAX,
        UINT64_C(0xffffffff00000001),UINT64_C(0x8000000080000001)};
    const limb_t precisions[]={2,24,53,64,113,BF_PREC_INF};
    unsigned checks=0;
    // Public constructor boundaries, checked against independent hi/lo assembly.
    for (unsigned i=0;i<sizeof(values)/sizeof(*values);++i) {
        assert(!bf_set_ui(&a,values[i]>>32));
        assert(!bf_mul_2exp(&a,32,BF_PREC_INF,BF_RNDZ));
        assert(!bf_add_si(&a,&a,(uint32_t)values[i],BF_PREC_INF,BF_RNDZ));
        assert(!bf_set_ui(&b,values[i])); assert(bf_cmp_eq(&a,&b));
    }
    for (int k=0;k<36;++k) {
        if (k<2) bf_set_zero(&a,k);
        else if (k<4) bf_set_inf(&a,k-2);
        else if (k==4) bf_set_nan(&a);
        else {
            assert(!bf_set_ui(&a,random64())); a.sign=k%2;
            assert(!bf_mul_2exp(&a,k-18,BF_PREC_INF,BF_RNDZ));
        }
        for (unsigned i=0;i<sizeof(values)/sizeof(*values)+4;++i) {
            uint64_t bits=i<sizeof(values)/sizeof(*values)?values[i]:random64();
            for (int op=0;op<3;++op)
                for (unsigned p=0;p<sizeof(precisions)/sizeof(*precisions);++p)
                    for (int rnd=BF_RNDN;rnd<=BF_RNDF;++rnd)
                        for (int alias=0;alias<2;++alias) {
                            check(&ctx,&a,bits,op,precisions[p],rnd,alias); ++checks;
                        }
        }
    }
    // Fail result allocation, including r==a. No stack pointer may reach realloc/free.
    for (int op=0;op<3;++op) for (int alias=0;alias<2;++alias) {
        bf_delete(&b); bf_init(&ctx,&b); assert(!bf_set_ui(&a,123));
        h.fail=1;
        int status=op==0?bf_mul_ui(alias?&a:&b,&a,7,BF_PREC_INF,BF_RNDZ):
                   op==1?bf_mul_si(alias?&a:&b,&a,-7,BF_PREC_INF,BF_RNDZ):
                         bf_add_si(alias?&a:&b,&a,-7,BF_PREC_INF,BF_RNDZ);
        h.fail=0; assert(status & BF_ST_MEM_ERROR);
    }
    bf_delete(&a); bf_delete(&b);
    benchmark(&ctx,&h);
    bf_context_end(&ctx); assert(!h.live);
    printf("PASS: %u arithmetic comparisons, %d-bit limbs, peak tracked heap %zu bytes\n",
           checks,LIMB_BITS,h.peak);
}
