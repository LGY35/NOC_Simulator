#include "Routing2.h"
#include <assert.h>

// 转发函数，根据消息s决定其路由路径
/*
这段代码实现的是一个多维路由选择算法，用于在一个3D Mesh网络中决定数据包（消息）的传输路径。
是一种自适应路由算法，因为它考虑了不同方向上缓冲区的使用情况来动态选择最佳的传输路径。
这种算法尤其适用于多维网络结构，如3D Mesh，能有效减少拥塞并平衡负载。

选择最大缓冲区的含义：
在多维网络（如3D Mesh）中，每个方向（x, y, z）都可能有独立的缓冲区。
*next = *temp1; 这行代码实际上是将 temp1（表示在x方向上的下一个节点及其相关信息）的内容赋给 next，这意味着路由算法决定沿x方向发送消息。
当决定使用某个维度时，通常考虑的是该方向上缓冲区的可用性。这里的 "x维度缓冲区最大" 指的是与其他可用维度相比，x方向的缓冲区（合并R1和R2缓冲区）拥有最大的剩余容量。
*/
NodeInfo *Routing2::forward(Message &s)
{

	int var1 = 0;
	int var2 = 0;
	int var3 = 0;
	int var = 0;
	next->node = -1;	// 初始化下一个节点标志为无效
	next->buff = NULL;	// 初始化下一个缓冲区指针为NULL

	// 调用xdimension函数和ydimension函数和zdimension函数来尝试在两个维度上确定路由路径
	NodeInfo *temp1 = xdimension(s);	//临时节点
	NodeInfo *temp2 = ydimension(s);
	NodeInfo *temp3 = zdimension(s);

	// 如果在x维或y维或z维找到合适的路由节点，相应的var1或var2或var3设为1
	if (temp1 != NULL)
		var1 = 1;
	if (temp2 != NULL)
		var2 = 1;
	if (temp3 != NULL)
		var3 = 1;
	// 计算缓冲区大小，决定采用哪个维度的路由信息
	int allbuff1 = 0;
	int allbuff2 = 0;
	int allbuff3 = 0;
	// 计算var值，用于决定采用哪个维度的路由信息         原来是2D的，所以为 0：表示没有维度是可用的。1：表示只有第二个维度（y维）是可用的。2：表示只有第一个维度（x维）是可用的。3：表示两个维度都是可用的。
	//这里是采用二进制编码，改成3位就变成 a*4 + b*2 + c
	var = var1 * 4 + var2 * 2 + var3;
	/*********** 编码 ****************
		0：没有维度是可用的。
		1：只有 z 维度是可用的。
		2：只有 y 维度是可用的。
		3：y 和 z 维度都是可用的。
		4：只有 x 维度是可用的。
		5：x 和 z 维度都是可用的。
		6：x 和 y 维度都是可用的。
		7：x、y 和 z 维度都是可用的。
	*********************************/
	switch (var)
	{
	case 7:	// 所有维度都有可用的路由节点
		allbuff1 = temp1->buff->r1 + temp1->buff->r2;// 计算x维度缓冲区的总大小
		allbuff2 = temp2->buff->r1 + temp2->buff->r2;// 计算y维度缓冲区的总大小
		allbuff3 = temp3->buff->r1 + temp3->buff->r2;// 计算z维度缓冲区的总大小
		// 三个维度都可用的情况下，通过buffer大小来判断
		if (allbuff1 >= allbuff2 && allbuff1 >= allbuff3) 			//如果x维度缓冲区最大
		{		
			*next = *temp1;		// 将 temp1（表示在x方向上的下一个节点及其相关信息）的内容赋给 next，这意味着路由算法决定沿x方向发送消息。
			next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
			next->buff->linkused = true; // 标记链路为已使用
		} 
		else if (allbuff2 >= allbuff3) 		//如果y维度最大
		{	
			*next = *temp2;
			next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
			next->buff->linkused = true; // 标记链路为已使用
		} 
		else 														//如果z维度最大			
		{											
			*next = *temp3;
			next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
			next->buff->linkused = true; // 标记链路为已使用
		}
		break;
	case 6: // x 和 y 可用
            if (temp1->buff->r1 + temp1->buff->r2 >= temp2->buff->r1 + temp2->buff->r2) 
            {
				*next = *temp1;
				next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
				next->buff->linkused = true; // 标记链路为已使用
			}
			else 
            {
				*next = *temp2;
				next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
				next->buff->linkused = true; // 标记链路为已使用
			}
            break;
	case 5: // x 和 z 可用
            if (temp1->buff->r1 + temp1->buff->r2 >= temp3->buff->r1 + temp3->buff->r2) 
            {
				*next = *temp1;
				next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
				next->buff->linkused = true; // 标记链路为已使用
			}
			else 
            {
				*next = *temp3;
				next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
				next->buff->linkused = true; // 标记链路为已使用
			}
            break;
	case 3: // y 和 z 可用
            if (temp2->buff->r1 + temp2->buff->r2 >= temp3->buff->r1 + temp3->buff->r2) 
            {
				*next = *temp2;
				next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
				next->buff->linkused = true; // 标记链路为已使用
			}
            else
            {
				*next = *temp3;
				next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
				next->buff->linkused = true; // 标记链路为已使用
			}
            break;
	case 4: // 只有 x 可用
            *next = *temp1;
			next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
			next->buff->linkused = true; // 标记链路为已使用
            break;
    case 2: // 只有 y 可用
            *next = *temp2;
			next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息·
			next->buff->linkused = true; // 标记链路为已使用
            break;
    case 1: // 只有 z 可用
            *next = *temp3;
			next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
			next->buff->linkused = true; // 标记链路为已使用
            break;
	default: // 没有可用的
            next->node = -1;
            break;
	}

// ----------------------- 注释：原来2D mesh的判断 -----------------------------------------
	// if (var == 7)	// 如果两个维度都有可用的路由节点
	// {
	// 	allbuff1 = temp1->buff->r1 + temp1->buff->r2;// 计算x维度缓冲区的总大小
	// 	allbuff2 = temp2->buff->r1 + temp2->buff->r2;// 计算y维度缓冲区的总大小
	// 	allbuff2 = temp3->buff->r1 + temp3->buff->r2;// 计算z维度缓冲区的总大小
	// }

	// if (var == 2 || (var == 3 && allbuff1 >= allbuff2))
	// {		 		// 如果只有y维度有效或者两个维度都有效但x维度缓冲区更大
	// 	*next = *temp1;		// 选择x维度的路由
	// 	next->buff->bufferMin(next->channel, MESSLENGTH);// 更新缓冲区信息
	// 	next->buff->linkused = true; // 标记链路为已使用
	// }

	// else if (var == 1 || (var == 3 && allbuff1 < allbuff2))
	// {				// 如果只有x维度有效或者两个维度都有效但y维度缓冲区更大
	// 	*next = *temp2;		// 选择y维度的路由
	// 	next->buff->bufferMin(next->channel, MESSLENGTH); // 更新缓冲区信息
	// 	next->buff->linkused = true;	// 标记链路为已使用
	// }

	return next;	 // 返回下一个节点的信息
}

