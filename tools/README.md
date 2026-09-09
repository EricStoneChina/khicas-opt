# KhiCAS 工具集

## 图标(fx-CG50 官方主菜单风格)

与 OS 自带应用图标(Run-Matrix / Statistics / Graph …)同一套视觉语言 —— 依据用户真机主菜单截图确定:
- 92x64,24bpp RGB(无 alpha,供 mkg3a)
- **满幅饱和色渐变磁贴**(直角)
- **四周一圈浅色细边框**(官方图标同款,约 2px)
- **白色图形**,大号、居中(本图标为 χ)
- **不要自己画右上角角标** —— 那个字母是系统根据按键生成的
- 未选中 = 深蓝渐变;选中 = 亮蓝渐变(更亮,体现高亮)

### 生成

```powershell
pwsh icon-generate.ps1        # Windows 本机(无需 bun/node)
```
输出:`out/khicasio.png`(未选中)、`out/khicasio1.png`(选中)、`out/compare_x4.png`(4 倍对比预览)

### 分析验证

```powershell
pwsh icon-analyze2.ps1        # 精确测主体边界框/留边/主色(排除背景)
```

### 模板
`fxcg50-icon-template.zip` — Lephe 官方风格模板(GIMP .xcf,手工精细调整用)。

> ⚠️ `generate-icons.ts` / `analyze-icons.ts`(bun 版)为早期"白底 + 彩色主体"方案,
> 已**被 icon-generate.ps1 取代**(风格不同),保留仅作参考。


## 帮助/其他工具

- `help-fix.ts` / `help-fr-audit.ts` — 法语残留修复与审计
- `icon-analyze.ps1` / `icon-analyze2.ps1` — 像素级图标分析(PS 版)
- `icon-generate.ps1` — 2048 风格图标生成(PS 版)
- `out/` — 生成的 PNG 输出目录(生成后需复制到 giacbf/ 并重新 mkg3a)
