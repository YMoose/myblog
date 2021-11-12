# [google Sanitizer](https://github.com/google/sanitizers)
## AddressSanitizer
AddressSanitizer (aka ASan) is a memory error detector for C/C++. Think Valgrind but much faster.  
AddressSanitizer包含两个部分：
- 编译时插桩模块：修改源代码（目前的实现依赖于LLVM编译框架）
  - 实现可以监控内存访问的shadow state
  - 在栈上创建隔离区来检测内存泄漏
- 运行时库：替换了malloc、free等相关内存操作函数，新函数会在分配出的堆附近创建隔离区，以实时检测非法堆内存的使用
### related work
#### shadow memory 影子内存
用额外一块内存（称为影子内存）区域来映射存储所用到的内存的元数据
#### instrumentation 插桩
内存泄漏检测所用的插桩通常指编译时插桩
#### debug allocators 替换内存分配器
有两种方案
1. 利用CPU页保护机制，针对跨页溢出在缺页时中断时检测报错
2. 在内存申请和释放的内存区域周边加入隔离区，并根据不同的操作将魔数写入到隔离区中，在之后检测隔离区的数值是否合法
#### 操作系统层面
金丝雀机制
### AddressSanitizer算法实现
用（特殊压缩过的）shadow memory记录应用程序运行时的内存是否可安全访问并用插桩技术来记录shadow memory。
#### shadow memory
通常情况下，malloc函数至少会和8 bytes对齐。那么就可以针对这个最小单位进行追踪，这8 bytes可能存在9种情况，前k(0 <= k <= 8)byte可以使用的而剩下的8-k个byte不可访问，这9种状态可以用单个byte记录，这构成了内存映射而成的shadow memory。