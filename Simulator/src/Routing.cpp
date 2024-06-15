#include "Routing.h"
#include "assert.h"

/*
路由算法的实现，功能是在一个3D Mesh网络中进行消息的路由。
代码包括对消息从一个节点到另一个节点的转发，检查和使用虚拟通道的能力，以及处理包裹在环链路上的路由。
它还管理缓冲区的使用，确保在有足够资源的情况下转发消息，以及在没有足够资源时正确地阻塞或丢弃消息。

*/

/*******

	if ALGORITHM == 1, west-first
	if ALGORITHM == 2, dor-routing


  ***********/
extern int ALGORITHM; // 定义外部变量ALGORITHM，指示所使用的路由算法

NodeInfo *Routing::forward(Message &s)
{

	return forward((*mesh)[s.routpath[0].node], (*mesh)[s.dst]);
} // 转发消息到目的地，这里使用mesh网络结构

int Routing::chkWrplnk(Q3DMeshNode *cur, Q3DMeshNode *dst)
{
	int curx = cur->x; // 当前节点x坐标
	int cury = cur->y; // 当前节点y坐标
	int curz = cur->z; // 当前节点z坐标

	int dstx = dst->x; // 目标节点x坐标
	int dsty = dst->y; // 目标节点Y坐标
	int dstz = dst->z; // 目标节点Z坐标

	int wraplink = 0; // 环链路数初始化为0

	if (((curx < dstx) && (dstx - curx) > (k / 2)) || ((curx > dstx) && (curx - dstx) > (k / 2)))
		wraplink++; // 检查x轴上是否需要环链路

	if (((cury < dsty) && (dsty - cury) > (k / 2)) || ((cury > dsty) && (cury - dsty) > (k / 2)))
		wraplink++; // 检查y轴上是否需要环链路

	if (((curz < dstz) && (dsty - curz) > (k / 2)) || ((curz > dstz) && (curz - dstz) > (k / 2)))
		wraplink++; // 检查y轴上是否需要环链路

	return wraplink;
} // 计算从当前节点到目标节点是否需要通过环链路

NodeInfo *Routing::forward(Q3DMeshNode *cur, Q3DMeshNode *dst)
{
	assert(cur && dst && (cur != dst)); // 确保当前节点和目标节点存在且不相同

	next->node = -1;
	next->buff = NULL;

	// int wraplink = chkWrplnk( cur,  dst);
	int wraplink = 0; // 实现的是mesh网络，所以没有环链路，wraplink为0

	if (wraplink == 0)
		return noWrapLinkrt(cur, dst); // 无环链路情况下的路由

	if (wraplink == 1)
		return oneWrapLinkrt(cur, dst); // 一个环链路情况下的路由

	if (wraplink == 2)
		return twoWrapLinkrt(cur, dst); // 两个环链路情况下的路由
}

/**********

		check whether buff1 has enough buffer,
		n	is the buffer number , if buff1 used, then record it:
		chn is R1 or R2 used,and record is the buffer(such as bufferxneg)

		 ***********/

bool Routing::checkBuffer(Buffer *buff1, int &chn, Buffer *&record)
{
	bool k = false;
	if (buff1->linkused == true)
	{
		return k;
	}

	if (chn == R1)
	{
		if (buff1->r1 >= MESSLENGTH)
		{
			k = true;
			buff1->linkused = true;
		}
	}
	/*else{
		if(  buff1->r1 >= MESSLENGTH){
			chn = R1;
			k = true;
			buff1->linkused = true;
		}
		else if(buff1->r2 >= MESSLENGTH){
			chn = R2;
			k = true;
			buff1->linkused = true;
		}
	}*/
	if (k)
	{
		record = buff1;
		buff1->bufferMin(chn, MESSLENGTH);
	}
	return k;
} // 检查缓冲区是否有足够空间，如果有，则标记为已使用

