# Chapter1 Interoduction
## 1. Basic Terminology 基本术语
协议栈中的各层以如下方式简称
| 简称 | TCP/IP网络层 | OSI model |
| -- | --- | ------- | 
| L2 | link layer | synonym for Ethernet |
| L3 | network layer | IP Version 4 or 6(不讲) |
| L4 | transport layer | UDP,TCP, or ICMP |
其他简称
| 简称 | 内容 |
| ---- | --- |
| RX | Receiving a data unit |
| TX | Transmitting a data unit |
| BH | Bottom half(下半部分) |
| IRQ | Interrupt |
## 2. Common Coding Patterns 通用设计模式
因为每个网络功能，都或多或少的和别的内核模块有牵连。所以，他们都尽可能的遵循相似的结构去实现相似的功能。  
他们的实现都有一些通用的需求，包括  
1. 申请很多个相同数据结构实例的内存
2. 跟踪一个数据结构相关的资源，以防止错误的内存释放  

下面的子章节会暂时几个通用的技巧来解决这些需求。  
### 2.1) memory cache 内存缓存
给常常申请和释放的相同数据结构申请一块特殊的内存缓存区。在内核的网络部分代码有下列结构会实现一个内存缓存区
1. Socket buffer descriptors  
   由 */net/core/skbuff.c* 文件中的 `skb_init` 函数申请所得
2. Neighboring protocol mappings  
   每个Neighboring protocol使用一个内存缓存区来管理L3-to-L2地址映射
3. Routing tables  
   路由部分代码使用了两个内存缓存区来定义路由  

下面是实现内存缓存区的内核关键函数
```
kmem_cache_create
kmem_cache_destory
kmem_cache_alloc    // return a buffer to the cache.
kmem_cache_free     // kmem_cache_alloc & kmem_cache_free 通常会通过一个装饰器来调用，这个装饰器负责从更高level来管理内存的申请与释放。
```
内存缓存区可申请的实例数量，通常会被上述装饰器定死的，有时可以在 */proc* 里配置
### 2.2) caching & hash tables 缓存和哈希表
查找缓存时，常常会跟一个输入参数来决定这个缓存是否需要添加到缓存中。  
缓存通常会用hash table实现。内核提供了一系列的数据类型来构造hash tables，比如单/双向链表。
hash tables上链表的长度要权衡以平衡查找时间和冲突达到最好的性能。
在一些子系统，比如neighboring layer，在进行hash的时候，会加入一些随机的key以对hash value进行进一步的随机化分布。这样可以减少Dos攻击的的成功率。
### 2.3) reference count 关联计数器
为了使得垃圾回收更简单高效，多数数据结构会保留一个关联计数器。针对有关联计数器的数据类型，内核通常会有两个函数负责计数器的增减`xxx_hold`和`xxx_release`（有时`xxx_release`也会被改成`xxx_put`）  
使用关联计数器，也会造成一些问题：
- 如果你释放了一个数据结构，但是忘记调用`xxx_release`，内核就不会真正释放这个数据结构的内存（同样的，重复调用`xxx_release`也会造成问题）
- 如果你使用了一个数据结构，但是忘记调用`xxx_hold`，内核可能会在使用前将其释放。这个错误比上一个更为严重，直接蓝屏你敢信？

当一个数据结构被移除时，关联计数器的管理会被告知以减少与其关联的数据结构的关联计数器。这一切都是通过通知链完成的。  
通常遇到如下情况时，关联计数器会增加：
- 当两个数据结构有非常紧密的关系。在其中一个数据结构中保留着指向另一个数据结构的指针。
- 当句柄访问这个数据结构时，计时器开始计时。
- 当在hash tables或者链表中查找并返回一个正确的元素时。

当最后一个关联被释放，这个数据结构的内存可能也随之释放，因为他可能不再被使用。注意是“可能”哦  
对于新sysfs文件系统的介绍可能可以帮助你更深入地了解关联计数器及其管理的相关代码。
### 2.4) garbage collection 垃圾回收
因为内核没有使用虚拟内存，所以对内核来说内存十分珍贵。大多数内核模块都实现了某种程度上的垃圾回收机制（更详细地可以参考Ch7）。主要有以下两种模型：
- Asynchronous 异步
  - 这类回收机制，不由某种特殊事件触发。只是在一段时间后就会进行一次扫描与释放。
