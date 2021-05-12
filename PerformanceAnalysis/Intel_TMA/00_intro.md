# 00.Intro
TMAM 自顶向下的CPU架构性能分析方法  
TMAM: Top-down Microarchitecture Analysis (TMA) Method  
- 目标: 通过分析检测出瓶颈(进一步定位出问题)&预估其消耗
## 性能分析
性能分析一般分为3个层级
1. System (Disk, Network, Graphics, ...)
2. Application + Runtime (Algorithm complexity, Parallelization/Multithreading, ...)
3. Architectural + μArch (vectorize key loops, reduce cache misses, ...) (这也是TMA主要针对的)
## TMAM层级
### 第一层
1. Retiring : 理想状况的流水线执行比重
2. Bad Speculation : 分支预测出错
3. Frontend Bound : 取指、译码
4. Backend Bound : 执行、访存
## 与传统分析方法的区别
### Traditional Methods
传统分析方法采用了`stall_cycles = sum(Penalty[i] * MissEvent[i]) `的计算方法。这样的计算方法存在以下问题
- 不同事件的penalty不同，且难以统计
- 存在分支预测的干扰
- 存在不同输入的干扰
- 预先定义的missEvent并不准确
- superscalar带来的不准确性
### TMAM
1. 层级式分析
   - TMAM的事件