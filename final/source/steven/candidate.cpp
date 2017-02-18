/**
 * @Author:      steven
 * @DateTime:    2017-02-09 17:03:53
 * @Description: Done
 */
#include "steven.h"
CandidateSolver candi;
/*************************************************************************
*  函数名称：Road::creatCandidates()
*  功能说明：初始化Node[]的候选集，candCn和cand[]
*  参数说明：无
*  函数返回：True代表使用ascent直接找到解了,False就是没有找到解
*  修改时间：2017-02-07 10:32:10
*************************************************************************/
unsigned char Road::creatCandidates(long long &bestCostMin)
{
    // 当init()执行完毕以后，会把atsp问题转换成tsp问题，并设置好精度，步长因子，迭代次数等变量，方便使用LKH算法
    candi.init(this);
    //求出double以后各节点的候选集
    unsigned char isFindTour = candi.create_candi();
    //求sumPi
    sumPi = 0;
    for (int x = 0; x < num; x++)
        sumPi += Node[x].outPi + Node[x].inPi;

    //norm == 0直接出解
    if (isFindTour)
    {
        Print("wanted tour is find in ascent!\n");
        bestCostMin = 0;
        for (int i = 0; i < num; i++)
        {
            bestCostMin += COST(&Node[i], Node[i].bestSucc);
            Node[i].succ = Node[i].bestSucc;    //给TourTable的insert用(bestSucc在create_candi中设置过)
        }
        bestCostMin = (bestCostMin - sumPi) >> 7u;

    }
    return isFindTour;
}

/*************************************************************************
*  函数名称：CandidateSolver::gen_candi
*  功能说明：根据alpha生成候选集
*  参数说明：alpha需要在本函数调用前先算好
*  函数返回：无
*  修改时间：2017-02-07 10:32:30
*************************************************************************/
void CandidateSolver::gen_candi(int max_candi, long long max_alpha, bool symmetric = false)
{
    int *alpha = best_pi;
    heap.init_with_key(best_pi); // 把best_pi当alpha排序用, 借用堆进行排序

    if (num != dim)
        max_candi = GetMin(max_candi + 1, num); // 自己到自己的复制点不算

    for (int x = 0; x < dim; x++)
    {
        // 对候选集进行排序
        for (int i = 0; i < numCandi[x]; i++)
        {
            int y = edge[x][i].to;
            alpha[y] = edge[x][i].alpha;
            cost[y] = edge[x][i].cost;          // 原cost表示结点并入mst的代价, 到这里用不上了.
            heap.push(y);
        }

        for (int i = 0; !heap.empty(); i++)
        {
            int y = heap.pop();
            edge[x][i].to = y;
            edge[x][i].alpha = alpha[y];
            edge[x][i].cost = cost[y];
        }

        // 截取前max_candi个
        numCandi[x] = GetMin(numCandi[x], max_candi);
    }
}

/*************************************************************************
*  函数名称：CandidateSolver::ascent
*  功能说明：使用次梯度优化得到一个最优带最佳pi值的1-tree
*  参数说明：无
*  函数返回：sum
*  修改时间：2017-02-07 10:32:40
*************************************************************************/
Int64 CandidateSolver::ascent()
{
    bool InitialPhase = true;
    Int64 W, W0, BestW;
    int BestNorm;

Start:
    MemClear(pi, 0, dim);
    MemClear(best_pi, 0, dim);
    W = mini_1tree(0);
    MemCopy(last_v, v, dim);

    if (Norm == 0) return W;
    gen_candi(AscentCandidates, UnReachCost, 0);

    BestW = W0 = W;
    BestNorm = Norm;
    for (int T = InitialStepSize * Precision, Period = InitialPeriod;
            Period > 0 && T > 0 && Norm != 0; Period /= 2, T /= 2)
    {
        for (int P = 1; T > 0 && P <= Period && Norm != 0; P++)
        {
            ForAllNodes(x)
            {
                if (v[x])
                {
                    pi[x] += T * (7 * v[x] + 3 * last_v[x]) / 10;
                    checkMin(pi[x], UnReachCost >> 2); // 将Pi值约束在[-UnReachCost/4, UnReachCost/4]之间
                    checkMax(pi[x], -(UnReachCost >> 2));
                }
                last_v[x] = v[x];
            }

            W = mini_1tree(1);
            if (Norm == 0) return W;
            if (W > BestW || (W == BestW && Norm < BestNorm))
            {
                if (W - W0 > (W0 >= 0 ? W0 : -W0) && AscentCandidates > 0
                        && AscentCandidates < dim)   // 候选集不够大
                {
                    W = mini_1tree(0);
                    if (W < W0)
                    {
                        AscentCandidates *= 2;
                        if (AscentCandidates > dim)
                            AscentCandidates = dim;
                        goto Start;
                    }
                    W0 = W;
                }

                BestW = W;
                BestNorm = Norm;
                MemCopy(best_pi, pi, dim);

                if (InitialPhase) T *= 2;
                if (P == Period) Period = GetMin(Period * 2, InitialPeriod);
            }
            else if (InitialPhase && P > InitialPeriod / 2)
            {
                InitialPhase = false;
                P = 0;
                T = 3 * T / 4;
            }
        }
    }

    MemCopy(pi, best_pi, dim);
    Int64 ret = mini_1tree(0);
    return ret;
}


