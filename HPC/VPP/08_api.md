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
### Common code
- vapi.h: 包括了c可以使用的一些基础api功能，如连接/断开连接，执行消息发现以及发送/接收消息的功能
- vapi.hpp: 
### Generated code
源代码树中存在的每个API文件都会自动转换为JSON文件，代码生成器会解析并生成可供client使用的代码以供于VPP进行交互。  
交互功能包括：
1. 自动字节交换
2. 基于上下文的自动请求-响应匹配
3. 调用回调时自动强制转换为适当的类型（类型安全）
4. 自动发送转储邮件的控制命令  

API支持阻塞/无阻塞的操作方式。阻塞模式下，调用和接受都会阻塞；非阻塞模式下，当无法执行操作时API都会返回VAPI_EAGAIN，让client决定是否等待或进行别的什么操作
### code generator
#### C的高级api
c的高级API严格基于回调，以提升最大效率。发送请求时需要注册处理响应的回调函数，当响应返回时则调用此回调函数。对于注册事件类型，事件到达时，也会调用回调函数
#### c++的高级API
c++ api中响应是和请求绑定在同一个(Request/Dump/Event_registration)对象中的且可以指定响应回调函数。(这里注意要释放响应数据的空间因为响应占共享内存的)
## message格式
### request
1. client_index: 请求时必须带上，用来识别客户端的cookie
2. context: 请求时必须带上，响应时会返回相同的context，让客户端匹配响应包
### reply
1. context: 由请求带过来的原样带回去，让客户端匹配响应包
2. 

## 参考
1. [vpp中plugin的api编程（1） - 简单使用](https://blog.51cto.com/zhangchixiang/2128565)
2. [vpp中plugin的api编程（2） - VPE vs. Plugin Messages id](https://blog.51cto.com/zhangchixiang/2128871)
3. [VPP API机制分析（上）](https://segmentfault.com/a/1190000019613730)