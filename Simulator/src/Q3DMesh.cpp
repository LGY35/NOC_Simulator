
// #define BUFFER  10
#include <assert.h>
#include "Q3DMesh.h"

/*****************************
 * Q3DMesh定义了整个网络结构，实现的是mesh网络结构，设置每个节点的id，每一维的坐标，以及相邻的节点，缓存等。
 * 
定义了一个Q3DMesh类，它模拟了一个三维mesh网格网络结构。
每个节点都是Q3DMeshNode类型，网络中的每个节点都可以与其在网格中的相邻节点进行直接通信。
****************************/

// Q3DMesh类的构造函数
Q3DMesh::Q3DMesh(int n, int R1buffer, int R2buffer)
{

	k = n; 							// 网络的边长  4 4 4 
	int t = k * k * k;				// 网络中节点的总数  
	head = new Q3DMeshNode[t];		// 创建节点数组
	assert(head);					// 断言，确保节点数组创建成功
	int x = 0, y = 0, z = 0;		// 用于遍历节点的坐标
	for (x = 0; x < k; x++)
	{
		for (y = 0; y < k; y++)
		{
			for (z = 0; z < k; z++)
			{
				int id = x + y * k + z * k * k;			 	// 计算当前节点的线性索引
				(head + id)->setMesh(this);			 		// 设置当前节点所属的Mesh
				(head + id)->setCoordinate(id, x, y, z);	// 设置当前节点的坐标
				(head + id)->setbuffer(R1buffer, R2buffer);	// 设置当前节点的缓冲区大小
			}
		}
	}
	// 设置每个节点的邻接节点链接
	/********************
	 * 这里就是根据遍历xyz的每个点，然后计算他的线性id——即变量id，
	 * 并把这个线性id与MESH拓扑中的相邻的节点进行链接——相邻节点的线性id就是计算的：xneg xpos yneg ypos zneg zpos。
	 * *******************/
	for (x = 0; x < k; x++)
	{
		for (y = 0; y < k; y++)
		{
			for(z = 0; z < k; z++)
			{
				int id = x + y * k + z * k * k;  
				// 线性索引最大值为 3 + 3*4 + 3*4*4 = 3 + 12 + 48 = 63
				int xneg, xpos, yneg, ypos, zneg, zpos;		// 存储相邻节点的索引

				// 设置该节点各个方向相邻的节点
				// 设置x维度的链接
				if (x != 0)			// 如果不是第一列，则向左的节点是前一个，否则不存在（-1）
					xneg = (x - 1) + y * k + z * k * k;
				else
					xneg = -1;
				if (x != k - 1)		// 如果不是最后一列，则向右的节点是后一个，否则不存在（-1）
					xpos = (x + 1) + y * k + z * k * k;
				else
					xpos = -1;
				// 设置y维度的链接
				if (y != 0)			// 如果不是第一行，则向上的节点是上一行的同列，否则不存在（-1）
					yneg = x + (y - 1) * k + z * k * k;
				else
					yneg = -1;
				if (y != k - 1)		// 如果不是最后一行，则向下的节点是下一行的同列，否则不存在（-1）
					ypos = x + (y + 1) * k + z * k * k;
				else
					ypos = -1;
				// 设置z维度的链接
				if (z != 0)			// 如果不是第一层，则向前的节点是前一层的同位置
					zneg = x + y * k + (z - 1) * k * k;
				else				// 第一层的节点在z负向没有相邻节点
					zneg = -1; 		
				if (z != k - 1)		// 如果不是最后一层，则向后的节点是后一层的同位置
					zpos = x + y * k + (z + 1) * k * k;
				else				// 最后一层的节点在z正向没有相邻节点
					zpos = -1;

				// TODO: 未解之谜
				//最开始使用上面的代码报错，gdb发现索引超出边界，然后改成下面的 > < 进行边界判断，运行成功。
				// 但是马上改回上面的代码又可以正常工作了。
				// xneg = (x > 0) ? x - 1 + y * k + z * k * k : -1;
				// xpos = (x < k - 1) ? x + 1 + y * k + z * k * k : -1;
				// yneg = (y > 0) ? x + (y - 1) * k + z * k * k : -1;
				// ypos = (y < k - 1) ? x + (y + 1) * k + z * k * k : -1;
				// zneg = (z > 0) ? x + y * k + (z - 1) * k * k : -1;
				// zpos = (z < k - 1) ? x + y * k + (z + 1) * k * k : -1;

				// 设置当前节点的各方向链接
				(head + id)->setLinkBuffer(xneg, xpos, yneg, ypos, zneg, zpos);
			}
		}
	}
}
// 重载操作符[]，用于直接访问指定索引的节点
Q3DMeshNode *Q3DMesh::operator[](int n)
{
	return (head + n);
}
// 清空所有节点的缓冲区
void Q3DMesh::clearAll()
{
	for (int i = 0; i < k * k * k; i++)
	{
		(head + i)->clearBuffer();
	}
}
