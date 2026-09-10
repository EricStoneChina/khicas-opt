"""Build repository integration/normalization against the host Giac ABI."""
import os, shlex, subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BASE = 'checkpoint/equations'

def source(ref, name):
    if ref == 'current':
        return (ROOT / name).read_text()
    return subprocess.check_output(['git', 'show', f'{BASE}:{name}'], cwd=ROOT, text=True)

def function(s, signature):
    start = s.index(signature)
    end = s.index('{', start) + 1
    depth = 1
    while depth:
        depth += (s[end] == '{') - (s[end] == '}')
        end += 1
    return s[start:end] + '\n'

def build(directory, ref='current'):
    directory.mkdir(parents=True, exist_ok=True)
    text = source(ref, 'yintg.cc').replace(
        '  // Left redimension p to degree n, i.e. size n+1',
        '  bool is_potential(const vecteur &,const vecteur &,gen &,GIAC_CONTEXT);\n'
        '  // Left redimension p to degree n, i.e. size n+1')
    (directory / 'yintg.cc').write_text(text)
    (directory / 'zintgab.cc').write_text(source(ref, 'zintgab.cc'))
    if ref == 'current':
        (directory / 'integration_guard.h').write_bytes((ROOT / 'integration_guard.h').read_bytes())
    syms = source(ref, 'ysym2poly.cc')
    normalized = '#include "giacPCH.h"\nnamespace giac {\n'
    for sig in ('  static bool sort_func(', '  static vecteur sort1(',
                '  gen ratnormal(const gen & e,GIAC_CONTEXT)',
                '  gen recursive_ratnormal(const gen & e,GIAC_CONTEXT)'):
        normalized += function(syms, sig)
    (directory / 'normalize.cc').write_text(normalized + '}\n')
    flags = [os.environ.get('CXX', 'c++'), '-std=c++11', '-O1', '-g',
             '-DHAVE_CONFIG_H', '-DGIAC_GENERIC_CONSTANTS', '-Wno-deprecated-declarations',
             '-I', os.environ.get('GIAC_INCLUDE', '/usr/include/giac')]
    flags += shlex.split(os.environ.get('CXXFLAGS', ''))
    libs = shlex.split(os.environ.get('LDFLAGS', '')) + ['-lgiac']
    libs += shlex.split(os.environ.get('GIAC_NUMERIC_LIBS', '-lgmp -lmpfr'))
    exe = directory / 'probe'
    subprocess.run(flags + ['-DKHICAS_TEST_INTEGRATION_LIMITS', str(directory / 'yintg.cc'),
        str(directory / 'zintgab.cc'), str(directory / 'normalize.cc'), str(ROOT / 'tests/integration_probe.cc')]
        + libs + ['-o', str(exe)], check=True, timeout=180)
    return exe
