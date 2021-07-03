# Perf Analysis
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
## 方法论
业务负载画像(workload characterization)