//  在两个缓冲区（Buffer）之间选择最适合的一个来进行数据传输。 它基于缓冲区的可用性和状态来决定哪个缓冲区应该被用于当前的消息传输。
int Routing::prefer(Buffer *buff1, Buffer *buff2, Buffer *buff3, int &chn1, int &chn2, int &chn3, Buffer *&record)
{
	// 检查是否有足够的缓存，链路是否可用
	// 首先检查每个缓冲区是否为空（即是否存在）。如果一个缓冲区存在而另一个不存在，函数将只检查存在的那个缓冲区。
	if (buff1 != NULL && buff2 == NULL && buff3 == NULL)	// 100
		// 使用 checkBuffer 函数来检查指定的缓冲区是否有足够的空间来处理当前的数据传输需求。
		// 如果有足够的空间，相应的通道标识（chn1 或 chn2）会被用来更新传输记录（record）。
		if (checkBuffer(buff1, chn1, record))	
			return 1;	// 1: 表示 buff1 被选择。
		else
			return 0;	// 0: 表示没有可用的缓冲区。
	// 如果两个缓冲区都存在，函数将尝试在两个缓冲区中找到一个可用的缓冲区。
	if (buff1 != NULL && buff2 != NULL && buff3 == NULL)	// 110
		if (checkBuffer(buff1, chn1, record))
			return 1;
		else
			return 0;

	if (buff1 != NULL && buff2 != NULL && buff3 != NULL)	// 111
		if (checkBuffer(buff1, chn1, record))
			return 1;
		else
			return 0;

	if (buff1 != NULL && buff2 == NULL && buff3 != NULL)	// 101
		if (checkBuffer(buff1, chn1, record))
			return 1;
		else
			return 0;

	
	if (buff1 == NULL && buff2 != NULL && buff3 != NULL)	// 011
		if (checkBuffer(buff2, chn2, record))
			return 2;	// 2: 表示 buff2 被选择。
		else
			return 0;

	if (buff1 == NULL && buff2 != NULL && buff3 == NULL)	// 010
		if (checkBuffer(buff2, chn2, record))
			return 2;	// 2: 表示 buff2 被选择。
		else
			return 0;
	
	if (buff1 == NULL && buff2 != NULL && buff3 != NULL)	// 001
		if (checkBuffer(buff3, chn3, record))
			return 3;	// 3: 表示 buff3 被选择。
		else
			return 0;
	
	if (buff1 == NULL && buff2 == NULL && buff3 == NULL)	// 000
		return 0;

	// 注释掉的代码：一个更复杂的逻辑，用于在两个缓冲区都可用时，根据通道的不同优先级（R1 vs R2）来决定优先使用哪个缓冲区。
	/*int bufferslc = 0;
	int var;

	if( chn1 == R1 && chn2 == R1) var = 1;
	if( chn1 == R1 && chn2 == R2) var = 2;

	if( chn1 == R2 && chn2 == R1) var = 3;
	if( chn1 == R2 && chn2 == R2) var = 4;

	switch (var){
	case 1: case 3: case 4:
		if(checkBuffer(buff1, chn1,record )){
						bufferslc = 1;
						return bufferslc;
		}

		if(checkBuffer(buff2, chn2,record )){
						bufferslc = 2;
						return bufferslc;
		}
		return bufferslc;

	case 2:

		if(checkBuffer(buff2, chn2,record )){
						bufferslc = 2;
						return bufferslc;
		}

		if(checkBuffer(buff1, chn1,record )){
						bufferslc = 1;
						return bufferslc;
		}
			return bufferslc;
	}*/
} // 基于缓存空间选择最优缓存路由