/*************************************************************************
*  函数名称：CandidateSolver::init
*  功能说明：由road初始化本类实体所代表的图, 对于有向图这里会将点double后再求候选集
*  参数说明：1.FirstNode为结点链表,
*           2.atsp_tag表示road代表的图是不是atsp
*           3.FN:结点列表(如果FN是空的, 本函数会初始化FN)
*           4.atsp_tag: true表示road代表的图是有向的, false表示road代表的图是无向的,
*  函数返回：无
*  修改时间：2017-02-07 10:32:53
*************************************************************************/
void CandidateSolver::init(Road *roadp)
{
    // num:合并起点和终点后，必经点的总数
    num = roadp->num;
    // atsp转tsp,所以dim=num*2
    dim = num * 2;
    // 必经点序列
    node = roadp->Node;
    for (int i = 0; i < dim; i++)
    {
        mst_list[i] = i;    // 最小生成树序列
        // numEdge[]:各节点邻边数;numCandi[]候选集数(<=边数)
        numEdge[i] = numCandi[i] = 0;
    }
    // 统计相邻的边的数量
    for (int i = 0; i < num; i++)
    {
        // 由于原点和复制点的关系,所以复制以后所有节点的邻边+1
        numEdge[i]++;
        numEdge[i + num]++;
        /**
         * @Author:      steven
         * @DateTime:    2017-02-09 16:41:44
         * @Description: 重复一遍:由于前面合并了起点和终点，所以在计算cost[i][j]的时候要注意:假设i不是起点或终点,j是起点和终点合并以后的点，那么
                         cost[i][j]代表的不是原图中从i到起点的权重,而是从i到终点的权重;同理，如果i是起点和终点合并后的点,j是普通点,那么cost[i][j]
                         代表的是从起点到j的权重，而不是从终点到j的权重
         */
        for (int j = 0; j < num; j++) if (i != j && roadp->cost[i][j] != UnReachCost)
            {
                numEdge[i]++;
                numEdge[j + num]++;
            }
    }
    // 根据前面求出来的每个节点邻边个数给边开辟空间,仅仅开辟空间，不做任何操作
    int cnt = 0;
    // 下面的for循环执行完毕以后，cnt等于所有节点(包括double出来的另一半)的邻边的总数
    for (int i = 0; i < dim; i++)
    {
        // edge_base[]:用来存储边的邻接表的一个一维列表
        edge[i] = &edge_base[cnt];
        cnt += numEdge[i];
    }
    /*
    edge[i]记录了这个一维列表中与i节点相关的边
    假设节点0有5个关联边，那么edge[0]=&edge_base[0];edge[1]=&edge_base[5]
    假设节点1有3个关联边，那么edge[2]=&edge_base[5+3]=&edge_base[8]，以此类推
    */
    // 前面只是double了点集,现在要把(无向)边的信息填进去
    for (int i = 0; i < num; i++)
    {
        /*
        7u表示7是一个无符号类型的数据
        -100000 << 7u 用int表示就是-12800000
        X << 7u 相当于(10进制范围内)把X变大128倍;这里不用乘号是因为移位运算比乘法省时
        这里设置原点与复制点的边cost=-12800000
        */
        add_edge(i, i + num, -100000 << 7u);
        for (int j = 0; j < num; j++)
            if (i != j && roadp->cost[i][j] != UnReachCost)
            {
                // cost<<7相当于(10进制的范围内)把cost变大128倍
                add_edge(i, j + num, roadp->cost[i][j] << 7);
            }
    }

    // 初始化参数
    SymmetricCandidates = true;//直接使用论文p34的代码计算出候选集
    InitialPeriod =  num < 100 ? 100 : num; //Ascent用来控制迭代次数的变量
    InitialStepSize = 1;//Ascent用来控制迭代步长的变量
    Excess = 1.0 / dim; //见论文P32
    CandidatesNum = 5; //候选集边的个数
    AscentCandidates = num;//给Ascent()函数用的，见论文p33;
    Precision = 128;//刚才"<<7u"就是放大128倍，所以精度Precision=128
}

