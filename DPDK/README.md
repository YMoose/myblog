# DPDK
as a user driver，DPDK被认为是一组用户态的网络包输入/输出库
## 网络数据帧到应用程序处理的过程
1. 数据帧由网络层到达网卡设备，网卡设备处理数据帧（设备及其初始化）
2. 网卡设备上送数据帧（DMA）
3. 操作系统获取数据（驱动）
4. 用户态应用程序处理数据（DPDK）
## Arch
### Core
1. EAL(Environment Abstraction Layer): set up initiation(像了解你的核数，要多少内存，哪些内存和哪个核绑定，网卡信息这些)
2. MBUF: 
3. MEMPOOL: 自己实现的内存池，被封为一个个小slab(使用大页内存，以减少TLB刷新)
4. RING: for lockless queues of packets send between threads and cores
5. TIMER: q
#### 内存配置
大页内存
内存访问的numa感知