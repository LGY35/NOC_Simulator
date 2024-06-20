#include "Routing2.h"
#include <assert.h>

// 路由类中的转发函数，根据消息s决定其路由路径
NodeInfo *Routing2::forward(Message &s)
{

	int var1 = 0;
	int var2 = 0;
	int var = 0;
	next->node = -1;	// 初始化下一个节点标志为无效
	next->buff = NULL;	// 初始化下一个缓冲区指针为NULL

	// 调用xdimension函数和ydimension函数来尝试在两个维度上确定路由路径
	NodeInfo *temp1 = xdimension(s);
	NodeInfo *temp2 = ydimension(s);

	// 如果在x维或y维找到合适的路由节点，相应的var1或var2设为1
	if (temp1 != NULL)
		var1 = 1;
	if (temp2 != NULL)
		var2 = 1;
	int allbuff1 = 0;
	int allbuff2 = 0;
	// 计算var值，用于决定采用哪个维度的路由信息
	var = var1 * 2 + var2;

	if (var == 3)	// 如果两个维度都有可用的路由节点
	{
		allbuff1 = temp1->buff->r1 + temp1->buff->r2;// 计算x维度缓冲区的总大小
		allbuff2 = temp2->buff->r1 + temp2->buff->r2;// 计算y维度缓冲区的总大小
	}

	if (var == 2 || (var == 3 && allbuff1 >= allbuff2))
	{		 		// 如果只有y维度有效或者两个维度都有效但x维度缓冲区更大
		*next = *temp1;		// 选择x维度的路由
		next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
		next->buff->linkused = true; // 标记链路为已使用
	}

	else if (var == 1 || (var == 3 && allbuff1 < allbuff2))
	{				// 如果只有x维度有效或者两个维度都有效但y维度缓冲区更大
		*next = *temp2;		// 选择y维度的路由
		next->buff->bufferMin(next->channel, MESSLENGTH); // 更新缓冲区信息
		next->buff->linkused = true;	// 标记链路为已使用
	}

	return next;	 // 返回下一个节点的信息
}

/*

check wether next hop could be along x dimension.




  */
// 处理沿x维度的路由决策
NodeInfo *Routing2::xdimension(const Message &s)
{
	Q2DTorusNode *cur = (*torus)[s.routpath[0].node];	// 获取当前节点
	Q2DTorusNode *dst = (*torus)[s.dst];				// 获取目的节点

	int curx = cur->x;  // 当前节点的x坐标
    int cury = cur->y;  // 当前节点的y坐标
    int dstx = dst->x;  // 目的节点的x坐标
    int dsty = dst->y;  // 目的节点的y坐标

	int xdis = dstx - curx;  // x方向的距离差
    int ydis = dsty - cury;  // y方向的距离差
    bool special = false;  // 特殊情况标志

    Buffer *xlink[2] = {cur->bufferxneglink, cur->bufferxposlink};  // x方向的缓冲区链接

    int xlinknode[2] = {cur->linkxneg, cur->linkxpos};  // x方向的节点链接

    if (xdis == 0)
        return NULL;  // 如果x方向距离为0，则无需进一步路由

    // 处理包装链路，如果x方向的距离大于节点数的一半，则表示需要跨越环绕链接
    if (xdis > k / 2 || xdis < -k / 2) {
        Buffer *temp = xlink[0];
        xlink[0] = xlink[1];
        xlink[1] = temp;

        int temp1 = xlinknode[0];
        xlinknode[0] = xlinknode[1];
        xlinknode[1] = temp1;
    }

	if (xlink[xdis > 0]->linkused == true)// 如果所需链路已被使用，则不能前进
		return NULL;

	// 标记特殊情况，如跨越环绕链路
	if (xdis > k / 2 && curx == 0)
		special = true;
	if (xdis < -k / 2 && curx == k - 1)
		special = true;
	if ((xdis <= k / 2 && xdis >= -k / 2) && (ydis <= k / 2 && ydis >= -k / 2))
		special = true;

	if (xlink[xdis > 0]->r1 < MESSLENGTH && !special)// 如果不是特殊情况且没有足够的缓冲区，则返回NULL
		return NULL;

	if (xlink[xdis > 0]->r1 < MESSLENGTH && xlink[xdis > 0]->r2 < MESSLENGTH && special)
		return NULL;

	// 创建并返回节点信息，设置对应的缓冲区和通道
	xtemp->buff = xlink[xdis > 0];
	xtemp->node = xlinknode[xdis > 0];

	if (xlink[xdis > 0]->r1 >= MESSLENGTH)
		xtemp->channel = R1; // R1
	else
		xtemp->channel = R2; // R2

	return xtemp;
}

// 处理沿y维度的路由决策，逻辑与xdimension相似
NodeInfo *Routing2::ydimension(const Message &s)
{

	Q2DTorusNode *cur = (*torus)[s.routpath[0].node];
	Q2DTorusNode *dst = (*torus)[s.dst];

	int curx = cur->x;
	int cury = cur->y;
	int dstx = dst->x;
	int dsty = dst->y;

	int xdis = dstx - curx;
	int ydis = dsty - cury;
	bool special = false;

	Buffer *ylink[2] = {cur->bufferyneglink, cur->bufferyposlink};

	int ylinknode[2] = {cur->linkyneg, cur->linkypos};

	if (ydis == 0)
		return NULL;

	if (ydis > k / 2 || ydis < -k / 2)
	{
		Buffer *temp = ylink[0];
		ylink[0] = ylink[1];
		ylink[0] = temp;

		int temp1 = ylinknode[0];
		ylinknode[0] = ylinknode[1];
		ylinknode[1] = temp1;
	}

	if (ylink[ydis > 0]->linkused == true)
		return NULL;

	bool xwrap = (xdis <= k / 2 && xdis >= -k / 2);

	if (xwrap && ydis > k / 2 && cury == 0)
		special = true;
	if (xwrap && ydis < -k / 2 && cury == k - 1)
		special = true;
	if (xwrap && ydis <= k / 2 && ydis >= -k / 2)
	{
		if (flowalg == 1)
		{
			if (xdis == 0)
				special = true;
		}

		else
		{
			if (xdis >= 0)
				special = true;
		}
	}
	if (ylink[ydis > 0]->r1 < MESSLENGTH && !special)
		return NULL;

	if (ylink[ydis > 0]->r1 < MESSLENGTH && ylink[ydis > 0]->r2 < MESSLENGTH && special)
		return NULL;

	ytemp->buff = ylink[ydis > 0];
	ytemp->node = ylinknode[ydis > 0];
	if (ylink[ydis > 0]->r1 >= MESSLENGTH)
		ytemp->channel = R1; // R1
	else
		ytemp->channel = R2; // R2

	return ytemp;
}
