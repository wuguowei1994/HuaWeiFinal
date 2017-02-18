/**
 * @Author:      steven
 * @DateTime:    2017-02-09 11:43:50
 * @Description: done
 */
#include "steven.h"
/*************************************************************************
*  函数名称：Road::_4_OPT
*  功能说明：有向4-opt交换(只有一种情况)
*  参数说明：函数实现的是4-non-sequential-opt
*  函数返回：false-------4-opt失败   true-------4-opt成功
*  修改时间：2017-02-07 10:08:12
*  实现过程: 原路:t1->t2------t5->t6------t4->t3------t8->t7------
*    4-opt交换后: t1->t3------t8->t6------t4->t2------t5->t7------
*    首先找出t1,在t1确定的情况下,t2必为t1的succ;
*    那么t3肯定是t1的候选集里的一个点，且t3!=t2;
*    在t3确定以后,t4必为t3的pred;
*    t4确定以后，t5必定位于(t2,t4)之间,同时t6=t5->succ
*    t5确定以后，t7为t5候选集，t8=t7->pred
*    综上:t3是t1的候选集,t7是t5的候选集
*          t2不是t4的候选集,t6不是t8的候选集
*    规律:if(i<j) t[i]不是t[j]的候选集
            else  t[i]是t[j]的候选集
     t1
     t2=t1->succ
     t3=t1->cand
     t4=t3->pred
     t5=(t2,t4)
     t6=t5->succ
     t7=t5->cand
     t8=t7->pred
* 2017-02-18 11:30:53 补充:这里所说的X-opt有多少种情况，都是以图形为基础的，如果有4张图
*                          A->B    C->D
*                          但是B和D除了节点的编号其他东西完全一样，那么这两个其实就是一张图
*************************************************************************/
unsigned char Road::_4_OPT(long long &bestCost)
{
	int i, j;
	unsigned char isImproved = False;
	// GainX代表去掉X条边，加入X条新边后的增益
	long long bestGain, Gain2, Gain4;
	VVNode *t1 = NULL, *t2 = NULL, *t3 = NULL, *t4 = NULL, *t5 = NULL, *t6 = NULL, *t7 = NULL, *t8 = NULL;
	VVNode *T1 = NULL, *T2 = NULL, *T3 = NULL, *T4 = NULL, *T5 = NULL, *T6 = NULL, *T7 = NULL, *T8 = NULL;

#if PRINT_TIME_INFO == 1
	long long timeStart = clock();
#endif
#if PRINT_OPT_INFO == 1
	printf("start run %s() *********************************************\n", __FUNCTION__);
#endif
	resetActive();
	while (NULL != (t1 = RemoveFirstActive()))
	{
		t2 = t1->succ;
		bestGain = 0;
		for (i = 0; i < t1->candCn; i++)
		{
			AssertContinue((t3 = t1->cand[i]) != t2);
			AssertContinue((t4 = t3->pred)    != t2);
			Gain2 =   COST(t1, t2) - COST(t1, t3)
			          + COST(t4, t3) - COST(t4, t2);
			//这一条很省时间!!!
			AssertContinue(Gain2 > 0);
			/*
			 t5和t6在(t2,t4)这边
			 或者在(t3,t1)那边
			 先不考虑t3,t4的位置，只看t1,t2,t5这三个点。
			 先这样遍历:t5=t2->succ
			 t5首先会出现在(t2,t4)之间，此时t7出现在(t3,t1)之间
			 随后t5出现在(t3,t1之间),此时t7出现在
			 */
			unsigned char t5InSegment24 = True;
			for (t5 = t2; t5 != t1 ; t5 = t5->succ)
			{
				if (t5 == t4)
				{
					t5InSegment24 = False;
					continue;
				}
				t6 = t5->succ;
				for (j = 0; j < t5->candCn; j++)
				{
					t7 = t5->cand[j];
					//判断t7是否在(t3,t1]或者(t2,t4)之间
					if (t5InSegment24)	{AssertContinue(isInSegment(t7, t3, t1));}
					else 				{AssertContinue(isInSegment(t7, t2, t4));}
					t8 = t7->pred;
					Gain4 = Gain2 + COST(t5, t6) - COST(t5, t7)
					        + COST(t8, t7) - COST(t8, t6);
					if (bestGain < Gain4)
					{
						bestGain = Gain4;
						RecordT(1); RecordT(2); RecordT(3); RecordT(4); RecordT(5); RecordT(6); RecordT(7); RecordT(8);
					}
				}
			}
		}
		if (bestGain > 0)
		{
			bestCost -= bestGain;
			isImproved = True;
			Active(T1);	Active(T2);	Active(T3);	Active(T4);	Active(T5);	Active(T6);	Active(T7);	Active(T8);
			//创建一条更短的路径，储存下来，然后将所有点的顺序按照它在新路径中的位置重置
			LinkA2B(T1, T3);	LinkA2B(T4, T2);	LinkA2B(T5, T7);	LinkA2B(T8, T6);
			resetOrder();
		}
	}
	return isImproved;
}
