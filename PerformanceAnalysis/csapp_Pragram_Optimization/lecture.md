1. 看一遍课程，有不懂的看书上又没有更详细的说明，再没有查看一下别的网络资料
2. 
# Program Optimization
## 
前提：程序的算法已经确定
通常编译器有一整套优化策略，但如果编译器无法确定一部分代码的语义，则不会对这部分代码进行优化
## Generally Useful Optimizations
编译器之所以不能对这些代码进行优化，一部分原因是因为其无法确定编码者的意图(参数是否在循环过程中变动(内存别名)、函数是否是动态库函数(函数调用的副作用))
### 1. code motion
将无用代码（运算、函数、取存）从循环中移除
``` C
// 例1
int n = 5;
int i = 0, j = 1;
for (i = 0; i < n; i++)
    a[n * i + j] = b[j]; // n * i完全可以移除循环 
```
``` C
// 例2
for (i = 0; i < strlen(s); i++)
    if (s[i] >= 'A' && s[i] <= 'Z')
        s[i] -= ('A'- 'a');
```
### 2. Reduction in Strength
将耗时多的操作符(*)改为耗时少的(<<)
```  C
// int ni = n * i;
for (i = 0; i < n; i++) {
    int ni = n * i; // * 换 +
    for (j = 0; j < n; j++){
        a[ni + j] = b[j];
    }
    //ni += n;
}
```
### 3. Share Common Subexpressions
提取共有的运算（某些场景下 gcc -O1会自动进行这部分优化）
``` C
// 矩阵中相邻的四个点
```
### 4. Cache friendly
#### 4.1. Spatial locality
改善多层循环最内层循环的存储访问缓存友好性(比如循环层次可以从ijk->kij) 
#### 4.2. Temporal locality
block 块访问技术
## Instruction-Level Parallelism
现代处理器乱序执行，性能尝尝受限于数据读写
CPE
关键路径 分离顺序指令的依赖数据 循环展开loop roll

结合律变换
## Vector Instructions
avx512
## Branches 
