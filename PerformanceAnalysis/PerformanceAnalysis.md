# Perf Analysis
## 方法论
### problem statement method
1. 是什么让你觉得有性能问题？
2. 系统有性能好过么？
3. 最近改动过什么？
4. 这个性能问题造成的影响？
5. 系统运行环境？硬件环境、软件版本、配置版本...
### workload characterization method 业务负载画像
分析对比系统运行正常时的业务负载（可以通过一些市面上已有的性能观测项目分析）
- Throughout(吞吐量)
- Utilization(利用率)
- Latency(延迟)
- Error
- (扩展性:热扩展?)
#### 目标
1. 知道自己能做什么 
#### 常用数字
### 原则
- 80/20法则
- 阿姆达尔定律（Amdahl’s law / Amdahl’s argument）
- 排队论(利特尔法则)

## 流程
1. 要先有性能测试的基础模型，以便每次修改可以检查效果
2. 决定优化目标
## 观测方法
1. 跟踪(tracing)
   - 记录原始事件和事件元数据
   - 比较详细但通常消耗较大
   - 工具: eBPF\tcpdump\strace
2. 采样(sampling)
   - 定时\定次或某种情况下观测并记录事件
   - 可能会遗漏事件导致不准确
   - 工具:
3. 使用计数器统计(counting)
   - 使用计数器检测统计事件的频次
   - 消耗较小但只计数，需要更多的知识
   - 工具: top/perf/pmu-tools
