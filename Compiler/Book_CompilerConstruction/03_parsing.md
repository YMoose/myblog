# parsing
通常语法分析基于给定的一套上下文无关语法的语法规则
上下文无关语法与正则不同的是上下文无关语法包含了递归规则，小小的改变带来的变化是巨大的
首先，数据结构就需要从正则使用的线性的tokens数组变为parse tree/syntax tree
parsing的算法可分为top-down parsing and bottom-up parsing
与scanner不同的另一点是需要错误处理（报告与恢复）
## 上下文无关语法 context-free grammar (CFG)
基本的语法格式是Backus-Naur form BNF
``` BNF
exp -> exp op exp | (exp) | number
op -> {+|-|*}
```
推导（derivation）指通过一系列替换操作从一个开始符号推导出一个句子的过程。一次替换操作只能应用一条语法规则。
终结符号：组成句子（串）的基本符号（与tokens含义相同）是推导的终结。于上面的例子就是 number、（和）
非终结符号：表示用于由文法生成的句子的集合，非终结符号可以由终结符号和其它非终结符号组成，最终推导至全为终结符号。于上面的例子就是 exp 和 op。
开始符号（start symbol）：一个特殊的非终结符号，表示文法生成的最基本的句子的集合（即文法的定义的最基本语言）。于上面的例子就是 exp。
由上下文无关文法语言的定义如下
**L(G) = {s | exp =>* s}**
L(G) 指由语法G定义的语言，上下文无关语法由多个产生式（production）组成。开始符号通常是
s 指由token 组成的句子（sentence）
exp 是开始符号
=>* 指推导的过程（星号意味着可能是多个推导的步骤）
产生式式的上下顺序决定了推导时选用产生式的优先级
对上下文无关语法更形式化的定义是
**G = (V, T, P, S)**
1. A set T of **terminals**
2. A set N of **non-terminals**
3. A set P of **productions(grammar rules)**, of the form A -> a, where A ∈ N and a ∈ (T ∪ N)*
4. A **start symbol** S from the set N.
最左推导（leftmost derivation）指每次推导优先替换最左边的非终结符号。对应了前序遍历（preorder traversal）的语法树。
最右推导（rightmost derivation）指每次推导优先替换最右边的非终结符号。对应了后序遍历（postorder traversal）的语法树。

## 解析树和抽象语法树
一个解析树（parse tree）对应一整个推导过程，其叶子节点是终结符号，非叶子节点是非终结符号，非叶子节点的数量也就对应着推导的次数。
parse tree存在一种更为简明的表示方式，抽象语法树（abstract syntax tree, AST），parser会遍历parse tree显示的所有推导过程，并生成最终的AST。
- parse tree：表示concrete syntax
- syntax tree：表示abstract syntax，所以信息更少
## 二义性语法
语法可能具有二义性（ambiguity），使得推导过程存在多种方式，最终生成的不同的parse tree和AST，可能导致句子具有不同的含义，也会在后续介绍的解析算法中失败，所以具有二义性的语法被视为不完善的语法，需要在语言设计阶段进一步消除二义性。
已经存在一系列的方法来消除二义性，这里介绍两个基本方法。
1. 方法一：给定一套规则，当存在二义性的情况时，说明哪个是正确的推导，好处是不用改变现有语法规则，坏处是语法结构的说明变复杂了，不再单独由语法规则给出。
2. 方法二：改变原有语法规则，改变了语法规则实际就改变了语言本身的含义，所以通常不采用这种方法
### 运算优先级（precedence）和结合性（associativity）
可以通过产生式的顺序来修正由运算优先级（precedence）导致的二义性
对于运算优先级可以使用优先级级联（precedence cascade），优先级级联是将不同优先级的操作打包并将优先级低的运算符产生式放置于高优先级之前，使其更接近于parse tree的root，更晚被执行的一种标准操作
在同一个优先级的操作符中需要考虑结合性（associativity）问题
左结合性是指当算子左右两边都有运算符时，算子优先属于左侧的运算符。
左递归产生式是产生式最左边的符号和产生式头部的非终结符相同。
一个左递归产生式使得运算符具有左结合性
一个右递归产生式使得运算符具有右结合性
### 悬挂else问题 the dangling else problem 
使用最近嵌套规则，需要在语法上描述一个中间状态（like：unmatched-stmt）产生式并放置在多个产生式中合理的位置来解决此类二义性。有些语言也会使用一个结束词（脚本里使用fi）
### 非本质二义性
如果一个二义性不影响语言语义的本质，则其为非本质二义性（比如可以应用结合律的+，和；分隔的句子，交换顺序后不影响语义的表达）
## EBNF 扩展巴科斯范式
相较于BNF，EBNF添加了重复的语法元素
``` EBNF
BNF  左递归重复: exp -> exp addop term | term
EBNF 左递归重复: exp -> term { addop term }
BNF  选择: exp -> exp addop term | term
EBNF 选择: exp -> [term addop ] term
```
EBNF还可以使用语法图表示