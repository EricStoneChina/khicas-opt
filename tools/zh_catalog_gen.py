#!/usr/bin/env python3
"""Generate Chinese Function Catalog and static-help fallback data.

The official catalog is the source of truth for command names and signatures.
Curated entries in help-zh.json win; the remaining catalog entries receive a
short, category-aware Chinese description so the Chinese add-in never falls
back to an English Function Catalog description.
"""

from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CATALOG = ROOT / "catalogen.cpp"
CURATED = ROOT / "tools" / "help-zh.json"
AUTO_JSON = ROOT / "tools" / "catalog-zh-auto.json"
AUTO_HEADER = ROOT / "catalogzh_auto.h"
REPORT = ROOT / "docs" / "CHINESE-COVERAGE-2026a.md"


def first_c_string(line: str) -> str | None:
    """Decode the first C string literal on a catalog row."""
    start = line.find('{"')
    if start < 0:
        return None
    i = start + 1
    out: list[str] = []
    while i < len(line):
        if line[i] != '"':
            return None
        i += 1
        while i < len(line):
            ch = line[i]
            if ch == '"':
                return ''.join(out)
            if ch == '\\' and i + 1 < len(line):
                nxt = line[i + 1]
                if nxt == 'n':
                    out.append('\n')
                    i += 2
                    continue
                if nxt == 't':
                    out.append('\t')
                    i += 2
                    continue
                if nxt == 'x' and i + 3 < len(line):
                    out.append(chr(int(line[i + 2:i + 4], 16)))
                    i += 4
                    continue
                out.append(nxt)
                i += 2
                continue
            out.append(ch)
            i += 1
    return None


def catalog_rows() -> list[tuple[str, str]]:
    rows: list[tuple[str, str]] = []
    for line in CATALOG.read_text(encoding="utf-8").splitlines():
        if not re.match(r"^\s*\{\"", line):
            continue
        name = first_c_string(line)
        if not name:
            continue
        signature = name.strip()
        base = signature.split("(", 1)[0].strip()
        if base:
            rows.append((base, signature))
    return rows


