# khicas-opt

KhiCAS(fx-CG50 / Graph 90+E 上的 Giac/Xcas)源码仓库与优化构建项目。

## 目标

以 Bernard Parisse 的 KhiCAS 源码为基础,研究并实施**更优化的编译配置**——
在保证功能完整的前提下,从**代码尺寸**和**运行速度**两个维度改进构建。

## 源码来源

- 上游:KhiCAS © B. Parisse,Université Grenoble Alpes(作者服务器 `~parisse/casio/giacbf.tgz`)
- 许可证:GPL2(部分 MIT;MicroPython 1.12;QR 码生成器 MIT)
- 本仓库 `giacbf/` 为上游源码的**干净导入**(已移除 .o/elf/g3a 等构建产物)

## 目录结构

```
giacbf/          # KhiCAS 源码(上游导入,勿直接修改,通过 patch 维护改动)
build.sh         # 一键构建脚本(拉取工具链依赖 + 构建)
docs/            # 优化分析与决策记录
```

## 构建依赖

| 依赖 | 来源 | 说明 |
|---|---|---|
| sh3eb-elf 工具链 | casiolocal.tgz(作者服务器) | gcc/g++/binutils,SuperH 大端 |
| libfxcg | 作者服务器 libfxcg.tgz | Casio OS syscall 封装 |
| mkg3a | 作者服务器 mkg3a.tgz | .g3a 打包器 |
| uSTL / tommath / micropy 库 | casiolocal.tgz 内 | 预编译静态库 |

构建环境:**Linux**(工具链为 Linux 二进制,Windows 下需 WSL2)。

## 优化方向(初步)

- 编译标志:当前 `-Os`(尺寸优先),可评估 `-O3` 与 `-Os` 的速度/尺寸权衡
- 链接:`-flto`(LTO 全程序优化,作者注明"2 addins 模式不可用"需验证)
- `-ffunction-sections`/`-fdata-sections` + `--gc-sections` 进一步裁剪
- 大数库 `libbf.c` 的算法级优化空间
- 内存:prizm_heap 192KB 的调整

## 状态

- [x] 源码导入(提交 d4806dd)
- [x] 优化方案制定(docs/优化方案.md)
- [x] 优化配置实施(Makefile.opt + build.sh)
- [ ] 构建环境搭建(需要 Linux/WSL2;casiolocal.tgz 231MB 下载在当前网络不稳定)
- [ ] 基线构建(上游 -Os)
- [ ] 优化构建(-O3 混合)与基准对比(bench/bench.xws)
