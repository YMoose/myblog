# Dynamic DMA mapping using the generic device  动态DMA映射
此文档介绍了DMA API。如需对API更通俗的介绍可以参考[动态DMA映射指南](https://www.kernel.org/doc/html/latest/core-api/dma-api-howto.html)

DMA API和此文档被分成了两部分。文档第一部分将介绍基本API。第二部分则会介绍API中支持不一致内存计算机的部分。除非您知道你的驱动程序必须要支持不一致内存的平台（通常来说是一些遗留平台），否则您应该只使用第一部分介绍的基本API。

## Part I - dma_API 第一部分 dma_API
要使用dma_API，您必须#include <linux/dma-mapping.h>。这个头文件声明了dma_addr_t数据结构及其接口。 
dma_addr_t 可以保存平台所有有效的 DMA 地址。它可以提供给设备这些DMA地址以用作 DMA的 源或目标。CPU 无法直接引用 dma_addr_t，因为其可能需要在物理地址空间和 DMA 地址空间之间转换。

### Part Ia - Using large DMA-coherent buffers 使用大容量DMA一致性缓存
```C
void *
dma_alloc_coherent(struct device *dev, size_t size,
                   dma_addr_t *dma_handle, gfp_t flag)
```
一致性内存（Consistent memory）是指一种特殊的内存区域，设备或处理器对它的写入操作可以立即被处理器或设备读取，而无需考虑缓存一致性问题（但需要注意：在通知设备读取该内存之前，可能需要先刷新处理器的写缓冲区（write buffers））。
本函数用于分配一段大小为 <size> 字节的一致性内存。
函数执行成功时返回值为指向分配区域的（处理器虚拟内存地址空间）指针，执行失败时返回值为`NULL`。
函数还会通过`dma_handle`参数返回这块一致性内存的DMA基址`dma_addr_t`给设备使用，该值可强制转换为与总线宽度相同的无符号整数。
注意：在某些平台上，一致性内存的分配成本较高，且最小分配大小可能和页面大小相同，因此申请时应尽可能将一致性内存的请求合并起来。最简单的方法是使用 dma_pool 接口（详见下文）。
`flag`参数（仅针对于`dma_alloc_coherent()`）允许调用者在申请时指定 GFP_ 标志（参考 kmalloc() 的用法）（但具体实现可能会忽略某些影响内存位置的标志，例如 GFP_DMA）。
```C
void
dma_free_coherent(struct device *dev, size_t size, void *cpu_addr,
                  dma_addr_t dma_handle)
```
释放已分配的DMA一致性内存区域。参数`dev`、`size`、`dma_handle`参数必须和`dma_alloc_coherent()`传入的时候相同。参数`cpu_addr`必须和`dma_alloc_coherent()`返回的虚拟内存地址相同
### Part Ib - Using small DMA-coherent buffers 使用小容量DMA一致性缓存
todo
### Part Ic - DMA addressing limitations DMA寻址限制
```C
int
dma_set_mask_and_coherent(struct device *dev, u64 mask)
```
检查指定的DMA地址掩码（mask）是否可用，若可用则同时更新设备的流式DMA掩码与一致性DMA掩码
返回值：成功则为0，错误则为错误码（一个负数值）
```C
int
dma_set_mask(struct device *dev, u64 mask)
```
检查指定的DMA地址掩码（mask）是否可用，若可用则更新设备的流式 DMA 掩码
返回值：成功则为0，错误则为错误码（一个负数值）
```C
int
dma_set_coherent_mask(struct device *dev, u64 mask)
```
检查指定的DMA地址掩码（mask）是否可用，若可用则更新设备的一致性 DMA 掩码
返回值：成功则为0，错误则为错误码（一个负数值）
```C
u64
dma_get_required_mask(struct device *dev)
```
此API返回保证平台高效运行的掩码。通常该掩码是能覆盖所有系统内存所需的最小值。支持可变描述符大小的驱动程序可以查看该掩码并可以根据需要选择更小的描述符。
获取所需掩码不会改变当前掩码。若要使用该掩码，应用该掩码作为参数调用dma_set_mask()进行设置。
```C
size_t
dma_max_mapping_size(struct device *dev);
```
返回设备的最大映射大小。映射函数(如dma_map_single()、dma_map_page()等)的size参数不应超过此返回值。
```C
size_t
dma_opt_mapping_size(struct device *dev);
```
返回设备的最佳最大映射大小。
映射更大的缓冲区在某些情况下可能耗时更长。此外，对于短时间高频率的流式映射，映射初始时间可能占请求总生命周期的较大比重。因此，若拆分大请求不会造成显著性能损失，建议设备驱动程序将DMA流式映射的总长度限制在此返回值的范围内。
```C
bool
dma_need_sync(struct device *dev, dma_addr_t dma_addr);
```
返回true则必须要调用dma_sync_single_for_{device,cpu}来转移内存所有权，返回false则这些调用可被跳过。
```C
unsigned long
dma_get_merge_boundary(struct device *dev);
```
返回DMA合并边界值。如果设备无法合并任何DMA地址段，则返回0。

### Part Id - Streaming DMA mappings 流式DMA映射
todo