/*

check wether next hop could be along x dimension.
当距离超过半个网格大小时，代码尝试通过交换链路和节点索引来处理环绕链路，这适用于环状（torus）网格。
而没有环绕链路的3D Mesh中，因为没有环绕链路，所以不需要判断环链路。//TODO: 环链路的3D torus学习
  */

// 处理沿x维度的路由决策
NodeInfo *Routing2::xdimension(const Message &s)
{
	Q3DMeshNode *cur = (*mesh)[s.routpath[0].node];	// 获取当前节点
	Q3DMeshNode *dst = (*mesh)[s.dst];				// 获取目的节点

	int curx = cur->x;  // 当前节点的x坐标
    int cury = cur->y;  // 当前节点的y坐标
    int curz = cur->z;  // 当前节点的y坐标
    int dstx = dst->x;  // 目的节点的x坐标
    int dsty = dst->y;  // 目的节点的y坐标
    int dstz = dst->z;  // 目的节点的y坐标

	int xdis = dstx - curx;  // x方向的距离差
    int ydis = dsty - cury;  // y方向的距离差
    int zdis = dstz - curz;  // y方向的距离差
    bool special = false;  // 特殊情况标志

    Buffer *xlink[2] = {cur->bufferxneglink, cur->bufferxposlink};  // x方向的缓冲区链接

    int xlinknode[2] = {cur->linkxneg, cur->linkxpos};  // x方向的节点链接

    if (xdis == 0)
        return NULL;  // 如果x方向距离为0，则无需进一步路由

	// 不需要环链路，所以删除	
    // // 处理包装链路，如果x方向的距离大于节点数的一半，则表示需要跨越环绕链接
    // if (xdis > k / 2 || xdis < -k / 2) {
    //     Buffer *temp = xlink[0];
    //     xlink[0] = xlink[1];
    //     xlink[1] = temp;

    //     int temp1 = xlinknode[0];
    //     xlinknode[0] = xlinknode[1];
    //     xlinknode[1] = temp1;
    // }

	if (xlink[xdis > 0]->linkused == true)// 如果所需链路已被使用，则不能前进
		return NULL;

	// // 标记特殊情况，如跨越环绕链路
	// if (xdis > k / 2 && curx == 0)	//当目标节点在x方向上相对于当前节点的距离大于网格尺寸的一半，并且当前节点的x坐标为0时，意味着消息需要从网格的一边跨越到另一边，使用环绕链接。
	// 	special = true;
	// if (xdis < -k / 2 && curx == k - 1)	//当目标节点在x方向上相对于当前节点的距离小于负网格尺寸的一半（即目标在当前节点左侧超过半网格距离），并且当前节点的x坐标为网格的最大值（k - 1，假设网格从0到k-1编号），则同样表示消息需要使用环绕链接跨越到网格的另一侧。
	// 	special = true;
	// if ((xdis <= k / 2 && xdis >= -k / 2) && (ydis <= k / 2 && ydis >= -k / 2) && (zdis <= k / 2 && zdis >= -k / 2))	//这行代码检查目标节点是否在当前节点的一个合理的非环绕距离内，即目标节点在x和y方向上的距离都不超过网格尺寸的一半。这种情况下，消息可以直接通过常规链接传递，无需使用环绕链接。
	// 	special = false;		// TODO: 这里原代码是不是写错了，应该为 false	回答：是的，这里写错了。

	if (xlink[xdis > 0]->r1 < MESSLENGTH && !special)// 如果不是特殊情况且没有足够的缓冲区，则返回NULL
		return NULL;

	// if (xlink[xdis > 0]->r1 < MESSLENGTH && xlink[xdis > 0]->r2 < MESSLENGTH && special)
	// 	return NULL;

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

	Q3DMeshNode *cur = (*mesh)[s.routpath[0].node];
	Q3DMeshNode *dst = (*mesh)[s.dst];

	int curx = cur->x;  
    int cury = cur->y;  
    int curz = cur->z;  
    int dstx = dst->x;  
    int dsty = dst->y;  
    int dstz = dst->z;  

	int xdis = dstx - curx;  // x方向的距离差
    int ydis = dsty - cury;  // y方向的距离差
    int zdis = dstz - curz;  // y方向的距离差
	bool special = false;

	Buffer *ylink[2] = {cur->bufferyneglink, cur->bufferyposlink};

	int ylinknode[2] = {cur->linkyneg, cur->linkypos};

	if (ydis == 0)
		return NULL;

	// if (ydis > k / 2 || ydis < -k / 2)
	// {
	// 	Buffer *temp = ylink[0];
	// 	ylink[0] = ylink[1];
	// 	ylink[0] = temp;

	// 	int temp1 = ylinknode[0];
	// 	ylinknode[0] = ylinknode[1];
	// 	ylinknode[1] = temp1;
	// }

	if (ylink[ydis > 0]->linkused == true)
		return NULL;

	// bool xwrap = (xdis <= k / 2 && xdis >= -k / 2);	// 当目标节点相对于当前节点在x方向的距离不超过网格尺寸的一半时为true，表示x方向不需要环绕
	// //如果xwrap为true（即在x方向不需要环绕），则检查y方向的距离：
	// if (xwrap && ydis > k / 2 && cury == 0)
	// 	special = true;	// 如果目标节点在y方向正向超出一半网格尺寸，并且当前节点的y坐标为0（即在网格的边界），则可以考虑环绕链路。
	// if (xwrap && ydis < -k / 2 && cury == k - 1)
	// 	special = true;	// 如果目标节点在y方向负向超出一半网格尺寸，并且当前节点的y坐标在网格的最底端（即另一个边界），也可以考虑环绕链路。
	// if (xwrap && ydis <= k / 2 && ydis >= -k / 2)
	// {	// 如果ydis在网格的一半内，且x方向也不需要环绕，特殊条件会根据flowalg的值进一步决定：
	// 	if (flowalg == 1)
	// 	{	// 当flowalg == 1时，只有当xdis == 0（即完全没有x方向移动）时，才将special设置为true。
	// 		if (xdis == 0)
	// 			special = true;
	// 	}

	// 	else	// 否则flowalg == 0，如果xdis >= 0（x方向为正或无移动），也将special设置为true。
	// 	{
	// 		if (xdis >= 0)
	// 			special = true;
	// 	}
	// }

	if (ylink[ydis > 0]->r1 < MESSLENGTH && !special)
		return NULL;
	// if (ylink[ydis > 0]->r1 < MESSLENGTH && ylink[ydis > 0]->r2 < MESSLENGTH && special)
	// 	return NULL;

	ytemp->buff = ylink[ydis > 0];
	ytemp->node = ylinknode[ydis > 0];
	if (ylink[ydis > 0]->r1 >= MESSLENGTH)
		ytemp->channel = R1; // R1
	else
		ytemp->channel = R2; // R2

	return ytemp;
}

