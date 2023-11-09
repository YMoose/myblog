# 2. 存储管理
## 2.1 Linux内存管理的基本框架
Linux内核的映射机制设置为三层：
- PGD->PMD->PE->PA
虚拟Linux内存管理单元分成四步:  
1. CR3寄存器上的PGD作数组指针[内存里的页面目录(PGD)下标]->中间目录(PMD)数组指针
2. 中间目录(PMD)数组指针[内存里的中间目录(PMD)下标]->页面表(PT)数组指针
3. 页面表(PT)数组指针[内存里的页面表(PT)下标]->页面表项(PTE)(也即真实物理页面首地址)
4. 真实物理页面首地址[内存里物理页面下标]=真实物理地址  

linux内核将内存空间分成两部分，最高的1G字节(逻辑地址0xC0000000~0xFFFFFFFF会被映射到物理地址的0x00000000~0xBFFFFFFF), 若Linux需要操作1G以外内存空间的内存，使用的是修改填充内核空间PTE以ZONE_HIGHMEM(逻辑地址空间)访问其他内存空间的内存。  

GDT(全局段描述符表)的表项包括：
0. 第0项: 0(防止在加电后，段寄存器未经初始化就进入保护模式并使用GDT)
1. 第1项：永远是0
2. 第2项：内核代码段
3. 第3项：内核数据段
4. 第4项：当前进程的代码段
5. 第5项：当前进程的数据段
6. 其他进程的LDT(局部段描述符表)
7. 其他进程的TSS(任务状态段)
## 2.2 地址映射的全过程
### 内存管理方式
页式存储优点：
- 固定大小，方便管理
- 当与硬盘发生交换时，代价相比段式存储小  

一般会根据硬件选择映射方式。因为历史原因，i386采用先段式映射再页式映射的方式。
### 实际流程
以一个命令跳转指令call 0x08048568来看：
1. 先进行段式映射:
   1. CPU的指令计数器EIP指向0x08048568(为程序执行代码位置)，所以其在代码段中。=>用代码段寄存器CS来作为段式映射的段
   2.  解析段寄存器CS。根据段寄存器的定义，查看此段寄存器使用的是GDT还是LDT(通常内核代码使用GDT，用户进程代码使用LDT)，段寄存器最低两位RPL为所要求的特权级别。内核在建立进程时会(通过宏start_thread(include/asm-i386/processor.h))设置其段寄存器
   3.  查看start_thread后可以看到linux在设置段寄存器时使用的段描述符模板基本上s都使用的是GDT(linux内核中基本不使用LDT。只有VM86模式中运行wine或模拟windows时会使用LDT)
   4.  查看全局段描述符表GDT，初始GDT的内容在arch/i386/kernel/head.S中定义且主要内容在运行中基本不变
   5.  在全局段描述表GDT中各项显示段基址全为0，段长度全为4G(0xffff ffff ffff ffff)即每个段都是从0开始的整个4G逻辑地址空间，也即段映射后的线性地址与逻辑地址的值相同
2. 进入页式映射
   1. 页式映射第一级找到PGD(页面目录): MMU会以控制寄存器CR3的值为PGD的指针，PGD的指针是保存在每个进程的mm_struct数据结构中，每当切换进程时由inline函数switch_mm(include/asm-i386/mmu_context.h)对CR3进行设置(注意这里会将PGD地址先转化为物理地址再放置)，设置完成后代码还在内核空间中运行(即使用相同的页面映射，即用户进程设置的PGD的页面映射和内核进程的PGD的页面映射在映射逻辑地址中的内核内存空间时，采用的是一样的映射)
   2. 取逻辑地址的高10位作为偏移，在PGD中找到目录项，根据偏移找到的目录项的高20位和后面12个0拼接为页面表指针。为什么取12个零点原因为，每个页面表占一个页面(4KB对齐，一个地址4B，所以一个页面表中存放1024个页面地址指针)。同时目录项不用的后12位可以记录别的信息(比如最低为为P标志位，表明页面表是否在内存中)，类似
   3. 取逻辑地址的中间10位作为偏移，在PT中找到目标项(PTE)，并推出物理内存页面指针
   4. 取逻辑地址的末12位为偏移，在物理内存页面中找到物理地址  
   5. MMU在映射过程中会首先检查各个表项的P标志位(_PAGE_PRESENT)，如果P标志位显示页面无效则会产生一次缺页异常。

