# VPP自身插件
## MACSWAPS
### 相关文件描述
##### myplugin.h
1. 在此文件下添加与业务相关的数据结构
2. 不要在此文件下船舰静态变量和全局变量（用于避免在插件间的命名冲突）
##### myplugin.c
这就是插件程序的main.c, 用于hook到vpp二进制API消息分发器上再添加他自己的消息到vpp全局的一个哈希表“message-name_crc"上(详见myplugin_init(...))。
1. VLIB_PLUGIN_REGISTER()用来生成vlib_plugin_registration_t结构。可以在此配置插件加载后的状态。也可以通过vpp命令行调整。
2. vpp用[dlsym函数](https://man7.org/linux/man-pages/man3/dlsym.3.html)来追踪其vlib_plugin_registration_t结构。Vpp只加载在plugin目录下的.so文件。
3. 默认生成的节点被VNET_FEATURE_INIT宏放到了vpp图节点中的device-input这一条边上
4. 使用VLIB_CLI_COMMAND宏配置此插件在VPP消息API表上信息
##### myplugin_test.c
此文件包含了用来生成单独.so文件的二进制API消息的生成代码。
1. vpp_api_test程序会加载这个.so，从而可以访问插件的API以进行外部客户端二进制API测试
2. vpp会加载测试插件，然后通过binary-api调试CLI，用于单元测试
##### node.c
这是图节点分发函数，通过修改此文件可以实现你自己的节点调度功能修改时尽量保留节点调度功能的结构）。
1. VLIB_NODE_FN是实际节点上的进行的函数，框架中在154-170实现了2路的包业务处理操作，在242-250实现了处理最后一个剩余（如果有的话）的包业务处理操作
### 流程
1. VLIB_REGISTER_NODE(myplugin/node.c:287)注册图节点
   ```
   VLIB_REGISTER_NODE (myplugin_node) = 
   {
    .name = "myplugin",
    .vector_size = sizeof (u32),
    .format_trace = format_myplugin_trace,
    .type = VLIB_NODE_TYPE_INTERNAL,
    
    .n_errors = ARRAY_LEN(myplugin_error_strings),
    .error_strings = myplugin_error_strings,

    // 轮询时的下一节点数量
    .n_next_nodes = MYPLUGIN_N_NEXT,

    /* edit / add dispositions here */
    .next_nodes = {
            [MYPLUGIN_NEXT_INTERFACE_OUTPUT] = "interface-output",
    },
   };
   ```

## Access Control Lists(ACLs)
