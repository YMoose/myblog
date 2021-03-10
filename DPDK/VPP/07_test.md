# issues
## 开发
1. 编译有时会遇到找不到依赖、头文件、动态库等: 开发前先将vpp源码编译生成rpm安装包，再rpm install后依赖、头文件、动态库就会安装在该在的目录中，之后就可以进一步开发调试了
## trex 测试性能
1. trex启动dpdk吃掉所有大页并报huge page init failed(https://github.com/cisco-system-traffic-generator/trex-core/issues/480): 添加一个TRex命令行参数 --new-memory(为了这个参数，可能要有一些依赖问题要解决)
2. 因checksum校验出错vpp在业务节点前丢弃数据包->测试用pcap文件中的数据包checksum就为0->抓包时因windows设置使得数据包直到网卡处才计算checksum并填写以加速，所以wireshark抓到的包是没有checksum的(https://blog.csdn.net/wangqi0079/article/details/9064557): 在适配器选项->选择适配器右键-属性->网络-配置-> 高级-硬件校验和->值-关闭(视网卡而定有些网卡不可配置)