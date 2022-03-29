# Initiation
![vpp init sequence](vpp-init-sequence.png)
## debug
### breakpoints
## process
### main函数之前
通过宏VLIB_INIT_FUNCTION(src/vlib/init.h:173)来定义vlib中一些基础引用库的初始化函数并将定义好的初始化函数以及创建好其函数指针，以便之后注册其到vlib_main_t->init_function_registrations中去。  
类似的由宏来定义和构造函数以便创建全局数据结构中的一些链表元素的还有(可以参考src/vlib/init.h)
|宏|链表|
| - | - |
|VLIB_INIT_FUNCTION(src/vlib/init.h:173)|vlib_main_t->init_function_registrations|
|VLIB_WORKER_INIT_FUNCTION(src/vlib/init.h:174)|vlib_main_t->worker_init_function_registrations|
|VLIB_MAIN_LOOP_ENTER_FUNCTION(src/vlib/init.h:176)|vlib_main_t->main_loop_enter_function_registrations|
|VLIB_MAIN_LOOP_EXIT_FUNCTION(src/vlib/init.h:178)|vlib_main_t->main_loop_exit_function_registrations|
|VLIB_CONFIG_FUNCTION(src/vlib/init.h:182)|vlib_main_t->config_function_registrations|
|VLIB_EARLY_CONFIG_FUNCTION(src/vlib/init.h:226)|vlib_main_t->config_function_registrations|
|VLIB_API_INIT_FUNCTION(src/vlibapi/api.h:42)|vlib_main_t->api_init_function_registrations|
|VLIB_CLI_COMMAND(src/vlib/cli.h:158)|vlib_main_t->cli_main|
|VLIB_REGISTER_NODE(src/vlib/node.h:170)|vlib_main_t->node_main.node_registrations|

### main函数
1. main函数(src/vpp/vnet/main.c:106)为程序入口，随后调用vlib_unix_main()
2. vlib_unix_main函数(src/vlib/unix/main.c)
   1. vlib_plugin_early_init(src/vlib/unix/plugin.c:587)函数(通过dlopen)加载插件目录下所有插件并(通过dlsym)取得插件的VLIB_PLUGIN_REGISTER宏构造的一些插件描述信息的符号地址 
   2.  vlib_call_all_config_functions(src/vlib/init.c:405)函数解析所有的命令行选项，并调用vlib_config_function_runtime_t->function([VLIB_CONFIG_FUNCTION](#main函数之前)构造出来的函数)对插件本身的一些配置进行解析配置
   3.  vlib_thread_stack_init(src/vlib/unix/main.c677)为下列线程创建线程栈
      - 普通线程：统计采集等工作
      - EAL线程：处理包的工作
      - processes：定期执行、相互写作的多线程(如DHCP租期续订等)，vpp主线程超时到期后会执行
   4. (通过clib_calljmp)跳转到thread0函数(src/vlib/unix/main.c:661)，调用vlib_main(src/vlib/main.c:2138)函数
3. vlib_main(src/vlib/main.c:2138)函数
   1. 调用 vlib_register_all_static_nodes(src/vlib/node.c:505)函数遍历([VLIB_REGISTER_NODE](#main函数之前)注册的)vlib_main_t->node_main.node_registrations来创建(只是创建)node，然后调用 vlib_node_main_init(src/vlib/node.c:596)函数对node grahp进行初始化
   2. 调用 vlib_call_all_init_functions()函数遍历([VLIB_INIT_FUNCTION](#main函数之前)注册的)vlib_main_t->init_function_registrations来调用各个初始化方法
   3. 调用vlib_call_all_main_loop_enter_functions(src/vlib/init.c:391)函数遍历([VLIB_MAIN_LOOP_ENTER_FUNCTION](#main函数之前)注册的)vlib_main_t->main_loop_enter_function_registrations
   4. 调用vlib_main_loop()函数
4. vlib_main_loop(src/vlib/main.c:2040)函数
   1. 创建前面2.3提到的线程(src/vlib/main.c:1792)
   2. 在while(1)循环中处理不同类型的node，并dispatch其到合适的中间node，再由dispatch_pending_node()函数进一步处理被分发的数据包。其中也会处理一部分中断请求并分发到中间node进行处理
      - pre-input node(src/vlib/main.c:1857): 类似于DBG_CLI的控制node
      - input node(src/vlib/main.c:1866): 主要收包node，主要从网卡或者硬件加速器获取数据包
      - local interrupts(src/vlib/main.c:1878)
      - remote interrupts(src/vlib/main.c:1888)
   3. (src/vlib/main.c:1908)在input node将收到的数据包添加到pending vector中并由dispatch_pending_node(src/vlib/main.c:1333)函数交给之后的node处理