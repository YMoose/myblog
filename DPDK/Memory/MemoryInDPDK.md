# DPDK中的内存
## 基本概念
1. 大页内存 hugepage: 使用大页内存可以用有限的TLB管理映射更多的内存，减少TLB miss
2. 内存NUMA亲和性: NUMA架构下CPU中内存控制器对本地内存的的访问更快，DPDK API提供明确的NUMA感知能力提升内存访问速率
3. 直接内存访问 DMA：DMA传输将数据从一个地址空间复制到另一个地址空间，提供在外设和存储器之间或者存储器和存储器之间的高速数据传输。当CPU初始化这个传输动作，传输动作本身是由DMA控制器来实现和完成的。实际上就是CPU将部分总线操作的工作交给了DMA控制器。一方面针对用户空间进程，DPDK利用DMA拷贝到内存后，用mmap直接将RX buffer数据映射到了用户空间，实现了零拷贝技术。另一方面针对设备，作为DMA用途的内存地址必须申请为[Pinned Memory(a.k.a. Page Locked Memory, Locked Pages, etc):Pinned memory are virtual memory pages that are specially marked so that they cannot be paged-out to disk](https://archive.kernel.org/oldlinux/htmldocs/kernel-api/API-get-user-pages-fast.html)，作为pinned memory，其物理地址会在一段时间内不改变，DPDK设备可以直接使用这些pinned memory的物理地址自行启动DMA传输，加速操作。
4. IOMMU & IOVA: 管理硬件设备可访问的内存，以便不可信进程污染内存。当IOMMU使用时，硬件设备访问的地址可能时由IOMMU分配的虚拟地址(IOVA)。DPDK可以感知物理内存布局，并利用IOMMU针对不同的用例更合理地利用内存。
5. DPDK内存分配: DPDK分配大页并在此内存中创建一个堆(heap)并将其提供给用户应用程序并用于存取应用程序内部的数据结构。自定义的内存分配器使得上述优化得以实行。DPDK也实现了固定大小内存的内存池，并进行了极致的优化，并向用户提供了可选的线程安全等级
## DPDK
DPDK是一个用户态应用框架，其提供了用户态PMD和一组API以实现完全以用户态执行IO操作。

## 参考
1. https://www.sdnlab.com/23475.html
2. https://www.sdnlab.com/23551.html
3. https://www.intel.com/content/www/us/en/developer/articles/technical/memory-in-dpdk-part-2-deep-dive-into-iova.html
4. https://www.intel.com/content/www/us/en/developer/articles/technical/memory-in-dpdk-part-3-1711-and-earlier-releases.html
5. https://www.intel.com/content/www/us/en/developer/articles/technical/memory-in-dpdk-part-4-1811-and-beyond.html
6. https://zhuanlan.zhihu.com/p/363622877
7. https://archive.kernel.org/oldlinux/htmldocs/kernel-api/API-get-user-pages-fast.html