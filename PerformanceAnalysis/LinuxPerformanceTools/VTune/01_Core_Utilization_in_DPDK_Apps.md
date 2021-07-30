# DPDK应用的CPU利用率
DPDK从18.11版本(带上编译参数`CONFIG_RTE_ETHDEV_RXTX_CALLBACKS`和`CONFIG_RTE_ETHDEV_PROFILE_WITH_VTUNE`(located in the`config/common_base config ` file))后开启对vtune(使用I/O分析)的支持。

