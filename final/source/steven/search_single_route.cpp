/**
 * @Author:      steven
 * @DateTime:    2017-02-09 16:27:19
 * @Description: Done
 */
#include "steven.h"
/*************************************************************************
*  函数名称：Road::search_single_route
*  功能说明：得到{s,V',t}的最短无环路径
*  参数说明：solveTspCn是单条路求tsp次数的计数器;num为合并起点和终点后必经点的总数
*  函数返回：无
*  修改时间： 2017-02-06 20:48:04
*************************************************************************/
void Road::search_single_route(Road *Rpo)
{
    bool haveSolve=false;
    long long timeStart = clock();
    //initial isMust、num、list、roadCost、solveTspCn、isUseCopy、cost
    // 将isMustInit拷贝到isMust
    memcpy(isMust, isMustInit, sizeof(unsigned char)*Gp->num);
    // cout << "点的总数：" << Gp->num << endl << endl;
    // cout << "起点终点合并后必经点总数::" << numInit << endl << endl;

    num = numInit;
    //设置list尾部为必经路径的终点
    list[num] = this->t;
    // 初始化路径权重和
    roadCost[2] = roadCost[1];
    roadCost[1] = roadCost[0];
    roadCost[0] = UnReachCost;
    solveTspCn = 0;
    //默认不使用double策略，除非第一次LKH算法计算出的结果重复边较多
    isUseCopy = False;

    //转化为tsp问题,每次都是新的tsp
    while(1)
    {
        // solveTspCn:单条路求tsp次数计数器
        solveTspCn++;		
        // 使用SPFA算法求所有必经点间的最短路径;如果i或者j中有一个点为非必经点，那么Vcost[i][j]为0
        setVcost_SPFA();	
        // 根据必经点个数选择算法
        if(num == 1)		//num=1 too easy
        {
            roadCost[0] = Vcost[this->s][this->t];
            tour[0] = this->s;
            tour[1] = this->t;
        }
        else if(num == 2)	//num=2 too easy
        {
            // 计算权重
            roadCost[0] = Vcost[s][list[1]] + Vcost[list[1]][this->t];//sourceNode--->list[1]------>terminalNode
            tour[0] = this->s;
            tour[1] = list[1];
            tour[2] = this->t;
        }
        else				//num>3 difficult
        {
            setCost();		
            roadCost[0] = LKH();
            if(roadCost[0] < UnReachCost) 	setTour();
        }
        if(roadCost[0] >= UnReachCost)
        {
            //进入这个if语句说明没有找到最优路径
            if(solveTspCn == 1)	break;	//case A:第一次就没求出路径直接退出
            // 这时才使用double策略
            if(!isUseCopy)				//case B:上次没使用复制点导致没有解,恢复有复制点的状态
            {
                isUseCopy = True;
                resumeBak();
                continue;
            }
            else break;					//case C:上次使用复制点了仍然没有求出解(其实不太可能)就跳出吧
        }
        else
        {
            //isUseCopy:是否使用double点策略
            isUseCopy = False;
            //有路径条件下：记录最终路径，包含所有must-node和部分non-must-node
            setVtour();
            //有路径条件下：没有重复点则跳出，正常输出路径；有重复点则把重复点加入VV，继续找
            if(examRepeatNode() == 0)	break;
            //还有重复点,但是time out,then break,output NA
            if(clock() > TIME_OUT)
            {
                roadCost[0] = UnReachCost;
                break;
            }
        }
    }
    if(roadCost[0] < UnReachCost)
    {
        // 根据求出的路径，记录边索引
        setVtourId();
        // 根据求出的路径，计算权重
        setVtourCost();
        haveSolve=true;
    }
    writeFile(haveSolve);
    singleRouteTime = clock() - timeStart;
}
