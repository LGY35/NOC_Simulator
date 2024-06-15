
// // #define BUFFER  10
// #include <assert.h>
// #include "Q2DTorus.h"

// /*****************************
// 定义了一个Q2DTorus类，它模拟了一个二维环形网格（Torus）网络结构。
// 每个节点都是Q2DTorusNode类型，网络中的每个节点都可以与其在网格中的相邻节点进行直接通信。
// ****************************/

// // Q2DTorus类的构造函数
// Q2DTorus::Q2DTorus(int n, int R1buffer, int R2buffer)
// {

// 	k = n; 						// 网络的边长  
// 	int t = k * k;				// 网络中节点的总数  
// 	head = new Q2DTorusNode[t];	// 创建节点数组
// 	assert(head);				// 断言，确保节点数组创建成功
// 	int x = 0, y = 0;			// 用于遍历节点的坐标
// 	for (x = 0; x < k; x++)
// 	{
// 		for (y = 0; y < k; y++)
// 		{

// 			int id = x + y * k;						 // 计算当前节点的线性索引
// 			(head + id)->setTorus(this);			 // 设置当前节点所属的Torus
// 			(head + id)->setCoordinate(id, x, y);	 // 设置当前节点的坐标
// 			(head + id)->setbuffer(R1buffer, R2buffer);	// 设置当前节点的缓冲区大小
// 		}
// 	}
// 	// 设置每个节点的邻接节点链接
// 	for (x = 0; x < k; x++)
// 	{
// 		for (y = 0; y < k; y++)
// 		{

// 			int id = x + y * k;
// 			int xneg, xpos, yneg, ypos;		// 存储相邻节点的索引
// 			// 注释中是torus网络结构，实现torus网络的同学可以借鉴
// 			/*if (x != 0)  xneg = (x - 1)  + y * k ;   // the node x negative direction link to
// 			else   xneg = (k - 1) + y * k ;
// 			if (x != k-1)  xpos = (x + 1)  + y * k ;
// 			else   xpos =  y * k ;

// 			if (y != 0)  yneg = x  + (y - 1) * k;  // the node y negative direction link to
// 			else   yneg = x  + (k - 1) * k;
// 			if (y != k-1)  ypos = x  + (y + 1) * k  ;
// 			else   ypos = x;*/

// 			// 设置该节点各个方向相邻的节点
// 			if (x != 0)			// 如果不是第一列，则向左的节点是前一个，否则不存在（-1）
// 				xneg = (x - 1) + y * k;
// 			else
// 				xneg = -1;
// 			if (x != k - 1)		// 如果不是最后一列，则向右的节点是后一个，否则不存在（-1）
// 				xpos = (x + 1) + y * k;
// 			else
// 				xpos = -1;

// 			if (y != 0)			// 如果不是第一行，则向上的节点是上一行的同列，否则不存在（-1）
// 				yneg = x + (y - 1) * k;
// 			else
// 				yneg = -1;
// 			if (y != k - 1)		// 如果不是最后一行，则向下的节点是下一行的同列，否则不存在（-1）
// 				ypos = x + (y + 1) * k;
// 			else
// 				ypos = -1;
// 			// 设置当前节点的各方向链接
// 			(head + id)->setLinkBuffer(xneg, xpos, yneg, ypos);
// 		}
// 	}
// }
// // 重载操作符[]，用于直接访问指定索引的节点
// Q2DTorusNode *Q2DTorus::operator[](int n)
// {
// 	return (head + n);
// }
// // 清空所有节点的缓冲区
// void Q2DTorus::clearAll()
// {
// 	for (int i = 0; i < k * k; i++)
// 	{
// 		(head + i)->clearBuffer();
// 	}
// }
