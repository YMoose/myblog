# 从intel_x86看计算机体系架构

## x86发展模式
![intel Tock-Tick 发展模式](pics/intel_tock-tick_mode.webp)
- Grantley是intel平台代号，代表搭配某一代CPU的主板chipset结构，不同的chipset结构就对应了不同的主板PCH接口
- Haswell是CPU代号
> **注：下面所说的CPU都是一个物理上的CPU，就是拿在手上那种一块cpu**
> 计算机架构从南北桥架构变成了主桥架构。这其中主要是原来北桥上的内存控制器、PCIE root port(IIO root port)等高速设备集成到了cpu内部，这些原来北桥上集成过来的功能在intel被称为cpu uncore modules。
> ![intel uncore 模块](pics/intel_uncore_module.png)
> 另一方面，慢速设备如sata磁盘接口，usb接口，主板集成网卡等io设备直接连接主板的PCH(Platform Controller Hub)芯片组再连接CPU

## CPU内部架构
![intel Haswell CPU内部架构](pics/cpu_ring_architecture.webp)
  Haswell这代CPU将core和LLC、memory controller、IO controller和inter-socket Intel QPI用ring架构集成。这样的架构会随着core数量增长而使各模块通信延时增长以及带宽降低。为避免这种情况则将core分到另个ring上。
![intel Xeon CPU内部架构](pics/cpu_mesh_architecture.webp)
  Skylake上则引入了mesh架构解决上述core数量增长带来的问题。同时还在mesh中集成了cache agent、home agent和IO 子系统以提高性能。同时每个Core上都有一个CHA以提供跨socket的intel UPI的cache一致性能力。

> 上图中除了core以外所有模组都属于uncore  
- CHA(combined Caching and Home Agent): 管理cache，提供跨core/socket的cache数据一致性的服务。上一代CPU每个Core上有一个CA，每个内存控制器上有一个HA。Skylake CPU则在每个Core和LLC bank上都放了一个CHA，用于处理core发来的请求和嗅探本地或远程CHA
  - Cbo(cache agent/core broadcast)
- LLC(Last Level Cache): L3 cache，一个socket上共享
  - Inclusive Shared L3 Cache: 包含式的缓存（即L2 cache的缓存行必定存在于L3中）。haswell中的L3 cache实现
  - Non-Inclusive shared L3 Cache: 非包含式的缓存（即L2 cache中的缓存行不一定存在于L3中，可以将L3 cache 看成是L2 cache的[Victim cache](https://en.wikipedia.org/wiki/Victim_cache)）。skylake中的L3实现，因为L3缓存的这种实现，将原有的L3 cache的一部分转移到了core中的L2 cache。
- SF(Snoop Filter): 
- Intel UPI(Ultra Path Interconnect): 保证跨socket的数据一致性的构件(替换了上一代CPU中的Intel QPI)
## 参考
https://www.intel.com/content/www/us/en/developer/articles/technical/xeon-processor-scalable-family-technical-overview.html
https://en.wikipedia.org/wiki/Tick%E2%80%93tock_model