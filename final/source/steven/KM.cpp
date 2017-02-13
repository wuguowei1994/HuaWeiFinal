/**
 * @Author:      steven
 * @DateTime:    2017-02-09 11:59:09
 * @Description: Done
 */
#include "steven.h"
/*************************************************************************
为克服LKH()算法的缺点,使用KM算法指定一个可行解(网上很多demo)
可以参考这个链接:http://www.cnblogs.com/skyming/archive/2012/02/18/2356919.html 中的第四题和代码
该题的代码具体解释请看KMDemo.cpp文件
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

如果把KM算法用在有向图中,它会返回环路，使得任何一个点都出现在某个环内(只出现一次，注意环的个数不止一个),且环的权重和最大。
KM算法运行结束后，出于权重最小的原则，需要把所有的环都反向链接。举个例子：如果KM算法计算出来一个环为:0-->2-->4-->6-->0
那么需要将它变成0-->6-->4-->2-->0。当然，如果环的个数不止一个，这里直接把KM算法求出来的方案简单粗暴的的拆成链表，然后再连成一个环
最后将这个环作为哈密顿问题的初始解
*************************************************************************/
bool KMTour::init_tour(int num, int _cost[][MaxV2], VVNode node[], long long &cost_sum) {
    n = num; cost = _cost; Node = node;
    // 运行KM算法;寻找每个节点的match[i]
    run_km();
    for (int i = 0; i < n; i++)
    {
        cout << "node[" << i << "]的匹配是:" << node[match[i]].id << endl;
    }
    for (int i = 0; i < n; i++)
    {
        cout << "从节点" << i << "的匹配点到节点"<<i<<"对应的权重为:" << cost[match[i]][i] << endl;
    }
    int cnt = 0;
    int *medge = slack;
    MemClear(vx, 0, n);
    for (int i = 0; i < n; i++)
    {
        /*************************************************************************
        说明一下这里为什么是 LinkA2B(&node[match[i]], &node[i]) 而不是  LinkA2B( &node[i],&node[match[i]])
        因为KM算法求的是最大权匹配，所以在有向图中它求出来的是权值最大的有向边。
        举两个例子:
        1.如果从节点1到节点2有一条权重2为的有向边，从节点2到节点1有一条权重为8的有向边，那么出于权重最大的原则，
          KM算法结束以后必定有match[2]=1(也就是取有向边2->1作为最终匹配),那么在KM算法结束后，为了让权重最小，
          需要链接1->2，也就是刚好与KM算法的方向相反。
        2.如果从节点1到节点2有一条权重为100的有向边，但是从节点2到节点1没有有向边，那么出于权重最大的原则，
          KM算法结束以后必定有match[2]=1(也就是取有向边2->1作为最终匹配),其实这里取的权重就是负无穷，因为从2到1没有有向边
          所以他们之间的权重就是负无穷。那么在KM算法结束后，为了让权重最小，需要链接1->2，和第一种情况一样，也是取KM算法的反方向
        *************************************************************************/
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
    // 如果cnt=1，那么下面的这个for循环其实什么都没有做，因为此时y和x相等
    // 那么LinkA2B(&node[match[x]], &node[y])就是LinkA2B(&node[k], &node[k])，而这一步在前面那个for循环中已经执行过了
    for (int i = 0; i < cnt; i++) { // 多个圈, 合并圈圈
        int x = medge[i], y = medge[i + 1];
        LinkA2B(&node[match[x]], &node[y]);
    }
    cout<<"cnt为:"<<cnt<<endl;

    cost_sum = 0;       // 路径和
    for (int i = 0; i < num; i++) cost_sum += COST(&node[i], node[i].succ);

    return cnt == 1;    // cnt==1表示当前指派问题已经形成一个圈
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

    cout << "KM算法被运行了" << endl;

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
