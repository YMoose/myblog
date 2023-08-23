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
#### meminfo
#### interrupts
可以查看cpu中断次数统计
#### irq
在此目录下可以根据不同的中断号获取不同的中断信息
##### smp_affinity_list
cpu对此类中断处理的亲和性（可以写入用于设定）
## 内核模块的/proc文件系统API

## 参考
1. [/proc文件系统内核文档](https://www.kernel.org/doc/Documentation/filesystems/proc.txt)