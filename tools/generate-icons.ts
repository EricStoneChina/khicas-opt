// KhiCAS 图标生成器 — 2048 风格(与 tools/icon-generate.ps1 等价)
// 参照用户认可的 2048 插件图标规范:
//   - 92x64
//   - 未选中:白底 + 高对比橙红主体 + 浅灰阴影(偏移 2,2)
//   - 选中:蓝渐变底(上浅下深)+ 白色主体 + 顶部高光
//   - 主体居中、四周留边(>=3px)、底部留白 >=12px(避开 OS 文字区)
//   - 不超限位(主体不得顶到画布边缘)
// 用法: bun generate-icons.ts [输出目录]   (默认 out/)
// 注:本机无 bun 时用 PowerShell 版 icon-generate.ps1,两者输出一致。
import { PNG } from "pngjs";
import * as fs from "fs";
import * as path from "path";

const W = 92, H = 64;
const outDir = process.argv[2] ?? path.join(import.meta.dir, "out");
fs.mkdirSync(outDir, { recursive: true });

type RGBA = [number, number, number, number];

// 画布:RGBA 浮点累加,最后合成(不透明绘制,等价 24bpp)
class Canvas {
  buf: Float64Array;
  constructor() { this.buf = new Float64Array(W * H * 4); }
  set(x: number, y: number, c: RGBA) {
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    const i = (y * W + x) * 4;
    this.buf[i] = c[0]; this.buf[i + 1] = c[1];
    this.buf[i + 2] = c[2]; this.buf[i + 3] = c[3];
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
      if (distToSeg(x + 0.5, y + 0.5, x0, y0, x1, y1) <= w / 2) c.set(x, y, col);
}

// χ 几何:两条圆头粗斜线,无喇叭弧(避免顶边超限位)
const CX = 46, Y0 = 10, Y1 = 44, HALF_X = 30, PEN_W = 9;

function drawChi(c: Canvas, col: RGBA, ox = 0, oy = 0) {
  line(c, CX - HALF_X + ox, Y0 + oy, CX + HALF_X + ox, Y1 + oy, PEN_W, col);
  line(c, CX + HALF_X + ox, Y0 + oy, CX - HALF_X + ox, Y1 + oy, PEN_W, col);
}

function render(selected: boolean): PNG {
  const c = new Canvas();
  // 背景
  if (selected) {
    for (let y = 0; y < H; y++) {
      const t = y / H;
      const r = Math.round(115 + (28 - 115) * t);
      const g = Math.round(195 + (86 - 195) * t);
      const b = Math.round(255 + (178 - 255) * t);
      for (let x = 0; x < W; x++) c.set(x, y, [r, g, b, 255]);
    }
    // 顶部高光
    for (let y = 0; y < 4; y++)
      for (let x = 0; x < W; x++) c.set(x, y, [225, 242, 255, 255]);
  } else {
    for (let y = 0; y < H; y++)
      for (let x = 0; x < W; x++) c.set(x, y, [255, 255, 255, 255]);
  }
  // 阴影(偏移 2,2)
  drawChi(c, selected ? [24, 70, 140, 255] : [198, 198, 198, 255], 2, 2);
  // 主体
  drawChi(c, selected ? [255, 255, 255, 255] : [226, 92, 48, 255]);
  return c.toPNG();
}

// mkg3a 只接受 RGB-8(无 alpha)
const iconOpts = { colorType: 2, deflateLevel: 9 };
fs.writeFileSync(path.join(outDir, "khicasio.png"), PNG.sync.write(render(false), iconOpts));
fs.writeFileSync(path.join(outDir, "khicasio1.png"), PNG.sync.write(render(true), iconOpts));
console.log("已生成(92x64,2048 风格,RGB-8 供 mkg3a):");
console.log("  khicasio.png   未选中:白底+橙红chi+阴影,留边 L11 T6 R10 B14");
console.log("  khicasio1.png  选中:蓝渐变+白chi+高光");