/*************************************************************************
*  函数名称：CandidateSolver::add_edge
*  功能说明：在节点i和j之间添加一条权重为cost的边(无向边)
*  参数说明：使用inline修饰是为了节省栈内存
*  函数返回：无
*  修改时间：2017-02-09 16:55:41
*************************************************************************/
inline void CandidateSolver::add_edge(int i, int j, int cost)
{
    // numCandi[]:候选边集
    // edge[][]:边的邻接表
    edge[i][numCandi[i]].to = j;//to:边的端点编号
    //cost:边的cost          cost_pi:带pi的cost值
    edge[i][numCandi[i]].cost = edge[i][numCandi[i]].cost_pi = edge[i][numCandi[i]].alpha = cost;
    numCandi[i]++;

    edge[j][numCandi[j]].to = i;
    edge[j][numCandi[j]].cost = edge[j][numCandi[j]].cost_pi = edge[j][numCandi[j]].alpha = cost;
    numCandi[j]++;
}


/*************************************************************************
*  函数名称：CandidateSolver::calc_alpha
*  功能说明：计算各边的alpha值
*  参数说明：本函数需要在gen_candi()函数之前调用
*  函数返回：无
*  修改时间：2017-02-07 10:33:16
*************************************************************************/
void CandidateSolver::calc_alpha()   // 见Page23
{
    int *beta = best_pi;    // best_pi作beta用
    int *mark = last_v;     // last_v作mark用

    // 考虑不在1-tree中的边
    MemClear(mark, -1, dim);
    for (int i = 0; i < dim; i++)
    {
        int from = mst_list[i];
        beta[from] = -UnReachCost;
        mark[from] = from;
        for (int to = from; to != 0; to = dad[to])   // 从from结点向上求beta值
        {
            beta[dad[to]] = GetMax(beta[to], cost[to]);
            mark[dad[to]] = from;
        }

        for (int j = 0; j < dim; j++)   // 向下求beta值
        {
            int to = mst_list[j];
            if (mark[to] != from)
                beta[to] = GetMax(beta[dad[to]], cost[to]);
        }

        for (int j = 0; j < numEdge[from]; j++)
        {
            int to = edge[from][j].to;
            if (from != nsp && to != nsp)
            {
                edge[from][j].alpha = edge[from][j].cost_pi - beta[to];
            }
            else
            {
                edge[from][j].alpha = GetMax(edge[from][j].cost_pi - esp->cost_pi, 0);
            }
        }
    }
}

/*************************************************************************
*  函数名称：CandidateSolver::create_candi
*  功能说明：创建候选集，得到各节点的cand, candCn.函数会把复制点的候选集解移动到原点，相当于删除了复制点。
*           由于前面double的时候用的是dim=num*2,所以这里将复制点移动以后，相当于删除了复制点
*  参数说明：本函数调用前需要先设置好如下参数:1. 图   2.最大候选集数、精度等
*  函数返回：无
*  修改时间：2017-02-07 10:33:25
*************************************************************************/
unsigned char CandidateSolver::create_candi()
{
    Int64 low_bound = ascent();

    // ascent找到一条环路, 将不会更新候选集而直接得到tour解
    if (Norm == 0)
    {
        for (int x = nsp; x != 0; x = dad[x])// 通过dad得到解
            node[x].bestSucc = &node[dad[x]];
        for (int x = esp->to; x != 0; x = dad[x])
            node[dad[x]].bestSucc = &node[x];
        node[esp->to].bestSucc = &node[nsp];

        // 点double了, 要去掉复制点
        if (node[0].bestSucc->id == num)    // 当前的环的反向是反的
        {
            for (int x = 0; x < num; x++)
                node[node[x + num].bestSucc->id].bestSucc = &node[x];
        }
        else
        {
            // 当前的环的方向是正的
            for (int x = 0; x < num; x++)
                node[x].bestSucc = node[x].bestSucc->bestSucc;
        }
        return True;
    }

    Int64 max_alpha = low_bound / Precision;//Excess * Abs(low_bound);

    calc_alpha(); // 计算alpha值
    gen_candi(CandidatesNum, max_alpha, SymmetricCandidates);

    // 将候选集解复制到node
    for (int x = 0; x < num; x++)
    {
        node[x].candCn = numCandi[x] - 1;
        for (int i = 0, j = 0; i < node[x].candCn; i++, j++)
        {
            if (edge[x][j].to == x + num) j++;
            node[x].cand[i] = &node[edge[x][j].to - num];
            node[x].alpha[i] = edge[x][j].alpha;//把alpha赋值到Node里面
        }
        //把pi赋值到Node里面
        node[x].outPi = pi[x];
        node[x].inPi  = pi[x + num];
    }
    return False;
}

