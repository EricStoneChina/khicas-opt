# KhiCAS 2026a 双语发行迁移计划

## 新项目目标

以 `Fadouse/khicas-opt:develop/khicas-official` 的 2026a 数学内核为主版本，
在同一份源码上构建两个可独立安装的发行包：

- KhiCAS EN：保留英文界面、英文目录和英文 Command Help。
- KhiCAS ZH：提供中文 Command Help、中文目录说明和必要的中文界面修复。

两版必须共享相同的 CAS、积分、方程转换和性能优化代码。语言差异只进入帮助、
目录、界面字符串、About 信息和打包名称，避免形成两套数学内核。

## Git 与迁移基线

- 当前集成分支通过双父桥接提交保留原 KhiCAS ZH 和官方 2026a 两条历史。
- 桥接后的文件树与 `develop/khicas-official` 提交 `284c8bd` 完全一致。
- 后续以根目录源码布局为准，不恢复旧版 `giacbf/` 目录结构。
- 旧提交不直接批量 cherry-pick；按照构建、编码、帮助数据、目录和界面分批移植。

## 双版本链接模型

英文版沿用现有对象组合：

```text
catalogen.o + helpen.o + khelpen.o
  -> khicasen.elf
  -> khicas50.g3a + khicas50.ac2
```

中文版使用独立的目录和静态帮助对象：

```text
catalogzh.o + helpen.o + khelpzh.o
  -> khicaszh.elf
  -> khicaszh.g3a + khicaszh.ac2
```

`catalogzh.cpp` 中的 `ram_filename` 必须指向 `khicaszh`，否则 G3A 会加载英文版
AC2，造成版本不匹配或启动失败。

## 旧中文资产迁移映射

| 原 `main` 路径 | 新基线路径 | 用途 |
| --- | --- | --- |
| `tools/help-zh.json` | `tools/help-zh.json` | 人工维护的命令翻译和示例 |
| `tools/help-zh-gen.ps1` | `tools/help-zh-gen.ps1` | 从英文帮助生成中文静态表 |
| `giacbf/static_helpzh.h` | `static_helpzh.h` | 中文 Command Help 生成物 |
| `giacbf/khelpzh.cc` | `khelpzh.cc` | 中文静态帮助查询对象 |
| `giacbf/catalogzh.cpp` | `catalogzh.cpp` | 中文 Function Catalog 和 AC2 名称 |
| `giacbf/khicas_gb18030.*` | `khicas_gb18030.*` | GB18030 标记、解码与显示 |
| `giacbf/khicas_zhui.*` | `khicas_zhui.*` | 中英文界面字符串选择 |
| `tools/ui-zh.json` | `tools/ui-zh.json` | 中文界面词条源数据 |
| `tools/ui-zh-gen.ps1` | `tools/ui-zh-gen.ps1` | 中文界面生成器 |

原中文帮助共有 149 个维护条目，中文界面表共有 98 个条目；新基线的英文静态帮助
包含 2075 行。生成器必须以新基线的 `static_helpen.h` 为母本，未翻译命令继续保留
英文内容。

## 新内核需要补充的 Command Help

2026a 新增或显著扩展的命令必须进入英文和中文目录，并提供可直接执行的示例：

- `cart2param`
- `cart2polar`
- `param2cart`
- `param2polar`
- `polar2cart`
- `polar2param`
- `Li2`

积分、复参数、主值分支和定义域相关说明应保持简短，并明确条件；不能把测试语料中的
特例描述成对所有输入都成立的规则。

## GitHub Actions 实施顺序

1. 恢复基础 CI，运行不依赖 SH4 工具链的原生测试和生成器一致性检查。
2. 缓存并安装 `casiolocal`、`libmpfr.so.4` 和 `mkg3a`。
3. 使用 `tools/build.py optimized` 构建英文版。
4. 扩展 `tools/build.py` 和 Makefile 生成逻辑，同时构建英文版与中文版。
5. 分别上传 `khicas-en` 和 `khicas-zh` artifact，禁止混合 G3A/AC2 文件对。
6. 输出 ELF、G3A、AC2 和两个链接区域的尺寸，阻止超出 fx-CG50 限制的构建。

## 每阶段验收条件

- 生成器重复运行后工作区无差异。
- 中文帮助条目数与英文母本一致，未翻译条目自动回退英文。
- GB18030 生成过程中没有无法编码的四字节字符被静默替换。
- 英文版仍加载 `khicas50.ac2`，中文版只加载 `khicaszh.ac2`。
- 两个版本均在 GitHub Actions 中完成交叉编译并上传成对产物。
- 数学内核原生测试通过；新增中文代码不得改变英文版计算结果。
- 真机发布前检查启动、Function Catalog、Command Help、长中文换行和 Plot Help。

## 暂不迁移的内容

网站和 Release 页面在双版本构建稳定后再改版。当前优先级是保持 2026a 数学能力，
恢复可复现构建，并完成 Command Help 的英文补充与中文化。
