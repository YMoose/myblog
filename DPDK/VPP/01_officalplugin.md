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
1. VLIB_NODE_FN是实际节点上的进行的函数，是图节点分发函数，框架中在154-170实现了2路的包业务处理操作，在242-250实现了处理最后一个剩余（如果有的话）的包业务处理操作
        ```
        // 节点处理分发函数框架
        VLIB_NODE_FN (myplugin_node) (vlib_main_t * vm,
                        vlib_node_runtime_t * node,
                        vlib_frame_t * frame)
        {
        u32 n_left_from, * from, * to_next; // (重要的变量): 表明我们还有多少任务要完成
        myplugin_next_t next_index; // 我们可能会将数据包发送过去的节点
        u32 pkts_swapped = 0;

        from = vlib_frame_vector_args (frame);
        n_left_from = frame->n_vectors;
        next_index = node->cached_next_index;

        // 处理分发双循环
        while (n_left_from > 0)
        {
        u32 n_left_to_next;

        vlib_get_next_frame (vm, node, next_index,
                                to_next, n_left_to_next); // 查看一下neighbour node(通常是next index)的资源及权限是否够本node使用，"to_next"就是我们可以写入数据包的指针，"n_left_to_next"就是我们可以写多少包。

        while (n_left_from >= 4 && n_left_to_next >= 2) // 如果要处理的数据包大于4，且可以写入到下一个节点的空间大于2个数据包大小，则进行双路处理。如果剩余要处理的包不满足条件回到单路循环
                {
                u32 next0 = MYPLUGIN_NEXT_INTERFACE_OUTPUT;
                u32 next1 = MYPLUGIN_NEXT_INTERFACE_OUTPUT;
                u32 sw_if_index0, sw_if_index1;
                u8 tmp0[6], tmp1[6];
                ethernet_header_t *en0, *en1;
                u32 bi0, bi1;
                vlib_buffer_t * b0, * b1;
                
                /* Prefetch next iteration. */
                {
                vlib_buffer_t * p2, * p3;
                
                p2 = vlib_get_buffer (vm, from[2]);
                p3 = vlib_get_buffer (vm, from[3]);
                
                vlib_prefetch_buffer_header (p2, LOAD);
                vlib_prefetch_buffer_header (p3, LOAD);

                CLIB_PREFETCH (p2->data, CLIB_CACHE_LINE_BYTES, STORE);
                CLIB_PREFETCH (p3->data, CLIB_CACHE_LINE_BYTES, STORE);
                }

                /* speculatively enqueue b0 and b1 to the current next frame */
                to_next[0] = bi0 = from[0];
                to_next[1] = bi1 = from[1];
                from += 2;
                to_next += 2;
                n_left_from -= 2;
                n_left_to_next -= 2;

                b0 = vlib_get_buffer (vm, bi0);
                b1 = vlib_get_buffer (vm, bi1);

                ASSERT (b0->current_data == 0);
                ASSERT (b1->current_data == 0);
                
                en0 = vlib_buffer_get_current (b0);
                en1 = vlib_buffer_get_current (b1);

                /* This is not the fastest way to swap src + dst mac addresses */
        #define _(a) tmp0[a] = en0->src_address[a];
                foreach_mac_address_offset;
        #undef _
        #define _(a) en0->src_address[a] = en0->dst_address[a];
                foreach_mac_address_offset;
        #undef _
        #define _(a) en0->dst_address[a] = tmp0[a];
                foreach_mac_address_offset;
        #undef _

        #define _(a) tmp1[a] = en1->src_address[a];
                foreach_mac_address_offset;
        #undef _
        #define _(a) en1->src_address[a] = en1->dst_address[a];
                foreach_mac_address_offset;
        #undef _
        #define _(a) en1->dst_address[a] = tmp1[a];
                foreach_mac_address_offset;
        #undef _

                sw_if_index0 = vnet_buffer(b0)->sw_if_index[VLIB_RX];
                sw_if_index1 = vnet_buffer(b1)->sw_if_index[VLIB_RX];

                /* Send pkt back out the RX interface */
                vnet_buffer(b0)->sw_if_index[VLIB_TX] = sw_if_index0;
                vnet_buffer(b1)->sw_if_index[VLIB_TX] = sw_if_index1;

                pkts_swapped += 2;

                if (PREDICT_FALSE((node->flags & VLIB_NODE_FLAG_TRACE)))
                {
                if (b0->flags & VLIB_BUFFER_IS_TRACED) 
                        {
                        myplugin_trace_t *t = 
                        vlib_add_trace (vm, node, b0, sizeof (*t));
                        t->sw_if_index = sw_if_index0;
                        t->next_index = next0;
                        clib_memcpy (t->new_src_mac, en0->src_address,
                                        sizeof (t->new_src_mac));
                        clib_memcpy (t->new_dst_mac, en0->dst_address,
                                        sizeof (t->new_dst_mac));
                        }
                        if (b1->flags & VLIB_BUFFER_IS_TRACED) 
                        {
                        myplugin_trace_t *t = 
                        vlib_add_trace (vm, node, b1, sizeof (*t));
                        t->sw_if_index = sw_if_index1;
                        t->next_index = next1;
                        clib_memcpy (t->new_src_mac, en1->src_address,
                                        sizeof (t->new_src_mac));
                        clib_memcpy (t->new_dst_mac, en1->dst_address,
                                        sizeof (t->new_dst_mac));
                        }
                }
                
                /* verify speculative enqueues, maybe switch current next frame */
                vlib_validate_buffer_enqueue_x2 (vm, node, next_index,
                                                to_next, n_left_to_next,
                                                bi0, bi1, next0, next1); // 直接两个包一起入队
                }
        // simple loop 单路处理分发循环
        while (n_left_from > 0 && n_left_to_next > 0)
                {
                u32 bi0;
                vlib_buffer_t * b0;
                u32 next0 = MYPLUGIN_NEXT_INTERFACE_OUTPUT; // 设置下一节点的index
                u32 sw_if_index0; // 用来抛出数据包的output类型的节点index
                u8 tmp0[6];
                ethernet_header_t *en0;

                /* speculatively enqueue b0 to the current next frame */
                bi0 = from[0]; // 设置buffer index 
                to_next[0] = bi0; // 让这个buffer index入队到下一个节点的frame
                from += 1;
                to_next += 1;
                n_left_from -= 1;
                n_left_to_next -= 1;

                b0 = vlib_get_buffer (vm, bi0); // 获取数据包buffer index，然后我们可以直接操作b0
                /* 
                * Direct from the driver, we should be at offset 0
                * aka at &b0->data[0]
                */
                ASSERT (b0->current_data == 0);
                
                en0 = vlib_buffer_get_current (b0); // 获取buffer的指针

                /* This is not the fastest way to swap src + dst mac addresses */
                // 用宏进行macswap(六元组替换)
        #define _(a) tmp0[a] = en0->src_address[a];
                foreach_mac_address_offset;
        #undef _
        #define _(a) en0->src_address[a] = en0->dst_address[a];
                foreach_mac_address_offset;
        #undef _
        #define _(a) en0->dst_address[a] = tmp0[a];
                foreach_mac_address_offset;
        #undef _

                sw_if_index0 = vnet_buffer(b0)->sw_if_index[VLIB_RX];

                /* Send pkt back out the RX interface */
                vnet_buffer(b0)->sw_if_index[VLIB_TX] = sw_if_index0; // 将数据包发回到接受数据包的网卡上去发送

                if (PREDICT_FALSE((node->flags & VLIB_NODE_FLAG_TRACE) 
                                && (b0->flags & VLIB_BUFFER_IS_TRACED))) {
                myplugin_trace_t *t = 
                vlib_add_trace (vm, node, b0, sizeof (*t));
                t->sw_if_index = sw_if_index0;
                t->next_index = next0;
                clib_memcpy (t->new_src_mac, en0->src_address,
                                sizeof (t->new_src_mac));
                clib_memcpy (t->new_dst_mac, en0->dst_address,
                                sizeof (t->new_dst_mac));
                }
                
                pkts_swapped += 1;

                /* verify speculative enqueue, maybe switch current next frame */
                vlib_validate_buffer_enqueue_x1 (vm, node, next_index,
                                                to_next, n_left_to_next,
                                                bi0, next0);
                }

        vlib_put_next_frame (vm, node, next_index, n_left_to_next); // 将我们的frame加到pending 队列中，之后会又vpp 主循环(vlib_main_loop)处理
        }

        vlib_node_increment_counter (vm, myplugin_node.index, 
                                MYPLUGIN_ERROR_SWAPPED, pkts_swapped); // 计数
        return frame->n_vectors;
        }
        ```
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
