# 初识Nginx
## 主要应用场景
1. 静态资源服务  
2. 反向代理服务
3. API服务  

一个web请求往往会先经过nginx，再到应用服务，再访问数据库。  
问题：应用服务运行效率低 => 解决：应用服务组成集群（提供高可用性）  
而集群又带来如下问题
- 问题1：请求传导 => 解决：nginx反向代理功能
- 问题2：动态扩容&容灾 => 解决：nginx负债均衡  

问题：整条链路过长，时延长 => 解决：nginx缓存&静态资源
问题：应用服务速率与数据库速率不匹配 => 解决：nginx直接访问数据库（提供API服务）
## 组成部分
1. nginx:可执行文件
2. nginx.conf:配置文件（控制nginx行为）
3. access.log:访问日志（记录每一条http请求信息）
4. error.log:错误日志（定位问题）
## 源码目录
|- auto 用于编译
    |- cc 用于编译
    |- os 对操作系统判断
    |- 其他 辅助configure脚本
|- change 版本修改（包括Feature、Bugfix、Change）
|- conf 示例文件
|- configure 编译前生成中间文件
|- contrib 
    |- vim vim配合nginx开发的工具
|- html 默认的两个网页文件一个是500，一个是index
|- man linux下的帮助文件
|- src 源代码
## 编译
`configure --help`  
查看configure支持哪些参数  
带with的参数是默认不编译的，带without的参数是默认编译的
configure生成的临时文件存放在objs文件夹中  
会编译进nginx的模块可以在 ngx_modules.c中查看  
`make`
make后的目标文件也是存放在objs/src目录中
版本升级时需要从此拷贝目标文件
如使用动态模块，则.so文件也在此  
`make install`  
安装完成后可以在prefix指定的目录下看到如下目录  
|- sbin
    |- nginx 可执行文件
|- conf 存放配置（从源码包拷贝过来的） 
|- logs 存放日志
## nginx配置
### 配置语法
1. 配置文件由指令与指令块构成
2. 每条指令以`;`结尾，指令与参数间以空格分隔
3. 指令块以{}大括号将多条指令组织在一起，是否有名字取决于模块
4. include语句允许组合多个配置文件提升可维护性
5. 使用`#`符号使用注释
6. 使用`$`符号使用变量
7. 部分参数支持正则表达式
