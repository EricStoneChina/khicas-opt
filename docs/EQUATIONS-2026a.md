# 方程转换、化简与参数式显示 checkpoint

本轮基于积分 checkpoint `f8af451`，积分算法源码暂不改动，单独保存方程工作。
计算器当前仍是 `895c848`；本轮没有向 USB 设备写入文件。

## 行为变化

- 等式的非零有理数公因子可以去除。例如
  `simplify(-27*x^3+81*x*y-27*y^3=0)` 得到 `x^3-3*x*y+y^3=0`。
  保留变量因子与含变量分母的定义域，`2*x*y=0` 不会丢掉任一条直线。
- 伯努利双纽线和笛卡尔叶形线优先使用紧凑参数式、极坐标式，
  检查零系数退化与未知参数因子，保留缺失的点或直线分支。
- 对可解出任一坐标的普通方程先做直接代换，避免不必要的高次求根。
  有理直线束识别使用次数、项数和工作量预算。
- 极坐标奇次根采用实根，保护 `sign(g)*abs(g)^(1/n)` 在零点的含义，
  外层 `simplify` 不再将这些表达式送入造成小栈崩溃的常规变换。
- 参数式的数学排版加上坐标与参数名称，底层坐标列表仍可用于绘图。
  具体调用和显示范围见 [CONVERSIONS.md](CONVERSIONS.md)。

## 验证范围

主机测试使用仓库的转换、归一化和实际 FXCG 化简入口，其余依赖为主机 Giac。
64 KiB 计算栈带 4 KiB guard page；这不是 SH4/MMU 模拟器。
主机运行时间和进程 RSS 不能当作 CG50 的时间、峰值堆内存或 TLB 故障诊断。

```sh
python3 tests/run-equation-extended.py
python3 tests/run-equation-stack.py --target-simplify --report STACK.json
python3 tests/run-simplify-nested.py
python3 tests/run-user-integrals.py --target-simplify --report INTEGRALS.json
TOOLS_DIR=/tmp/khicas-toolchain python3 tools/build.py optimized khicas50.g3a khicas50.ac2
python3 tools/check-resources.py --report RESOURCES.json
```

六个转换命令的代码移到 AC2 区，为主 ROM 留出空间；CAS 堆配置不变。
这只改变代码布局，不能据此声称计算速度提升或已经达到实机资源上限。
通用求解器仍有不能参数化的曲线，也没有用一个有理参数式覆盖任意隐式曲线的保证。

## 本轮最终检查

- 转换：23 个直角坐标精确分支检查、48 个实极坐标残差样本、8 个拒绝检查；
  紧凑极坐标形式及原有 36 例通过。
- 40 个转换／外层化简组合均通过 64 KiB 栈检查，报告
  [equation-stack-2026a.json](benchmarks/equation-stack-2026a.json)。
- 等式预算、112 个嵌套幂分支样本、448 个实奇次根样本及特殊函数回归通过。
- 原始 8 道积分仍全部精确，普通与小栈结果一致；原先对数三角积分故障回归通过。
  对应报告为 `user-eight-equations-2026a.json`、`simplify-safety-equations-2026a.json`。
- 带名称显示的数据结构、选择映射和原始分支的 `plotparam` 兼容性通过主机测试。
  SH4 完整编译通过，尚未验证计算器屏幕像素效果。

[最终空间报告](benchmarks/resources-equations-2026a.json)：

| 区域 | 使用字节 | 剩余字节 |
| --- | ---: | ---: |
| 主 ROM | 2,047,368 | 17,784 |
| 静态 RAM | 424,620 | 17,748 |
| AC2 | 2,363,100 | 196,896 |

相比积分 cycle 4，主 ROM 减少 928 字节、AC2 增加 16,620 字节，
静态 RAM 不变。搬移并不等于总代码减少：两段代码合计增加 15,692 字节。
37 个积分 helper 与六个转换入口的位置均已按 ELF／map 检查；CAS 堆仍为 1,572,864 字节。
