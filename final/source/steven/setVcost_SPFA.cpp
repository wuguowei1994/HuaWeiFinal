/**
 * @Author:      steven
 * @DateTime:    2017-02-09 16:27:54
 * @Description: Done
 */
#include "steven.h"
/*************************************************************************
*  函数名称：Road::setVcost_SPFA
*  功能说明：1.得到任意必经点（must点）u,v间的距离Vcost，
*           2.如果i或者j中有一个点为非必经点，那么Vcost[i][j]为0
*  参数说明：无
*  函数返回：无
*  修改时间：2017-02-07 10:34:09
*  函数补充：1.SPFA算法比较简单，网上很多demo,这里直接借用了其核心算法。
*           2.也可以用dijkstra算法求必经点间的最短路径,这两个算法效率差不多
*           3.不建议使用floyd算法，因为它比1和2的效率低很多
*************************************************************************/
static unsigned char vis[MaxV];
static Queue que;
void Road::setVcost_SPFA()
{
    int u, v;
    for (int j = 0; j < Gp->num; j++)
    {
        if (isMust[j] == False)
        {
            //不计算非必经点
            continue;
        }
        // 初始化
        for (int i = 0; i < Gp->num; i++)
            Vcost[i][j] = UnReachCost;
        Vcost[j][j] = 0;
        que.clear();
        que.push(j);
        vis[j] = true;
        while (!que.empty())
        {
            v = que.pop();
            vis[v] = false;
            /**
             * @Author:      setven
             * @DateTime:    2017-02-10 15:37:36
             * @Description: rand()是伪随机函数，在linux操作系统下，它每次都会这样生成整数：
                             rand(0)1804289383 //0代表rand函数第几次运行
                             rand(1)846930886
                             rand(2)1681692777
                             rand(3)1714636915
                             rand(4)1957747793
                             rand(5)424238335
                             rand(6)719885386
                             rand(7)1649760492
                             rand(8)596516649
                            如果rand()为偶数,那么不会满足if(rand() & 1)
             */
            if(rand() & 1)
            {
                for (int i = 0; i < Gp->Node[v].predCn; i++)
                {
                    u = Gp->Node[v].predList[i];
                    if (Gp->cost[u][v] + Vcost[v][j] < Vcost[u][j])
                    {
                        Vcost[u][j] = Gp->cost[u][v] + Vcost[v][j];
                        Vpath[u][j] = v;
                        if (!vis[u] && !isMust[u])
                        {
                            que.push(u);
                            vis[u] = true;
                        }
                    }
                }
            }
            else
            {
                for (int i = Gp->Node[v].predCn - 1 ; i >= 0; i--)
                {
                    u = Gp->Node[v].predList[i];
                    if (Gp->cost[u][v] + Vcost[v][j] < Vcost[u][j])
                    {
                        Vcost[u][j] = Gp->cost[u][v] + Vcost[v][j];
                        Vpath[u][j] = v;
                        if (!vis[u] && !isMust[u])
                        {
                            que.push(u);
                            vis[u] = true;
                        }
                    }
                }
            }
        }
    }
    for(int i = 0; i < Gp->num; i++)
        Vcost[i][i] = UnReachCost;
    /**
    计算完毕后:
    如果Vcost[i][j]为UnReachCost，说明从i到j为不可达
    如果Vcost[i][j]为0,说明i或者j中至少有一个点是非必经点。
    如果以上都不是，那么Vcost[i][j]即为求出来的最小路径值
    **/
}