- Synchrnonou 同步
  - 当内存紧缺且等不到异步回收时，此类回收方式就会立即执行。不过这类回收机制对是否对某块内存进行释放的评判标准与异步回收不尽相同。
### 2.5) func points & virtual func tables(VFTS) 函数指针与虚函数表
使用函数指针可以使得C语言代码利用一些面对对象语言的编程方式。比如以下代码：
```
struct sock {  // 对象类型
    ...
    void    (*sk_state_change)(struct sock *sk);   // 对象方法1
    void    (*sk_data_ready)(struct sock *sk, int bytes);   // 对象方法2
    ...

};
```
函数指针有一个非常主要的优势————它可以依据不同的条件进行初始化，因此`sk_state_change`可以根据不同的socket调用不同的函数。
函数指针在内核网络实现中被广泛的运用。以下是一些例子：
- 当数据包被路由子系统接受或者发送，路由子系统会在缓冲区中初始化两个例程（详见ch35）。ch2中会有一个关于函数指针的详细列表。
- 当数据包已经准备好在网络硬件上传输时，数据包会被交给`hard_start_xmit`函数指针。会根据不同的网络设备进行不同的初始化。
- 当一个L3协议想要传输数据包，它会调用一系列函数指针。他们会被地址解析协议初始化为一系列例程。根据初始化后的函数，L3-to-L2的地址解析才能正确执行（比方说，ipv4会用arp）。当不需要地址解析时，另一个例程会被使用（详情见PART4）

接下来我们看一个更具体的例子。一个设备驱动向内核注册成为了一个网络设备需要经过很多一般性步骤（不管你是啥设备）。在某些时间点，内核会调用这个网络设备的某些函数指针来完成一些额外的工作。对这样的函数指针的初始化时由内核完成的，驱动代码是没有办法插手的。  
对函数指针调用前的检查时必不可少的。
```
if (dev->init && dev->init(dev) != 0) {
   ···
}
```
函数指针有个坏处就是它让看源代码更难了！当调用一个函数指针时，还得研究一下函数指针是咋么初始化的。这又得依据如下因素判断：
- 当函数指针的初始化依据数据类型（协议句柄、接受数据包的设备驱动）不同而不同时比较容易找到其源头。就找到对应数据类型所在的文件就行
- 当函数指针的初始化是基于比较复杂的逻辑时（比如L3-to-L2地址映射的解析状态），就蛋疼了，要仔细分析，要通过分析外部事件来搞。

有一系列的函数指针被打包在了一个虚函数表(Virtual Function Table, VFT)中，当VFT被当作两个子系统的接口使用时（比方说，L3和L4协议层）或者VFT被输出为通用内核构件接口时，VFS其中的函数指针数量就比较庞大了。每个功能可能只用到其中的一小块（详见PART4）。
### 2.6) goto statements
Linux中存在一些goto语法的使用。
因为C语言没有提供显式的异常（其他语言也因为性能和代码复杂度的问题一定程度上回避了这个问题），所以谨慎地使用goto语句可以更容易跳转到错误处理函数上。而错误也常常在内核运行时产生。
### 2.7) vector definitions
在一些数据类型定义中会在其尾部留一些可调整的空白。
```
struct abc {
    int age;
    char *name[20];
    ...
    char    placeholder[0];
}
```
这个可调整的空白以`placeholder`开始，`placeholder`定义为一个0字节长的数组，也就是说placeholder是一个指向这个数据类型末尾的指针，同时也是空白部分的开始。
如果这个数据结构被很多代码使用，但同时代码需要添加一些新的数据类型，就可以使用这个作为拓展（详见ch19）。
### 2.8) conditional directives(#ifdef and family) 条件编译
条件编译主要用来查看内核模块对模块的支持与否。
### 2.9) compile-time optimization for condition checks 对条件判断的编译时优化
多数情况，代码中的条件判断是可以被预测的。通过编译器的检查，可以在编译时就解决条件判断。  
如下代码可以捕获err：
```
err = do_something(x,y,z);
if (err)
    handle_error(err);
```
因为err不常发生可以优化为如下代码：
```
err = do_something(x,y,z);
if (unlikely(err))
    handle_error(err);
```
`likely(value)` \ `unlikely(value)`两个宏分别假定value常常为1\0。
在转发ip包时，会根据ch18中的规则对其进行处理，最终会由函数`ip_forward_finish`进行处理，这个函数就用了`unlikely(value)`宏(详情见ch20)。
### 2.10) mutual exclusion 互斥
在linux内核中比较常见的互斥实现手段包括以下三种：
- Spin locks 自旋锁  
  自旋锁只能被一个线程所持有，其他想要获取自旋锁的进程会在循环中等待锁被释放。因为循环造成的损耗，自旋锁只能被用在多处理器系统，进一步来说，只能被用在锁只能被持有一小段时间的代码中，而且持有锁的进程不能休眠。
