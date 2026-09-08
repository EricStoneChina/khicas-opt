// KhiCAS 官方风格图标生成器
// 依据 Cemetech Prizm Icon Design Guidelines(t=6211)与 Lephe 模板(t=17770)
// 规范:
//   - 92x64 尺寸
//   - 未选中:纯黑背景 + 抖动图案 + 主体 + 阴影
//   - 选中:蓝色渐变背景 + 抖动图案 + 高光 + 主体 + 阴影
//   - 主体居中留边,带向下偏移 1-2px 的阴影
// 用法: bun generate-icons.ts [输出目录]
import { PNG } from "pngjs";
import * as fs from "fs";
import * as path from "path";

const W = 92, H = 64;
const outDir = process.argv[2] ?? path.join(import.meta.dir, "out");
fs.mkdirSync(outDir, { recursive: true });

type RGBA = [number, number, number, number];

// 画布:RGBA 浮点累加,最后合成
class Canvas {
  buf: Float64Array;
  constructor() { this.buf = new Float64Array(W * H * 4); }
  blend(x: number, y: number, c: RGBA) {
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    const i = (y * W + x) * 4;
    const a = c[3] / 255;
    this.buf[i] = c[0] * a + this.buf[i] * (1 - a);
    this.buf[i + 1] = c[1] * a + this.buf[i + 1] * (1 - a);
    this.buf[i + 2] = c[2] * a + this.buf[i + 2] * (1 - a);
    this.buf[i + 3] = Math.max(this.buf[i + 3], c[3]);
  }
  toPNG(): PNG {
    const png = new PNG({ width: W, height: H });
    for (let i = 0; i < W * H * 4; i++) png.data[i] = Math.max(0, Math.min(255, Math.round(this.buf[i])));
    return png;
  }
}

function distToSeg(px: number, py: number, x0: number, y0: number, x1: number, y1: number) {
  const dx = x1 - x0, dy = y1 - y0;
  const len2 = dx * dx + dy * dy;
  let t = len2 ? ((px - x0) * dx + (py - y0) * dy) / len2 : 0;
  t = Math.max(0, Math.min(1, t));
  return Math.hypot(px - (x0 + t * dx), py - (y0 + t * dy));
}

function line(c: Canvas, x0: number, y0: number, x1: number, y1: number, w: number, col: RGBA) {
  for (let y = Math.floor(Math.min(y0, y1) - w); y <= Math.max(y0, y1) + w; y++)
    for (let x = Math.floor(Math.min(x0, x1) - w); x <= Math.max(x0, x1) + w; x++)
      if (distToSeg(x + 0.5, y + 0.5, x0, y0, x1, y1) <= w / 2) c.blend(x, y, col);
}

function arc(c: Canvas, cx: number, cy: number, r: number, w: number, from: number, to: number, col: RGBA) {
  for (let y = cy - r - w; y <= cy + r + w; y++)
    for (let x = cx - r - w; x <= cx + r + w; x++) {
      const d = Math.hypot(x + 0.5 - cx, y + 0.5 - cy);
      if (Math.abs(d - r) > w / 2) continue;
      const ang = (Math.atan2(y + 0.5 - cy, x + 0.5 - cx) * 180 / Math.PI + 360) % 360;
      const inRange = from <= to ? (ang >= from && ang <= to) : (ang >= from || ang <= to);
      if (inRange) c.blend(x, y, col);
    }
}

// χ 主体:两条交叉斜线 + 顶部/底部小弧(oy 为阴影偏移)
function drawChi(c: Canvas, col: RGBA, oy = 0) {
  const cx = 46, y0 = 15 + oy, y1 = 55 + oy;
  line(c, 24, y0, 68, y1, 6.5, col);
  line(c, 68, y0, 24, y1, 6.5, col);
  arc(c, cx, y0 - 2, 15, 5, 200, 340, col);
  arc(c, cx, y1 + 2, 13, 4, 20, 160, col);
}

// 右下角 sin 曲线装饰
function drawCurve(c: Canvas, col: RGBA, oy = 0) {
  const base = 50 + oy;
  for (let x = 14; x <= 78; x += 0.7) {
    const y = base + Math.sin(x / 9) * 6;
    c.blend(Math.round(x), Math.round(y), col);
    c.blend(Math.round(x), Math.round(y) + 1, col);
  }
}

function dither(c: Canvas, col: RGBA) {
  for (let y = 0; y < H; y++)
    for (let x = 0; x < W; x++)
      if (x % 2 === 0 && y % 2 === 0) c.blend(x, y, col);
}

function render(selected: boolean): PNG {
  const c = new Canvas();
  // 背景
  if (selected) {
    for (let y = 0; y < H; y++) {
      const t = y / H;
      c.blend(0, y, [0, 0, 0, 0]); // noop 占位
      const r = Math.round(60 + 50 * (1 - t));
      const g = Math.round(140 + 80 * (1 - t));
      const b = Math.round(220 + 35 * (1 - t));
      for (let x = 0; x < W; x++) c.blend(x, y, [r, g, b, 255]);
    }
    // 顶部高光
    for (let y = 0; y < 6; y++)
      for (let x = 0; x < W; x++) c.blend(x, y, [210, 235, 255, 70]);
    // 底部渐暗
    for (let y = H - 10; y < H; y++)
      for (let x = 0; x < W; x++) c.blend(x, y, [0, 35, 80, 140]);
  } else {
    for (let y = 0; y < H; y++)
      for (let x = 0; x < W; x++) c.blend(x, y, [0, 0, 0, 255]);
  }
  // 抖动(白色网点,选中更亮)
  dither(c, selected ? [255, 255, 255, 90] : [255, 255, 255, 55]);
  // 阴影(下移 2px 的半透明黑)
  drawChi(c, [0, 0, 0, 110], 2);
  drawCurve(c, [0, 0, 0, 110], 2);
  // 主体
  drawChi(c, [255, 255, 255, 255]);
  drawCurve(c, selected ? [255, 205, 60, 255] : [160, 160, 160, 255]);
  return c.toPNG();
}

fs.writeFileSync(path.join(outDir, "khicasio.png"), PNG.sync.write(render(false)));
fs.writeFileSync(path.join(outDir, "khicasio1.png"), PNG.sync.write(render(true)));
console.log(`已生成(92x64,官方风格):`);
console.log(`  ${path.join(outDir, "khicasio.png")}   (未选中:黑底+抖动+χ+阴影)`);
console.log(`  ${path.join(outDir, "khicasio1.png")}  (选中:蓝渐变+高光+χ+阴影)`);
