/**
 * @Author:      steven
 * @DateTime:    2017-02-09 15:49:09
 * @Description: Done
 */
#include "steven.h"
//initial static variables in class Road
Graph *Road::Gp;
int Road::s = 0, Road::t = 0;
long long Road::roadCostSum[3] = {UnReachCost, UnReachCost, UnReachCost};
long long Road::roadCostSumMin = UnReachCost;
int Road::sLKHuseCn = 0;
int Road::routeCn = 0;
int Road::iteration = 0, Road::iter[routeCnMax] = {10, 10, 10, 10};
int Road::trial = 0;
int Road::Trials = 0;
/*************************************************************************
*  函数名称：Road::initial
*  功能说明：将demand.csv中的必经点按照如下顺序存入list数组：起点---必经点----终点,并将它们的属性设置为isMust
*  参数说明：filename是demand.csv的路径
*  函数返回：无
*  修改时间：2017-02-06 19:35:53
*************************************************************************/
// 函数的实参:initial(demand[0], &G, 0);
void Road::initial(char *demand, Graph *Gp, int id)
{
    //initial id,Gp
    this->id = id;
    Road::Gp = Gp;
    if (2 == sscanf(demand, "%d,%d,%d", &this->s, &this->t, list + (++numInit)))
    {
        // 如果进入这个if语句，说明demand.csv只含有起点和终点，没有要求必经点
        list[0] = this->s;
        list[1] = this->t;
    }
    else
    {
        // 按照这个顺序给list数组添加数据:起点---必经点----终点,
        list[0] = this->s;
        numInit++;
        //set list={s,V'}
        for (int i = 0, str_len = strlen(demand); i < str_len; i++)
            if (demand[i] == '|')
                if (1 != sscanf(demand + i + 1, "%d", list + (numInit++)))
                    numInit--;
        list[numInit] = this->t;
    }
    //将它们的属性设置为isMust
    //numInit+1等于必经点个数
    // list[i]的值是点的索引
    for (int i = 0; i < numInit; i++)
        isMustInit[list[i]] = True;
    isMustInit[this->t] = True;
    //initial Node[].id
    int nodeNum = sizeof(Node) / sizeof(VVNode);
    for (int i = 0; i < nodeNum; i++)
        Node[i].id = i;
}


/*************************************************************************
*  函数名称：Road::setCost
*  功能说明：
*  1.将起点s和终点t合并成一个点;
*  2.删除s的入度;
*  3.t的入度(不包括从s来的)作为s的入度
*  4.用cost[i][j]记录必经点集中第i+1个点到第j+1个点的最短路径大小
*  5.得到必经点之间的最短路径
*  参数说明：
*  1.isUseCopy:是否使用了double点的方法
*  2.必经点集:包括起点和终点，同时将必经点中起点和终点以外的顶点按照索引从低到高的顺序排列)
*  函数返回：无
*  修改时间：2017-02-06 20:18:52
*  补充: 由于合并了起点和终点,同时将终点的入度作为起点的入度,所以有些点的出入度发生了变化，
*  例如：list:{0,2,3,1}合并起终点就变成了list:{0,2,3}那么计算cost[1][0]的时候不是计算从节点2到节点0的距离，而是计算从节点2到节点1的距离(因为要将终点的入度作为起点的入度)
*************************************************************************/
void Road::setCost()
{
    if (!isUseCopy)
    {
        // 这里num=必经点总数-1,因为合并了起点和终点
        for (int ri = 0; ri < num; ri++)
            // 因为rj从1开始，所以s的入度被删除
            for (int rj = 1; rj < num; rj++)
                // cost[i][j]代表了必经点集中第i+1个点到第j+1个点的最短路径大小
                cost[ri][rj] = Vcost[list[ri]][list[rj]];
        cost[0][0] = UnReachCost;       //s到自己距离为无穷
        for (int ri = 1; ri < num; ri++) //t的入度(不包括从s来的)作为s的入度
            cost[ri][0] = Vcost[list[ri]][this->t];
    }
    else
    {
        int n = num - numRepeat;                //n={s,必须点,重复点,t}
        //先重设一下点间距离
        for (int ri = 0; ri < num; ri++)
            for (int rj = 0; rj < num; rj++)
                cost[ri][rj] = UnReachCost;
        //集合{s,必须点,重复点,t}内部点的距离
        for (int ri = 0; ri < n - 1; ri++)  //t不要出度
            for (int rj = 1; rj < n; rj++)  //s不要入度
                cost[ri][rj] = Vcost[list[ri]][list[rj]];
        //t的出度:t除了到第一个复制点可达,其它点均不可达
        cost[n - 1][n] = 0;
        //所有重复点的复制点(除了最后一个)
        for (int ri = n; ri < num - 1; ri++)
        {
            cost[ri][ri + 1] = 0;           //相邻两个复制点
            cost[ri][ri - numRepeat - 1] = 0;   //复制点到原点
            cost[ri - numRepeat - 1][ri + 1] = 0;   //原点到下一个复制点
        }
        //最后一个复制点
        cost[num - 1][n - 2] = 0;           //复制点到原点
        //s的出度:除了最后一个重复点和它的复制点到s可达,其它点均不可达
        cost[n - 2][0] = 0;             //最后一个重复点
        cost[num - 1][0] = 0;               //最后一个复制点
    }
}


