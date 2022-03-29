# Memory
## 现代计算机架构
因为高速便宜的网络设备的普及，现代计算机多使用平行架构而非垂直架构。多数计算机采用了如下的南北桥架构：  
![Structure with Northbridge and Southbridge](pics/Structure_with_Northbridge_and_Southbridge.png)  
- CPUs
- CPU通过一条前端总线(the Front Side Bus, FSB)连到北桥上。
- 北桥：包含内存控制器(其实现决定了内存芯片类型)
- 南桥：包含I/O控制器(集成了PCI，PCIe, SATA, USB等设备)，现其功能也已慢慢被集成进北桥，形成主桥架构  
这样的架构决定了数据传输中
## refer
1. [What every programmer should know about memory - Ulrich Drepper](https://lwn.net/Articles/250967/)
2. [MEMORY DEEP DIVE SERIES - Frank Denneman](https://frankdenneman.nl/2015/02/18/memory-configuration-scalability-blog-series/)
3. 
4. https://lwn.net/Articles/253361/ Section 4 virtual memory implemention
5. https://lwn.net/Articles/254445/ Section 5 NUMA system
6. https://lwn.net/Articles/255364/ Section 6 (important) program advice
7. https://lwn.net/Articles/257209/ Section 6.5 
8. https://lwn.net/Articles/256433/ Section 7 tool
9.  https://lwn.net/Articles/258154/ Section 8 furture work
10. https://lwn.net/Articles/258188/ mem9