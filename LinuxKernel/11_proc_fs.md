# /proc文件系统
可以通过/proc虚拟文件系统与操作系统内核进行通信，并控制部分操作系统的参数
## 获取各项系统信息
### 进程信息-数字目录
数字对应响应pid的进程
#### cmdline
进程启动命令
#### status
进程对应的详细信息(文本)
#### statm
进程对应的内存信息(仅数值)
#### stat
进程对应的详细信息(仅数值)
#### maps
进程的内存映射关系以及权限(文本)
#### smaps
比maps更为详细的一些内存信息
#### smaps_rollup
对smaps的统计
#### clear_refs
可以通过将一些特殊的值写入此文件对进程关联的物理与虚拟内存页面上的一些标志bit置位，具体使用参考文档（Documentation/admin-guide/mm/soft-dirty.rst）
#### pagemap
进程页面框号（Page Frame Number,PFN）(二进制数值)，通过PFN可以在/proc/kpageflags中查看页面的一些具体信息包括一些标志位，具体使用参考文档 （Documentation/admin-guide/mm/pagemap.rst）
#### numa_maps
进程内存与numa的亲和性(绑定策略)

### 内核信息
#### cmdline
内核启动参数
#### cpuinfo
#### *内存相关*
#### meminfo
提供了内存的分布与使用情况
#### slabinfo
slab的使用情况（一些特有的对象拥有自己的slab pool（比如network buffers，directory cache等）），slab pool在linux中作为page之上更细分的内存管理单位。
#### buddyinfo
显示当前可用的不同大小的内存页框数量
#### pagetypeinfo
更详细的buddyinfo
#### interrupts
可以查看cpu中断次数统计，也可以查看中断号对应的中断函数
#### irq
在此目录下可以根据不同的中断号获取不同的中断信息（cpu亲和性配置），也可以对其进行设置，通常情况下，IRQ（中断请求）的路由由IO-APIC（输入/输出高级可编程中断控制器）处理，并且它会在允许处理该中断的所有CPU之间进行轮询（Round Robin）。另外需要注意的是，默认设置通常是最适合大多数情况的选择。内核对系统中的中断请求进行了优化，以提供最佳的性能和稳定性。因此，对于大多数用户来说，使用默认设置是合理的选择。
##### [0-9]*/smp_affinity_list
cpu对此类中断处理的亲和性（可以写入用于设定）
##### [0-9]*/node
numa对此类中断处理的亲和性（可以写入用于设定）
#### net
#### scsi
#### sys

## 内核模块的/proc文件系统API

## 参考
1. [/proc文件系统内核文档](https://www.kernel.org/doc/Documentation/filesystems/proc.txt)