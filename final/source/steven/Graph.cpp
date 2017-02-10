/**
 * @Author:      steven
 * @DateTime:    2017-02-09 11:52:16
 * @Description: Done
 */
#include "steven.h"
/*************************************************************************
*  函数名称：Graph::initial
*  功能说明：把topo.csv里面记录的边的信息放入cost[s][t]矩阵中
*  参数说明：topo为输入字符指针数组，edge_num为边的条数
*  函数返回：无
*  修改时间：2017-02-06 19:32:53
*************************************************************************/
void Graph::initial(char *topo[MAX_EDGE_NUM], int edge_num)
{
	char *line;
	int getChar, linkID, srcID, dstID, linkCost, lineLength;
	for (int i = 0, j; i < edge_num; i++)
	{
		line = topo[i];
		//lineLength为行的长度
		lineLength = strlen(line);
		j = linkID = srcID = dstID = linkCost = 0;
		//sdigit是C语言中的一个函数，主要用于检查参数c是否为阿拉伯数字0到9。
		//getChar = line[j++]先取出line[j]的值，然后才把j+1
		while (!isdigit(getChar = line[j++]) && j < lineLength);
		if (isdigit(getChar))	linkID = getChar - '0';
		else	continue;
		while (isdigit(getChar = line[j++]))	 linkID = 10 * linkID + (getChar - '0');
		while (!isdigit(getChar = line[j++])); srcID = getChar - '0';
		while (isdigit(getChar = line[j++]))	 srcID = 10 *  srcID + (getChar - '0');
		while (!isdigit(getChar = line[j++])); dstID = getChar - '0';
		while (isdigit(getChar = line[j++]))	 dstID = 10 *  dstID + (getChar - '0');
		while (!isdigit(getChar = line[j++])); linkCost = getChar - '0';
		while (isdigit(getChar = line[j++]))	 linkCost = 10 *  linkCost + (getChar - '0');
		/*
		经过上面的循环以后:
		1.linkID---->linkid
		2.srcID---->sourceID
		3.dstID---->destinationID
		4.linkCost---->linkcost
		*/
		//update cost、subCost、id、subId、Node、
		//cost[srcID][dstID]的初始值均为零
		if (cost[srcID][dstID] == 0)
		{
			cost[srcID][dstID] = linkCost;
			id[srcID][dstID] = linkID;
			//记录节点t的前驱节点s
			Node[dstID].addPred(srcID);
		}
		else if (linkCost < cost[srcID][dstID])
		{
			subCost[srcID][dstID] = cost[srcID][dstID];
			subId[srcID][dstID] = id[srcID][dstID];
			cost[srcID][dstID] = linkCost;
			id[srcID][dstID] = linkID;
		}
		else if (subCost[srcID][dstID] == 0 || linkCost < subCost[srcID][dstID])
		{
			subCost[srcID][dstID] = linkCost;
			subId[srcID][dstID] = linkID;
		}
		//update num、MaxReachCost
		num = GetMax(num , GetMax(srcID, dstID));
	}
	num++;
	setCost();
}
/*************************************************************************
*  函数名称：Graph::setCost
*  功能说明：设置cost，subCost,如果没有值(系统初始化为0)就设置MaxReachCost
*  参数说明：无
*  函数返回：无
*  修改时间：2017-02-06 21:02:25
*************************************************************************/
void Graph::setCost()
{
	for (int i = 0; i < num; i++)
		for (int j = 0; j < num; j++)
		{
			if (cost[i][j] == 0)
			{
				cost[i][j] = subCost[i][j] = UnReachCost;
				id[i][j] = subId[i][j] = -1;
			}
			else if (subCost[i][j] == 0)
			{
				subCost[i][j] = UnReachCost;
				subId[i][j] = -1;
			}
		}
}
