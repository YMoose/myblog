# perfmon 插件
vpp在21.06版本对intel cpu pmu进行了支持可以更好的利用硬件收集性能数据为提升性能提供建议
## usage
首先要在配置文件中plugins字段中 enable perfmon_plugin.so，[使用demo视频](https://asciinema.org/a/aFN5rMFYw0RPvGOZiFsziXV5w)
```
# 显示可供监控的选项
vpp# show perfmon bundle
Name                Type      Source              Description
inst-and-clock      node      intel-core          instructions/packet, cycles/packet and IPC
cache-hierarchy     node      intel-core          cache hits and misses
context-switches    thread    linux               per-thread context switches
branch-mispred      node      intel-core          Branches, branches taken and mis-predictions
page-faults         thread    linux               per-thread page faults
load-blocks         node      intel-core          load operations blocked due to various uarch reasons
power-licensing     node      intel-core          Thread power licensing
memory-bandwidth    system    intel-uncore        memory reads and writes per memory controller channel
# 开始监控
vpp# perfmon start bundle <bundle name>
# 结束监控
vpp# perfmon stop
# 展示监控数据
vpp# show perfmon statistics
# 重置监控数据
vpp# perfmon reset
```
## 代码解析

## issues
1. 此插件初始化时需要从/sys/devices/system/node/has_cpu文件读取numa node数，但因为一些linux发行版在文件内容存在[bug](https://lore.kernel.org/lkml/1381857176-22999-2-git-send-email-toshi.kani@hp.com/t/)，所以在此类机器上启动时会报bug，这里可以应用perfmon.diff文件进行修改
2. 在使用时reset后瞬间再开始新的监控任务时会有bug，稳定出现再intel-core的bundle的event数量大于2时。推荐先用stop命令再用reset
## references
https://docs.fd.io/vpp/21.06/d0/dc3/clicmd_src_plugins_perfmon.html