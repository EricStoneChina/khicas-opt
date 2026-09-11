# KhiCAS 2026a

KhiCAS is an open-source symbolic mathematics and graphing add-in for the Casio fx-CG50 family. This branch follows Fadouse's `develop/khicas-official` line and carries the 2026a calculus, equation, simplification, and plotting work on top of the official KhiCAS source.

The distribution is English-only and uses one matching pair:

- `khicas50.g3a` — the calculator application
- `khicas50.ac2` — the matching application data image

Keep both files from the same build together when installing them.

## Capabilities

KhiCAS provides exact algebra, calculus, equations, matrices, statistics, geometry, graphing, and special functions on the fx-CG50. The 2026a line focuses on bounded symbolic simplification, robust integration and derivative domains, parametric and polar equation workflows, and practical resource use on calculator hardware.

## Build

The CI build uses Linux, Python 3.12 or newer, a host C++ compiler, the Casio SH4 toolchain, and `mkg3a`.

```bash
TOOLS_DIR=/tmp/khicas-toolchain python3 tools/build.py optimized
```

Optimized files are written to `.build/optimized/`. The `official` target reproduces the upstream snapshot:

```bash
TOOLS_DIR=/tmp/khicas-toolchain python3 tools/build.py official
```

## Install

Download `khicas50.g3a` and `khicas50.ac2` from the [latest release](https://github.com/KCISEastCampus/khicas-opt/releases), connect the fx-CG50 over USB, and copy both files to the calculator storage root. Launch KhiCAS from the main menu. If loading fails, verify that both files came from one release.

The project guide is available at [khicas.kcisec.site](https://khicas.kcisec.site/).

## Verification

```bash
python3 tests/run-native.py
python3 tests/check-registration.py
python3 tests/run-symbolic.py
python3 tests/run-integration.py --report /tmp/integration-report.json
python3 tests/run-calculus.py --compare --report /tmp/calculus-report.json
```

Host-side tests provide regression coverage; calculator timing still needs to be measured on hardware with the scripts in `bench/`.

KhiCAS is distributed under the GNU General Public License v2. Contributions and issue reports are welcome at [github.com/KCISEastCampus/khicas-opt](https://github.com/KCISEastCampus/khicas-opt).