NodeInfo *Routing::noWrapLinkrt(Q3DMeshNode *cur, Q3DMeshNode *dst)
{										// routing without wraplink.
	assert(cur && dst && (cur != dst)); // 确保当前节点和目标节点存在且不相同
	int curx = cur->x;
	int cury = cur->y;
	int curz = cur->z;

	// R1 preferred, leave more space for escape channel R2.
	int dstx = dst->x;
	int dsty = dst->y;
	int dstz = dst->z;

	int xdis = dstx - curx; // x方向的偏移
	int ydis = dsty - cury; // y方向的偏移
	int zdis = dstz - curz; // y方向的偏移

	int bufferslc; // 0 no buffer available, 1: select  x direction buffer,2 y direction, 3 z direction
	int var1;
	int var2;
	int var3;

	int vchx; // virtual channel of x axis
	int vchy;
	int vchz;

	// 判断X方向
	if (xdis < 0)
		var1 = 0; // 负方向
	else if (xdis == 0)
		var1 = 1; // 没有移动
	else  // xdis > 0
		var1 = 2; // 正方向

	if (xdis == 0)	//只有x等于0的时候 y 才可以移动
	{
		if (ydis < 0)
			var2 = 0;
		else
		{
			if (ydis == 0)
				var2 = 1;
			else if (ydis > 0)
				var2 = 2;
		}
	}
	else
	{
		var2 = 1; // xy路由算法：x方向的偏移不为0时,不能走y方向
	}

	// TODO: west-first 里面 是去掉几个可能的转弯，所以这里是不是不只是这么简单的判断
	if(xdis == 0 && ydis == 0)
	{
		if (zdis < 0)
			var3 = 0;
		else if (zdis == 0)
			var3 = 1;
		else 
			var3 = 2;
	}
	else
	{
		var3 = 1; // xyz路由算法：x 和 y方向的偏移不全为0时,不能走z方向
	}

	Buffer *xlink[3] = {cur->bufferxneglink, NULL, cur->bufferxposlink};
	Buffer *ylink[3] = {cur->bufferyneglink, NULL, cur->bufferyposlink};
	Buffer *zlink[3] = {cur->bufferzneglink, NULL, cur->bufferzposlink};

	int xlinknode[3] = {cur->linkxneg, -1, cur->linkxpos};
	int ylinknode[3] = {cur->linkyneg, -1, cur->linkypos};
	int zlinknode[3] = {cur->linkzneg, -1, cur->linkzpos};

	int virtualch[3] = {R2, 0, R1};

	/**********************************************

		ALGORITHM == 2   dor ROUTING 路由算法的实现，这里实现的xyz

	  *****************************************************/

	if (ALGORITHM == 2)
	{
		/*if( var1 != 1) {
			vchx = R2;
			vchy = R1;

		}
		else{
			vchy = R2;
		}*/
		vchx = R1; // 无虚拟通道都使用r1
		vchy = R1;
		vchz = R1;

		bufferslc = prefer(xlink[var1], ylink[var2], zlink[var3], vchx, vchy, vchz, next->buff);
		switch (bufferslc)
		{
		case 0:
			next->node = -1;
			break;
		case 1:
			next->node = xlinknode[var1];
			next->channel = vchx;
			break;
		case 2:
			next->node = ylinknode[var2];
			next->channel = vchy;
			break;
		case 3:
			next->node = zlinknode[var3];
			next->channel = vchz;
			break;
		}
		return next;
	}
}

NodeInfo *Routing::oneWrapLinkrt(Q3DMeshNode *cur, Q3DMeshNode *dst)
{ // routing one wraplink.
	assert(cur && dst && (cur != dst));
	int curx = cur->x;
	int cury = cur->y;
	int curz = cur->z;

	// R1 preferred, leave more space for escape channel R2.
	int dstx = dst->x;
	int dsty = dst->y; 
	int dstz = dst->z; 

	int xdis = dstx - curx;
	int ydis = dsty - cury; 
	int zdis = dstz - curz; 

	int bufferslc; // 0 no buffer available, 1: select  x direction buffer,2 y direction, 3 z direction
	int var1;
	int var2;
	int var3;

	int vchx = R1; // virtual channel of x axis
	int vchy = R1;
	int vchz = R1;

	// 此处的路由算法与上面有区别。  
	if (xdis < 0)
		var1 = 0;
	else
	{
		if (xdis == 0)
			var1 = 1;
		else if (xdis > 0)
			var1 = 2;
	}

	if (ydis < 0)
		var2 = 0;
	else
	{
		if (ydis == 0)
			var2 = 1;
		else if (ydis > 0)
			var2 = 2;
	}

	if (zdis < 0)
		var3 = 0;
	else
	{
		if (zdis == 0)
			var3 = 1;
		else if (zdis > 0)
			var3 = 2;
	}

	Buffer *xlink[3] = {cur->bufferxneglink, NULL, cur->bufferxposlink};
	Buffer *ylink[3] = {cur->bufferyneglink, NULL, cur->bufferyposlink};
	Buffer *zlink[3] = {cur->bufferzneglink, NULL, cur->bufferzposlink};

	int xlinknode[3] = {cur->linkxneg, -1, cur->linkxpos};
	int ylinknode[3] = {cur->linkyneg, -1, cur->linkypos};
	int zlinknode[3] = {cur->linkzneg, -1, cur->linkzpos};

	if (xdis > k / 2 || xdis < -k / 2)
	{
		Buffer *temp = xlink[0];
		xlink[0] = xlink[2];
		xlink[2] = temp;

		int temp2 = xlinknode[0];
		xlinknode[0] = xlinknode[2];
		xlinknode[2] = temp2;
		if (xdis > k / 2 && cur->x == 0)
			vchx = R2;
		if (xdis < -k / 2 && cur->x == k - 1)
			vchx = R2;
	}

	if (ydis > k / 2 || ydis < -k / 2)
	{
		Buffer *temp = ylink[0];
		ylink[0] = ylink[2];
		ylink[2] = temp;

		int temp2 = ylinknode[0];
		ylinknode[0] = ylinknode[2];
		ylinknode[2] = temp2;
		if (ydis > k / 2 && cur->y == 0)
			vchy = R2;
		if (ydis < -k / 2 && cur->y == k - 1)
			vchy = R2;
	}

	if (zdis > k / 2 || zdis < -k / 2)
	{
		Buffer *temp = zlink[0];
		zlink[0] = zlink[2];
		zlink[2] = temp;

		int temp2 = zlinknode[0];
		zlinknode[0] = zlinknode[2];
		zlinknode[2] = temp2;
		if (zdis > k / 2 && cur->z == 0)
			vchz = R2;
		if (zdis < -k / 2 && cur->z == k - 1)
			vchz = R2;
	}

	bufferslc = prefer(xlink[var1], ylink[var2], ylink[var2], vchx, vchy, vchz, next->buff);
	switch (bufferslc)
	{
	case 0:
		next->node = -1;
		break;
	case 1:
		next->node = xlinknode[var1];
		next->channel = vchx;
		break;
	case 2:
		next->node = ylinknode[var2];
		next->channel = vchy;
		break;
	case 3:
		next->node = zlinknode[var3];
		next->channel = vchz;
		break;
	}
	return next;
}

