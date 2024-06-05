# PMU(Performance Monitoring Unit)
PMU是由一系列寄存器包括PMC（Performance Monitoring Counter）和MSR（model specific registers）组成的在cpu上的硬件单元，用于测量cpu的性能。
大多数CPU上，有两种PMC，一种是记录固定事件的，一种是记录给定事件的。
## 原理
### counting
最简单的用法，观测开始前让一个pmc记录某一事件发生的次数。可以用`perf stat`指定事件
另一方面，可以通过分时复用技术来切换pmc上要记录的事件，使得观察时间内可以统计远大于pmc个数的事件的数量变化。
### sampling
可以用`perf record`指定事件来触发
软件触发的sampling（Interrupt-based sampling）：VTune会在被分析的应用程序中嵌入一个agent库。这个库会为应用程序中的每个线程设置操作系统定时器（OS timer）。一旦定时器到期，应用程序就会收到一个由collector处理的SIGPROF信号或者其他运行时信号。使用软件来驱动采样的方式在某些平台上可能会导致采样时机的延迟，会造成数据不准确的情况。
硬件触发的sampling（PEBS，Processor Event-based sampling）：通过pmc的overflow事件来触发采样时机，采样时硬件将所需记录的一些状态直接写到指定的内存（PEBS buffer）上。
## 参考
1. https://easyperf.net/blog/2018/06/01/PMU-counters-and-profiling-basics