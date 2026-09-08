// 帮助法语残留审计工具
// 解析 static_help.h(母本),检测英语槽位(index 1)中的法语残留,
// 输出待修复条目清单(命令名 + 当前英语文本)。
// 用法: bun help-fr-audit.ts [输出文件]
import * as fs from "fs";
import * as path from "path";

const src = path.join(import.meta.dir, "..", "giacbf", "static_help.h");
const outFile = process.argv[2] ?? path.join(import.meta.dir, "..", "docs", "help-french-fix-list.md");

const content = fs.readFileSync(src, "utf8");

// 解析每行条目 {"cmd",{"fr","en","es","el","de"},"syntax","related","examples"},
const lines = content.split("\n").filter(l => l.trim().startsWith("{"));
const entries = [];
const str = String.raw`"(?:[^"\\]|\\.)*?"`; // 匹配一个转义安全的字符串
const re = new RegExp(
  `^\\{(${str}),\\{(${str})\\s*,\\s*(${str})\\s*,\\s*(${str})\\s*,\\s*(${str})\\s*,\\s*(${str})\\s*\\},(${str}),(${str}),(${str})\\},?$`
);
for (const line of lines) {
  const m = line.match(re);
  if (!m) { continue; }
  // m[1]=cmd, m[2..6]=howtos, m[7]=syntax, m[8]=related, m[9]=examples
  entries.push({
    cmd: JSON.parse(m[1]),
    howtos: [JSON.parse(m[2]), JSON.parse(m[3]), JSON.parse(m[4]), JSON.parse(m[5]), JSON.parse(m[6])],
    syntax: JSON.parse(m[7]), related: JSON.parse(m[8]), examples: JSON.parse(m[9]),
  });
}

// 法语特征:整段法语开头短语(注意 Simplifie 是英语 Simplifies 的前缀,用 (?!s) 排除)
const frStarts = /^(Renvoie|Calcule(?!s)|Retourne|Donne|La fonction|L'instruction|L'argument|Ecrit|Efface|Transforme|Convertit|Définit|Affiche|Trace|Factorise(?!s)|Simplifie(?!s)|Résout|Approche|Arrondit|Teste|Ajoute|Supprime|Crée|Calculez|Dessine|Tapez|Appuyez|Sélectionne|Voir |Cherche|Met |Utilise|Commande|Opérateur|Nombre|Variable|Résultat|Fonction)/;
// 法语词残留(在英语句子中)
const frWords = /\b(et|avec|pour|dans|une|des|du|sur|ou|de)\b/gi;

let fullFrench = 0, mixedFrench = 0;
const problems = [];

for (const e of entries) {
  const en = e.howtos[1] ?? "";
  // 跳过别名/纯关键字条目(do/from/faire 等循环词)
  if (en.length < 10) continue;
  if (frStarts.test(en.trim())) {
    fullFrench++;
    problems.push({ cmd: e.cmd, type: "整段法语", en });
  } else {
    // 英语中夹法语介词(排除合法英文 "or/of/and")
    const frHits = (en.match(/\b(et|avec|pour|dans|une|des|du)\b/gi) ?? []);
    if (frHits.length) {
      mixedFrench++;
      problems.push({ cmd: e.cmd, type: "混合残留:" + frHits.join(","), en });
    }
  }
}

let md = `# 帮助文件法语残留修复清单\n\n`;
md += `> 来源: static_help.h(母本)英语槽位(index 1)。共 ${problems.length} 条\n`;
md += `> 整段法语: ${fullFrench} 条 | 混合残留: ${mixedFrench} 条\n\n`;
md += `| 命令 | 类型 | 当前英语文本 |\n|---|---|---|\n`;
for (const p of problems) {
  md += `| ${p.cmd} | ${p.type} | ${p.en.replace(/\|/g, "\\|").slice(0, 150)}... |\n`;
}

fs.writeFileSync(outFile, md, "utf8");
console.log(`发现 ${problems.length} 条法语残留:`);
console.log(`  整段法语: ${fullFrench}`);
console.log(`  混合残留: ${mixedFrench}`);
console.log(`清单已写入: ${outFile}`);
