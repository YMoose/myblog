# QEMU TCG plugin 开发环境搭建

## 介绍

## 环境
本文使用的环境是 Ubuntu18.04.04 + vscode。
## 编译QEMU
接下来开始编译qemu，不过先安装好所需要的库
```
apt-get install gcc build-essential zlib1g-dev pkg-config libglib2.0-dev binutils-dev libboost-all-dev autoconf libtool libssl-dev libpixman-1-dev libpython-dev python-pip python-capstone virtualenv
wget https://download.qemu.org/qemu-5.0.0.tar.xz
tar xvJf qemu-5.0.0.tar.xz
cd qemu-5.0.0
./configure -eaable-plugins
make
```
## 安装vscode
（ps:以下命令均为root权限执行）
[vscode官网](https://code.visualstudio.com/Download)下载.deb文件。进入下载目录，安装
```
cd <你存放vscode安装包的路径>
dpkg --install <你的vscode安装包>
```

## 调试编译
接下来就可以进入调试分析了