NodeInfo *Routing::twoWrapLinkrt(Q3DMeshNode *cur, Q3DMeshNode *dst)
{
	assert(cur && dst && (cur != dst));
	int curx = cur->x;
	int cury = cur->y;
	int curz = cur->z;

	// R1 preferred, leave more space for escape channel R2.
	int dstx = dst->x;
	int dsty = dst->y;   
	int dstz = dst->z;   

	int xdis = dstx - curx;
	int ydis = dsty - cury;
	int zdis = dstz - curz;

	int bufferslc; // 0 no buffer available, 1: select  x direction buffer,2 y direction, 3  direction
	int var1;
	int var2;
	int var3;

	int vchx = R1; // virtual channel of x axis
	int vchy = R1;
	int vchz = R1;

	if (xdis < 0)
		var1 = 0;
	else
	{
		if (xdis == 0)
			var1 = 1;
		else if (xdis > 0)
			var1 = 2;
	}

	if (ydis < 0)
		var2 = 0;
	else
	{
		if (ydis == 0)
			var2 = 1;
		else if (ydis > 0)
			var2 = 2;
	}

	if (zdis < 0)
		var3 = 0;
	else
	{
		if (zdis == 0)
			var3 = 1;
		else if (zdis > 0)
			var3 = 2;
	}

	Buffer *xlink[3] = {cur->bufferxposlink, NULL, cur->bufferxneglink};
	Buffer *ylink[3] = {cur->bufferyposlink, NULL, cur->bufferyneglink};
	Buffer *zlink[3] = {cur->bufferzposlink, NULL, cur->bufferzneglink};

	int xlinknode[3] = {cur->linkxpos, -1, cur->linkxneg};
	int ylinknode[3] = {cur->linkypos, -1, cur->linkyneg};
	int zlinknode[3] = {cur->linkzpos, -1, cur->linkzneg};

	bufferslc = prefer(xlink[var1], ylink[var2], ylink[var2], vchx, vchy, vchz, next->buff);
	switch (bufferslc)
	{
	case 0:
		next->node = -1;
		break;
	case 1:
		next->node = xlinknode[var1];
		next->channel = vchx;
		break;
	case 2:
		next->node = ylinknode[var2];
		next->channel = vchy;
		break;
	case 3:
		next->node = zlink[var3];
		next->channel = vchz;
		break;
	}
	return next;
}
