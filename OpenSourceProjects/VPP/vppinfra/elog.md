# event-log
数据结构头文件目录
- src/vppinfra/elog.h
- src/vppinfra/elog.c
- src/vppinfra/test_elog.c
## 简介
eventlog是vpp实现的非常轻量级（小于100ns）的事件日志服务
序列化让eventlog的保存与最终的合并变得简单。
- event-logger在vlib_main(src/vlib/main.c:2145)函数中启动
- 主要数据结构为elog_main_t, 存储在vlib_main_t->elog_main
- 默认ring size 为128K个事件
- 每个event槽32bytes
  - u64: time-in-cpu-clocks
  - u16: event-id
  - u16: track
  - 20 bytes: data 
- 线程安全, 记录速度小于100ns
- 观察者效应：最多每个节点每一个frame记2个event 
## example(src/example/vlib/elog_examples.c)
可以配合官方开发者开发的一个[图形化小工具](https://wiki.fd.io/view/VPP/g2)配合查看