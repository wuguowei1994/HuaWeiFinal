/**
 * @Author:      steven
 * @DateTime:    2017-02-09 11:43:32
 * @Description: Done
 */
#include "steven.h"
/*************************************************************************
*  函数名称：Road::_3_OPT
*  功能说明：有向3-opt交换（只有一种情况）
*  参数说明：函数实现的是3-sequential-opt
*  函数返回：false:3-opt失败;true:3-opt成功
*  修改时间：2017-02-07 10:08:21
*  实现过程: 原路: t1->t2------t4->t3------t6->t5------
*    3-opt交换后: t1->t3------t6->t2------t4->t5------
*    首先找出t1,在t1确定的情况下,t2必为t1的succ;
*    那么t3肯定是t1的候选集里的一个点，且t3!=t2;
*    在t3确定以后,t4必为t3的pred;
*    确定t4以后，t5必为t4的候选集中的一个点
*    t5确定以后，t6为t5的pred
*    综上，t3是t1的候选集,t5是t4的候选集,但是t2不是t6的候选集
*************************************************************************/
unsigned char Road::_3_OPT(long long &bestCost)
{
	int i,j;
	unsigned char isImproved = False;
	// Gain1代表去掉一条边，然后加入一条新边后的增益
	// Gain3代表去掉三条边，然后加入三条新边后的增益
	long long bestGain,Gain1,Gain3;
	VVNode *t1=NULL,*t2=NULL,*t3=NULL,*t4=NULL,*t5=NULL,*t6=NULL;
	VVNode *T1=NULL,*T2=NULL,*T3=NULL,*T4=NULL,*T5=NULL,*T6=NULL;
	
	#if PRINT_TIME_INFO == 1
		long long timeStart = clock();
	#endif
	#if PRINT_OPT_INFO == 1
		printf("start run %s() *********************************************\n",__FUNCTION__);
	#endif
	resetActive();
	while(NULL != (t1=RemoveFirstActive()))
	{
		t2 = t1->succ;
		bestGain = 0;
		printf("t1的候选边共有:%d个\n",t1->candCn);
		for(i = 0;i < t1->candCn;i++)
		{
			t3 = t1->cand[i];
			AssertContinue(t3 != t2);
			Gain1 = COST(t1,t2) - COST(t1,t3);
			AssertContinue(Gain1 > 0);
			t4 = t3->pred;
			
			for(j = 0;j < t4->candCn;j++)
			{
				t5 = t4->cand[j];
				//exam if t5 is in segment(t3,t1]
				AssertContinue(isInSegment(t5,t3,t1));
				t6 = t5->pred;
				Gain3 = Gain1 + COST(t4,t3) - COST(t4,t5)
							  + COST(t6,t5) - COST(t6,t2);
				if(bestGain < Gain3)
				{
					bestGain = Gain3;
					RecordT(1);RecordT(2);RecordT(3);RecordT(4);RecordT(5);RecordT(6);
					goto _3_OPT_Update;
				}
			}
		}
_3_OPT_Update:
		if(bestGain > 0)
		{
			#if PRINT_OPT_INFO == 1
				Print("%s Gain: ",__FUNCTION__);
				printf_3var(bestCost,bestGain,bestCost-bestGain,%lld);
			#endif
			bestCost -= bestGain;
			isImproved = True;
			Active(T1);	Active(T2);	Active(T3);	Active(T4);	Active(T5);	Active(T6);
			//create a shorter tour,store it and reset Order
			LinkA2B(T1,T3);	LinkA2B(T4,T5);	LinkA2B(T6,T2);
			resetOrder();
		}
	}
	#if PRINT_TIME_INFO == 1
		Print("Time : %s() used time %lld\n\n",__FUNCTION__,clock() - timeStart);
	#endif
	
	return isImproved;
}












