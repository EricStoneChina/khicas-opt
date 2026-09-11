# Khicas Opt

Khicas Opt 是基于 KhiCAS 的团队协作项目，面向 fx-CG50 / Graph 90+E 提供 Giac/Xcas 优化构建。

## 目标

以 Bernard Parisse 的 KhiCAS 源码为基础,研究并实施**更优化的编译配置**——
在保证功能完整的前提下,从**代码尺寸**和**运行速度**两个维度改进构建。

## 团队分工

- Fadouse：负责内核和函数优化，包括符号计算、积分、导数、定义域和资源边界。
- 项目团队：共同维护构建配置、Help、界面、网站、CI、发布和设备验证。

项目统一称为 **Khicas Opt**，所有成员都基于 KhiCAS 协作。
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
- [x] 帮助法语残留修复(12 条 → 0;help-fix.ts + help-fr-audit.ts)
- [x] 图标替换为官方风格(generate-icons.ts + analyze-icons.ts)
- [x] 画图操作速查表(docs/画图操作速查.md)
- [x] AC Break 防御补丁(patch/03-ac-break-hardening.patch)
- [x] GitHub Actions CI(.github/workflows/build.yml)
- [x] ✅ 构建成功!产物 khicas50.g3a(1.9MB)+ khicas50.ac2(2.4MB)
      可下载:GitHub Actions → 最新 run → Artifacts → khicas50-optimized
      或运行 CI 时 build 步骤自动上传
- [x] 图标重绘为 2048 风格(未选中:白底+橙红 χ;选中:蓝渐变+白 χ;留边充足、不超限位)
- [x] 中文帮助阶段一:数据管道(help-zh-gen.ps1)+ GB18030 渲染接入 + `khicaszh` 构建目标
      产物:CI Artifacts → `khicaszh-chinese`(khicaszh.g3a + khicaszh.ac2)
      说明见 docs/中文翻译方案.md 第 12 节(含待真机验证清单)


