# 查看系统信息
cat /proc/cpuinfo   
cat:查看文本文件内容
/proc/cpuinfo:/proc是linux内核提供的一种伪文件系统，只存在在内存当中，而不占用外存空间，以文件系统的方式访问系统内核数据的操作提供接口。其中cpuinfo文件中写了cpu的基本信息。  
/proc/meminfo：内存基本信息

cat /sys/module/kvm_intel/parameters/XXXX  
可以用来根据sysfs文件系统中kvm_intel模块的当前参数值查看是否打开XXXX特性(EPT/VPID)  

ps -eLo ruser,pid,ppid,lwp,psr,args
ps:显示当前系统进程状态
- -e: 显示所有进程
- -L: 将线程（LWP，light-weight process）也显示出来
- -o: 以用户自定义格式输出
- ruser: 运行进程的用户
- pid: 进程ID
- ppid: 父进程ID
- args: 运行命令及参数  

free -m  
free：用于查看内存使用情况。  
dmesg  
dmesg：用于输出内核信息   

remod <*mod name*>  
remod：移除linux内核模块  
modeprobe <*mod name*> <args>  
modeprobe：移除或添加linux内核模块  
lsmod  
列出已加载的内核模块  

getconf <*conf name*>  
getconf：查看系统的配置信息  
