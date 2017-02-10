/**
 * @Author:      steven
 * @DateTime:    2017-02-09 16:26:49
 * @Description: Done
 */
 #include "steven.h"
Graph G;
Road Road0;
/*************************************************************************
*  函数名称：search_route()
*  功能说明：计算topo.csv中满足demand.csv的可行解
*  参数说明：(char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM], int demand_num)
			         ||                      ||             ||                          ||  
            (      topo.csv          ,     边的总数 ,    demand.csv               ,  必经路径的数量);
*  函数返回：无
*  修改时间：2017-02-06 19:30:14
*************************************************************************/
void search_route(char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM], int demand_num,const char * const filename)
{
	// 功能说明：把topo.csv里面记录的边的信息放入cost[s][t]矩阵中
	G.initial(topo,edge_num);
	// 功能说明：将demand.csv中的必经点按照如下顺序存入list数组：起点---必经点----终点,并将它们的属性设置为isMust
	Road0.initial(demand[0], &G, 0);
	Road0.fileName=filename;
	Road0.search_single_route(&Road0);
}
