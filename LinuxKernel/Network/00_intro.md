# 客户端和服务端进程建立通信
## 通过socket连接
socket也可以用“一切皆文件”来理解，可将其视为一类特殊的文件，其使用遵循“打开open-读写read/write-关闭close”的模式。
### socket接口
一个socket连接是由它两端的套接字地址唯一确定的。这对套接字地址叫做套接字对(socket pair):(cliaddr:cliport, servaddr:servport)

![建立连接时使用的socket interface及其流程](pics/socket-interface.png)
#### socket接口常用数据结构
```C
// #include <sys/socket.h>
// #include <bits/sockaddr.h>
/* POSIX.1g specifies this type name for the `sa_family' member.  */
typedef unsigned short int sa_family_t;

/* This macro is used to declare the initial common members
   of the data types used for socket addresses, `struct sockaddr',
   `struct sockaddr_in', `struct sockaddr_un', etc.  */

#define	__SOCKADDR_COMMON(sa_prefix) \
  sa_family_t sa_prefix##family

/* Structure describing a generic socket address.  */
/* 指向要绑定给sockfd的协议地址。这个地址结构会根据地址创建socket时的地址协议族的不同而不同 */
struct sockaddr
{
    __SOCKADDR_COMMON (sa_);	/* Common data: address family and length. */
    /* sa_family_t sa_family; */
    char sa_data[14];		/* Address data.  */
};

/* Add more `struct sockaddr_AF' types here as necessary.
   These are all the ones I found on NetBSD and Linux.  */

# define __SOCKADDR_ALLTYPES \
  __SOCKADDR_ONETYPE (sockaddr) \
  __SOCKADDR_ONETYPE (sockaddr_at) \
  __SOCKADDR_ONETYPE (sockaddr_ax25) \
  __SOCKADDR_ONETYPE (sockaddr_dl) \
  __SOCKADDR_ONETYPE (sockaddr_eon) \
  __SOCKADDR_ONETYPE (sockaddr_in) \
  __SOCKADDR_ONETYPE (sockaddr_in6) \
  __SOCKADDR_ONETYPE (sockaddr_inarp) \
  __SOCKADDR_ONETYPE (sockaddr_ipx) \
  __SOCKADDR_ONETYPE (sockaddr_iso) \
  __SOCKADDR_ONETYPE (sockaddr_ns) \
  __SOCKADDR_ONETYPE (sockaddr_un) \
  __SOCKADDR_ONETYPE (sockaddr_x25)

struct sockaddr_in 
{ 
    sa_family_t sin_family; /* address family: AF_INET */ 
    in_port_t sin_port; /* port in network byte order */ 
    struct in_addr sin_addr; /* internet address */ 
}; 
/* Internet address. */ 
struct in_addr 
{ 
    uint32_t s_addr; /* address in network byte order */
};

/* ipv6对应的是：*/
struct sockaddr_in6
{
    sa_family_t sin6_family; /* AF_INET6 */ 
    in_port_t sin6_port; /* port number */ 
    uint32_t sin6_flowinfo; /* IPv6 flow information */ 
    struct in6_addr sin6_addr; /* IPv6 address */ 
    uint32_t sin6_scope_id; /* Scope ID (new in 2.4) */
}; 
struct in6_addr
{ 
    unsigned char s6_addr[16]; /* IPv6 address */ 
};

/* Unix域对应的是：*/
#define UNIX_PATH_MAX 108 
struct sockaddr_un 
{ 
    sa_family_t sun_family; /* AF_UNIX */
    char sun_path[UNIX_PATH_MAX]; /* pathname */ 
};

```
## 参考
https://zhuanlan.zhihu.com/p/100151937
