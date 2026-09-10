#!/usr/bin/env python3
"""Exercise bounded real integration rules directly, including rejection paths."""
from pathlib import Path
import os, shlex, subprocess, tempfile
from integration_build import ROOT, function
s=(ROOT/'yintg.cc').read_text()
text='#include "giacPCH.h"\nnamespace giac {\ngen linear_integrate_nostep(const gen &,const gen &,gen &,int,GIAC_CONTEXT);\n'
for sig in ('  void decompose_prod(', '  gen extract_cst(',
            '  static bool integration_rational(', '  static gen integration_syntax(',
            '  static bool integration_power(',
            '  static bool integration_one_plus(', '  static gen integration_coefficient(',
            '  static bool integrate_real_root(', '  static bool integrate_residue_kernel(',
            '  static bool integrate_real_definite('):
    text+=function(s,sig).replace('  static ', '  ',1)
text+='}\n'
with tempfile.TemporaryDirectory(prefix='khicas-real-definite-') as tmp:
    p=Path(tmp);(p/'rules.cc').write_text(text)
    flags=[os.environ.get('CXX','c++'),'-std=c++11','-O2','-DHAVE_CONFIG_H','-DGIAC_GENERIC_CONSTANTS','-Wno-deprecated-declarations','-I',os.environ.get('GIAC_INCLUDE','/usr/include/giac')]+shlex.split(os.environ.get('CXXFLAGS',''))
    libs=shlex.split(os.environ.get('LDFLAGS',''))+['-lgiac']
    subprocess.run(flags+[str(p/'rules.cc'),str(ROOT/'tests/real_definite.cc')]+libs+['-o',str(p/'test')],check=True)
    subprocess.run([str(p/'test')],check=True,timeout=60)
