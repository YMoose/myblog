# lab0 准备开发环境
## 1. 编译
老师已经写好了Makefile，在labX目录下直接make即可，会在labX/bin文件夹下生成所需二进制文件
## 2. 调试
ucore启动端
```
qemu-system-i386 \ # 使用qemu模拟i386架构
    -S \ # 在启动时，不直接运行，当在monitor中输入'c'后，才开始
    -s \ # 等待gdb连接到端口(默认端口1234，可以用'-p'参数修改端口)
    -hda ./bin/ucore.img \ # 使用ucore.img作为硬盘0的镜像
    -monitor stdio # 重定向monitor到主机设备stdio中
```
gdb调试端(先写好gdb启动文件labX/tools/gdbinit)
```
gdb \
    -tui \ # 带图形界面启动gdb
    -x tools/gdbinit \ # 启动后执行gdbinit里的脚本
```