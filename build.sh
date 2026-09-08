#!/bin/bash
# ============================================================================
# build.sh — KhiCAS 优化构建脚本(Linux / WSL2)
#
# 功能:拉取工具链依赖 → 应用优化配置(Makefile.opt)→ 构建 → 输出产物
# 用法:
#   ./build.sh            # 完整构建(下载依赖 + 构建 khicas50)
#   ./build.sh fast       # 仅构建(依赖已就绪)
#   ./build.sh clean      # 清理构建产物
#
# 环境变量:
#   TOOLS_DIR 依赖安装目录(默认 $HOME/khicas-toolchain)
# ============================================================================
set -euo pipefail

REPO_DIR="$(cd "$(dirname "$0")" && pwd)"
BASE_URL="https://www-fourier.univ-grenoble-alpes.fr/~parisse/casio"
TOOLS_DIR="${TOOLS_DIR:-$HOME/khicas-toolchain}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

info()  { echo -e "\033[1;34m[build]\033[0m $*"; }
ok()    { echo -e "\033[1;32m[ok]\033[0m $*"; }
die()   { echo -e "\033[1;31m[error]\033[0m $*" >&2; exit 1; }

# 带重试的下载
download() { # $1=url  $2=输出文件
    local url="$1" out="$2"
    for i in 1 2 3 4 5; do
        if curl -fL --retry 3 --retry-delay 5 --ssl-no-revoke -C - --max-time 540 -o "$out" "$url"; then
            return 0
        fi
        info "下载失败(尝试 $i/5),3 秒后重试: $(basename "$out")"
        sleep 3
    done
    die "下载失败: $url"
}

# --- 1. 平台检查 -----------------------------------------------------------
if [ "$(uname -s)" != "Linux" ]; then
    die "需要 Linux 或 WSL2(sh3eb-elf 工具链为 Linux 二进制)"
fi

# --- 2. 下载依赖 -----------------------------------------------------------
mkdir -p "$TOOLS_DIR"
cd "$TOOLS_DIR"

if [ ! -x "$TOOLS_DIR/casiolocal/opt/sh3eb-elf/bin/sh3eb-elf-gcc" ] \
   && [ ! -x "$TOOLS_DIR/opt/sh3eb-elf/bin/sh3eb-elf-gcc" ]; then
    info "下载 casiolocal(工具链 + 库,约 231MB)..."
    download "$BASE_URL/casiolocal.tgz" casiolocal.tgz
    info "解压 casiolocal..."
    tar xzf casiolocal.tgz
    rm -f casiolocal.tgz
else
    info "casiolocal 已就绪"
fi

# --- 3. 探测工具链与库路径 -------------------------------------------------
SH3BIN=""
for cand in \
    "$TOOLS_DIR/casiolocal/opt/sh3eb-elf/bin" \
    "$TOOLS_DIR/opt/sh3eb-elf/bin"; do
    if [ -x "$cand/sh3eb-elf-gcc" ]; then SH3BIN="$cand"; break; fi
done
[ -n "$SH3BIN" ] || die "未找到 sh3eb-elf-gcc(请检查 casiolocal 解压结构)"
export PATH="$SH3BIN:$PATH"
info "工具链: $SH3BIN ($(sh3eb-elf-gcc --version | head -1))"

CASIOLOCAL=""
for cand in \
    "$TOOLS_DIR/casiolocal" \
    "$TOOLS_DIR/casio" \
    "$HOME/casiolocal" \
    /home/parisse/casiolocal; do
    if [ -d "$cand/include/ustl" ] && [ -d "$cand/lib" ]; then
        CASIOLOCAL="$cand"; break
    fi
done
if [ -z "$CASIOLOCAL" ]; then
    # libfxcg.tgz 内可能包含 include;尝试从其解压位置寻找
    for cand in "$TOOLS_DIR/libfxcg" "$TOOLS_DIR/libfxcg/../casiolocal"; do
        [ -d "$cand/include" ] && [ -d "$cand/lib" ] && CASIOLOCAL="$cand" && break
    done
fi
[ -n "$CASIOLOCAL" ] || die "未找到 casiolocal 库目录(include/ustl + lib)"
info "库目录: $CASIOLOCAL"

# --- 4. mkg3a 打包器 -------------------------------------------------------
if ! command -v mkg3a >/dev/null 2>&1; then
    info "构建 mkg3a..."
    download "$BASE_URL/mkg3a.tgz" mkg3a.tgz
    tar xzf mkg3a.tgz
    rm -f mkg3a.tgz
    cd mkg3a/build
    cmake .. >/dev/null
    make -j"$JOBS" >/dev/null
    sudo make install >/dev/null 2>&1 || make install PREFIX="$HOME/.local" >/dev/null
    cd "$TOOLS_DIR"
    if ! command -v mkg3a >/dev/null 2>&1; then
        export PATH="$HOME/.local/bin:$PATH"
        command -v mkg3a >/dev/null 2>&1 || die "mkg3a 安装失败"
    fi
    ok "mkg3a 就绪"
else
    info "mkg3a 已就绪"
fi

# --- 5. 进入源码目录构建 ---------------------------------------------------
cd "$REPO_DIR/giacbf"

if [ "$#" -ge 1 ] && [ "$1" = "clean" ]; then
    make clean
    exit 0
fi

# 应用优化配置(若 Makefile 已是优化版则跳过)
if ! grep -q "Makefile.opt 优化构建" Makefile 2>/dev/null; then
    info "应用优化构建配置(Makefile.opt)..."
    cp "$REPO_DIR/Makefile.opt" Makefile
fi

info "开始构建(khicas50.g3a + khicas50.ac2, 优化配置)..."
info "  计算模块: -O3 -finline-functions | GUI/IO: -Os | 函数分段: 开"
make CASIOLOCAL="$CASIOLOCAL" -j"$JOBS" khicas50.g3a khicas50.ac2

echo
ok "构建完成!产物:"
ls -la khicas50.g3a khicas50.ac2 2>/dev/null
echo
info "下一步:"
info "  1. 用 fx-CG Manager 模拟器测试(或复制到计算器)"
info "  2. 跑基准脚本 bench/bench.xws 对比速度"
info "  3. 查看 khicas.map 检查内存占用"
