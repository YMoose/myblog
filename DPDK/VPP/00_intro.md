# VPP The Vector Packet Processor
## 配置VPP
对于所以配置参数，可以在源代码中搜索VLIB_CONFIG_FUNCTION和VLIB_EARLY_CONFIG_FUNCTION的调用。  
VLIB_CONFIG_FUNCTION(foo_config, "foo"):函数将"foo"参数块中给出的所有参数(foo{arg1 arg2 arg3})传递给foo_config函数进行调用
### unix
启动和行为类型属性，以及和os相关的属性
### api-trace
跟踪，转储和重播控制平面视图要求转发平面执行的操作和数据
