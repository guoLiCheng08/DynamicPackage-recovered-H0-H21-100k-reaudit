# `matrix2quat` 审计状态

原 ELF `matrix2quat` 的完整反汇编已保存为 `matrix2quat_abi_audit.txt`。当前可见其接受 RDI 所指向的 `DpQuatAbi` 输出对象，并在多个 trace/对角线最大分支中从矩阵元素构造四元数；各分支包含 SSE sqrt 正常路径与 libc `sqrt` 回退。由于完整输入矩阵 descriptor 的入参寄存器、分支判定顺序、NaN/退化矩阵写回与输出 descriptor 指针语义尚未完成分段审计，本阶段未建立金标，未写入恢复端，未纳入自检。

后续工作应先阅读反汇编起始段和所有调用点，精确恢复输入矩阵参数；之后以旋转单位阵、各轴半转和非退化旋转矩阵建立分支金标，最后再实现。

已建立首个直接原 ELF probe：`matrix2quat(DpQuatAbi *out, const DpMatrix *input)` 对 3×3 单位阵安全执行，输出 `w=1`、xyz=`[0,0,0]`。quat descriptor 中 `xyz.data` 指针是调用期地址，后续比较须只比较 w、元数据和独立 xyz backing。单位阵 quat/xyz 金标 SHA-256 分别为 `5c696f5b26d4031d69a28bbd8e5c5d767ddcbc4cd1c2deed334d83b7c59e23fe`、`9d908ecfb6b256def8b49a7c504e6c889c4b0e41fe6ce3e01863dd7b61a20aa0`。这仅覆盖 trace 主分支，尚不足以实现。

已采集三个 180° 轴旋转分支：diag `[1,-1,-1]`、`[-1,1,-1]`、`[-1,-1,1]` 分别产生 xyz `[1,0,0]`、`[0,1,0]`、`[0,0,1]`（w 为 0）。聚合 xyz SHA-256 为 `1332df685c45a61a0ac008a6a653295b8c5655361c1d34b6304b22ec40235fb8`。这些边界矩阵验证了三个对角线选择分支，但尚缺非退化混合旋转、负 trace 近退化和 NaN 处理，故仍未实现。

补充非退化正 trace 矩阵 `[[0,-1,0],[1,0,0],[0,0,1]]`：原 ELF 输出 `w=0.7071067811865476`、xyz=`[0,0,-0.7071067811865475]`。这固定了当前矩阵行主序约定下 Z 分量符号，quat/xyz 金标 SHA-256 分别为 `2450d62d001dca8f3f3f1324d00c3e0a780a6b81ea2f11bc72cb356d24e61dfa`、`83e12fcf1a8935de66a56ed4a166c963459a7834b8d75252a5fd5f2d69a167bc`。
