# vpp vlib node
## node 类型
src/vlib/node.h
```
typedef enum
{
  /* An internal node on the call graph (could be output). */
  VLIB_NODE_TYPE_INTERNAL, // 真正处理数据包的业务node

  /* Nodes which input data into the processing graph.
     Input nodes are called for each iteration of main loop. */
  VLIB_NODE_TYPE_INPUT, // 收包逻辑node，如：dpdk，pcap等

  /* Nodes to be called before all input nodes.
     Used, for example, to clean out driver TX rings before
     processing input. */
  VLIB_NODE_TYPE_PRE_INPUT, // 目前只有一个epoll node，对socket相关逻辑提供服务，主要使用于控制业务

  /* "Process" nodes which can be suspended and later resumed. */
  VLIB_NODE_TYPE_PROCESS, // 该类型node可以被挂起也可以被恢复，有独立的分配在heap上的运行时栈。类似于在一个线程中实现了多任务调度机制。主要用来修改vpp node内部参数

  VLIB_N_NODE_TYPE,
} vlib_node_type_t;
```
### 1. VLIB_NODE_TYPE_INTERNAL 
最常见也是最重要的节点类型，此类节点是真正处理数据包业务的node
### 2. VLIB_NODE_TYPE_INPUT
此类节点用于包的起点(input)，其内部分为以下几类：
src/vlib/node.h
```
#define foreach_vlib_node_state					\
  /* Input node is called each iteration of main loop.		\
     This is the default (zero). */				\
  _ (POLLING)							\
  /* Input node is called when device signals an interrupt. (我猜测是用于packet-generator) */	\
  _ (INTERRUPT)							\
  /* Input node is never called. */				\
  _ (DISABLED)

typedef enum
{
#define _(f) VLIB_NODE_STATE_##f,
  foreach_vlib_node_state
#undef _
    VLIB_N_NODE_STATE,
} vlib_node_state_t;
```
另外input node 类型的数据结构中有一个叫`input_main_loop_per_call`的计数器，每次main_loop循环这个计数器会递减，直到此项为0后，才真正执行其函数。这样使得可以控制main_lopp循环时某些input node(此计数器值较小者)可以比其他input node更常调用
### 3. VLIB_NODE_TYPE_PRE_INPUT
此类节点是在input节点之前的节点，常用于实现控制平面的功能
### 4. VLIB_NODE_TYPE_PROCESS
类似线程的函数(类似中断处理程序)，可以被挂起、等待事件执行(timer相关)、取消挂起恢复···
   