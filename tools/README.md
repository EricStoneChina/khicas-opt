# KhiCAS 工具集

## 图标(2048 风格)

图标规范(用户认可的 2048 插件风格,非 WikiPrizm 旧黑底规范):
- 92x64,24bpp RGB(无 alpha,供 mkg3a)
- **未选中**:白底 + 高对比橙红主体 + 浅灰阴影(偏移 2,2)
- **选中**:蓝渐变底(上浅下深)+ 白色主体 + 顶部高光
- 主体居中、四周留边 >=3px、**底部留白 >=12px(避开 OS 文字区)**、不超限位

### 生成

```powershell
pwsh icon-generate.ps1        # Windows 本机(无 bun/node 依赖,推荐)
```
或 Linux/bun:
```bash
bun generate-icons.ts [输出目录]
```
两者输出一致(几何/配色等价)。

### 分析验证

```powershell
pwsh icon-analyze2.ps1        # 精确测主体边界框/留边/主色(排除背景)
```
或用 bun 版 `bun analyze-icons.ts [目录]`(通用分析,含风格评估)。

### 模板
`fxcg50-icon-template.zip` — Lephe 官方风格模板(GIMP .xcf,手工精细调整用)。

## 帮助/其他工具

- `help-fix.ts` / `help-fr-audit.ts` — 法语残留修复与审计
- `icon-analyze.ps1` / `icon-analyze2.ps1` — 像素级图标分析(PS 版)
- `icon-generate.ps1` — 2048 风格图标生成(PS 版)
- `out/` — 生成的 PNG 输出目录(生成后需复制到 giacbf/ 并重新 mkg3a)
