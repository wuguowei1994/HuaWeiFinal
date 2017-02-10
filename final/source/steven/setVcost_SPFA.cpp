/**
 * @Author:      steven
 * @DateTime:    2017-02-09 16:27:54
 * @Description: Done
 */
#include "steven.h"
/*************************************************************************
*  函数名称：Road::setVcost_SPFA
*  功能说明：得到任意必经点（must点）u,v间的距离Vcost，此时其它点间的cost没求，想要的话看Gp->cost（思路：不拿必经点去做更新）
*  参数说明：Gp->num:图中点的总数
*  函数返回：无
*  修改时间：2017-02-07 10:34:09
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
            //由于第一条路径只包含了0,1,3这三个点，所以j=2,4,5时会直接continue;
            continue;
        }
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
            /*
            如果rand()为偶数,那么不会满足if(rand() & 1)
            rand()是伪随机函数，在linux操作系统下，他每次都会这样生成整数
            rand(0)1804289383 //0代表rand函数第几次运行
            rand(1)846930886
            rand(2)::: 1681692777
            rand(3)1714636915
            rand(4)1957747793
            rand(5)424238335
            rand(6)719885386
            rand(7)1649760492
            rand(8)596516649
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
    // for(int i=0;i<Gp->num;i++)
    //     for(int j=0;j<Gp->num;j++)
    //         cout<<i<<" "<<j<<" "<<Vcost[i][j]<<endl;
    /**
    计算完毕后:
    如果为4亿多，说明从i到j没有路径
    如果为0,说明i或者j中至少有一个点是非必经点。
    如果以上都不是，那么Vcost[i][j]即为求出来的最小路径值
    i j Vcost[i][j]
    0 0 400202001
    0 1 1
    0 2 0
    0 3 3
    0 4 0
    0 5 0
    1 0 400202001
    1 1 400202001
    1 2 0
    1 3 2
    1 4 0
    1 5 0
    2 0 400202001
    2 1 400202001
    2 2 400202001
    2 3 1
    2 4 0
    2 5 0
    3 0 400202001
    3 1 400202001
    3 2 0
    3 3 400202001
    3 4 0
    3 5 0
    4 0 400202001
    4 1 400202001
    4 2 0
    4 3 1
    4 4 400202001
    4 5 0
    5 0 400202001
    5 1 400202001
    5 2 0
    5 3 2
    5 4 0
    5 5 400202001
    **/
}




//省时间版本(用于solveTspCn>1)
void Road::setVcost_reduced_SPFA()
{
    unsigned char isMustNew[MaxV] = {0};
    for(int i = 0; i < Gp->num; i++)
        if(!isMustOld[i] && isMust[i])
            isMustNew[i] = True;

    int u, v;
    for (int j = 0; j < Gp->num; j++)
    {
        bool update_required = isMustNew[j]; // 当前结点是否需要作为终点跑SPFA, 本身是新的必经节点需要更新
        if (isMustOld[j])
        {
            for (v = 0; v < Gp->num && !update_required; v++)
            {
                if(isMustOld[v] == False || v == j)
                    continue;
                if (Vcost[v][j] != UnReachCost)
                {
                    for (int k = Vpath[v][j]; k != j; k = Vpath[k][j])
                    {
                        if (isMustNew[k])
                        {
                            update_required = true;	// 存在路径经过了新的必经点, 需要更新
                            break;
                        }
                    }
                }
            }
        }
        if (update_required)
        {
            for (int i = 0; i < Gp->num; i++) Vcost[i][j] = UnReachCost;
            Vcost[j][j] = 0;

            que.clear();
            que.push(j);
            vis[j] = true;
            while (!que.empty())
            {
                v = que.pop();
                vis[v] = false;
                if(rand() & 1)
                {
                    for (int i = 0; i < Gp->Node[v].predCn; i++)
                    {
                        u = Gp->Node[v].predList[i];
                        if (Gp->cost[u][v] + Vcost[v][j] < Vcost[u][j])
                        {
                            Vcost[u][j] = Gp->cost[u][v] + Vcost[v][j];
                            Vpath[u][j] = v;
                            if (!vis[u] && !isMustOld[u] && !isMustNew[u])
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
                            if (!vis[u] && !isMustOld[u] && !isMustNew[u])
                            {
                                que.push(u);
                                vis[u] = true;
                            }
                        }
                    }
                }
            }
        }
    }
    //Print("Time : %s() used time %lld\n\n",__FUNCTION__,clock() - timeStart);
}