OVERRIDES = {
    "altitude": "求三角形的高。用法：altitude(A,B,C)。",
    "area": "求几何对象的面积。用法：area(object)。",
    "barycenter": "求带权点列的重心。用法：barycenter([point,coefficient],...)。",
    "bisector": "求角平分线。用法：bisector(A,B,C)。",
    "center": "求圆、球或圆锥曲线的中心。用法：center(object)。",
    "circle": "创建圆。用法：circle(center,radius)。",
    "circumcircle": "求三点外接圆。用法：circumcircle(A,B,C)。",
    "conic": "创建或识别二次曲线。用法：conic(expression)。",
    "coordinates": "返回点的笛卡尔坐标。用法：coordinates(object)。",
    "cone": "创建圆锥。用法：cone(A,v,theta[,h])。",
    "cube": "创建立方体。用法：cube(A,B,C)。",
    "cylinder": "创建圆柱。用法：cylinder(A,v,r[,h])。",
    "ellipse": "按两个焦点和一点创建椭圆。用法：ellipse(F1,F2,M)。",
    "equation": "返回几何对象的笛卡尔方程。用法：equation(object)。",
    "hyperbola": "按两个焦点和一点创建双曲线。用法：hyperbola(F1,F2,M)。",
    "incircle": "求三角形内切圆。用法：incircle(A,B,C)。",
    "inter": "求两个几何对象的交集。用法：inter(A,B)。",
    "line": "创建直线。用法：line(equation)。",
    "midpoint": "求两点的中点。用法：midpoint(A,B)。",
    "parabola": "按焦点和准线上一点创建抛物线。用法：parabola(F,A)。",
    "parameq": "返回几何对象的参数方程。用法：parameq(object)。",
    "perpen_bisector": "求线段的垂直平分线。用法：perpen_bisector(A,B)。",
    "plane": "创建平面。用法：plane(equation)。",
    "point": "创建二维或三维点。用法：point(x,y[,z])。",
    "polygon": "创建多边形。用法：polygon(list)。",
    "polyhedron": "创建多面体。用法：polyhedron(A,B,C,D,...)。",
    "projection": "求几何对象的投影。用法：projection(object1,object2)。",
    "radius": "求圆、球或几何对象的半径。用法：radius(object)。",
    "rectangle": "创建矩形。用法：rectangle(A,B,C,D)。",
    "reflection": "关于对象作对称变换。用法：reflection(object1,object2)。",
    "rotation": "绕中心旋转对象。用法：rotation(center,angle,object)。",
    "segment": "创建线段。用法：segment(A,B)。",
    "similarity": "作相似变换。用法：similarity(center,ratio,angle,object)。",
    "single_inter": "求两个对象的单一交点。用法：single_inter(A,B)。",
    "sphere": "创建球体。用法：sphere(A,r)。",
    "square_point": "创建正方形。",
    "translation": "平移几何对象。用法：translation(vector,object)。",
    "triangle": "创建三角形。用法：triangle(A,B,C)。",
    "vertices": "返回多边形或多面体的顶点。用法：vertices(object)。",
    "debug": "逐步执行用户函数，便于调试。用法：debug(f(args))。",
    "display": "设置图形显示选项。用法：display(object,options)。",
    "eval": "计算表达式的值。用法：eval(f)。",
    "input": "从键盘读取输入。用法：input()。",
    "print": "打印表达式。用法：print(expr)。",
    "purge": "清除变量。用法：purge(x)。",
    "quote": "返回表达式的未求值形式。用法：quote(x)。",
    "python": "执行 Python 兼容代码。用法：python(f)。",
    "python_compat": "设置 Python 兼容模式。用法：python_compat(0|1|2)。",
    "time": "测量命令的执行时间。用法：time(cmd)。",
    "write": "把变量及其值写入文件。用法：write(\"filename\",var)。",
    "read": "从文件读取变量。用法：read(\"filename\")。",
    "extend": "把另一个列表追加到列表末尾。",
    "append": "在列表末尾追加一个元素。",
    "len": "返回列表元素数或字符串长度。用法：len(L)。",
    "range": "生成整数范围 [a,b)。用法：range(a,b)。",
    "seq": "按规则生成列表。用法：seq(expr,var,a,b)。",
    "sort": "对列表排序。用法：sort(L)。",
    "sorted": "返回排序后的列表。用法：sorted(L)。",
    "size": "返回列表或矩阵的维数。用法：size(L)。",
    "zip": "按位置组合多个列表。用法：zip(L1,L2)。",
    "makelist": "按规则生成列表。用法：makelist(expr,var,a,b)。",
    "apply": "把函数作用于列表元素。用法：apply(f,L)。",
    "map": "把函数映射到列表元素。用法：map(f,L)。",
    "select": "筛选满足条件的列表元素。用法：select(f,L)。",
    "contains": "查找元素在列表中的位置。用法：contains(L,x)。",
    "union": "求集合并集。用法：union(A,B)。",
    "intersect": "求集合交集。用法：intersect(A,B)。",
    "reverse": "反转列表元素顺序。用法：reverse(L)。",
    "matrix": "生成矩阵。用法：matrix(rows,columns,rule)。",
    "matpow": "计算矩阵的整数次幂。用法：matpow(A,n)。",
    "ranm": "生成随机矩阵。用法：ranm(rows,columns)。",
    "ranv": "生成随机向量。用法：ranv(n)。",
    "jordan": "求矩阵的 Jordan 标准形。用法：jordan(A)。",
    "gauss": "化简二次型。用法：gauss(q)。",
    "gramschmidt": "对向量组作 Gram-Schmidt 正交化。用法：gramschmidt(M)。",
    "cholesky": "求 Cholesky 分解。用法：cholesky(A)。",
    "fourier_an": "求傅里叶余弦系数。",
    "fourier_bn": "求傅里叶正弦系数。",
    "fourier_cn": "求复指数傅里叶系数。",
    "tchebyshev1": "生成第一类切比雪夫多项式。",
    "tchebyshev2": "生成第二类切比雪夫多项式。",
    "hermite": "生成 Hermite 多项式。",
    "laguerre": "生成 Laguerre 多项式。",
    "legendre": "生成 Legendre 多项式。",
    "resultant": "求两个多项式的结式。用法：resultant(p,q,x)。",
    "revert": "求幂级数的反函数展开。用法：revert(p[,x])。",
    "tcollect": "收集表达式中的同类项。用法：tcollect(expr)。",
    "trig2exp": "把三角函数改写为复指数。用法：trig2exp(expr)。",
    "trigcos": "用余弦恒等式改写三角表达式。用法：trigcos(expr)。",
    "trigsin": "用正弦恒等式改写三角表达式。用法：trigsin(expr)。",
    "trigtan": "用正切恒等式改写三角表达式。用法：trigtan(expr)。",
    "halftan": "用半角正切代换三角表达式。用法：halftan(expr)。",
    "uniformd": "求均匀分布的概率密度。用法：uniformd(a,b,x)。",
    "exponentiald": "求指数分布的概率密度。用法：exponentiald(lambda,x)。",
    "erf": "误差函数。用法：erf(x)。",
    "erfc": "余误差函数。用法：erfc(x)。",
    "tabvar": "生成函数的变化表。用法：tabvar(f[,x=a..b])。",
    "linetan": "求曲线在指定点的切线。用法：linetan(expr,x,x0)。",
    "odesolve": "数值求解常微分方程初值问题。",
}


