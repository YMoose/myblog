# 内核网络协议栈
![linux networking stack](pics/linux_net_stack.svg)
struct proto (linux/include/net/sock.h)定义了网络协议的各类操作接口
由proto_register (linux/net/core/sock.c)函数注册
由inet_init (linux/net/ipv4/af_inet.c)函数进行初始化
inet协议族的proto都会映射到inetsw_array



## 参考
https://nxw.name/2022/linux-network-stack
https://blog.csdn.net/maimang1001/article/details/123276405
[一个简单的驱动实现](https://zhou-yuxin.github.io/articles/2017/%E7%AC%AC%E4%B8%80%E4%B8%AALinux%E7%BD%91%E7%BB%9C%E8%AE%BE%E5%A4%87%E9%A9%B1%E5%8A%A8%E2%80%94%E2%80%94%E6%9C%80%E7%AE%80%E8%99%9A%E6%8B%9F%E7%BD%91%E5%8D%A1virnet/index.html)