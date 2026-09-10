# KhiCAS 官方源码开发基线

本 worktree 从作者官网 `giacbf.tgz` 重新建立，源码位于仓库根目录。
官方快照日期为 **2026-07-31**，来源、校验值及导入清单见
[UPSTREAM.md](UPSTREAM.md) 和 [UPSTREAM.json](UPSTREAM.json)。
保留官方图标、界面、帮助和功能；没有带入原项目的汉化、网站或品牌改动。

## 性能 checkpoint

`checkpoint/performance`：大数小整数乘加减少堆分配，矩阵转置减少复制和临时数组；
优化构建优先对积分、符号化简、求解、矩阵模块使用 `-O2 -finline-functions`。
`yintg.cc`（含符号与数值积分）也纳入速度配置，其他模块保持 `-Os`。
没有启用 fast-math、改变时钟或调整算法精度。测试范围和容量见
[docs/PERFORMANCE.md](docs/PERFORMANCE.md)。

## 构建

需要 Linux、Python 3.12+、make、主机 C++ 编译器和作者提供的 SH4 工具链。
工具链目录结构与官网 `casiolocal.tgz` 一致，同时需要 `mkg3a` 打包器。
工具链依赖目录默认 `$HOME/khicas-toolchain`，可以用 `TOOLS_DIR` 或 `--tools-dir` 指定。
本次开发环境已经准备在 `/tmp/khicas-toolchain`，临时目录被清理后需重新准备。

```bash
# 在此 worktree 根目录运行
TOOLS_DIR=/tmp/khicas-toolchain python3 tools/build.py optimized
TOOLS_DIR=/tmp/khicas-toolchain python3 tools/build.py official
```

生成文件分别在 `.build/optimized/`、`.build/official/`。
默认构建官方英文版 `khicas50.g3a` + `khicas50.ac2`。
两文件必须成对使用，不要把官方基线的 `.ac2` 与优化版 `.g3a` 混用。

构建脚本在独立目录复制源码，参数化上游 Makefile 中的路径并移除作者本机的复制操作，
不改写根目录源码或原始 Makefile。`official` 模式从官方基线 Git 引用提取源码并逐文件
校验 SHA-256；`optimized` 模式使用当前根目录源码，输入变化时重新构建。
原始 Makefile 留在根目录供比对，日常请使用上述脚本。

首次安装依赖的官方说明：
https://www-fourier.univ-grenoble-alpes.fr/~parisse/casio/khicasioen.html
（Developer infos）。工具链的 `libmpfr.so.4` 可放在 `$TOOLS_DIR/runtime/`；
脚本会设置相应 `LD_LIBRARY_PATH`。`mkg3a` 可放在 `$TOOLS_DIR/bin/`。

## 测试与真机基准

```bash
python3 tests/run-native.py
# Debian/Ubuntu 主机安装 libgiac-dev 后：
python3 tests/run-cas.py
```

受限运行器不支持 LeakSanitizer 时，可用
`ASAN_OPTIONS=detect_leaks=0 python3 tests/run-native.py`；测试仍会跟踪大数堆分配并检查泄漏。
主机 Giac 的数学测试用于回归检查，不代表 SH4 上的执行时间。

计算器上按顺序输入或通过程序编辑器执行 `bench/bench.xws`、`bench/integration.xws`。
FXCG 版上游 `read()` 是空实现，不要用它加载脚本。保持相同精度、弧度模式及超频设置，
比较官方版与优化版至少五次运行的中位数。尚未声称真机积分或整机有固定加速倍数。
