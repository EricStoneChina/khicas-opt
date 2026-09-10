# 参数方程的数学显示

CG50 在直接运行 `cart2param`、`polar2param`，以及外层使用 `simplify` 或赋值时，数学窗口显示带名称的坐标，例如：

```text
cart2param(x^3+y^3=3*x*y,[x,y],t)
[x(t)=3*t/(1+t^3), y(t)=3*t^2/(1+t^3)]

cart2param(u=v^2,[u,v],s)
[u(s)=s^2, v(s)=s]
```

多分支分行显示；常值坐标也保留正确的名称。极坐标转换输出的是笛卡尔坐标，因此 `polar2param(...,[rho,phi],s)` 显示 `x(s)`、`y(s)`。

这是只读数学视图。按 EXE 将完整的原始参数列表放入 `last`；复制或存储所选单元格得到右侧表达式，选中整行得到原始坐标对。方向键移动，Shift-F3 或 F9 调整字号。标签不参与求值，也不会作为函数执行。`cart2param`、`polar2param` 的返回值、赋值结果和计算历史仍是 `[[X(t),Y(t)],...]`，各分支继续按原接口用于 `plotparam`。

显示名称来自本次原始调用，避免从右侧公式猜测变量。在程序返回结果、再次查看变量 `b` 或 `last` 时，缺少原始转换调用，仍显示原始数据格式；没有跨结果全局缓存。包含额外算术或带约束的 `simplify` 调用也不冒用转换名称。输入包装层最多 8 层、分支最多 16 个，带标签后的表达式另受现有数学显示大小上限约束。

实现入口是 `main.cc` 的 `check_do_graph → eqw_presentation`，实际排版使用 `zdisplay.cc` 的 `Equation_compute_size`；`kdisplay.cc` 不是当前 CG50 构建所用文件。`parametric_display.h` 只构造显示对象并映射原始单元格。

验证：`tests/run-parametric-display.py` 检查真实转换结果、自定义名称、多分支、常值点、包装调用、原始选择数据、标签不求值和范围限制，并检查原始分支仍可交给主机 Giac 的 `plotparam`。CG50 `main.cc` 已通过 SH4 交叉编译。当前未运行真实屏幕或像素级渲染模拟，不能将这些检查视为屏幕截图验证。
