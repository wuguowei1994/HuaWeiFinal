##########################################################################
#脚本名称：compile.sh
#功能说明：
	# 1.判断是否存在steven文件夹，如果没有则报错
	# 2.判断系统是否安装了cmake软件，如果没有则报错
	# 3.如果条件1、2同时满足，编译steven文件夹
#参数说明：无
#脚本返回：成功(0);失败(-1);
#修改时间：2017-02-08 09:52:00
##########################################################################


#!/bin/bash
SCRIPT=$(readlink -f "$0") #得到绝对路径(带文件名)
BASEDIR=$(dirname "$SCRIPT") #得到绝对路径(不带文件名)
cd $BASEDIR #进入这个路径
#[ -d FILE ] 如果 FILE 存在且是一个目录则为真
#[ -f FILE ] 如果 FILE 存在且是一个普通文件则为真
#先判断steven文件夹是否存在
if [ ! -d steven ]
then
	echo "错误:没有找到steven文件夹"
	#设置退出码为-1
	exit -1
fi
#linux启动后，会默认打开3个文件描述符，分别是：标准输入standard input 0,正确输出standard output 1,错误输出：error output 2
# 每一条shell命令都会继承父进程的文件描述符。因此，所有运行的shell命令，都会有默认3个文件描述符
# 将标准错误输出(2)的执行结果重定向标准输出(1)
# $(cmake --version 2>&1) ：获取linux中cmake的版本
tmp=$(cmake --version 2>&1) 
if [ $? -ne 0 ] # $?表示上一次程序退出值，0表示没有错误，其他任何值表明有错误；-ne 表示 不等于 
then
	echo "错误:请先安装cmake"
	exit
fi
#编译
cd build
cmake ../steven
make
cd ..
#打包
mkdir code
# 加上-r参数拷贝文件夹内所有文件(包括子文件夹)
cp -r steven code/steven
cd code/steven
rm -rf lib
rm -f CMakeLists.txt
rm -f steven.cpp
rm -f io.cpp
cd ..
# 打包
tar -zcPf steven.tar.gz steven/
cp steven.tar.gz ../
rm -f steven.tar.gz
cd ..
rm -rf code
echo "编译成功"
