/**
 * @Author:      steven
 * @DateTime:    2017-02-09 12:00:46
 * @Description: Done
 */
#include "steven.h"
/*************************************************************************
*  函数名称：Road::LKH()
*  功能说明：使用LKH算法解决atsp问题
*  参数说明：无
*  函数返回：返回路径长
*  修改时间：2017-02-06 20:52:56
*************************************************************************/
long long Road::LKH()
{
	// long是32位宽，占4个字节，long long通常被定义成64位宽
	long long bestCostMin = UnReachCost,bestCost;
	//寻路总共使用LKH的次数
	sLKHuseCn++;					
	//使用次梯度优化来得到候选集,如果直接得到tour就跳到"存储返回"
	if(creatCandidates(bestCostMin))
		return bestCostMin;
	
	//使用Trials次findTour(主要是opt边交换操作)
	resetBestSucc();
	for(trial = 0,Trials = num;trial < Trials;trial++)//尝试Trials次初始化和opt操作,记录最好的路
	{
		bestCost = findTour(bestCostMin<UnReachCost);
		if(bestCost < bestCostMin)
		{
			bestCostMin = bestCost;
			storeBestSucc();
		}
	}
	return bestCostMin;
}


/*************************************************************************
*  函数名称：Road::findTour()
*  功能说明：在LKH的核心思路K_OPT,被LKH调用
*  参数说明：无
*  函数返回：这一轮的路径权值
*  修改时间：2017-02-06 20:55:00
*************************************************************************/
long long Road::findTour(unsigned char hasFindTour)
{
	unsigned char KMFindTour = false;
	// 将KMFindTour作为函数的实参，如果在initialTour中使用了KM算法，那么会将KMFindTour置为true
	long long bestCost = initialTour(hasFindTour,KMFindTour);
	// 如果在initialTour()中使用了KM算法，那么不会进入这个if语句
	if(!KMFindTour)
	{
		while(1)
		{
			_5_OPT(bestCost);
			if(_4_OPT(bestCost))	continue;
			break;
		}
	}
	return (bestCost - sumPi)>>7u;
}


/*************************************************************************
*  函数名称：Road::storeBestSuccPred()
*  功能说明：记录每次K_OPT找路得最优邻居,被findTour()调用
*  参数说明：无
*  函数返回：无
*  修改时间：2017-02-06 20:55:39
*************************************************************************/
void Road::resetBestSucc()
{	
	for(int i = 0;i < num;i++)
		Node[i].lastBestSucc = Node[i].bestSucc = NULL;
}

void Road::storeBestSucc()
{
	for(int i = 0;i < num;i++)
	{
		Node[i].lastBestSucc = Node[i].bestSucc;
		Node[i].bestSucc = Node[i].succ;
	}
}

