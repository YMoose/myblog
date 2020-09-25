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
2. Neighboring protocol mappings
3. Routing tables
下面是实现内存缓存区的内核关键函数
```
kmem_cache_create
kmem_cache_destory
kmem_cache_alloc    // return a buffer to the cache.
kmem_cache_free     // kmem_cache_alloc & kmem_cache_free 通常会通过一个装饰器来调用，这个装饰器负责从更高level来管理内存的申请与释放。
```
内存缓存区可申请的实例数量，通常会被上述装饰器定死的，有时可以在 */proc* 里配置
### 2.2) caching & hash tables 缓存和哈希表

### 2.3) reference count 相关计数器
### 2.4) garbage collection 垃圾回收
### 2.5) func points & virtual func tables(VFTS)
### 2.6) goto statements
### 2.7) vector definitions
### 2.8) conditional directives(#ifdef and family)
### 2.9) compile-time optimization for condition checks
### 2.10) mutual exclusion
### 2.11) conversions between host and network order
### 2.12) catching bugs
### 2.13) statistics
### 2.14) measuring time
## 3. User-Space Tools