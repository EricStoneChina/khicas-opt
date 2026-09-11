#include "giacPCH.h"
#include <cassert>
#include <iostream>
using namespace giac;

int main() {
  context ctx;
  unsigned cases=0;
  for (int rows=1;rows<=24;++rows) for (int cols=1;cols<=25;++cols) {
    matrice a;
    for (int i=0;i<rows;++i) {
      vecteur row;
      for (int j=0;j<cols;++j)
        row.push_back(gen(100*i+j)/gen(7)); // exact rationals
      a.push_back(row);
    }
    gen saved(a);
    for (int width=0;width<=rows+2;++width) {
      matrice result;
      mtran(a,result,width,false);
      int n=width?width:rows;
      assert(result.size()==unsigned(cols));
      for (int j=0;j<cols;++j) {
        const vecteur &r=*result[j]._VECTptr;
        assert(r.size()==unsigned(n));
        for (int i=0;i<n;++i)
          assert(r[i]==(i<rows?(*a[i]._VECTptr)[j]:gen(0)));
      }
      assert(gen(a)==saved); ++cases;
    }
    matrice in_place(a),result;
    mtran(in_place,in_place,0,false);
    mtran(in_place,result,0,false);
    assert(gen(result)==saved);
    gen b=_tran(saved,&ctx);
    assert(b.subtype==_MATRIX__VECT && *b._VECTptr==in_place);
    assert(saved==gen(a)); ++cases;
  }
  gen row=gen("[1,2,3]",&ctx).eval(1,&ctx);
  gen result=_tran(row,&ctx);
  assert(result==gen("[[1],[2],[3]]",&ctx).eval(1,&ctx));
  // Symbols and nested polynomial coefficients keep their values and subtypes.
  gen mixed=gen("[[x,1/3],[sqrt(2),y]]",&ctx).eval(1,&ctx);
  assert(_tran(_tran(mixed,&ctx),&ctx)==mixed);
  std::cout << "PASS: " << cases+2 << " transpose cases\n";
}