在现代计算机中会将PT和PTE装入cache中加快访问速度，另外整个过程由硬件实现，相对较快
### 其他
linux为了仿真运行段式存储软件还提供了两个相关的系统调用
1. modify_ldt(int func, void *ptr, unsigned long bytecount)
   可以改变当前进程的局部段描述表为`ptr`指向的一个结构modify_ldt_ldt_s中设置的局部段描述表(段式存储转换后是线性地址，所以仍然在页式存储的管理下)
2. ym86(struct vm86_struce *info)
   用于模拟另一种寻址方式VM86，用来在保护模式下模拟运行实地址模式的软件
## 2.3 重要的数据结构和函数
从硬件上来说，linux内核只需设置好PGD、PT、GDT、LDT及相关寄存器就可以完成地址映射。
从软件上来说，还需要进行内存管理。
### 硬件相关
#### 页式映射相关
1. 结构pgd_t/pmd_t/pte_t(include/asm-i386/page.h)
2. 结构pgprot_t(include/asm-i386/page.h): pte_t等中低12位用于页面的状态信息和访问权限不在pte_t等结构中，而在此结构中说明
   其值与低12位相对应，在(include/asm-i386/pgtable.h:148-158)中定义
3. 宏mk_pte(include/asm-i386/pgtable-2level.h): 将结构1左移12位和结构2两个结构结合成为实际的表项
4. 全局变量mem_map: 指向一个存放page数据结构的数组。软件可以用`mem_map[页面表项pte_t]`找到物理页面page数据结构(include/linux/mm.h)
5. 结构mem_map_t(include/linux/mm.h): 上述的物理页面page数据结构(结构中各个元素的次序是为了结构更加紧凑减少padding)
   ```
   /*
   * Try to keep the most commonly accessed fields in single cache lines
   * here (16 bytes or greater).  This ordering should be particularly
   * beneficial on 32-bit processors.
   *
   * The first line is data used in page cache lookup, the second line
   * is used for linear searches (eg. clock algorithm scans). 
   */
   typedef struct page {
      struct list_head list;
      struct address_space *mapping;
      unsigned long index; // 当内存页面来自于一个文件时，index代表该页面在文件中的序号
      struct page *next_hash;
      atomic_t count;
      unsigned long flags;	/* atomic flags, some possibly updated asynchronously */
      struct list_head lru;
      unsigned long age;
      wait_queue_head_t wait;
      struct page **pprev_hash;
      struct buffer_head * buffers;
      void *virtual; /* non-NULL if kmapped */
      struct zone_struct *zone;
   } mem_map_t;
   ```
   系统在初始化时根据物理内存的大小建立page结构数组mem_map，物理页面在其中被划分为ZONE_DMA\ZONE_NORMAL两个管理区(根据系统配置还可能有第三个管理区ZONE_HIGHMEM，用于物理地址超过1GB的存储空间)，每个管理区有个zone_struct数据结构
   - ZONE_DMA: DMA不经过MMU提供的地址映射，所以有很多额外的需求
6. 结构zone_struct(include/linux/mmzone.h):
   ```
   /*
 * Free memory management - zoned buddy allocator.
 */

#define MAX_ORDER 10

typedef struct free_area_struct {
	struct list_head	free_list;
	unsigned int		*map;
} free_area_t;

struct pglist_data;

typedef struct zone_struct {
	/*
	 * Commonly accessed fields:
	 */
	spinlock_t		lock;
	unsigned long		offset;
	unsigned long		free_pages;
	unsigned long		inactive_clean_pages;
	unsigned long		inactive_dirty_pages;
	unsigned long		pages_min, pages_low, pages_high;

	/*
	 * free areas of different sizes
	 */
	struct list_head	inactive_clean_list;
	free_area_t		free_area[MAX_ORDER];

	/*
	 * rarely used fields:
	 */
	char			*name;
	unsigned long		size;
	/*
	 * Discontig memory support fields.
	 */
	struct pglist_data	*zone_pgdat;
	unsigned long		zone_start_paddr;
	unsigned long		zone_start_mapnr;
	struct page		*zone_mem_map;
} zone_t;
   ```
7. 宏set_pte(include/asm-i386/pgtable-2level.h): 把一个表项的值设置到一个页面表项中
8. 宏pte_none(include/asm-i386/pgtable-2level.h): 判断页面表项是否为0(未给这个表项及其所代表的页面)建立映射
9.  宏pte_present(include/asm-i386/pgtable.h): 判断P标志位是否为0(已经映射，但物理页面不在内存中，在交换设备上)
10.  宏pte_page(include/asm-i386/pgtable-2level.h): 利用mem_map，再根据物理内存地址找到物理页面page数据结构
11. 宏virt_to_page(include/asm-i386/page.h): 根据虚存地址找到相应的物理页面page数据结构 