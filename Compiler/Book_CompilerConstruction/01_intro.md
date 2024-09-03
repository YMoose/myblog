# Intro
## Compilers History
1. 最初是机器码的二进制纸袋
2. 汇编语言->汇编器->二进制
3. 高级语言FORTRAN：编译器的开发过程中一些编译原理的知识还没有得到总结和研究
4. Chomsky研究了自然语言的结构，针对语言的语法复杂程度进行了分类，提出了Chomsky hierarchy，其中type2（context-free 上下文无关）语法被证明是最有用的编程语言。有穷自动机和正则表达式是type3类语法
5. 解析问题得到了更完善的解决，并逐步成为了编译器中的标准部分。
6. 为了追求更好的编译和执行效率，研究人员不断地研究和提高优化技术（代码提升技术）
7. 研究人员依据逐步完善的解析问题解决方案开发出了解析器生成器（比如：yacc）
8. 另一方面，研究人员对有穷自动机的研究逐步深入，开发出了另一种工具——扫描器生成器（比如：Lex）
9. 近来，一部分编译器发展了一些对代码的分析简化技术，同时集成进了IDE
## 与编译器相关的程序
### 1. 解释器 iterpreters
没有生成目标代码的步骤，直接翻译执行。通常适用于代码常常修改调整的教学和软件开发场景。
### 2. 汇编器 assemblers
将汇编代码转换为目标机器码
### 3. 链接器 linkers
编译器和汇编器都需要链接器的参与，链接器将目标文件中分散的目标代码片段进行组合填充为一个真正的可执行文件
### 4. 加载器 loaders
加载器解析**可重定位代码**，找到可执行文件的起始点并开始执行。
### 5. 预处理器 preprocessors
对代码文件的预处理，包括处理注释、头文件、**宏**等
### 6. 调试器 debuggers
一般来说，会和编译器一起提供，编译器会保留一些信息（符号信息）给调试器调试时使用，以帮助更好的获取运行时程序的状态信息
### 7. profilers
profilers会采集运行时的统计信息帮助分析
## translation process
![编译过程](pics\figure1_1_the_phases_of_a_compiler.png)
1. scanner：输入源代码->进行词法分析（lexical analysis）->生成tokens，同时会需要将一些数据写入到符号表和字面量表中
2. parser：输入tokens->进行语法分析（syntax analysis）->生成parsing tree后优化为语法树（syntax tree）\抽象语法树（abstract syntax trees）
3. semantic analyzer：输入syntax tree->根据语言的定义分析静态语义（执行优先级、声明和类型检查）->带有属性标记的语法树，同时会把属性加入到符号表中
4. source code optimizer：输入带有属性标记的语法树->基于源码分析生成中间码（通常是三地址码 three-address code 或 P-code）并对其进行优化->intermediate representation (IR)
5. code generator：输入IR->根据目标机器的指令集架构（ISA）生成目标代码（target code）->目标机器码（target code）
6. target code optimizer：输入目标机器码->分析目标机器码并优化（高效指令替换低效指令、访存效率提升等）->优化后的目标机器码（target code）
