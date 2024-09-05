# Intro
## 1. Compilers History
1. 最初是机器码的二进制纸袋
2. 汇编语言->汇编器->二进制
3. 高级语言FORTRAN：编译器的开发过程中一些编译原理的知识还没有得到总结和研究
4. Chomsky研究了自然语言的结构，针对语言的语法复杂程度进行了分类，提出了Chomsky hierarchy，其中type2（context-free 上下文无关）语法被证明是最有用的编程语言。有穷自动机和正则表达式是type3类语法
5. 解析问题得到了更完善的解决，并逐步成为了编译器中的标准部分。
6. 为了追求更好的编译和执行效率，研究人员不断地研究和提高优化技术（代码提升技术）
7. 研究人员依据逐步完善的解析问题解决方案开发出了解析器生成器（比如：yacc）
8. 另一方面，研究人员对有穷自动机的研究逐步深入，开发出了另一种工具——扫描器生成器（比如：Lex）
9. 近来，一部分编译器发展了一些对代码的分析简化技术，同时集成进了IDE
## 2. 与编译器相关的程序
### 2.1. 解释器 iterpreters
没有生成目标代码的步骤，直接翻译执行。通常适用于代码常常修改调整的教学和软件开发场景。
### 2.2. 汇编器 assemblers
将汇编代码转换为目标机器码
### 2.3. 链接器 linkers
编译器和汇编器都需要链接器的参与，链接器将目标文件中分散的目标代码片段进行组合填充为一个真正的可执行文件
### 2.4. 加载器 loaders
加载器解析**可重定位代码**，找到可执行文件的起始点并开始执行。
### 2.5. 预处理器 preprocessors
对代码文件的预处理，包括处理注释、头文件、**宏**等
### 2.6. 调试器 debuggers
一般来说，会和编译器一起提供，编译器会保留一些信息（符号信息）给调试器调试时使用，以帮助更好的获取运行时程序的状态信息
### 2.7. profilers
profilers会采集运行时的统计信息帮助分析
## 3. translation process
![编译过程](pics\figure1_1_the_phases_of_a_compiler.png)
1. scanner：输入源代码->进行词法分析（lexical analysis）->生成tokens，同时会需要将一些数据写入到符号表和字面量表中
2. parser：输入tokens->进行语法分析（syntax analysis）->生成parsing tree后优化为语法树（syntax tree）\抽象语法树（abstract syntax trees）
3. semantic analyzer：输入syntax tree->根据语言的定义分析静态语义（执行优先级、声明和类型检查）->带有属性标记的语法树，同时会把属性加入到符号表中
4. source code optimizer：输入带有属性标记的语法树->基于源码分析生成中间码（通常是三地址码 three-address code 或 P-code）并对其进行优化->intermediate representation (IR)
5. code generator：输入IR->根据目标机器的指令集架构（ISA）生成目标代码（target code）->目标机器码（target code）
6. target code optimizer：输入目标机器码->分析目标机器码并优化（高效指令替换低效指令、访存效率提升等）->优化后的目标机器码（target code）
## 4. Data Structures In A Compiler
1. tokens: 文本识别后的最小单元，同时会包含文本、位置信息、类型信息等属性
2. the syntax tree: 对token组成的单元的抽象，不同的节点中会包含一些不同属性
3. the symbol table: 通常用hash表来保存标识符、变量、常量和数据结构的相关信息。几乎参与到整个编译过程中。
4. the literal table: 通常用hash表来保存字面量，例如字符串、常量等，不可删除。
5. intermediate code: 通常使用三地址码或P-code形式来表示
6. temporary files: 以往会因为内存不足而使用临时文件保存一些中间结果，比如代码生成时的回填地址（backpatch addresses）
## 5. Compiler Structure
编译器从不同的视角看有不同的结构和组成部分
### 5.1. Analysis and Synthesis
analysis: 分析源代码得出程序作用
synthesis: 综合程序作用生成目标代码
### 5.2. Front End and Back End
front end: 分析源代码，生成中间代码（intermediate code）或抽象语法树（abstract syntax tree）等中间表示（intermediate representation）
back end: 综合中间表示生成目标代码
### 5.3. Passes 
在生成目标代码，甚至中间代码前，编译器倾向于多次扫描处理源代码。每过一遍源代码称为一次pass。
### 5.4. Language Definition and Compilers
language reference manual（language definition）：用正则表达式和上下文无关语法描述的语言规范，其逻辑体现在词法分析和语法分析中。还有事用数学语言对语言语义的形式化描述（方法有Denotational Semantics、Operational Semantics、Axiomatic Semantics）。
runtime environment：运行时环境的支持度很大程度地影响了对语言和编译器实现的复杂度
### 5.5. Compiler Options and Interfaces
提供给用户的编译器选项和编译器使用的接口
### 5.6. Error Handling
反映源代码中存在的错误
一般可以分类为两类错误，语法错误和语义错误



