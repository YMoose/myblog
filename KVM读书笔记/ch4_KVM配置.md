# ch4 KVM核心基础功能
## CPU配置
### 配置参数
-smp *n*[,maxcpus=*cpus*][,cores=*cores*][,threads=*threads*][,sockets=*sockets*]
- *n*: guest机中使用的vCPU（逻辑CPU）数量（默认为1）
- *maxcpus*: 客户机中最大可能被使用的vCPU（逻辑CPU）数量（包括启动时处于下线状态的CPU数量，多用于需要热插拔hot-plug加入CPU的场景）（目前有bug，不推荐使用）
- *sockets*: guest中OS可以识别出来的CPU socket数量(主板上可以插cpu的槽数，也就是“路”)
- *cores*: guest机的一个cpu的核数（默认为1）
- *threads*: guest机的cpu一个核的超线程数量（默认为1）  
*n* = *sockets* * *cores* * *threads*

-cpu <*cpu type*>
- *cpu type*: guest机OS可以看到的cpu类型(默认为qemu64/qemu32)，若指定其他类型的CPU，需要在cpus-x86_64.conf文件中写明。
### 注意事项
- 不建议使用CPU热插拔功能
- 不推荐将一个guest机的vCPU数量配置为超过物理硬件的CPU数量，会导致性能下降以及不稳定的问题。可以配置多个客户机，每个客户机少量vCPU（总vCPU大于物理CPU没问题）。
- 通常SMP系统中，linux内核进程调度采用时间片法则，这会导致进程进入不同的cpu执行。进程的处理器亲和性（Processor Affinity）是指CPU的绑定设置，是指将进程绑定到特定的一个或多个CPU上执行。设置进程的处理器亲和性可以减少进程在多个CPU之间交换运行带来的缓存命中失效（cache missing），从而带来一定的性能提升。但同时会破坏系统的负载均衡（load balance）。这可能使得整个系统的进程调度变得低效。所以看具体需求配置处理器亲和性选项（配置方法是在linux内核启动的命令行上加上“isolcpus=”的参数，然后使用taskset工具进行绑定）。

## 内存配置
### 配置参数
-m <*memory size*> <M(MB)\G(GB)(unit,default M(MB))>
- *memory size*: guest机的内存大小
- 默认单位为MB，也可以加上G指定GB作为单位。
- 如果不指定明确大小QEMU将默认设置为128MB。  

-mem-path <*fILE*>
- *file*: 挂载的文件系统   

-mem-prealloc  
mem-prealloc：让host机启动guest机时就全部分配好guest机的内存，而不是按需给guest分配内存。  

### 注意事项
- 分配大页内存时，对于内存访问密集型的应用，在guest机中可以明显提高guest机性能，另一方面大页内存也因其置换效率太低而在系统层面上禁止了其被换出（swap out），也不能使用ballooning（TODO）方式自动增长。  
- 内存和CPU一样也可以过载使用。一般不建议过多使用过载，性能不咋样且稳定性降低。目前KVM有如下三种方式实现内存的过载使用。目前来说第一种是最成熟的，也是目前广泛使用的，不过相对另外两种效率低一些。
- - 1) 内存交换(swapping):用交换空间（swap space）来弥补内存的不足。使用swapping方式，要求有足够的交换空间(swap space)，可用的物理内存空间 + 交换空间 >= Sum（所有客户机分配的内存）。否则会因为内存不足崩溃。
- - 2) 气球(ballooning):通过virio_balloon驱动来实现Host机Hypervisor和guest机之间的协作
- - 3) 页共享(page sharing):通过KSM(Kernel Samepage Merging)合并多个客户机进程中使用相同的内存页。  

## 存储配置
### 配置参数
### 注意事项
## 网络配置
### 配置参数

### 注意事项
- qemu-kvm主要向guest提供了4种不同模式的网络
## 参考资料
[1] https://www.cnblogs.com/yubo/archive/2010/04/23/1718810.html  
[2] https://blog.csdn.net/ustc_dylan/article/details/8817756  
[3] https://zhuanlan.zhihu.com/p/34659353 大页内存简介  
[4] https://lvwenhan.com/操作系统/496.html#comment-12622  大页内存性能优化的分析
