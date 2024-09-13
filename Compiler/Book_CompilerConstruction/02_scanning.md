# Scanning
## token
token record
attributes:
1. string value\lexeme：字符串值\词素
## regular expression
正则运算优先级：* > concatnate > |
## 语言中需要用正则识别的token类型
1. 数字
2. 关键字
3. 识别符
4. 注释（C的比较复杂，在scanner里通常使用ad hoc method解决）
5. token delimiter 分隔符

另外一些可能出现歧义的情况，会应用两个基本原则
1. 关键字优先原则：都满足关键字和识别符的条件时（例如：C语言中的if），优先认定为关键字
2. 最长字串原则：可以是一个或多个token时，优先认定一个token
## regular expression and Finite Automaton 有限自动机
Deteministic Finite Automaton DFA 确定有限自动机可以描述一条正则表达式来识别一种类型的token
Scanner需要将多个DFA合并以扫描识别不同种类的tokens，如果不同DFA存在相同的状态转移函数，那么就可以将其合并。
为了能供合并，需要引入空字符串转换（ε转换）和选择（多条转换）（即一个状态转移函数的结果是一个集合），这就是nondeteministic finite automaton(NFA)
## 从正则到DFA
### regular expression -> NFA
这步使用的是大名鼎鼎的Thompson's construction algorithm 汤普森构造算法。其使用空字符串转换来将正则的操作转换到NFA上。
遵循两个基本规则
1. 每个状态最多有两个状态转移函数（最多两个出边），如果有两个状态转移函数，两个都是空字符串转换（ε转换）
2. 状态一旦生成不能删除，且除了到结束状态的状态转移函数外，原状态转移函数不变
以下是正则转换的说明
1. 基本正则：接受一个字符集中的字符、空字符串（ε）或者空集
2. 连接操作：通过空字符串转换（ε转换）将前一个正则r的结束状态连到后一个状态s的起始状态
3. 选择操作：添加一个共有的起始状态通过空字符串转换（ε转换）分别连到两个正则的起始状态再通过空字符串转换（ε转换）将两个正则的结束状态连接到新的共有的结束状态
4. 重复操作：添加一个起始状态通过空字符串转换（ε转换）连到原正则r起始状态，再通过空字符串转换（ε转换）将原正则r的结束状态连到其起始状态，再添加一个结束状态将原正则的结束状态连到新结束状态，最后将新起始状态和新结束状态用空字符串转换（ε转换）连起
### NFA -> DFA
将NFA转换为DFA事实上就是消除空字符串转换和多条转换，也就是将多条转换函数到达的状态集合缩减，和空字符串转换到达的状态集合缩减
1. eliminating ε-transitions 去除空字符串转换
2. eliminating multiple transitions 去除多条转换


subset construction algorithm 子集构造算法
the ε-closure of a state: 指的是指定状态可以通过空字符转换到达的状态的集合（包括当前指定状态和多次空字符转换）
the ε-closure of a set of states: 状态机中所有状态的ε-closure的并集
1. 计算开始状态的ε-closure，得到DFA的开始状态集合Mstart
2. 将Mstart中包含的非ε转换的转换函数列出，相同的转换条件可以得到一个转换后的状态子集Msub_temp将Msub_temp和其ε-closure取并集得到Msub，如果Msub集合中包含结束状态，则Msub就是一个结束状态
3. 用Msub往复第二步直到没有非ε转换的转换函数

最小化DFA
自动机理论中可以论证对于给定的任意一个DFA，有且仅有一个等价的拥有最少状态的DFA存在
1. 假设两个状态集合：Sa：包含了所有的结束状态；Sna：包含了所有的非结束状态
2. 对状态集合中状态转移函数的字符集合中的字符逐条分析，对于任意一个字符a，如果两个状态都有a的状态转移函数（包括转移到error状态的）且状态转移函数的转移目的状态一致，则两个状态集合等价，否则不等价，将不等价的状态与当前状态区分使其为一个新的状态集合
3. 对新的状态集合继续进行步骤二

## 用代码表示DFA
1. 使用跳转表示状态转换（状态隐含于代码中）
2. 使用变量表示状态，switch-case语句表示状态转换（状态显义于变量）
3. 在用变量表示状态的基础上，将状态转换函数转换为表格形式（状态转移表）
## scanner 开发流程
1. 添加特殊符号（+-*/()）的DFA
2. 添加数字的DFA
3. 添加注释、空白符和赋值的DFA
4. 其他一般会先识别为标识符，然后再到查看是否在关键字列表中（查找算法很关键，hash通常使用minimal perfect hash function）；另一种方法是提前放在符号表中，这样就只需要维护一张表。lex就没法做到这点，一般这种方法在真实的编译器中使用的更多，因为关键字的识别会导致lex的代码剧增。
## 使用lex
### lex 输入文件
``` lex
{definitions}
%%
{rules}
%%
{auxilizry routines/ user routines}
```
#### definitions
1. %{任何需要被插入的外部C函数%}
2. 正则表达式定义
#### rules
包括正则匹配命中后操作的C代码
#### auxilizry routines / user routines
rules部分中调用的函数实现和其他未定义的函数实现