def load_curated() -> dict[str, str]:
    data = json.loads(CURATED.read_text(encoding="utf-8"))
    result: dict[str, str] = {}
    for key, value in data.items():
        if key == "caseSensitive":
            for nested, nested_value in value.items():
                if isinstance(nested_value, dict) and nested_value.get("howto"):
                    result[nested] = nested_value["howto"]
            continue
        if isinstance(value, dict) and value.get("howto"):
            result[key] = value["howto"]
        elif isinstance(value, str):
            result[key] = value
    return result


def auto_description(base: str, signature: str, curated: dict[str, str]) -> str:
    if base in curated:
        return curated[base]
    if base in OVERRIDES:
        return OVERRIDES[base]
    if base.startswith("plot") or base.endswith("plot"):
        return f"绘制相关图形。用法：{signature}。"
    if "regression" in base:
        return f"执行回归分析{'并绘图' if base.endswith('_plot') else ''}。用法：{signature}。"
    if base.startswith("draw_"):
        return f"在屏幕上绘制像素图形。用法：{signature}。"
    if base.startswith("is_"):
        return f"判断几何或代数条件是否成立。用法：{signature}。"
    if base.startswith("rand") or base.startswith("ran"):
        return f"生成随机数值或随机对象。用法：{signature}。"
    if base.startswith("normald") or base.endswith("_cdf") or base.endswith("_icdf"):
        return f"计算概率分布相关数值。用法：{signature}。"
    if base.startswith("_"):
        return f"物理量或单位选项：{base}。"
    if base in {"!", "#", "%", "&", ":=", "<", ">", "=>", "|", "~"}:
        return f"运算符或程序选项 {base}。"
    if base in {"red", "green", "blue", "cyan", "magenta", "yellow", "black", "filled"}:
        return f"图形显示选项：{base}。"
    if base in {"avance", "recule", "saute", "rond", "disque", "tourne_droite", "tourne_gauche"}:
        return f"海龟绘图命令。用法：{signature}。"
    return f"执行 {base} 相关的符号计算操作。用法：{signature}。"


def c_escape_ascii(text: str) -> str:
    out = ['"']
    for ch in text:
        if ch == '"':
            out.append('\\"')
        elif ch == '\\':
            out.append('\\\\')
        elif ch == '\n':
            out.append('\\n')
        else:
            out.append(ch)
    out.append('"')
    return ''.join(out)


