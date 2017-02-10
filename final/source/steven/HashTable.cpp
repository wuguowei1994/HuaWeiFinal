/**
 * @Author:      steven
 * @DateTime:    2017-02-09 11:55:14
 * @Description: Done
 */
#include "steven.h"
/*************************************************************************
*  函数名称：HashTable::insert(Road *Rp)
*  功能说明：将找到的路径存入HashTable中
*  参数说明：Rp为Road指针,通常是this
*  函数返回：无
*  修改时间：2017-02-09 11:54:34
*************************************************************************/
void HashTable::insert(Road *Rp)
{
	// 说明:cn=0;
	rpoHash[cn]  = Rp->rpoHash;
	VtourCn[cn]  = Rp->VtourCn;
	roadCost[cn] = Rp->roadCost[0];
	VtourId[cn] = (int *)malloc(sizeof(int) * Rp->VtourCn);
	Vtour[cn]   = (int *)malloc(sizeof(int) * Rp->VtourCn);
	//Rp->VtourId赋值给VtourId[cn]，也就是VtourId[0]
	memcpy(VtourId[cn], Rp->VtourId, sizeof(int)*Rp->VtourCn);
	memcpy(Vtour[cn],  Rp->Vtour,   sizeof(int)*Rp->VtourCn);
	cn++;
}