// 处理沿z维度的路由决策
NodeInfo *Routing2::zdimension(const Message &s)
{
	Q3DMeshNode *cur = (*mesh)[s.routpath[0].node];	// 获取当前节点
	Q3DMeshNode *dst = (*mesh)[s.dst];				// 获取目的节点

	int curx = cur->x;  
    int cury = cur->y;  
    int curz = cur->z;  
    int dstx = dst->x;  
    int dsty = dst->y;  
    int dstz = dst->z;  

	int xdis = dstx - curx;  // x方向的距离差
    int ydis = dsty - cury;  // y方向的距离差
    int zdis = dstz - curz;  // z方向的距离差
    bool special = false;  // 特殊情况标志

    Buffer *zlink[2] = {cur->bufferzneglink, cur->bufferzposlink};  // z方向的缓冲区链接

    int zlinknode[2] = {cur->linkzneg, cur->linkzpos};  // z方向的节点链接

    if (zdis == 0)
        return NULL;  // 如果z方向距离为0，则无需进一步路由

	if (zlink[zdis > 0]->linkused == true)// 如果所需链路已被使用，则不能前进
		return NULL;

	if (zlink[zdis > 0]->r1 < MESSLENGTH && !special)// 如果不是特殊情况且没有足够的缓冲区，则返回NULL
		return NULL;
	// if (zlink[zdis > 0]->r1 < MESSLENGTH && zlink[zdis > 0]->r2 < MESSLENGTH && special)
	// 	return NULL;

	// 创建并返回节点信息，设置对应的缓冲区和通道
	ztemp->buff = zlink[zdis > 0];
	ztemp->node = zlinknode[zdis > 0];

	if (zlink[zdis > 0]->r1 >= MESSLENGTH)
		ztemp->channel = R1; // R1
	else
		ztemp->channel = R2; // R2

	return ztemp;
}

