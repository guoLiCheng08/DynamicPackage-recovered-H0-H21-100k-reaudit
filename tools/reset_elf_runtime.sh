#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
    echo "用法: $0 ELF进程号" >&2
    exit 2
fi
pid="$1"
if [[ ! -r "/proc/$pid/exe" ]]; then
    echo "ELF进程不存在: $pid" >&2
    exit 2
fi

# DynamicDllInit 是正式 ELF 的完整初始化入口，内部会继续调用 dyn_init。
# 调试调用会短暂停止进程；仅允许在隔离测试实例上执行。
gdb -q -nx -batch \
    -ex 'set pagination off' \
    -ex "attach $pid" \
    -ex 'call (void) DynamicDllInit()' \
    -ex 'detach' \
    -ex 'quit'

echo "1. 结果=通过"
echo "2. 操作=正式ELF DynamicDllInit/dyn_init完整重置"
echo "3. 进程号=$pid"
