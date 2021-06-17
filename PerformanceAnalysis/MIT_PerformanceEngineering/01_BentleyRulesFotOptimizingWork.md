# 01_BentleyRulesFotOptimizingWork
<Writing Efficient Programs> - Jon Louis Bentley

## Data Structures
1. Packing and encodign: 压缩数据空间以减少内存访问
2. Augmentation: 添加数据结构元素以加速某些常用操作(如添加链表尾指针以加速链表添加操作)
3. Percomputation&Compile-time initialization: 预计算以节约时间(可以利用元编程在编译阶段进行预计算)
4. Software Cache: 存储计算的输入与结果，以便之后相同的输入时可以从cache中快速获得结果
5. Sparsity: 利用CSR(稀松矩阵压缩存储法，可以拓展到图等数据结构)压缩数据大小
## Loops

## Logic
1. Constant Folding and Propagation: 编译器在const变量计算中复用之前计算的结果
2. Common-subexpression elimination: 编译器
## Functions