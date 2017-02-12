/**
 * @Author:      steven
 * @DateTime:    2017-02-09 11:59:09
 * @Description: Done
 */
#include "steven.h"
/*************************************************************************
为克服LKH()算法的缺点,使用KM算法(网上很多demo)指定一个可行解
参数:
    1. num: 结点数
    2. _cost: 结点间的cost, 实际使用的加上inPi, outPi后的修正权值
    3. node: 结点(需要用到里面的inPi, outPi值)
    4. cost_sum: 计算解的边权和
得到:
    1. node中的succ, pred属性连接构成一个循环双向链表, 即解
    2. cost_sum
返回值:
    true: 当前解就是最优解(单个圈), 无需迭代求优
    false: 当前解还不是最优解(多个圈)
算法复杂度:
    KM算法, O(n^3)
如果把KM算法用在有向图中,其实就是找多个环，使得任何一个点都出现在某个环内(只出现一次),且环的权重和最小
比赛的问题可以看成是这个问题再加一个限制条件，就是环的个数只能有一个。这里是直接把KM算法求出来的方案(可能含有多个环)，简单的拆成链表，然后连成一个环
最后将这个环作为哈密顿问题的初始解
*************************************************************************/
bool KMTour::init_tour(int num, int _cost[][MaxV2], VVNode node[], long long &cost_sum) {
    n = num; cost = _cost; Node = node;
    // 运行KM算法;寻找每个节点的match[i]
    run_km();
    int cnt = 0;
    int *medge = slack;
    MemClear(vx, 0, n);
    for (int i = 0; i < n; i++)
    {
        LinkA2B(&node[match[i]], &node[i]);
        if (!vx[i]) {
            vx[i] = true;
            int mcost = cost[match[i]][i], k = i;
            for (int j = match[i]; j != i; j = match[j]) {
                vx[j] = true;
                if (checkMax(mcost, cost[match[j]][j])) k = j;
            }
            medge[cnt++] = k;
        }
    }

    medge[cnt] = medge[0];
    for (int i = 0; i < cnt; i++) { // 多个圈, 合并圈圈
        int x = medge[i], y = medge[i + 1];
        LinkA2B(&node[match[x]], &node[y]);
    }

    cost_sum = 0;       // 路径和
    for (int i = 0; i < num; i++) cost_sum += COST(&node[i], node[i].succ);

    return cnt == 1;    // cnt==1表示当前指派问题形成一个圈, 解已经找到, 否则还没有
}

/*************************************************************************
*  函数名称：KMTour::run_km
*  功能说明：寻找完备匹配下的最大权匹配
*  参数说明：无
*  函数返回：无
*  修改时间：2017-02-12 19:48:22
*************************************************************************/

void KMTour::run_km() {
    MemClear(match, -1, n);
    MemClear(lx, false, n);
    MemClear(ly, false, n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) checkMax(lx[i], -cost[i][j]);

    for (int x = 0; x < n; x++) {
        for (int i = 0; i < n; i++) slack[i] = UnReachCost;

        while (1) {
            MemClear(vx, 0, n);
            MemClear(vy, 0, n);

            if (dfs(x)) break;

            int d = UnReachCost;
            for (int i = 0; i < n; i++)
                if (!vy[i] && d > slack[i]) d = slack[i];

            for (int i = 0; i < n; i++) {
                if (vx[i]) lx[i] -= d;
                if (vy[i]) ly[i] += d; else slack[i] -= d;
            }
        }
    }
}

bool KMTour::dfs(int u) {
    vx[u] = true;
    for (int v = 0; v < n; v++) {
        if (vy[v]) continue;
        int t = lx[u] + ly[v] + cost[u][v];
        if (t == 0) {
            vy[v] = true;
            if (match[v] == -1 || dfs(match[v])) {
                match[v] = u;
                return true;
            }
        }
        else if (slack[v] > t) slack[v] = t;
    }
    return false;
}
