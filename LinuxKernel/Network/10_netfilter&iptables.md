# netfilter & iptables
netfilter是一个内核模块，是linux内核处理网卡收到内核的数据包的一个框架
iptables是对应的用户工具，用户通过iptables命令下发规则，规则在netfilter框架中应用，匹配并按照规则处理IP数据包，iptables主要是应对ipv4协议（以及传输层协议）的规则，相应的还有ip6tables应对ipv6协议（以及传输层协议）的规则、arptables等工具
## netfilter
netfilter定义了一系列HOOK标志点（include\linux\netfilter.h）
```C
enum nf_inet_hooks {
	NF_INET_PRE_ROUTING,
	NF_INET_LOCAL_IN,
	NF_INET_FORWARD,
	NF_INET_LOCAL_OUT,
	NF_INET_POST_ROUTING,
	NF_INET_NUMHOOKS
};
```
以ipv4为例，这几个HOOK标志点具体执行的位置为(linux-2.6.34)
- NF_INET_PRE_ROUTING: `ip_rcv() (linux/net/ipv4/ip_input.c)`中执行注册在此HOOK标志点的hook函数。执行完成后会通过`ip_rcv_finish()`这个`okfn`调用执行`ip_route_input()`函数。在`ip_route_input()`函数中，会根据数据包的目的（指向本机/不指向本机）通过`skb_dst_set()`函数进行sk_buff的路由dst的设置（`ip_local_deliver()`/`ip_forward()`），或者在路由查找失败或ip选项处理失败的特殊情况下会丢弃此sk_buff，不再处理。此HOOK标志点被视为IP数据包的总入口。
- NF_INET_LOCAL_IN: `ip_local_deliver() (linux/net/ipv4/ip_input.c)`中执行注册在此HOOK标志点的hook函数。如果经过路由判断后，sk_buff的路由`struct dst_entry->input`函数为`ip_local_deliver()`或者通过别的input最终调用到`ip_local_deliver`都会触发执行hook函数。之后数据包会继续交给协议栈上层处理。此HOOK标志点被视为IP数据包中指向本机的数据包的总入口。
- NF_INET_FORWARD: `ip_forward() (net\ipv4\ip_forward.c)`中执行注册在此HOOK标志点的hook函数。如果经过路由判断后，sk_buff的`struct dst_entry->input`函数为`ip_forward()`就会触发执行hook函数。此HOOK标志点被视为IP数据包中不指向本机的数据包的总入口。
- NF_INET_LOCAL_OUT: `ip_local_out() (net\ipv4\ip_output.c)`中执行注册在此HOOK标志点的hook函数。协议栈上层需要封装发送数据包时会调用此函数。此HOOK标志点被视为由本机发出的IP数据包的总出口。
- NF_INET_POST_ROUTING: `ip_mc_output() (net\ipv4\ip_output.c)` 中执行注册在此HOOK标志点的hook函数。`ip_mkroute_output()`函数会在路由表某些需要发送出去的项的`struct dst_entry->output`函数设置为`ip_mc_output()`，`dst_output()`函数调用`ip_mc_output()`时就会触发执行hook函数。此HOOK标志点被视为IP数据包的总出口。

以此可见，netfilter框架实际运行于Network Protocol层中

在内核中通过`nf_register_hook()/nf_register_hooks() (net/netfilter/core.c)`将`nf_hook_ops`注册到`struct list_head nf_hooks[NFPROTO_NUMPROTO][NF_MAX_HOOKS]`这个链表数组中
```C
/**
 * @hooknum: 上面提到的HOOK标志点
 * @skb: 待处理的sk_buff
 * @in: 输入网络设备，由nf_hook_thresh传递进来的
 * @out: 输出网络设备，由nf_hook_thresh传递进来的
 * @okfn: 处理中可能用到的回调函数，由nf_hook_thresh传递进来的
 */ 
typedef unsigned int nf_hookfn(unsigned int hooknum,
			       struct sk_buff *skb,
			       const struct net_device *in,
			       const struct net_device *out,
			       int (*okfn)(struct sk_buff *));

struct nf_hook_ops {
	struct list_head list;

	/* User fills in from here down. */
	nf_hookfn *hook;
	struct module *owner;
	u_int8_t pf;
	unsigned int hooknum;
	/* Hooks are ordered in ascending priority. */
	int priority;
};
```
内核协议栈在处理某个协议的包时，会调用`NF_HOOK()`宏（或是函数），并最终调用`nf_hook_thresh() (include/linux/netfilter.h)`函数，`nf_hook_thresh()`函数定义如下
```C
/**
 * @pf: 协议族（Protocol family），如 PF_INET（IPv4）或 PF_INET6（IPv6）
 * @hook: 上面提到的HOOK标志点
 * @skb: 待处理的sk_buff
 * @indev: 输入网络设备（可为 NULL）
 * @outdev: 输出网络设备（可为 NULL）
 * @okfn: 所有hook函数处理完成后调用的回调函数
 * @thresh: 钩子优先级阈值，仅执行优先级 >= thresh 的钩子
 */ 
static inline int nf_hook_thresh(u_int8_t pf, unsigned int hook,
				 struct sk_buff *skb,
				 struct net_device *indev,
				 struct net_device *outdev,
				 int (*okfn)(struct sk_buff *), int thresh);
```
最终会通过`nf_iterate (net/netfilter/core.c)`函数遍历执行`nf_hooks[pf][hook]`nf_hook_ops的hook函数（会根据proiority过滤），hook函数会返回以下结果之一(include\linux\netfilter.h)，`NF_HOOK()`会根据返回结果对skb进行处理。
```C
/* Responses from hook functions. */
#define NF_DROP 0
#define NF_ACCEPT 1
#define NF_STOLEN 2
#define NF_QUEUE 3
#define NF_REPEAT 4
#define NF_STOP 5
#define NF_MAX_VERDICT NF_STOP
```
## iptables
iptables对规则的管理从两个维度出发
- 表 table：根据规则的功能维度来区分，通过`-t`参数可以指定表
- 链 chain：根据规则作用的时间点（HOOK标志点）来区分

路由判断 ip_rcv_finish->ip_route_input->skb_dst_set 
        dst_output

## 参考
1. ![netfilter源码和tc使用介绍（翻译自Traffic Control HOWTO）](https://wiki.dreamrunner.org/public_html/Linux/Networks/netfilter.html)
2. ![](https://zchan.moe/2024/08/25/%E5%A6%82%E4%BD%95%E9%85%8D%E7%BD%AEiptables/)