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
- 内存和CPU一样也可以过载使用。一般不建议过多使用过载，性能不咋样且稳定性降低。
  1. 内存交换(swapping):用交换空间（swap space）来弥补内存的不足。使用swapping方式，要求有足够的交换空间(swap space)，可用的物理内存空间 + 交换空间 >= Sum（所有客户机分配的内存）。否则会因为内存不足崩溃。
  2.  气球(ballooning):通过virio_balloon驱动来实现Host机Hypervisor和guest机之间的协作
  3.   页共享(page sharing):通过KSM(Kernel Samepage Merging)合并多个客户机进程中使用相同的内存页。  

  目前KVM有如下三种方式实现内存的过载使用。目前来说第一种是最成熟的，也是目前广泛使用的，不过相对另外两种效率低一些。

## 存储配置
### 配置参数
-hda *file*  
hda:将*file*文件作为guest机中的第一个IDE设备（序号0），在客户机中表现为/dev/hda设备或/dev/sda。这个*file*也可以指定为一个硬盘设备而进行整个硬盘的模拟。    
-hdb *file*  
-hdc *file*  
-hdd *file*  
hdb&hdc&hdd:用于将*file*文件作为guest机中的第二&三&四个IDE设备（序号1&2&3）。  
-fda *file*  
-fdb *file*  
fda&fdb:将*file*文件作为guest机中的第一&二个软盘设备（序号0&1），在客户机中表现为/dev/fd0&/dev/fd1。*file*也可以使用宿主机中的软驱（/dev/fd0）。  
-cdrom *file*  
cdrom:将*file*作为guest机中的光盘CD-ROM。也可以将host机中的光驱作为*file*。不可以与-hdc参数同时使用，因为-cdrom就是客户机中的第三个IDE设备。一般安装操作系统时，镜像文件用此参数指定  
-mtdblock *file*  
mtdblock:使用*file*文件作为guest机自带的一个flash存储器（闪存）  
-sd *file*  
sd:使用*file*文件作为guest机中的sd卡  
-pflash *file*  
pflash:使用*file*文件作为客户机的并行flash存储器  
-drive file=*file*,if=*interface*,bus=*bus*,unit=*unit*,index=*index*,media=*media*,snapshot=*snapshot*,cache=*cache*,aio=*aio*,format=*format*
serial=*serial*,addr=*addr*,id=*name*,
readonly=*on|off*  
drive:可用来详细定义一个存储驱动器。  
*file*:镜像文件  
*interface*:驱动器使用的接口类型，可用的类型有：ide\scsi\sd\mtd\floopy\pflash\virtio等。  
*bus*&*unit*:驱动器在guest机中的总线编号和单元编号。  
*index*:同一种接口的驱动器中的索引  
*media*:驱动器中媒介的类型，值为“disk”或“cdrom”  
*snapshot*:其值为“on”或“off”，当snapshot启用时，QEMU不会将磁盘数据的更改写回到镜像文件中，而写到临时文件中。但也可以在qemu monitor中使用commit命令强行将数据保存到镜像文件中去。  
*cache*:其值为“none”（“off”）、“writeback”、“writethrough”（默认值）等。如关闭则qemu将在调用open系统调用打开镜像文件时使用“O_DIRECT”标识，所以其读写数据会绕过缓存直接从块设备中读写。一些块设备文件（如qcow2格式文件）在“writethrough”模式下性能较差，推荐使用“writeback”。  
*aio*:异步IO的方式，其值为“threads”或“native”。thread为默认值，指让一个线程池去处理异步IO，native只适用于“cache=none”的情况，就是Linux原生的AIO。  
*format*:磁盘格式，默认自动检测。  
*serial*:分配给设备的序列号。  
*addr*:分配给驱动控制器的PCI地址，只有在使用virtio接口才有用。  
*name*:该驱动器的ID，可在qemu monitor中用“info block”看到。  
readonly:该参数配置驱动器是否只读。  
-boot [order=*drives*][,once=*drive*][,menu=*on|off*][,splash=*splashfile*][,splash-time=*sp-time*]
*drives*:设备名，详见下方
- a\b:第一、二个软驱
- c:第一个硬盘（默认）
- d:CD-ROM光驱
- n：网络启动  

once：表示第一次启动顺序，重启后无效。  
menu：设置交互式的启动菜单选项（要是使用的guest机bios支持），默认off。只有这一项是on时，后面两个选项才生效。
*splashfile*:BIOS启动logo图片。
*sp-time*:显示图片的时间，单位毫秒（ms）  
### 注意事项
- 注意guest机镜像存储介质和磁盘驱动的配置，挑选合适的组合

