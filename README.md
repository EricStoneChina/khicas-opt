# Khicas Opt

Khicas Opt is a team-maintained KhiCAS project for Casio fx-CG50 and Graph 90+E calculators. It combines upstream KhiCAS with practical kernel, function, build, Help, interface, and release improvements.

The project name is **Khicas Opt**. It is a team project based on KhiCAS.

## What it provides

- A calculator-ready Giac/KhiCAS build for symbolic and numeric mathematics.
- Kernel and function improvements for calculus, algebra, domains, and resource limits.
- A smaller and faster build tuned for the fx-CG50 memory layout.
- Clear command Help and examples for calculator users.
- Repeatable GitHub Actions builds and downloadable release artifacts.
- A project website with installation and usage information.

## Team work

- **Fadouse** works on the kernel and function implementation. This includes symbolic calculation, integrals, derivatives, domains, and resource bounds.
- **The project team** maintains build configuration, command Help, the user interface, the website, CI, releases, and real-device validation.

Both areas are maintained as one Khicas Opt project. Fadouse's work is part of the team project; it is not a separate product edition.

## Supported devices

The main target is the Casio fx-CG50. The build also follows the Graph 90+E KhiCAS layout where the upstream toolchain supports it.

The main calculator package produces:

- `khicas50.g3a` — the main add-in
- `khicas50.ac2` — the second program segment used by the add-in

## Build locally

The supplied toolchain is a Linux binary. Use Linux or WSL2 on Windows.

```sh
chmod +x build.sh
./build.sh
```

The script downloads the required Casio toolchain and libraries, applies `Makefile.opt`, and builds the calculator files under `giacbf/`.

To remove generated objects and start a clean build:

```sh
./build.sh clean
```

The first build downloads about 231 MB of toolchain data. Set `TOOLS_DIR` to choose another toolchain directory and `JOBS` to control parallel build jobs:

```sh
TOOLS_DIR=$HOME/khicas-toolchain JOBS=4 ./build.sh
```

## Continuous builds and releases

GitHub Actions builds changes pushed to `main` and changes opened as pull requests. Each successful build uploads the calculator artifacts for testing. Release packages are published from the repository's GitHub Releases page.

The project website is available at [khicas.kcisec.site](https://khicas.kcisec.site/).

## Repository layout

| Path | Purpose |
| --- | --- |
| `giacbf/` | KhiCAS engine and calculator integration source |
| `Makefile.opt` | Khicas Opt compiler and linker settings |
| `build.sh` | Linux/WSL2 build entry point |
| `bench/` | Calculator benchmark and regression material |
| `docs/` | Design notes, Help work, and release records |
| `patch/` | Small, reviewable source patches |
| `site/` | Source for the project website |
| `.github/workflows/` | Build and Pages automation |

## Upstream and licensing

Khicas Opt is based on [KhiCAS](https://www-fourier.univ-grenoble-alpes.fr/~parisse/casio/khicasio.html) by Bernard Parisse. Upstream and bundled components keep their own license terms. Read the source headers and upstream notices before redistributing modified builds.

## Contributing

Please keep changes focused and explain the user-visible effect. Kernel and function changes should include a small regression case when possible. Build, Help, interface, and website changes should be checked by the relevant GitHub Actions workflow. Test calculator files on an fx-CG50 or a compatible emulator before a release.

