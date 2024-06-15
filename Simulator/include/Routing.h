/*********
	Luo Wei ,9/11/2007



  *************/

#ifndef ROUT_ING
#define ROUT_ING

#include "Allrouting.h"
#include "Message.h"
#include "Q3DMeshNode.h"
class Q3DMesh;

class Buffer;

// Routing类是从Allrouting类派生的
class Routing : public Allrouting
{
public:
    // 构造函数，接受一个三维网格的指针
    Routing(Q3DMesh *mesh1)
    {
        mesh = mesh1; // 存储网格指针
        k = mesh1->getNode(); // 获取网格中节点的数量
        next = new NodeInfo(); // 初始化NodeInfo对象
    }

    int chkWrplnk(Q3DMeshNode *cur, Q3DMeshNode *dst); // 检查从当前节点到目标节点需要经过多少个环链路(wrap links)。
    bool checkBuffer(Buffer *buff1, int &chn, Buffer *&record); // 检查缓存区是否足够，记录缓存的使用情况。

    NodeInfo *forward(Message &); // 根据消息推导出下一个节点。
    NodeInfo *forward(Q3DMeshNode *cur, Q3DMeshNode *dst); // 根据当前和目标节点计算下一步节点。
    NodeInfo *noWrapLinkrt(Q3DMeshNode *cur, Q3DMeshNode *dst); // 不需要环链路的路由计算。
    NodeInfo *oneWrapLinkrt(Q3DMeshNode *cur, Q3DMeshNode *dst); // 需要一个环链路的路由计算。
    NodeInfo *twoWrapLinkrt(Q3DMeshNode *cur, Q3DMeshNode *dst); // 需要两个环链路的路由计算。

    int prefer(Buffer *buff1, Buffer *buff2, Buffer *buff3, int &chn1, int &chn2, int &chn3, Buffer *&buff); // 根据缓存区优先级决定消息的路由。
};

#endif



// #ifndef ROUT_ING
// #define ROUT_ING

// #include "Allrouting.h"
// #include "Message.h"
// #include "Q2DTorusNode.h"
// class Q2DTorus;

// class Buffer;

// class Routing : public Allrouting
// {

// public:
// 	Routing(Q2DTorus *torus1)
// 	{
// 		torus = torus1;
// 		k = torus1->getNode();
// 		next = new NodeInfo();
// 	}

// 	int chkWrplnk(Q2DTorusNode *cur, Q2DTorusNode *dst); // check how many wrap links need from cur to dst.
// 	bool checkBuffer(Buffer *buff1, int &chn, Buffer *&record);

// 	NodeInfo *forward(Message &);
// 	NodeInfo *forward(Q2DTorusNode *cur, Q2DTorusNode *dst);	   // return the nextnode of cur.
// 	NodeInfo *noWrapLinkrt(Q2DTorusNode *cur, Q2DTorusNode *dst);  // from cur to do not need wraplink.
// 	NodeInfo *oneWrapLinkrt(Q2DTorusNode *cur, Q2DTorusNode *dst); // from cur to  need  one wraplink.
// 	NodeInfo *twoWrapLinkrt(Q2DTorusNode *cur, Q2DTorusNode *dst); // from cur to  need  two wraplink.

// 	int prefer(Buffer *buff1, Buffer *buff2, int &chn1, int &chn2, Buffer *&buff);
// };

// #endif