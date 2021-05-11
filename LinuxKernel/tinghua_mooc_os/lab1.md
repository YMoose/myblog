# lab1
## 实验目的
1. 计算机原理
    - CPU的编址与寻址: 基于分段机制的内存管理
    - CPU的中断机制
    - 外设：串口/并口/CGA，时钟，硬盘
2. Bootloader软件
    - 编译运行bootloader的过程
    - 调试bootloader的方法
    - PC启动bootloader的过程
    - ELF执行文件的格式和加载
    - 外设访问：读硬盘，在CGA上显示字符串
3. ucore OS软件
    - 编译运行ucore OS的过程
    - ucore OS的启动过程
    - 调试ucore OS的方法
    - 函数调用关系：在汇编级了解函数调用栈的结构和处理过程
    - 中断管理：与软件相关的中断处理
    - 外设管理：时钟
## 实验内容
### lab1 
1. make V=:可以看到make的详细执行步骤
```
# 这里用dd命令生成包含10000个扇区(每个扇区512bytes)d的ucore.img文件
dd if=/dev/zero of=bin/ucore.img count=10000
10000+0 records in
10000+0 records out
5120000 bytes (5.1 MB) copied, 0.00896992 s, 571 MB/s
# 用dd命令将bootblock程序(即我们的bootloader)放在ucore.img的第一个扇区
dd if=bin/bootblock of=bin/ucore.img conv=notrunc
1+0 records in
1+0 records out
512 bytes (512 B) copied, 0.000172564 s, 3.0 MB/s
# 用dd命令将我们的kernel程序放在1一个扇区后的位置(即bootblock之后)
dd if=bin/kernel of=bin/ucore.img seek=1 conv=notrunc
138+1 records in
138+1 records out
70724 bytes (71 kB) copied, 0.000287529 s, 246 MB/s
```
2. makefile里通过sign(生成主引导扇区的程序)将bootblock.out生成为标准的主引导扇区，也正因为修改了所以gdb无法读取符号信息。
```
$(bootblock): $(call toobj,$(bootfiles)) | $(call totarget,sign)
	@echo + ld $@
	$(V)$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 $^ -o $(call toobj,bootblock)
	@$(OBJDUMP) -S $(call objfile,bootblock) > $(call asmfile,bootblock)
	@$(OBJCOPY) -S -O binary $(call objfile,bootblock) $(call outfile,bootblock)
    # 这里是用sign生成标准主引导扇区
	@$(call totarget,sign) $(call outfile,bootblock) $(bootblock)
```
### lab2
