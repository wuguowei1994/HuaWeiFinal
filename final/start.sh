#!/bin/bash
# 指定测试文件夹所在文件夹
testdir="case/"
#编译执行
./source/compile.sh
cp ./source/bin/steven ./
echo -e "请输入需要测试的文件夹名称:"
read casename
# 获取文件名
# >> ： 如果文件不存在，将创建新的文件，并将数据送至此文件；如果文件存在，则将数据添加在文件后面
# echo "测试文件夹名 :"$casename #>> runout.txt
casedir=$testdir$casename #casedir 文件目录+文件名
if [ ! -d $casedir ]
then
	echo "错误:没有找到测试文件夹"
	#设置退出码为-1
	exit -1
fi
#获取文件
topo=$casedir"/topo.csv" demand=$casedir"/demand.csv" result=$casedir"/result.csv"
# 运行steven 同时把$topo 和 $demand 作为这个程序的输入文件
./steven $topo $demand $result
