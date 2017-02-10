/**
 * @Author:      steven
 * @DateTime:    2017-02-08 16:38:17
 * @Description: done
 */
/*************************************************************************
文件描述：
steven.h                        :所有的头文件、宏定义、类声明、变量声明、类定义
steven.cpp                      :有main()主函数、结果检查函数、自己的结果写入result.csv函数
route.cpp,route.h               :寻找路径的几个主要函数,全局变量定义,Road类静态变量初始化
Graph.cpp                       :Graph类几个基础函数
Road.cpp                        :Road类几个基础函数
LKH.cpp                         :LKH算法,函数均为Road的成员函数
candidate.cpp                   :使用次梯度优化求候选集
Minimun1Tree.cpp                :求最小1-tree
MinimunSpanningTree.cpp         :求最小生成树
initial.cpp                     :初始化路径,函数均为Road的成员函数
Directed_3_OPT.cpp              :有向边交换操作(3opt只有一种,且是sequential的)
Directed_4_OPT.cpp              :有向边交换操作(4opt只有一种,且是non-sequential的)
Directed_5_OPT.cpp              :有向边交换操作(5opt只有八种,且是sequential的)(已经把3opt包含,所以不需要再使用3opt)
exam_repeat.cpp                 :检查重复点,选择加入的规则,函数均为Road的成员函数
io.cpp,lib/...                  :SDK自带的读入输出和头文件
*************************************************************************/
#include "steven.h"
/***
argc是命令行总的参数个数
argv[]是argc的参数数组，其中第0个参数是程序的全名，以后的参数依次对应输入的参数顺序
在start.sh中是这样调用的：./steven $topo  $demand $result
所以：
argc=3
argv[0]为:./steven
argv[1]为:case/caseXX/topo.csv
argv[2]为:case/caseXX/demand.csv
argv[3]为:case/caseXX/result.csv
其中caseXX为用户输入的文件夹名
***/
int main(int argc, char *argv[])
{
    char *topo[MAX_EDGE_NUM];
    int edge_num;
    char *demand[MAX_DEMAND_NUM];
    int demand_num;
    //argv[1]为:case/caseXX/topo.csv
    char *topo_file = argv[1];
    // 这个函数是华为官方给出的，它会返回topo.csv中边的数量（不用精读）
    edge_num = read_file(topo, MAX_EDGE_NUM, topo_file);
    // cout<<endl<<"边的总数:"<<edge_num<<endl;
    if (edge_num == 0)
    {
        printf("您输入的topo.csv格式错误.\n");
        return -1;
    }
    // argv[2]为:case/caseXX/demand.csv
    char *demand_file = argv[2];
    // 这个函数是华为官方给出的，它会返回demand.csv中必经路径的数量（不用精读）
    demand_num = read_file(demand, MAX_DEMAND_NUM, demand_file);
    if (demand_num != MAX_DEMAND_NUM)
    {
        printf("您输入的demand.csv格式错误.\n");
        return -1;
    }
    // argv[3]为:case/caseXX/result.csv
    char *result_file = argv[3];
    // 参数含义:(topo.csv,边的总数,demand.csv,必经路径的数量,result.csv);
    search_route(topo, edge_num, demand, demand_num,result_file);
    //释放读文件的缓冲区
    release_buff(topo, edge_num);
    release_buff(demand, demand_num);
    printf("运行时间 =%5ldms\n", clock() / 1000);
    return 0;
}