/*
bool xwrap = (xdis <= k / 2 && xdis >= -k / 2);
	bool ywrap = (ydis <= k / 2 && ydis >= -k / 2);
	
	//TODO: 
	// 标记特殊情况，如跨越环绕链路
	if (xwrap && ydis > k / 2 && cury == 0)
		special = true;	// 如果目标节点在y方向正向超出一半网格尺寸，并且当前节点的y坐标为0（即在网格的边界），则可以考虑环绕链路。
	if (xwrap && ydis < -k / 2 && cury == k - 1)
		special = true;	// 如果目标节点在y方向负向超出一半网格尺寸，并且当前节点的y坐标在网格的最底端（即另一个边界），也可以考虑环绕链路。
	if (xwrap && ydis <= k / 2 && ydis >= -k / 2)
	{	// 如果ydis在网格的一半内，且x方向也不需要环绕，特殊条件会根据flowalg的值进一步决定：
		if (flowalg == 1)
		{	// 当flowalg == 1时，只有当xdis == 0（即完全没有x方向移动）时，才将special设置为true。
			if (xdis == 0)
				special = true;
		}

		else	// 否则flowalg == 0，如果xdis >= 0（x方向为正或无移动），也将special设置为true。
		{
			if (xdis >= 0)
				special = true;
		}
	}
*/