## 网络配置
### 配置参数
-net nic[,vlan=*index*][,macaddr=*mac*][,model=*type*][,name=*name*][,addr=*addr*][,vectors=*v*]
net:用于配置网路的参数，如没有设置，则默认使用“-net nic -net user"（完全基于QEMU内部实现的用户模式下的网络协议栈）。
- -net nic：是必须的参数。可以使用多个-net nic配置多个网卡。
- *index*：将网卡放入编号为index的VLAN，默认为0
- *mac*：网卡的MAC地址，默认以host机网卡地址分配，若局域网内guest机过多，建议自己设置MAC地址，以防止MAC地址冲突。
- *type*：模拟网卡的类型
- *name*：网卡名称，该名称仅在QEMU monitor中使用
- *addr*：设置网卡在客户机中的PCI设备地址
- *v*：设置该网卡的MSI-X向量数量（此项仅对virtio驱动的网卡有效，设置为0是关闭virtio网卡的MSI-X中断方式）

-net nic,model=?  
使用这个参数可以查询到当前qemu-kvm工具支持了哪些网卡的模拟。rtl8139是qemu-kvm默认的模拟网卡类型（qemu默认intel e1000系列网卡）。  

-nettap[,vlan=*index*][,name=*name*][,fd=*h*][,ifname=*ifname*][,script=*file*][,downscript=*dfile*][,helper=*helper*][,sndbuf=*nbytes*][,vnet_hdr=*on|off*][,vhost=*on|off*][,vhostfd=*h*][,vhostforce=*on|off*]  
- nettap：表明使用了TAP设备。TAP设备是虚拟网络设备，仿真了一个数据链路层设备，其像以太网数据帧一样处理第二层数据包。（TUN与TAP类似也是一种虚拟网络设备，其是对网络层的仿真。TAP用于创建网络桥，而TUN与路由相关。）  
- *h*：连接到现在已打开的TAP接口的文件描述符。此项不可以与其他项同时使用。
- *ifname*：host机中添加到TAP虚拟设备的名称（如tap1）。当不设置这个参数时，QEMU会依据系统中目前的情况产生一个TAP接口的名称。
- *file*：host机在启动guest机时自动执行的网络配置脚本。如不指定，默认执行“/etc/qemu-ifup”，如不需要则设置参数值为“no”。
- *dfile*：host机在关闭guest机时自动执行的网络配置脚本。如不设置，则默认执行“/etc/qemu-ifdown”,如不需要则设置参数值为“no”。
- *helper*：启动guest机时在host机中运行的插桩函数，包括建立一个TAP虚拟设备。默认值为“/usr/local/libexec/qemu-brideg-helper”。
- *nbyte*：限制TAP设备的发送缓冲区大小为n字节，需要流量控制的时候可以配置此选项。默认为0，即不限制大小。
- 其余选项与virtio相关
### 注意事项
- qemu-kvm主要向guest提供了4种不同模式的网络。
  1. 基于网桥（bridge）的虚拟网卡：让guest机和host机共享一个物理网络设备连接网络，但guest机有独立IP。
  2. 基于NAT（Network Address Translation）的虚拟网络：属于广域网接入技术的一种，将内网地址转化为外网的合法IP地址。主要是将内网IP数据包头中的源IP地址转换为一个外网的IP地址，使得多个内网机可以共用一个IP地址接入网络，并隐藏内部IP和NAT内部网络拓扑结构和IP信息，从而避免攻击，不过也因此无法将外部访问定位到内网机去。QEMU/KVM中默认使用IP伪装实现NAT。
  3. QEMU内置的用户模式网络（user mode networking）
  4. 直接分配网络设备的网络（包括VT-d和SR-IOV）（会在后续详细讲述TODO）  
- 使用网桥模式，需要安装并配置bridge-utils和tunctl包。将tun模块加载进内核，并让当前用户拥有可读写权限。建立bridge，并将其绑定到网络接口上，让其成为本机与外部网络的接口。
- 网桥模式中不推荐设置fd项参数，而是让QEMU自动创建一个TAP接口，因为使用了fd参数后，ifname\script\downscript\helper\vnet_hdr等选项都不可使用了（TODO:why）
-  
  
## 显示配置
### 配置参数
### 注意事项
## 参考资料
[1] https://www.cnblogs.com/yubo/archive/2010/04/23/1718810.html  
[2] https://blog.csdn.net/ustc_dylan/article/details/8817756  
[3] https://zhuanlan.zhihu.com/p/34659353 大页内存简介  
[4] https://lvwenhan.com/操作系统/496.html#comment-12622  大页内存性能优化的分析  
[5] 

