# VPP的图节点 node
vpp框架中，一个数据包的处理的逻辑由call graph定义，call graph由node组成，node是vpp框架（src/vlib/main.c:vlib_main_or_worker_loop函数）中的最小执行单元。
## node类型
src/vlib/node.h定义了node的分类
``` C
typedef enum
{
  /* An internal node on the call graph (could be output). */
  VLIB_NODE_TYPE_INTERNAL,

  /* Nodes which input data into the processing graph.
     Input nodes are called for each iteration of main loop. */
  VLIB_NODE_TYPE_INPUT,

  /* Nodes to be called before all input nodes.
     Used, for example, to clean out driver TX rings before
     processing input. */
  VLIB_NODE_TYPE_PRE_INPUT,

  /* "Process" nodes which can be suspended and later resumed. */
  VLIB_NODE_TYPE_PROCESS,

  VLIB_N_NODE_TYPE,
} vlib_node_type_t;
``` 
### 1. VLIB_NODE_TYPE_INTERNAL
通常运行于worker线程，负责处理数据包的业务node，当处理过程中有数据包在处理时被指定流向某node时才由框架调度（dispatch_pending_node函数调用）某node（.function）对数据包（frame）进行处理。
### 2. VLIB_NODE_TYPE_INPUT
大部分运行于worker线程上，但main线程上也会有。在执行顺序上先于INTERNAL node节点执行，所以有以下功能
1. 负责收取数据包
2. 负责处理INTERNAL node执行前的准备工作
INPUT node 以及 后面的PRE_INPUT node存在以下三种状态（定义于src/vlib/node.h）
1. VLIB_NODE_STATE_POLLING：默认状态，每次main loop都会被框架调度（dispatch_pending_node函数调用）其node（.function），且在三种不同的INPUT node中最先执行，此类型INPUT node使用一个叫`input_main_loops_per_call`的计数器，每次main_loop循环这个计数器会递减，直到此项为0后，才真正执行其函数。这样使得可以控制main_lopp循环时某些INPUT node(此计数器值较小者)可以比其他input node更常调用
2. VLIB_NODE_STATE_INTERRUPT：当给此类node发送信号（src/vlib/node_funcs.h:vlib_node_set_interrupt_pending函数）时，才会被框架调度，在三种不同的INPUT node中第二顺位被执行
3. VLIB_NODE_STATE_DISABLED：此类node默认不运行，需要通过（src/vlib/node_funcs.h:vlib_ndoe_set_state函数）调整node状态转变为上述两种状态之一后才可能被调度，一般在等待某个事件或准备工作完成后再调用vlib_ndoe_set_state函数使其运行，比如配置读取完成后。
### 3. VLIB_NODE_TYPE_PRE_INPUT
大部分运行于worker线程上，但main线程上也会有。在执行顺序上先于INPUT node节点执行，类型上与INPUT node相似，比较少使用，常用于实现控制平面的功能，比如对socket相关逻辑提供服务
### 4. VLIB_NODE_TYPE_PROCESS
运行在main线程上的协程，该类型node可以被挂起、等待事件执行(timer相关)、取消挂起恢复···，有独立的分配在heap上的运行时栈。类似于在一个线程中实现了多任务调度机制。主要用来修改vpp node内部参数
## node注册
```C
VLIB_REGISTER_NODE (ip4_input_node) = { // VLIB_REGISTER_NODE宏会生成带__attribute__((__constructor__))的函数代码，__attribute__((__constructor__))会使得函数在main函数前被调用
  .name = "ip4-input", // name字段是vpp内串联node使用的唯一标识，所以必须唯一
  .vector_size = sizeof (u32),
  .protocol_hint = VLIB_NODE_PROTO_HINT_IP4,

  .n_errors = IP4_N_ERROR, // 报错的计数，可以用来报错，也可以用来记录正常的数据包数量(show errors 命令显示)
  .error_strings = ip4_error_strings, // 计数统计的提示字符串

  .n_next_nodes = IP4_INPUT_N_NEXT, //next node的数量
  .next_nodes = {
    [IP4_INPUT_NEXT_DROP] = "error-drop",
    [IP4_INPUT_NEXT_PUNT] = "error-punt",
    [IP4_INPUT_NEXT_OPTIONS] = "ip4-options",
    [IP4_INPUT_NEXT_LOOKUP] = "ip4-lookup",
    [IP4_INPUT_NEXT_LOOKUP_MULTICAST] = "ip4-mfib-forward-lookup",
    [IP4_INPUT_NEXT_ICMP_ERROR] = "ip4-icmp-error",
    [IP4_INPUT_NEXT_REASSEMBLY] = "ip4-full-reassembly",
  },

  .format_buffer = format_ip4_header,
  .format_trace = format_ip4_input_trace, // show trace 显示路径的信息(一般是数据包到这个node时要输出的信息)
};
```
## node初始化
1. 在vlib_main(src/vlib/main.c:2138)函数中调用 vlib_register_all_static_nodes(src/vlib/node.c:505)函数遍历([VLIB_REGISTER_NODE](#main函数之前)注册的)vlib_main_t->node_main.node_registrations调用register_node(src/vlib/node.c:294)来创建(只是创建并基本的初始化不连接)所有的node。所有注册的node都会保存到vlib_main_t->node_main->nodes中，nodes则存储在一个vec_header_t中(这样相比链表更容易找到，但每次添加时要重新申请内存)
2. 在register_node(src/vlib/node.c:294)函数中不仅会将node添加到vlib_main_t->node_main->nodes中，还会根据node的类型区分添加到vlib_main_t->node_main另外两个数据结构(也都是vec_header_t的方式存储)中：
   - VLIB_NODE_TYPE_PROCESS: vlib_main_t->node_main->processes
   - 其他所有类型: vlib_main_t->node_main->nodes_by_type
3. 调用 vlib_node_main_init(src/vlib/node.c:596)函数对node graph进行初始化(可通过此函数返回值查看是否初始化成功)，包括根据node之间关系进行串联。
## node操作
在vlib_main_loop(src/vlib/main.c:2040)函数中处理node中的操作
### 进入vlib_main_loop

### 核心node操作循环
1. 收包的入口(src/vlib/main.c:1867-1872)
2. dispatch_node(src/vlib/main.c:1142)函数
   1. 执行node->function(由宏VLIB_NODE_FN宏生成,src/plugins/myplugin/node.c:93)
   2. node->function里会获取下一级node，并调用vlib_put_next_frame(src/vlib/main.c:483)函数将要交给下一级node的frame放到pending frame中以供下一级node可以取用并处理
   3. vlib_node_runtime_update_stats()函数更新node_runtime里的一些状态(如处理时间、vector数据包数量等)
   4. 如果在中断模式下，vector速率超过阈值，切换到polling模式
3. 根据nm->pending_frame的大小决定时回到1(收包入口dispatch_node)或是dispatch_pending_node(src/vlib/main.c:1912)