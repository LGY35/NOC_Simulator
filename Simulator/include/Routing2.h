/*********
Luo Wei ,12/5/2009

earthquake anniversary
clue with flow control

  *************/

#ifndef RT_2_H
#define RT_2_H

//这段代码定义了一个名为 `Routing2` 的路由类，该类继承自 `Allrouting` 类，专门用于处理三维网格 (`Q3DMesh`) 中的消息路由。
//类的成员包括三个用于计算下一跳节点的临时节点信息（`xtemp`, `ytemp`, `ztemp`），以及实现了多维度路由计算的方法。

#include "Q3DMeshNode.h"
#include "Message.h"
#include "Allrouting.h"
class Q3DMesh;
class Message;
extern int flowalg; // 流控算法的全局变量

class Routing2 : public Allrouting
{
public:
    // 构造函数，初始化路由信息和相关节点
    Routing2(Q3DMesh *mesh1)
    {
        mesh = mesh1;                 // 网格对象引用
        k = mesh1->getNode();         // 获取网格节点总数
        next = new NodeInfo();        // 下一个路由节点信息
        xtemp = new NodeInfo();       // 临时节点信息，用于X维度计算
        ytemp = new NodeInfo();       // 临时节点信息，用于Y维度计算
        ztemp = new NodeInfo(); // 临时节点信息，用于Z维度计算
	}
	NodeInfo *xtemp; // 用于处理X维度的节点信息
	NodeInfo *ytemp; // 用于处理Y维度的节点信息
	NodeInfo *ztemp; // 用于处理Z维度的节点信息

	// 根据消息返回下一个节点信息
	NodeInfo *forward(Message &);          // 返回消息将要到达的下一个节点
	// 检查消息在X维度上的下一跳是否可行
	NodeInfo *xdimension(const Message &); // 检查是否可以沿X维度进行下一步跳转
	// 检查消息在Y维度上的下一跳是否可行
	NodeInfo *ydimension(const Message &); // 检查是否可以沿Y维度进行下一步跳转
	// 检查消息在Z维度上的下一跳是否可行
	NodeInfo *zdimension(const Message &); // 检查是否可以沿Z维度进行下一步跳转
};
#endif



// #ifndef RT_2_H
// #define RT_2_H

// #include "Q2DTorusNode.h"
// #include "Message.h"
// #include "Allrouting.h"
// class Q2DTorus;
// class Message;
// extern int flowalg; // flow control algorithm

// class Routing2 : public Allrouting
// {

// public:
// 	Routing2(Q2DTorus *torus1)
// 	{
// 		torus = torus1;
// 		k = torus1->getNode();
// 		next = new NodeInfo();
// 		xtemp = new NodeInfo();
// 		ytemp = new NodeInfo();
// 	}
// 	NodeInfo *xtemp;
// 	NodeInfo *ytemp;

// 	NodeInfo *forward(Message &);		   // return the nextnode of the message takes;
// 	NodeInfo *xdimension(const Message &); // check wether next hop could be along x dimension.
// 	NodeInfo *ydimension(const Message &); // check wether next hop could be along y dimension.
// };

// #endif