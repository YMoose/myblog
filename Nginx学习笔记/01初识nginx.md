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
查看configure支持哪些参数