- Read-write spin locks 读写锁  
  当在使用中可以将情况分为读写与只读时，使用读写锁是一个不错的选择。
  当前锁状态 | 读锁请求 | 写锁请求
  --------- | -------- | -------
  无锁|可以|可以
  读锁|可以|阻塞
  写锁|阻塞|阻塞
- Read-Copy-Update (RCU)  
  RCU是linux近期才有的一种互斥机制。当满足如下条件时，RCU是个不错的选择：
  - 写锁请求比读锁请求少很多
  - 持有锁的代码自动执行不会休眠
  - 被锁保护的数据结构是通过指针传递的 
   
  判断RCU是否更合适需要考虑很多因素，包括SMP系统上处理器cache的效率。  
  在linux内核网络模块中的路由子系统中有使用RCU。查找路由表比更新路由表更为频繁，而且查找路由表的实现代码不会在查找中间被阻塞。
- Semaphores 信号量  
  信号量也在linux内核中被使用，当网络模块使用的比较少。
### 2.11) conversions between host and network order
超过1byte的数据结构在内存中有两种存储形式：大端和小端。  
因此，为了避免不同机器不同方式带来的麻烦，统一为network byte order 和 host byte order(看你自己的系统)。有如下几个宏(include/linux/byteorder/generic.h)负责转换。
宏| 含义（short is 2 bytes, long is 4 bytes)
-- | ----
htons|Host-to-network byte order (short)
htonl|Host-to-network byte order (long)
ntohs|Network-to-host byte order (short)
ntohl|Network-to-host byte order (long)

根据不同的系统架构，由include/asm-XXX/byteorder.h、include/linux/byteorder/little_endian.h或include/linux/byteorder/big_endian.h来决定上述宏的具体实现。  
对于每一个上述宏XXX都有一个对应的 __constant_XXX对应，这是针对常量值的宏。  
大端序与小端序有时也会在bit码中的序列有区分。
### 2.12) catching bugs 
少数函数会在特定情况下被调用，在另一些特殊条件下不会被调用，内核代码使用了`BUG_ON`和`BUG_TRAP`两个宏去决定。当输入条件为`BUG_TRAP`为false时，内核输出错误信息，如果是`BUG_ON`则打印错误信息并崩溃退出。
### 2.13) statistics
收集一些特殊情况是好习惯，比如cache命中或未命中、内存申请成功或失败。
### 2.14) measuring time
内核需要常常需要判断距离一个给定时间节点的时间，比如进程调度等。在网络模块中的垃圾回收实现中也有应用。
在内核中计量时间用的是ticks，一个tick就是两个连续到期的定时器中断。  
每当定时器到期，一个叫jiffies的全局变量就会增加，就是说jiffies就代表了系统启动到当前时刻的时间。  
当一个函数需要获取时间长度时，其可以将jiffies保存在一个临时变量中，在到需要的时候对那时的jiffies进行比较，即可得出时间长度。
```
unsigned long start_time = jiffies;
int job_done = 0;
do {
    do_something(&job_done);
    If (job_done)
        return;
while (jiffies - start_time < 1);
```
更多例子详见第10章和第27章
## 3. User-Space Tools
可用如下工具对控制内核达到学习的效果：
- iputils  
- net-tools
- IPROUTE2
## 4. Browsing the Source Code
cscope工具看代码
### DEAD CODE
会有一些根本没有用的代码
## 5. When a Feature Is Offered as a Patch
