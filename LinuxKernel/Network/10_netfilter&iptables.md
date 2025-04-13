# netfilter & iptables
netfilter是一个内核模块，是linux内核处理网卡收到内核的数据包的一个框架
iptables是对应的用户工具，用户通过iptables命令下发规则，规则在netfilter框架中应用，匹配并按照规则处理数据包，iptables主要是应对ipv4协议（以及传输层协议）的规则，相应的还有ip6tables应对ipv6协议（以及传输层协议）的规则、arptables等工具
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
- NF_INET_PRE_ROUTING: incoming packets pass this hook in the `ip_rcv() (linux/net/ipv4/ip_input.c)` function before they are processed by the routing code.
- NF_INET_LOCAL_IN: all incoming packets addressed to the local computer pass this hook in the function `ip_local_deliver() (linux/net/ipv4/ip_input.c)`.
- NF_INET_FORWARD: incoming packets are passed this hook in the function `ip_forward() (net\ipv4\ip_forward.c)`.
- NF_INET_LOCAL_OUT: all outgoing packets created in the local computer pass this hook in the function `ip_local_out() (net\ipv4\ip_output.c)`.
- NF_INET_POST_ROUTING: this hook in the `ip_mc_output() (net\ipv4\ip_output.c)` function before they leave the computer.

以此可见，netfilter框架实际运行于Network Protocol层中

在内核中通过`nf_register_hook()/nf_register_hooks() (net/netfilter/core.c)`将`nf_hook_ops`注册到`struct list_head nf_hooks[NFPROTO_NUMPROTO][NF_MAX_HOOKS]`这个链表数组中
```C
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
内核协议栈在处理某个协议的包时，会调用`NF_HOOK()`宏（或是函数），`NF_HOOK()`需要两个参数
- pf：协议族
- hook：上面提到的HOOK标志点
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
## 参考
1. ![netfilter源码和tc使用介绍（翻译自Traffic Control HOWTO）](https://wiki.dreamrunner.org/public_html/Linux/Networks/netfilter.html)
2. ![](https://zchan.moe/2024/08/25/%E5%A6%82%E4%BD%95%E9%85%8D%E7%BD%AEiptables/)