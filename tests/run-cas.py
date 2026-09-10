#!/usr/bin/env python3
"""Check transpose, integration and conversions using host libgiac-dev."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def function(text, signature):
    start = text.index(signature)
    end = text.index("{", start) + 1
    depth = 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end] + "\n"


with tempfile.TemporaryDirectory(prefix="khicas-cas-") as tmp:
    work = Path(tmp)
    flags = [os.environ.get("CXX", "c++"), "-std=c++11", "-O2", "-DHAVE_CONFIG_H",
             "-DGIAC_GENERIC_CONSTANTS", "-Wno-deprecated-declarations", "-I",
             os.environ.get("GIAC_INCLUDE", "/usr/include/giac")]
    flags += shlex.split(os.environ.get("CXXFLAGS", ""))
    libs = shlex.split(os.environ.get("LDFLAGS", "")) + ["-lgiac"]
    content = (ROOT / "zvecteur.cc").read_text()
    transpose = work / "transpose.cc"
    transpose.write_text('#include "giacPCH.h"\nnamespace giac {\n' +
        function(content, "  void mtran(const matrice & a,matrice & res,int ncolres,bool ckundef)") +
        function(content, "  gen _tran(const gen & a,GIAC_CONTEXT)") + "}\n")
    output = work / "transpose"
    subprocess.run(flags + [str(transpose), str(ROOT / "tests/matrix_transpose.cc")] +
                   libs + ["-o", str(output)], check=True)
    subprocess.run([str(output)], check=True)
    output = work / "integration"
    subprocess.run(flags + [str(ROOT / "tests/integration.cc")] + libs +
                   ["-o", str(output)], check=True)
    subprocess.run([str(output), str(ROOT / "bench/bench.xws"),
                    str(ROOT / "bench/integration.xws")], check=True)
    output = work / "conversions"
    # Resolve quoted headers against host Giac, not the calculator-only headers.
    conversion = work / "kconvert.cc"
    conversion.write_bytes((ROOT / "kconvert.cc").read_bytes())
    subprocess.run(flags + [str(conversion),
        str(ROOT / "tests/equation_conversions.cc")] + libs + ["-o", str(output)], check=True)
    subprocess.run([str(output), str(ROOT / "bench/conversions.xws")], check=True)
