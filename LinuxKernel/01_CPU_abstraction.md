# 1. CPU的抽象
进程就是计算任务（job）的直接抽象，CPU就是job最直接使用的硬件资源。

## 1.1 通过时分共享将单个CPU抽象为多个CPU
通过硬件提供的机制，操作系统接管CPU，为多个job调度CPU，达成某一时间段内多个job并发进行。
## 1.1.1 操作系统的机制
受限直接执行
## 1.2. linux中的实现
### 1.2.1. 术语 terminology
进程描述符（process descriptor）：struct task_struct (include/linux/sched.h)
任务队列（task list/array）：struct task_struct的双向循环列表
进程标识符（process identification value）：pid sttuct task_struct -> pid_t pid（其中pid_t是一个隐含数据类型，即不确定其具体类型）
进程上下文（process context）：当一个进程（通过调用系统调用或异常、中断等方法）进入内核空间，内核“代表进程执行”，称为处于（当前）进程上下文中（在此上下文中宏current有效）。
#### 1.2.1.1. 进程状态
todo
### 1.2.2. 内核访问进程描述符
内核访问当前进程的进程描述符是一个非常频繁的操作，通过宏current(include/asm/current.h)获得
- 在x86架构，寄存器比较少，为了保证效率，早期linux把task_struct放在进程内核栈栈底，后续task_strcut改由slab分配，则引入了中间层struct thread_info(include/asm/thread_info.h)来间接访问（同时thread_info提供了一些通用功能），thread_info相比于task_struct数据结构更小，相对固定，放在放在进程内核栈栈底可以通过栈指针快速访问
- 在PowerPC架构，寄存器较多，可以直接将task_struct指针存放于寄存器上。

### 1.2.3. 进程创建
基于Unix系统的KISS设计哲学，unix系统中的进程创建（spawn）被拆解成了两个步骤：fork和exec。