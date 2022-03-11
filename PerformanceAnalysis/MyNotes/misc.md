# 性能杂谈
## 
根据CPU的LSD(loop stream decoder)的设计，LSD 循环流监测器也算包含在解码部分，它的作用是: 假如程序使用的循环段(如 for..do/do..while 等)少于 28 个 uops，那么 Nehalem 就可以将这个循环保存起来，不再需要重新通过取指单元、分支预测操作，以及解码器，Core 2 的 LSD 放在解码器前方，因此无法省下解码的工作。