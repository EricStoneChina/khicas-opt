// 帮助文件法语残留修复工具
// 只修改母本 static_help.h 的英语槽位(index 1)和 static_helpen.h,
// 避免影响法语/西语/希腊语/德语槽位。
// 用法: bun help-fix.ts
import * as fs from "fs";
import * as path from "path";

const giacbf = path.join(import.meta.dir, "..", "giacbf");
const master = path.join(giacbf, "static_help.h");
const enFile = path.join(giacbf, "static_helpen.h");

// 替换映射:法语残留 → 英语(子串级,仅英语槽位)
const FIXES: [string, string][] = [
  ["et g are taylor's", "and g are Taylor"],            // Airy_Ai / Airy_Bi
  ["arg1=M ou zM", "arg1=M or zM"],                      // circle
  [" et (center=M", " and (center=M"],                   // circle
  [" et radius=abs(zN)", " and radius=abs(zN)"],         // circle
  ["*i pour [1.1,1.2]", "*i for [1.1,1.2]"],             // complexroot
  ["AB=AC et angle", "AB=AC and angle"],                 // isosceles_triangle
  ["column 0 et yk", "column 0 and yk"],                 // ligne_polygonale 等(3 处)
  ["sum(p)=1 et sum(k)=n", "sum(p)=1 and sum(k)=n"],     // multinomial
  ["arguments et the pyramid", "arguments and the pyramid"], // pyramid
];

const str = String.raw`"(?:[^"\\]|\\.)*?"`;
const re5 = new RegExp(
  `^\\{(${str}),\\{(${str})\\s*,\\s*(${str})\\s*,\\s*(${str})\\s*,\\s*(${str})\\s*,\\s*(${str})\\s*\\},(${str}),(${str}),(${str})\\},?$`
);
const re1 = new RegExp(`^\\{(${str}),(${str}),(${str}),(${str}),(${str})\\},?$`);

function fixText(t: string): string {
  let r = t;
  for (const [f, t2] of FIXES) r = r.split(f).join(t2);
  return r;
}

let masterFixed = 0, enFixed = 0, totalChanged = 0;

// --- 母本:只改英语槽位(howtos[1]) ---
{
  const lines = fs.readFileSync(master, "utf8").split("\n");
  const out = lines.map(line => {
    const m = line.match(re5);
    if (!m) return line;
    const howtos = [JSON.parse(m[2]), JSON.parse(m[3]), JSON.parse(m[4]), JSON.parse(m[5]), JSON.parse(m[6])];
    const fixed = fixText(howtos[1]);
    if (fixed === howtos[1]) return line;
    howtos[1] = fixed;
    masterFixed++;
    // 捕获组已含引号(如 "circle"),直接拼接,不再加引号
    return `{${m[1]},{${howtos.map(h => JSON.stringify(h)).join(",")}},${m[7]},${m[8]},${m[9]}},`;
  });
  const changed = out.filter((l, i) => l !== lines[i]).length;
  totalChanged += changed;
  fs.writeFileSync(master, out.join("\n"), "utf8");
}

// --- 英文单语言文件:直接替换 ---
{
  const lines = fs.readFileSync(enFile, "utf8").split("\n");
  const out = lines.map(line => {
    const m = line.match(re1);
    if (!m) return line;
    const howto = JSON.parse(m[2]);
    const fixed = fixText(howto);
    if (fixed === howto) return line;
    enFixed++;
    return `{${m[1]},${JSON.stringify(fixed)},${m[3]},${m[4]},${m[5]}},`;
  });
  const changed = out.filter((l, i) => l !== lines[i]).length;
  totalChanged += changed;
  fs.writeFileSync(enFile, out.join("\n"), "utf8");
}

console.log(`修复完成:`);
console.log(`  母本 static_help.h 英语槽位: ${masterFixed} 条`);
console.log(`  static_helpen.h: ${enFixed} 条`);
console.log(`  共修改 ${totalChanged} 行`);
