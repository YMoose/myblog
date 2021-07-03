# BPF tools
eBPF 是在内核内的虚拟机(in-kernel JIT for opcodes)，可用于往内核安插探针。在user mode写好后会被编译为eBPF专用的字节码并被eBPF执行。
## 组成
- 虚拟指令集(因为采用了虚拟指令集规范，所以可将bpf视作一种虚拟机实现)(这些指令由Linux内核的BPF运行时模块执行)
- 存储对象
- 辅助函数
- ···
## 限制
eBPF的执行存在一定的限制
- eBPF自身限制
  - MAX 4096 instructions
  - 512B stack
- 系统限制
  - CONFIG_BPF_SYSCALL 编译参数
  - linux版本(随着版本升级，可供插桩)
    ![eBPF with kernel version](eBPF_with_kernel_version.png)
## 执行流程
![eBPF process](eBPF_process.png)
- 用户层
  1. 用JIT编译器将BPF指令动态生成为BPF bytecode
  2. 加载到内核
- 内核
  1. verifier判断是否合法
  2. 插入到指定位置挂钩
  3. 挂钩函数执行结果传递给BPF maps
- 用户层
  3. 用户层通过 perf_output到per-event detail/通过异步读取BPF maps
## 不同的bpf前端
1. BCC(BFP Compiler Collection):开发复杂的脚本和作为后台进程使用
2. bfptrace: 编写功能强大的单行程序、短小的脚本
3. ply: 轻量化、依赖最小化，适合嵌入式Linux环境

