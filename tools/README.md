# KhiCAS 工具集

## analyze-icons.ts — 图标风格分析

对照 CASIO 官方图标规范(92x64、纯黑/蓝底、抖动、阴影、留边)检查 PNG:

```bash
bun analyze-icons.ts [PNG目录]   # 默认分析 giacbf/*.png
```

## generate-icons.ts — 官方风格图标生成

按 Cemetech 规范生成 KhiCAS 图标(未选中:黑底+抖动+χ+阴影;
选中:蓝渐变+高光+χ+阴影):

```bash
bun generate-icons.ts [输出目录]   # 默认输出到 out/
```

生成后:
1. 用 analyze-icons.ts 验证风格
2. 视觉确认(Windows 直接打开 PNG)
3. 替换 giacbf/khicasio.png + khicasio1.png
4. 重新 mkg3a 打包(见 Makefile.opt)

## fxcg50-icon-template.zip — Lephe 官方风格模板

GIMP(.xcf)分层模板:背景(uns/sel)、抖动、高光、图标+阴影。
作者:Lephe(gint/fxSDK 作者)。来源:Planète Casio 论坛 t=17770。
适合手工精细调整时使用。
