# lab0.实验环境准备及基础知识学习
## C内联汇编
1. 汇编无外部联系
```
__asm__
```
2. 汇编有外部联系
因为有外部联系所以要有约束条件
```
__asm__ __volatile__(
    "rep ; movsl\n\t"  // 指令部()
    "testb $2, %b4(%和数字表示需要使用寄存器的样板操作数，%和%寄存器名字表示使用寄存器，操作数的编号从输出部的第一个约束(序号为0)开始，可以用%[bh][1-9]表示某字节，如果输入部使用和输出部相同的约束条件，则在输入部队约束条件""中填写操作数的编号)
    :  "=&c"(d0) // 输出部(输出约束以=开头，然后是表示操作数类型的字母，括号里是最终输出到什么变量的约束)
    :   // 输入部(表示操作数类型的字母，括号里是替代的变量)
    :   // 损坏部(记录上述过程中未在输出部和输入部出现的并需要恢复执行前的值的操作数，告诫编译器假设此约束条件上的值已于之前不同)
)
```
### 约束条件
|字母|	含义|
| - | - |
|m, v, o | s内存单元|
|R | 任何通用寄存器|
|Q | 寄存器eax, ebx, ecx,edx之一|
|I, h | 直接操作数|
|E, F | 浮点数|
|g | 任意(当gcc选用任意寄存器时，会给此寄存器插入现场恢复的指令)|
|a, b, c, d | 寄存器eax/ax/al, ebx/bx/bl, ecx/cx/cl或edx/dx/dl|
|S, D | 寄存器esi或edi|
|I | 常数（0～31）|