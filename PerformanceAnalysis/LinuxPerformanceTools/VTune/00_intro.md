# Intel VTune
## 0. Intel VTune 介绍
VTune是一套能力强大的性能分析工具。
### 0.1. Intel VTune的生态位
#### 0.1.1. 性能分析生态
性能提升是一个发现问题解决问题的过程
- 原理: 主要还是采样
- 能力: 可以本机的，也可以远程的，可以普通程序，也可以attach到系统进程
## Vtune 官方测试模板
![Intel VTune测试模板](vtune_type.png)
1. Performance Snapshot:分析时可以首先进行一次performance snapshot测试，其会给出下一步分析的建议
## 一些建议
要收集到更准确的数据就需要减少收集的数据
1. 减少采集数据的时间
2. 设置合适的"Duration Time Estimate"
3. 在linux机器上设置TMPDIR环境量为本地文件系统的目录
4. 设置"CPU Mask"
5. 手动设置"SAV(sample after value)"值
6. 减少需要采集的Performance Event数量

## 其他
### DPDK应用的CPU利用率
DPDK从18.11版本(带上编译参数`CONFIG_RTE_ETHDEV_RXTX_CALLBACKS`和`CONFIG_RTE_ETHDEV_PROFILE_WITH_VTUNE`(located in the`config/common_base config ` file))后开启对vtune(使用I/O分析)的支持。
## 参考
1. []