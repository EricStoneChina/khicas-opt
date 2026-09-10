# 本轮积分规则的推导与适用边界

源码 checkpoint：`checkpoint/user-matrix-next-2026a`。以下为规则族推导，不是按题目字符串查表。实际输出用独立进程和 SymPy 核对；[33个变体](../tests/user-matrix-next.json)有132次精确验证，[25个独立数值积分](benchmarks/user-matrix-next-reference-2026a.json)另以65位精度核对定义积分。

## 实绝对值及周期

实仿射函数 (u=ax+b) 满足

\[
\int |ax+b|\,dx=\frac{(ax+b)|ax+b|}{2a},\qquad a\ne0.
\]

直接代入有限端点覆盖跨零点和反向区间；无需递归找零点。只有证明内层为实仿射或实三角函数时，才将原始 `sqrt(u^2)` 识别为 `abs(u)`。每个长度为 (\pi) 的完整周期，(\int |\sin u|du=\int|\cos u|du=2)。非完整周期继续交给其他积分路径。

## 四次根式的反双曲分支

令 (D>0\)、(L>-2D\)、(K=L+2D>0\)、(Q=x^4+Lx^2+D^2\)，则

\[
Q-Kx^2=(x^2-D)^2,\quad
\frac{d}{dx}\frac{\operatorname{atanh}(\sqrt K x/\sqrt Q)}{\sqrt K}
=-\frac{x^2+D}{(x^2-D)\sqrt Q}.
\]

因此给定分子比例、分母比例和正的根式比例，就能生成对应原函数。在 (x\ne\pm\sqrt D\) 的每个实区间，根式为正，atanh 参数绝对值小于1。并不将跨越二阶分母极点的定积分当作收敛积分。原先 (d>0) 的 asin/asinh 分支及其15个变体保持通过；新分支另检查多个 (D,L) 和极点两侧。

## Catalan 与圆弧 atan

记 (G=\sum_{k\ge0}(-1)^k/(2k+1)^2=(\Psi_1(1/4)-\Psi_1(3/4))/16\)。由幂级数在单位区间的积分极限，

\[
\int_0^1\frac{\arctan u}{u}du=G,\qquad
\int_0^1\frac{\ln u}{1+u^2}du=-G.
\]

对 (u=cx^q\) 使用精确端点条件及雅可比，不展开高次幂。对数要求实正参数；atan 的负比例利用奇性。

设 (F(b)=\int_0^1\arctan(bu)/(u\sqrt{1-u^2})du\)。在有限实 (b) 下可微分，令 (u=\sin\theta\)：

\[
F'(b)=\int_0^{\pi/2}\frac{d\theta}{1+b^2\sin^2\theta}
=\frac{\pi}{2\sqrt{1+b^2}},\quad F(0)=0.
\]

故 (F(b)=\pi\operatorname{asinh}(b)/2\)。将 (1-h^2x^{2q}\) 的平方根及 `dx/x` 一并换元，可覆盖不同频率、尺度、正负 atan 参数，而不是只匹配用户的 (b=1)。

三角对数使用

\[
H(n\pi/4)=-n\pi\ln2/4-G\sigma(n)/2,\quad
\sigma(n)=(0,1,0,-1)\quad (\bmod 4).
\]

无绝对值时仅允许正的半波；带绝对值可跨负半波，端点对数奇点可积。旧的一阶仿射权重矩保持原有公式，不能把零阶结果误用于任意权重。

B3 用 (u\mapsto\pi/2-u\) 配对仿射权重，化为权重的区间中点值乘以
\(\int_0^{\pi/2}du/(\sin u+\cos u)=\sqrt2\ln(1+\sqrt2)\)。分母比例、角度的线性斜率和原区间都纳入结果。

## 衰减、相消和普通反常积分

(a>0,b\in\mathbb R\) 时，

\[
\int_0^\infty e^{-ax}\frac{\sin bx}{x}dx=\arctan(b/a),\qquad
\int_0^\infty e^{-ax}\frac{1-\cos bx}{x}dx=\tfrac12\ln(1+b^2/a^2).
\]

这些公式可由对 (b) 微分后的初等 Laplace 积分，再用 (b=0) 定常数得到。新符号参数路径仍须由当前假设证明 (a>0\)，不是看到字母就假定收敛。

当 (a_j>0\)、共同指数常数项相同、\(\sum_j c_j=0\) 时，

\[
\int_0^\infty \frac{\sum_j c_j e^{-a_jx}}x\cos(bx)dx
=-\tfrac12\sum_j c_j\ln(a_j^2+b^2).
\]

在原点先利用相消，远处利用各项正衰减率，故整体绝对收敛。实际实现先验证这些条件，再形成对数；不分别求发散积分。带额外 `ln(x)`、不同相位或 `1/x^2` 的振荡情形尚不套用这条新公式。

未衰减的非零实频率 sin/cos 在无限端点没有普通反常积分极限，返回 `undef` 并明确记录发散。没有使用 Abel/Cesàro 值。C6 的非零 `1/x` 尾部检测继续区分普通积分与主值。

## 正值三角对数和 Fourier 核

对实 (A>|B|\)，令 (d=\sqrt{A^2-B^2}>0\)、\(q=-B/(A+d)\)，则

\[
A+B\cos u=\frac{A+d}{2}(1-2q\cos u+q^2),\qquad |q|<1.
\]

由一致收敛的 Fourier 级数及余弦正交性，每个完整周期长度 (T\) 上，

\[
\int\frac{\cos(nu)}{A+B\cos u}du=\frac{Tq^{|n|}}d,
\]
\[
\int\ln(A+B\cos u)du=T\ln((A+d)/2),\qquad
\int\ln(A+B\cos u)\cos(nu)du=-\frac{Tq^{|n|}}{|n|}\quad(n\ne0).
\]

实现还允许端点为整数倍 \(\pi\) 的半周期，使用实际变量区间长度吸收线性换元。匹配谐波时验证整数阶及相位完全一致。对零阶单独处理，不除以零；对符号非零阶还须证明严格正的绝对阶数。

Poisson 情形 (A=1+r^2,B=-2r,|r|<1\) 直接得到 (d=1-r^2,q=r\)，保留简洁的符号 (r^n\)。`is_assumed_integer` 的直接测试确认追加不等式后仍保留整数类型；无整数条件、非整数频率、错误相位、含实极点的分母拒绝进入本路径。

对 (P\cos^2u+Q\sin^2u\)，直接读出 (A=(P+Q)/2,B=(P-Q)/2\)，不做通用三角展开。当正值条件成立时，均值写为

\[
2\ln\frac{\sqrt P+\sqrt Q}{2},
\]

从而打印更接近教科书的正值根式之和。常量缩放同样纳入均值。

## 化简资源限制

只含一个对数的短乘积，若对数参数含多个变量和根式，保留该对数为原子，对外部系数进行有理化简。这样不引入实占位变量，不更改复杂对数的分支，也不为了化简系数构造多变量代数扩张。[16个专项测试](benchmarks/simplify-algebraic-log-next-2026a.json)包含声明为复数的参数、不同系数、等价根式尺度、冷加载及eager binding、普通栈和64 KiB栈。

这是一条有边界的优化，不能证明所有 `simplify` 路径都不会触发实机 TLB。更深的通用代数扩张、计算前参数求值，以及 D1/D2/D4 的现有小栈失败仍需继续处理。
