#!/usr/bin/env python3
"""Exercise actual FXCG simplification without algebraic extension blow-up."""
from pathlib import Path
import argparse, os, shlex, subprocess, tempfile
from integration_build import ROOT, function
parser=argparse.ArgumentParser()
parser.add_argument('--replacement',type=Path,help='Read pending replacement simplify()')
args=parser.parse_args()
s=(ROOT/'ksubst.cc').read_text()
text='#include "giacPCH.h"\n#define FXCG\n#define NO_STDEXCEPT\nnamespace giac {\n'
text+='gen ataninv2atan(const gen &,GIAC_CONTEXT);\ngen cklin(const gen &,GIAC_CONTEXT);\n'
text+=function((ROOT/'zprog.cc').read_text(),'  gen symb_prog3(')
text+=function(s,'  gen tsimplify_noexpln(')
if args.replacement:
    text+=args.replacement.read_text()
else:
    if '  static unsigned simplify_special_terms(' in s:
        text+=function(s,'  static unsigned simplify_special_terms(')
        text+=function(s,'  static gen simplify_special_core(')
    text+=function(s,'  gen simplify(const gen & e_orig,GIAC_CONTEXT)')
text+=function(s,'  gen _simplify(')+'}\n'
norm='#include "giacPCH.h"\nnamespace giac {\n'
s=(ROOT/'ysym2poly.cc').read_text()
for sig in ('  static bool sort_func(', '  static vecteur sort1(', '  gen ratnormal(const gen & e,GIAC_CONTEXT)', '  gen recursive_ratnormal(const gen & e,GIAC_CONTEXT)'):
    norm+=function(s,sig)
norm+='}\n'
with tempfile.TemporaryDirectory(prefix='khicas-simplify-nested-') as tmp:
    p=Path(tmp);(p/'simplify.cc').write_text(text);(p/'normalize.cc').write_text(norm)
    flags=[os.environ.get('CXX','c++'),'-std=c++11','-O1','-g','-DHAVE_CONFIG_H','-DGIAC_GENERIC_CONSTANTS','-Wno-deprecated-declarations','-I',os.environ.get('GIAC_INCLUDE','/usr/include/giac')]+shlex.split(os.environ.get('CXXFLAGS',''))
    libs=shlex.split(os.environ.get('LDFLAGS',''))+['-lgiac']
    for test in ('simplify_nested_powers.cc','simplify_special_functions.cc'):
        subprocess.run(flags+[str(p/'simplify.cc'),str(p/'normalize.cc'),str(ROOT/'tests'/test)]+libs+['-o',str(p/'test')],check=True)
        subprocess.run([str(p/'test')],check=True,timeout=60)
