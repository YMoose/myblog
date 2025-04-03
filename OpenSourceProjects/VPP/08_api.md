# VPP API
VPP API模块允许通过共享内存/Unix域套接字(SOCK_STREAM)和VPP通信，有以下三类API
- common code - low-level API
- generated code - high-level API
- code generator - to generate users' high-level API e.g. for custom plugins  

vpp二进制api的信息交换方式包括一下三种
- Request/Reply: 客户端发送请求，服务器响应。通常命名为 method_name + _reply
- Dump/Detail: 客户端批量发送请求，服务端用大量细节信息响应，这些信息可能不是同一种类型。这类请求的响应会跟随一个控制信息，否则客户端无法知道信息何事结束。通常请求命名为 method + _dump，响应命名为method + _details。(与Request/Reply的区别是此方法要大量信息，常用于要一整个转发表) 
- Events: 客户端可以向服务端注册异步通知(返回的通知结果是在服务端定义的)，常用于获取网卡状态改变的信息。通常命名为 want_XXX。
## API使用
VPP API的connect接口好像暂时还不支持网络socket，只能使用unix socket
一般应用开发推荐使用Generated code，写好api文件后可以通过vpp的代码生成器vppapigen自动生成代码。
### API 文件
api文件主要用于定义api消息，有三种消息
除了消息以外还可以定义一些消息使用的数据结构和常量
``` vpp api
enum address_family {
  ADDRESS_IP4 = 0,
  ADDRESS_IP6,
};

union address_union {
  vl_api_ip4_address_t ip4;
  vl_api_ip6_address_t ip6;
};

typedef address {
  vl_api_address_family_t af;
  vl_api_address_union_t un;
};
```
#### 1. request/reply
主要用于对vpp的控制
``` vpp api
define show_version
{
  u32 client_index; // 请求时必须带上，用来识别客户端的cookie
  u32 context;      // 请求时必须带上，响应时会返回相同的context，让客户端匹配响应包
};

define show_version_reply
{
  u32 context;
  i32 retval;
  string program [32];
  string version [32];
  string build_date [32];
  /* The final field can be a variable length argument */
  string build_directory [];
};

autoreply define show_sub_version // 使用autoreply宏会自动创建reply消息，且此reply消息仅有retval字段
{
    u32 client_index;
    u32 context;
} 
```
#### 2. Dump/Detail
主要用于获取vpp内部的配置信息。
#### 3. Events
用于注册观察者，当vpp内部特定事件触发时通知客户端。
### 生成代码
通过vpp的代码生成器vppapigen自动生成代码，或者vpp提供的cmake中在某些目标的编译中填充API_FILES编译变量。
会生成
- xxx.api_enum.h: message的类型枚举定义头文件
- xxx.api_type.h: message的数据结构定义头文件
- xxx.api.vapi.h: message的相关函数头文件
- xxx.api.c: message在vpp内部注册的函数实现
### vpp内部注册api
使用api需要在vpp内部实现对api message的处理函数
```C
vl_api_xxx_t_handler(vl_api_xxx_t *mp)
{
    //todo
}
```
在使用前，需要在vpp框架中通过调用xxx.api.c:setup_message_id_table()函数对api进行注册。
### 外部程序调用api
```C
#include <vapi/vapi.h>
#include "vapi/xxx.api.vapi.h"

vapi_error_e
xxx_cb (struct vapi_ctx_s *ctx, void *callback_ctx, vapi_error_e rv, bool is_last, vapi_payload_xxx_reply *reply)
{
    // handle reply
    return VAPI_OK;
}

int main (int argc, chat *argv[])
{
    vapi_ctx_t ctx;
    vapi_error_r rv = vapi_ctx_alloc (&ctx);

    if (rv != VAPI_OK)
        return rv;
    
    /* 通过/run/vpp/api.sock 与vpp进行socket连接 */
    rv = vapi_connect (ctx, "test", NULL, 64, 32, VAPI_MODE_BLOCKING, true);

    if (rv != VAPI_OK)
        return rv;
    
    /* 每次消息传递需要申请消息的内存，vpp内部接受消息后会释放，不用在外部程序中手动释放，下次再次发送消息需要重新申请 */
    vpai_msg_xxx* sv = vapi_alloc_xxx(ctx);

    // fill sv
    
    /* 发送消息 */
    rv = vapi_xxx (ctx, sv, xxx_cb, NULL);
    if (rv != VAPI_OK)
        return rv;

    rv = vapi_disconnect (ctx);
    if (rv != VAPI_OK)
        return rv;
    
    vapi_ctx_free (ctx);
    return 0;
}
```
## 参考
1. vpp api官方文档
2. [vpp中plugin的api编程（1） - 简单使用](https://blog.51cto.com/zhangchixiang/2128565)
3. [vpp中plugin的api编程（2） - VPE vs. Plugin Messages id](https://blog.51cto.com/zhangchixiang/2128871)
4. [VPP API机制分析（上）](https://segmentfault.com/a/1190000019613730)