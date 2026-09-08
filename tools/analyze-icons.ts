// KhiCAS 图标分析工具
// 用法: bun analyze-icons.ts [目录]
// 输出:每个图标的尺寸、主色调、透明占比、风格评估(对照 CASIO 官方风格)
import { PNG } from "pngjs";
import * as fs from "fs";
import * as path from "path";

const dir = process.argv[2] ?? path.join(import.meta.dir, "..", "giacbf");

// 官方风格特征(来自 Cemetech Prizm Icon Design Guidelines)
// 1. 92x64 尺寸
// 2. 彩色(非灰度)、接近 3D(有渐变/多色调)
// 3. 有阴影(黑色低透明度像素簇)
// 4. 未选中:纯黑背景;选中:非黑背景
// 5. 主对象不占满全图(留边)
function quantize(v: number): number {
  return Math.round(v / 24) * 24;
}

function analyze(file: string) {
  const buf = fs.readFileSync(file);
  const png = PNG.sync.read(buf);
  const { width: w, height: h, data } = png;

  const colorCount = new Map<string, number>();
  let transparent = 0;
  let black = 0;
  let nonBlack = 0;
  let shadowPx = 0; // 半透明黑(阴影特征)
  let edgeMarginMin = Infinity; // 主对象距边缘的最小像素

  // 扫描非透明像素的边界
  let minX = w, maxX = 0, minY = h, maxY = 0;

  for (let y = 0; y < h; y++) {
    for (let x = 0; x < w; x++) {
      const i = (y * w + x) * 4;
      const [r, g, b, a] = [data[i], data[i + 1], data[i + 2], data[i + 3]];
      if (a < 32) { transparent++; continue; }
      const key = `${quantize(r)},${quantize(g)},${quantize(b)}`;
      colorCount.set(key, (colorCount.get(key) ?? 0) + 1);
      if (r < 40 && g < 40 && b < 40) black++;
      else {
        nonBlack++;
        minX = Math.min(minX, x); maxX = Math.max(maxX, x);
        minY = Math.min(minY, y); maxY = Math.max(maxY, y);
      }
      if (a < 200 && r < 60 && g < 60 && b < 60) shadowPx++;
    }
  }

  // 主色排序
  const top = [...colorCount.entries()].sort((a, b) => b[1] - a[1]).slice(0, 6);
  const total = w * h;
  const colorful = colorCount.size; // 量化后颜色数(多样=彩色)
  const margin = Math.min(minX, minY, w - 1 - maxX, h - 1 - maxY);

  console.log(`\n=== ${path.basename(file)} ===`);
  console.log(`  尺寸: ${w}x${h}  (官方: 92x64)`);
  console.log(`  透明: ${(transparent / total * 100).toFixed(1)}%`);
  console.log(`  纯黑像素: ${black} (${(black / total * 100).toFixed(1)}%)  非黑像素: ${nonBlack}`);
  console.log(`  主色数量: ${colorful}`);
  console.log(`  阴影像素(半透明黑): ${shadowPx}`);
  console.log(`  主体边距: ${margin}px (官方建议留边,主对象不占满)`);
  console.log(`  主色调(量化RGB): ${top.map(([k, v]) => `rgb(${k})×${v}`).join("  ")}`);

  const styleNotes: string[] = [];
  if (w !== 92 || h !== 64) styleNotes.push("尺寸不符");
  if (colorful < 4) styleNotes.push("颜色单一,官方风格多彩");
  if (shadowPx < 10) styleNotes.push("缺少阴影(官方风格有阴影)");
  if (black / total > 0.9 && nonBlack < 500) styleNotes.push("接近纯黑背景(未选中风格)");
  if (margin < 3) styleNotes.push("主体顶到边缘,建议留边");
  console.log(`  风格评估: ${styleNotes.length ? styleNotes.join("; ") : "基本符合官方风格"}`);
}

const files = fs.readdirSync(dir).filter(f => f.endsWith(".png"));
if (!files.length) { console.log("未找到 PNG"); process.exit(1); }
for (const f of files) analyze(path.join(dir, f));