/*************************************************************************
*  函数名称：Road::setTour()
*  功能说明：得到环路中当前必经点顺序,并计算去pi权重
*  参数说明：minBestCost为带pi权重
*  函数返回：无
*  注意事项：tour[num] = this->t;
*  修改时间：2017-02-06 20:29:24
*************************************************************************/
void Road::setTour()
{
    unsigned char isListUse[MaxV2], isFindt = False;
    memset(isListUse, False, sizeof(unsigned char)*MaxV2);

    //tsp问题转化回去寻路问题,输出tour
    VVNode *N = &Node[0];
    for (int i = 0; i < num; i++) //此时的num={s,必须点,用到的准必须点,t,未到的准必须点}
    {
        if (!isFindt)
            isListUse[N->id] = True;
        tour[i] = list[N->id];
        if (tour[i] == this->t)
            isFindt = True;
        N = N->bestSucc;
    }

    //reset list[]:{s,最初的必须点,用到的准必须点,t},num下标的地方继续为t
    //撤回没用到的准必须点(isMust恢复False,冲list里面踢出去)
    //升级用到的准必须点(已经在list和must里面设置了,就不用重读设置了)
    if (isUseCopy)
    {
        int listNew[MaxV2], numNew = 0;
        for (int i = 0; i < num - numRepeat - 1; i++) //检查{s,必须点,重复点}是否都用到了
        {
            if (isListUse[i] == True)
                listNew[numNew++] = list[i];
            else
                isMust[list[i]] = False;
        }
        memcpy(list, listNew, sizeof(int)*numNew);
        num = numNew;
        list[num] = this->t;
    }
    tour[num] = this->t;        //必须要设置,因为setVtour用到了
//按照点的索引打印
#if PRINT_TOUR == 1
    Print("Tour is : \n");
    for (int i = 0; i < num; i++)
        Print("%d,", tour[i]);
    Print("%d\n\n", this->t);
#endif
}


/*************************************************************************
*  函数名称：Road::setVtour
*  功能说明：得到最终序列，点的数量，Vtour包含所有必须点和一些非必须点
   Vtour[]:s->{V',T}->t,T是一些non-must-node的集合
*  参数说明：无
*  函数返回：无
*  注意事项：Vtour[VtourCn-1] = this->t;
*  修改时间：2017-02-06 21:09:54
*************************************************************************/
void Road::setVtour()
{
    int vnext, v2;
    // 求出的路径中所有点(包含非必经点)的数量
    VtourCn = 0;
    for (int i = 0; i < num; i++) //此时num和list已经reset过
    {
        /*
        tour[0]:0
        tour[1]:1
        tour[2]:3
        */
        vnext = tour[i];
        v2 = tour[i + 1];
        while (vnext != v2)
        {

            Vtour[VtourCn++] = vnext;
            //环路用到的所有点间路径
            vnext = Vpath[vnext][v2];
        }
    }
    Vtour[VtourCn++] = this->t;
    // cout << "Vtour is :";
    // for (int i = 0; i < VtourCn; i++)
    // {
    //     if (isMust[Vtour[i]])
    //     {
    //         if (i == VtourCn - 1)
    //             cout << Vtour[i] << endl;
    //         else
    //             cout << Vtour[i] << "----";
    //     }
    // }
}


/*************************************************************************
*  函数名称：Road::setVtourId
*  功能说明：set VtourId[],要放在swapGraphCostBack()之前,因为此时要用经过交换后的id[][]
*  参数说明：无
*  函数返回：无
*  修改时间：2017-02-06 21:10:14
*************************************************************************/
void Road::setVtourId()
{
    int v1, v2;
    EDGENUM = 0;
    for (int i = 0; i < VtourCn - 1; i++)
    {
        v1 = Vtour[i];
        v2 = Vtour[i + 1];
        //两点之间最短边的索引
        VtourId[i] = Gp->id[v1][v2];
        EDGENUM++;
    }
    cout << "解为:";
    // 按照边的索引打印
    for (int i = 0; i < EDGENUM - 1; i++)
        cout << VtourId[i] << "|";
    cout << VtourId[EDGENUM - 1] << endl;
}
/*************************************************************************
*  函数名称：Road::writeFile
*  功能说明：输出最优解
*  参数说明：无
*  函数返回：无
*  修改时间：2017-02-09 15:46:30
*************************************************************************/
void Road::writeFile(bool haveSolve)
{
    string result;
    if (haveSolve) {
        // 按照边的索引输出
        for (int i = 0; i < EDGENUM - 1; i++)
            result = result + int2string(VtourId[i]) + "|";
        result += int2string(VtourId[EDGENUM - 1]);
    }
    else
        result = "NA";
    ofstream SaveFile(fileName);
    SaveFile << result;
    SaveFile.close();
}

/*************************************************************************
*  函数名称：Road::int2string
*  功能说明：int转string
*  参数说明：无
*  函数返回：string
*  修改时间：2017-02-09 15:47:26
*************************************************************************/
string Road::int2string(int in) {
    stringstream stream;
    string str;
    stream << in;
    stream >> str;
    return str;
}


//set roadCost[0],VtourCost[]
void Road::setVtourCost()
{
    int v1, v2;
    roadCost[0] = 0;
    for (int i = 0; i < VtourCn - 1; i++)
    {
        v1 = Vtour[i];
        v2 = Vtour[i + 1];
        if (VtourId[i] == Gp->id[v1][v2])
            VtourCost[i] = Gp->cost[v1][v2];
        else
            VtourCost[i] = Gp->subCost[v1][v2];
        roadCost[0] += VtourCost[i];
    }
    cout << "权重:" << roadCost[0] << endl;
    //set HashTable
    ht.insert(this);
}
