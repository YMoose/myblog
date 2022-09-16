# vpp unix 相关
## file
src/vlib/unix/unix.h  
vlib实现了一个poll(event-loop-like) node用于监听文件描述符事件并执行回调函数。像是cli的实现就是监听文件描述符0(代码在vnet/unix/tapcli.c)