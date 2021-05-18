# TMAM 层级结构分析

## TMAM 层级
![TMAM 层级结构图](TMAarch.jpg)
### 如何分类
![Top level breakdown](Top_level_breakdown.png)  
- Uop Issued? Uop正常在流水线上流通么？
  - 正常->Uop正常完成执行去下一个流水线步骤了么？
    -  正常->Retiring
    -  中途退出了->预测出错
  - 不正常(卡住了)->是Back-end卡住了么？
    - 是->backend限制
    - 否->frontend限制
### 1. Frontend Bound : 取指、译码
#### 1.1. Fetch Latency : 主要指取指的延迟，造成的原因如下
##### 1.1.1. i-cache miss
##### 1.1.2. i-TLB miss
##### 1.1.3. Branch Resteers: 由于流水线刷新造成的，这一项于分支预测错误有很大关系
#### 1.2. Fetch Bandwidth : 主要指译码的低效，通常与特定cpu型号下特定的指令有关

### 2. Backend Bound : 执行、访存
#### 2.1. Memory Bound : 因加载内存而损失的性能
##### 2.1.1. L1 Cache Bound : 除了包括L1 cache miss 还有有store uop阻塞了load uop的情况
##### 2.1.2. L2 Cache Bound
##### 2.1.3. L3 Cache Bound
##### 2.1.4. Stores Bound : store uop而损失的性能，通常为执行端口利用率（execution port utilization）较低，以及存在大量需要消耗资源用来缓冲 store 操作的周期。
##### 2.1.5. Ext.Memory Bound : 上面都是cache层面的性能损失，这里是cache都miss的内存读取的性能损失
###### 2.1.5.1 Mem Bandwidth : 大多数内存读取时可以同步进行的，其余的会归为此类。根据是当前有多少请求依赖从内存中获取的数据。每当这类请求的占用率超过一个高阈值时（例如最大请求数的70％），TMAM 将其标记为可能受内存带宽的限制(启发式算法)
###### 2.1.5.2 Mem Latency : 低于70%的就是这个
#### 2.2. Core Bound : 计算核心的性能限制(好比说除法会有更低的计算性能，缺少指令级别的并行(考虑使用SIMD))，得靠编译器优化

### 3. Retiring : 理想状况的流水线执行比重(一定程度上可以理解为回写)(但仍然可以优化!)
#### 3.1. BASE
##### 3.1.1. FP-arithmetic : 浮点数计算
###### 3.1.1.1. Scalar : 标量的
###### 3.1.1.2. Vector : 矢量的(SIMD)
#### 3.2. Micro Sequencer : 像浮点数计算支持等微指令也会影响性能，被计数在这个部分

### 4. Bad Speculation : 分支预测出错
#### 4.1. Branch Misspredict
#### 4.2. Machine Clears: 数据预取错误，MOB(memory ordering buffer)刷新带来的问题和自修改代码(self modifying code)

## CPU PMU
PMU中主要由PMC(performance monitor counter)组成，cpu中有若干PMC(可以通过cpuid命令查看)(在intel硬件层面称为(MSR: Model Specific Register))可被用户(通过注册，注意有些MSR被注册后没被释放会造成冲突问题)指定监控指定event。PMC通过分时复用来采集大于其数量的event。通过对不同event数据采集统计分析可以得到更为直观的性能评价(metric)。可通过直接读取0x38F这个MSR来查看event counter是否被占用，只有当返回值不为0的时候才意味着没有其他的进程占用了event counter。    
根据event的不同PMC可分为
- core event : 直接工作在CPU物理核心上的计数器
- offcore event : 是相关HT技术导致的同一物理核心上两个不同的HT上不得不共享的一些事件的计数器
- uncore event : 在同一个CPU插槽所有core共享的计数器，使用前需要预先找到对应的虚拟pcie设备，并通过虚拟设备读取而非直接通过CPU指令读取。uncore有以下分类
  - uncore/cha， uncore/imc，uncore/cbo，etc 这类内存和三级缓存访问有关的event
  - uncore/upi UPI相关
  - uncore/iio iio和pcie相关