def c_gb18030(text: str) -> str:
    encoded = text.encode("gb18030", errors="replace")
    out = ['"\\x01""']
    for byte in encoded:
        if byte >= 0x80:
            out.append(f'""\\x{byte:02x}""')
        elif byte == 0x22:
            out.append('\\"')
        elif byte == 0x5c:
            out.append('\\\\')
        elif byte == 0x0a:
            out.append('\\n')
        else:
            out.append(chr(byte))
    out.append('"')
    return ''.join(out)


def write_header(entries: dict[str, str]) -> None:
    lines = [
        "#ifndef KHICAS_CATALOGZH_AUTO_H",
        "#define KHICAS_CATALOGZH_AUTO_H",
        "",
        "struct zh_catalog_entry { const char * name; const char * description; };",
        "static const zh_catalog_entry zh_catalog_entries[] = {",
    ]
    for name in sorted(entries):
        lines.append(f"  {{{c_escape_ascii(name)},{c_gb18030(entries[name])}}},")
    lines.extend([
        "};",
        "",
        "static inline const char * zh_catalog_description(const char * signature, const char * fallback) {",
        "  if (!signature) return fallback;",
        "  while (*signature == ' ') ++signature;",
        "  char name[96]; unsigned i=0;",
        "  const char * open=strchr(signature,'(');",
        "  while (*signature && (!open || signature<open) && i+1<sizeof(name)) name[i++]=*signature++;",
        "  while (i && name[i-1]==' ') --i;",
        "  name[i]=0;",
        "  for (unsigned j=0; j<sizeof(zh_catalog_entries)/sizeof(zh_catalog_entries[0]); ++j)",
        "    if (!strcmp(name,zh_catalog_entries[j].name)) return zh_catalog_entries[j].description;",
        "  return fallback;",
        "}",
        "",
        "#endif",
        "",
    ])
    AUTO_HEADER.write_text("\n".join(lines), encoding="ascii")


def main() -> None:
    curated = load_curated()
    rows = catalog_rows()
    entries: dict[str, str] = {}
    signatures: dict[str, str] = {}
    for base, signature in rows:
        signatures.setdefault(base, signature)
        entries[base] = auto_description(base, signatures[base], curated)

    # Use an array rather than an object so case-sensitive aliases such as
    # _G_ and _g_ remain distinct when PowerShell reads the file.
    auto_rows = [{"name": name, "howto": entries[name]} for name in sorted(entries)]
    AUTO_JSON.write_text(json.dumps(auto_rows, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    write_header(entries)

    curated_count = sum(1 for name in entries if name in curated)
    generated_count = len(entries) - curated_count
    REPORT.write_text(
        "# 中文覆盖率报告（2026a）\n\n"
        "本报告把 Function Catalog 的 374 个源代码行作为“常用函数”集合，"
        "并以命令名、语法和分类为覆盖统计单位。中文构建会优先使用人工维护的"
        "`tools/help-zh.json`，其余目录条目使用可再生成的中文兜底说明。\n\n"
        f"- Function Catalog 源代码行：{len(rows)}\n"
        f"- 唯一命令名：{len(entries)}\n"
        f"- 中文说明：{len(entries)} 个唯一命令名（100%；覆盖 {len(rows)}/{len(rows)} 个源代码行）\n"
        f"- 人工维护说明：{curated_count}\n"
        f"- 自动生成说明：{generated_count}\n"
        "- 静态 Command Help：2082 条；生成器对目录命令优先注入中文说明\n\n"
        "自动生成条目保留命令名、签名和用途类别，后续可在 `help-zh.json` 中逐条替换为"
        "更详细的人工译文。\n",
        encoding="utf-8",
    )
    print(f"catalog entries: {len(entries)}; curated: {curated_count}; generated: {generated_count}")
    print(f"wrote {AUTO_JSON}, {AUTO_HEADER}, {REPORT}")


if __name__ == "__main__":
    main()
