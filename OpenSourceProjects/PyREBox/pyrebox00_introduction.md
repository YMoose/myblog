# PyREBox沙箱

笔者在学习过程中主要看得还是官方文档[1]，在此也感谢前辈翻译过的官方文档[2]，其也在这一系列文章中写到了不少自己的实践和看法。可以作为官方文档的补充来看。本文也是基于此做再做一些补充。  

## 简介  
[PyREBox](https://github.com/Cisco-Talos/pyrebox)是一款支持python脚本的逆向自动化分析沙箱。以QEMU的全系统仿真为基础，结合[Volatility](https://www.volatilityfoundation.org)、[TSK](http://www.sleuthkit.org)这些开源取整框架实现其VMI技术，由此可以从外部监视虚拟机内部的系统信息。开发者可以基于这些系统行为选择合适的时机对程序进行动态插桩。

## 安装
目前官方还没有安装包，所以最好根据官方文档一步一步从源码开始build，其中有几个要注意的点。  
首先PyREBox的VMI技术实现使用了Volatility的框架，所以安装时也会需要安装Volatility的依赖，若使用的distorm3的最新版本可能会有一些问题。所以笔者自己使用的是distorm3-3.3.4，可以正常运行。另外在使用Volatility里的yara插件时（记得用之前`pip install yara` ），可能会报错，原因是因为在虚拟环境里的yara库位置不对，你可以通过如下命令：  
```
ln -s <你虚拟环境的位置>/lib/python2.7/site-packages/usr/lib/libyara.so <你虚拟环境的位置>/lib/libyara.so
```
创建一个软链接来解决[3]。 
最后，官方强烈推荐在virtualenv创建的虚拟环境下安装你的python依赖库（因为若不在虚拟环境下安装，其会对你本地的volatility库产生一定的影响），当然也不要忘记每次要启动pyrebox的时候先进入你的虚拟环境。

## 使用
### 启动准备及启动配置
首先你要准备好一份配置文件pyrebox.conf
```
[MODULES]
scripts.script_example.py: True
scripts.volatility_example: False

[VOL]
profile: WinXPSP3x86

[AGENT]
name: win_agent_32.exe
conf: win_agent_32.exe.conf

[SYMBOL_CACHE]
path: symbols.WinXPSP3x86
```
[MODULES]部分包含一个python脚本模块的列表（可以使用标准python表示法（使用点）指定包和子包）。可以根据需要启用或禁用脚本。这些脚本将自动加载。  
[VOL]部分包含Volatility所需要的配置，主要是依据对虚拟机的操作系统对profile文件做出调整。  
[AGENT]部分允许你配置好你要分析的二进制文件以及其配置文件（在文档中和后续文章会有详细说明）。  
[SYMBOL_CACHE]部分允许你指明一个路径用于存放不同会话的已解析的符号以提升性能。这个路径必须每个虚拟机单独一个。  
剩下的准备与配置，就不细说了，文档和参考资料里写得很详细了。因为PyREBox是一个基于QEMU开发的，所以如果你懂得如何启动一个QEMU虚拟机，那么基本上你也就明白如何启动PyREBox了。

### QEMU interface
使用PyREBox时为了与QEMU交互需要开启`-monitor stdio`选项，开启此选项可以在命令行中与QEMU虚拟机进行交互。在命令行中键入help可以查看支持的所有命令。

#### 虚拟机快照  
其中PyREBox实现了虚拟机快照功能，命令如下：  
列出所有快照信息
```
(qemu) info snapshots
```
创建名为init的快照
```
(qemu) savevm init
```
恢复为名为init的快照
```
(qemu) loadvm init
```
也支持在QEMU的启动参数中配置` -loadvm init `让QEMU直接从快照中恢复。

#### python脚本模块
PyREBox不仅支持在启动时加载已配置好的模块，同时也支持动态加载模块，命令如下：  
列出已加载模块
```
(qemu) list_modules
```
加载scripts包里的模块my_plugin
```
(qemu) import_module scripts.my_plugin
```
重新加载模块1
```
(qemu) reload_module 1
```
卸载模块1
```
(qemu) unload_module 1
```

### IPython Shell
将需要的模块加载后，可以启动PyREBox shell进行进一步的分析。
```
(qemu) sh
```
启动后
```
pyrebox> %list__commands
```
可以列出所有的命令及说明（%不是必要的）。    
退出可以键入`quit`、`q`、`c`、`continue`或者直接ctrl+d。
使用时有一些注意事项和要点：  
1.键入命令可以用tab补齐。  
2.键入`who`命令可以打印出可用对象。  
3.键入`help <命令或对象>`或者`<命令或对象>?`可以获取详细信息。
4。其同时支持嵌入python代码。  
5.使用地址分析时，涉及进程上下文的问题，可以使用`proc <PID\PGD\进程名>`命令来指定进程。另外，在使用过程中如果要使用虚拟地址，直接使用十六进制，如 `0x7c871235`；如果要使用物理地址要在前面加上`p`前缀,如`p0x00100000`。  



#### Volatility
在PyREBox shell中可以使用Volatility插件的功能。
```
pyrebox> %list_vol_commands
```
可以列出所有的Volatility支持的命令及说明，使用这些命令时要在命令前加上`vol `，如
```
pyrebox> vol pslist
```
用Volatility获取虚拟机的进程列表。 

## 总结
对于PyREBox的简介也先告一段落，之后会花更多的笔墨去写一些PyREBox的插件相关的介绍。
笔者也尝试使用并粗浅地研究了其他PyREBox介绍中提到的项目如DECAF、PANDA、S2E以及另外的TEMU、RVMI这些类似的qemu-based沙箱。相比较而言，笔者个人感觉PyREBox更为成熟，使用过程中也比较顺手。

## 参考资料

[1]https://pyrebox.readthedocs.io/en/latest/  
[2]https://blog.csdn.net/dmbjzhh/article/details/81163429 等一系列文章  
[3]https://stackoverflow.com/questions/41255478/issue-oserror-usr-lib-libyara-so-cannot-open-shared-object-file-no-such-fi
