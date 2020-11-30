# Kubernetes核心概念
k8s 是工业级的容器编排平台  
自动化的容器编排平台，负责应用的
- 部署
- 弹性
- 管理
## 核心功能
1. 服务发现与负载均衡
2. 容器自动编排
3. 存储编排
4. 自动容器恢复
5. 自动发布与回滚
6. 配置与密文管理
7. 批量执行
8. 水平伸缩
## 架构
二层的server-client架构。
### master
中央管控节点server
1. API server:组件与组件之间的消息传送，所有的组件都会和其进行连接。
2. contorller:对集群状态的管理
3. scheduler:完成调度操作
4. etcd:分布式存储系统，存储API server中需要的原信息。
### node
运行业务负载的，每个业务负载会以pod的形式运行。
1. pod:k8s最小的调度以及资源单元，由一个或多个容器组成。
2. kubelet:运行pod的组件，通过API server接受所需Pod运行的状态，然后提交到Container Runtime组成中。
3. Storage Plugin:对容器的存储进行管理
4. Network Plugin:对容器的网络进行管理
5. Kube-proxy:提供service 组网（利用了iptables组建Kubernetes的net work，也就是cluster network）
## 核心概念
### Pod
1. k8s最小的调度以及资源单元
2. 由一个或多个容器组成
3. 定义容器运行的方式（Command、环境变量）
4. 提供给容器共享的运行环境（网络、进程空间、存储）
### Volume 卷
1. Pod中容器可以访问的文件目录
2. 可以被挂载在Pod中一个或多个容器的指定路径下
3. 支持多种后端存储的抽象（本地存储、分布式存储、云存储等）
### Deployment 部署
1. 定义一组Pod的副本数量、版本等
2. 通过controller对Pod进行管理
   - 维持live pod的数量
   - 管理pod的版本
### Service
1. 提供访问一个或多个Pod实例的稳定访问地址，一遍使用者使用
2. 支持多种访问方式实现
   - ClusterIP
   - NodePort
   - LoadBalancer
###  Namespace
1. 一个集群内部的逻辑隔离机制（鉴权、资源额度）
2. 每个资源都属于一个Namespace
3. 同一个Namespace中的资源命名唯一
4. 不同Namespace中的资源可重名
## k8s API
HTTP(访问方式)+JSON/YAML(访问内容)
