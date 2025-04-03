# VPP src
vpp/src  
├── cmake  
├── CMakeLists.txt  
├── examples  
├── pkg  
├── plugins  
├── scripts  
├── svm  
├── tools  
├── vat  
├── vat2  
├── vcl  
├── vlib: leverage the facility  
├── vlibapi   
├── vlibmemory  
├── vnet: spent most of time doing Feature  
├── vpp  
├── vpp-api  
└── vppinfra: leverage the facility  
## vppinfra
vppinfra下定义了一些非常基础的高性能的变量类型&结构及其以供后续开发使用,提供一些基本的通用的功能函数库：包括内存管理，向量操作，hash， timer等
1. types.h: 定义一些基本变量
2. cache.h: 定义与cache相关的选项(一般cache-line大小为64bytes)，用于cache优化加速
3. vector.h: 定义一些与vector大小相关的变量，用于利用SIMDvector指令加速)
4. vec.h: 定义一些与vec结构(vec是存储pool里buffer的偏移的的可变数组结构，通过这个可以拥有接触到buffer里packet数据的能力)操作相关的函数方法
    ```
    # add 1 element E to vec V 
    vec_add1(V, E)              vec_add1_ha(V, E, 0, 0)
    vec_add1_aligned(V, E, A)   vec_add1_ha(V, E, 0, A)

    # add2 elements to vec V
    vec_add2(V, P, N)           vec_add2_ha(V, P, N, 0, 0)
    vec_
    ```
5. bitmap.h: 与bitmap使用相关(常常用于描述pool中的元素), bitmap数据结构实际就是clib_bitmap_t(uword)组成的vec
6. pool.h: 与内存池使用相关, pool数据结构实际上是由vec结构和bitmap结构结合形成的，用来快速alloc和free *固定大小且独立的数据结构*，pool非常适合用来分配per-session的数据结构。
7. heap.h: 用来快速alloc和free *可变大小的数据结构*，效率比pool慢，所以比较少用(分类器中会用)
8. time.h: 与时间相关的结构与操作，如获取当前时间、睡眠指定时长等
9. clib.h: 定义一些与c、汇编、cpu指令相关的底层宏
10. bihash_8_8.h: 哈希表结构相关。前一个8代表hash的key size，后一个8代表hash的Value size。VPP中的bihash是ipv6 FIB(转发表)的核心
11. bihash_template.h: bihash(Bounded-index extensible hash)实现原理相关。用户可以按照自己需求参考这个模板自己生成一个哈希表结构
12. format.h: 输入输出格式化，可用于命令解析。format == write from date to string 和sprintf用法类似；unformat和sscanf类似
## vlib
vlib主要提供基本的应用管理库实现：buffer管理，graph node管理，vector管理，线程，CLI，trace等
1. node.h: 定义一些与图节点相关的操作(如VLIB_REGISTER_NODE)
2. unix/plugin.h: 定义一些插件相关的操作(如初始化，加载等)
3. unix/unix.h: 实现了一个poll(event-loop-like) node用于监听文件描述符事件并执行回调函数。像是cli的实现就是监听文件描述符0(代码在vnet/unix/tapcli.c)
## plugins
plugins主要为实现一些功能，在程序启动的时候加载，一般情况下会在插件中加入一些node节点去实现相关功能
## vnet
vnet提供网络资源能力：比如设备，L2,L3,L4功能，session管理，控制管理，流量管理等
## vpp
vpp包含了程序主流程代码(src/vpp/vnet/main.c)