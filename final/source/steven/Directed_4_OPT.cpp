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
*************************************************************************/
unsigned char Road::_4_OPT(long long &bestCost)
{
	int i,j;
	unsigned char isImproved = False;
	long long bestGain,Gain2,Gain4;
	VVNode *t1=NULL,*t2=NULL,*t3=NULL,*t4=NULL,*t5=NULL,*t6=NULL,*t7=NULL,*t8=NULL;
	VVNode *T1=NULL,*T2=NULL,*T3=NULL,*T4=NULL,*T5=NULL,*T6=NULL,*T7=NULL,*T8=NULL;
	
	#if PRINT_TIME_INFO == 1
		long long timeStart = clock();
	#endif
	#if PRINT_OPT_INFO == 1
		Print("start run %s() *********************************************\n",__FUNCTION__);
	#endif
	
	resetActive();
	while(NULL != (t1=RemoveFirstActive()))
	{
		t2 = t1->succ;
		bestGain = 0;
		for(i = 0;i < t1->candCn;i++)
		{
			AssertContinue((t3=t1->cand[i]) != t2);
			AssertContinue((t4=t3->pred)    != t2);
			Gain2 =   COST(t1,t2) - COST(t1,t3)
					+ COST(t4,t3) - COST(t4,t2);
		   //这一条很省时间!!!
			AssertContinue(Gain2 > 0);
			//1.不使用Gain2>0规则,t5在单边[t2,t4)
			//2.等价于1,使用Gain1>0规则,t5在双边[t2,t4),[t3,t1),本程序使用的是这种情况
			unsigned char t5InSegment24 = True;
			for(t5=t2; t5!=t1 ;t5=t5->succ)
			{
				if(t5==t4)
				{
					t5InSegment24 = False;
					continue;
				}
				t6 = t5->succ;
				for(j = 0;j < t5->candCn;j++)
				{
					t7 = t5->cand[j];
					//exam if t7 is in segment(t3,t1] or (t2,t4]
					if(t5InSegment24)	{AssertContinue(isInSegment(t7,t3,t1));}
					else 				{AssertContinue(isInSegment(t7,t2,t4));}
					t8 = t7->pred;
					Gain4 = Gain2 + COST(t5,t6) - COST(t5,t7)
								  + COST(t8,t7) - COST(t8,t6);
					if(bestGain < Gain4)
					{
						bestGain = Gain4;
						RecordT(1);RecordT(2);RecordT(3);RecordT(4);RecordT(5);RecordT(6);RecordT(7);RecordT(8);
						//goto _4_OPT_Update;
					}
				}
			}
		}
//_4_OPT_Update:
		if(bestGain > 0)
		{
			#if PRINT_OPT_INFO == 1
				Print("%s Gain: ",__FUNCTION__);
				printf_3var(bestCost>>7u,bestGain>>7u,(bestCost-bestGain)>>7u,%lld);
			#endif
			bestCost -= bestGain;
			isImproved = True;		
			Active(T1);	Active(T2);	Active(T3);	Active(T4);	Active(T5);	Active(T6);	Active(T7);	Active(T8);
			//creat a shorter tour,store it and reset Order
			LinkA2B(T1,T3);	LinkA2B(T4,T2);	LinkA2B(T5,T7);	LinkA2B(T8,T6);
			resetOrder();
			//if(UsedTourTable && ((bestCost-sumPi)>>7u) < UnReachCost)
			//	tt.insert(this,(bestCost-sumPi)>>7u);
		}
	}
	#if PRINT_TIME_INFO == 1
		Print("Time : %s() used time %lld\n\n",__FUNCTION__,clock() - timeStart);
	#endif
	
	return isImproved;
}
