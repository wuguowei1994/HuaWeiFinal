#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
using namespace std;
int n, m;
const int maxn = 205;
const int INF = 0xffffff;
int w[maxn][maxn];
int lx[maxn], ly[maxn]; //顶标
int linky[maxn];
int visx[maxn], visy[maxn];
int slack[maxn];
/**
 * @param x:需要计算増广轨的x节点
 * @return
 *         true: 找到了x节点的増广轨
 *         false:没有找到x节点的増广轨
 * @date 2017-02-13 09:28:37
 * @description 计算x节点的増广轨.本函数可以被递归调用
 */
bool find(int x)
{
	visx[x] = true;
	for (int y = 1; y <= n; y++)
	{
		if (visy[y]) continue;
		int t = lx[x] + ly[y] - w[x][y]; //顶标之和减去权重
		if (t == 0)
		{
			visy[y] = true;
			// 如果节点y还没有找到匹配;或者节点y已经有匹配，但是在这次查找中可以找到经过y节点的増广轨;这是一个递归
			if (linky[y] == -1 || find(linky[y]))
			{
				linky[y] = x;
				return true;//找到増广轨
			}
		}
		else if (slack[y] > t)
		{
			// 在KM算法中slack等于A和B的顶标之和减去A->B之间的权重
			slack[y] = t;
		}
	}
	return false;//没有找到増广轨
}
/**
 * [KM description]
 * @return 返回最优匹配的值;如果返回1说明没有找到最优匹配
 */
int KM()
{
	int i, j;
	// 所有的节点y初始化为未访问的状态
	memset(linky, -1, sizeof(linky));
	// 所有的y节点的顶标初始化为0
	memset(ly, 0, sizeof(ly));
	// 初始化x节点的顶标为所有与x节点相连的边的最大权重
	for (i = 1; i <= n; i++)
	{
		lx[i] = -INF;
		for (j = 1; j <= n; j++)
			if (w[i][j] > lx[i])
				lx[i] = w[i][j];
		cout<<"节点"<<i<<"的顶标为:"<<lx[i]<<endl;
	}
	for (int x = 1; x <= n; x++)
	{
		for (i = 1; i <= n; i++)
			slack[i] = INF;
		// 一直循环直到找到节点x的増广轨
		while (true) {
			// 初始化x和y集合中的节点访问状态
			memset(visx, 0, sizeof(visx));
			memset(visy, 0, sizeof(visy));
			//如果找到了x的増广轨，直接退出
			if (find(x))
				break;
			// 如果没有找到，那就通过改变d这个变量对x和y集合的顶标进行调整，然后再找
			int d = INF;
			for (i = 1; i <= n; i++) {
				if (!visy[i] && d > slack[i])
					d = slack[i];
			}
			for (i = 1; i <= n; i++)
			{
				if (visx[i])
					// 将x的顶标减少d
					lx[i] -= d;
			}
			for (i = 1; i <= n; i++) {
				if (visy[i])
					// 将y的顶标增加d
					ly[i] += d;
				else
					slack[i] -= d;
			}
		}
	}
	int result = 0;
	for (i = 1; i <= n; i++)
	{
		cout<<"最终找到的linky["<<i<<"]为"<<linky[i]<<endl;
		cout<<"它的权重是:"<<w[linky[i]][i]<<endl;
		// 如果有一个点的linky[]为-1,说明没有点和它匹配，不满足最优匹配的条件，输出"无解"
		// 如果一个点的linky[]和它相连以后的权重是负无穷，说明没办法形成一个环，不满足条件，输出无解。
		if (linky[i] == -1 || w[linky[i]][i] == -INF)
			return 1;
		// 注意这里链接的是[linky[i]][i]而不是[i][linky[i]]，因为这里要让权重最小，而不是最大
		else result += w[linky[i]][i];
	}
	return result;

}

int main() {
	freopen("/home/steven/sublime-text-imfix/test/input.txt", "r", stdin);
	int cas;
	scanf("%d", &cas);
	while (cas--)
	{
		scanf("%d%d", &n, &m);
		// 初始化所有节点的权重为负无穷
		for (int i = 1; i <= n; i++)
		{
			for (int j = 1; j <= n; j++)
				w[i][j] = -INF;
		}
		// 将边的信息写入邻接矩阵，注意这里写入的是相反数
		for (int i = 1; i <= m; i++)
		{
			int a, b, c;
			scanf("%d%d%d", &a, &b, &c);
			if (-c > w[a][b])
				w[a][b] = -c;
		}
		/*
		for (int i = 1; i <= n; i++)
		{
			for (int j = 1; j <= n; j++)
				if (w[i][j] != -INF)
					cout << "w[" << i << "][" << j << "]的权重为:" << w[i][j] << endl;
		}
		*/
		printf("%d\n", -KM());
	}
	fclose(stdin);//关闭文件
	return 0;
}