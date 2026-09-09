// KhiCAS 图标分析工具(适配 2048 白底风格规范)
// 用法: bun analyze-icons.ts [目录]  (默认 giacbf/)
// 输出:尺寸、背景色、主体边界框与留边、主色、风格评估
import { PNG } from "pngjs";
import * as fs from "fs";
import * as path from "path";

const dir = process.argv[2] ?? path.join(import.meta.dir, "..", "giacbf");

function quantize(v: number): number {
  return Math.round(v / 24) * 24;
}

function analyze(file: string) {
  const png = PNG.sync.read(fs.readFileSync(file));
  const { width: w, height: h, data } = png;

  // 背景 = 四角平均
  function px(x: number, y: number): [number, number, number, number] {
    const i = (y * w + x) * 4;
    return [data[i], data[i + 1], data[i + 2], data[i + 3]];
  }
  const corners = [px(0, 0), px(w - 1, 0), px(0, h - 1), px(w - 1, h - 1)];
  const bg = [
    corners.reduce((s, c) => s + c[0], 0) / 4,
    corners.reduce((s, c) => s + c[1], 0) / 4,
    corners.reduce((s, c) => s + c[2], 0) / 4,
  ];

  const colorCount = new Map<string, number>();
  let minX = w, maxX = -1, minY = h, maxY = -1;
  let subject = 0;
  let shadowPx = 0; // 距背景远且偏灰/偏暗的像素(阴影特征)

  for (let y = 0; y < h; y++) {
    for (let x = 0; x < w; x++) {
      const [r, g, b, a] = px(x, y);
      if (a < 32) continue;
      const dist = Math.abs(r - bg[0]) + Math.abs(g - bg[1]) + Math.abs(b - bg[2]);
      if (dist > 60) {
        subject++;
        minX = Math.min(minX, x); maxX = Math.max(maxX, x);
        minY = Math.min(minY, y); maxY = Math.max(maxY, y);
        const key = `${quantize(r)},${quantize(g)},${quantize(b)}`;
        colorCount.set(key, (colorCount.get(key) ?? 0) + 1);
        // 阴影:比背景暗、低饱和
        if (r + g + b < bg[0] + bg[1] + bg[2] && Math.max(r, g, b) - Math.min(r, g, b) < 70) shadowPx++;
      }
    }
  }

  const top = [...colorCount.entries()].sort((a, b) => b[1] - a[1]).slice(0, 5);
  const mL = minX, mT = minY, mR = w - 1 - maxX, mB = h - 1 - maxY;
  const minMargin = Math.min(mL, mT, mR, mB);
  const isWhiteBg = bg[0] > 200 && bg[1] > 200 && bg[2] > 200;
  const isBlueBg = bg[2] > bg[0] + 30 && bg[2] > 120;
  const isBlackBg = bg[0] < 40 && bg[1] < 40 && bg[2] < 40;

  console.log(`\n=== ${path.basename(file)} ===`);
  console.log(`  尺寸: ${w}x${h} (规范 92x64)`);
  console.log(`  背景(四角均): RGB(${bg.map(v => Math.round(v)).join(",")})`);
  if (subject === 0) { console.log("  主体: 未检出"); return; }
  console.log(`  主体边界框: x[${minX}..${maxX}] y[${minY}..${maxY}]`);
  console.log(`  留边: L${mL} T${mT} R${mR} B${mB} -> 最小 ${minMargin}px (要求 >=3px)`);
  console.log(`  主体主色: ${top.map(([k, v]) => `rgb(${k})x${v}`).join("  ")}`);
  console.log(`  阴影像素: ${shadowPx}`);

  const notes: string[] = [];
  if (w !== 92 || h !== 64) notes.push("尺寸不符");
  if (minMargin < 3) notes.push("主体顶到边缘(超限位)");
  if (mB < 10) notes.push("底部留白不足(OS 文字区)");
  if (shadowPx < 10) notes.push("缺少阴影");
  if (isWhiteBg) notes.push("白底(2048 未选中风格)");
  else if (isBlueBg) notes.push("蓝底(选中风格)");
  else if (isBlackBg) notes.push("黑底(旧官方未选中风格)");
  else notes.push("背景异常(非白/非蓝/非黑)");
  console.log(`  风格: ${notes.join("; ")}`);
}

const files = fs.readdirSync(dir).filter(f => f.endsWith(".png"));
if (!files.length) { console.log("未找到 PNG"); process.exit(1); }
for (const f of files) analyze(path.join(dir, f));
