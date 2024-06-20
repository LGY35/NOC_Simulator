#include "Event.h"

#include <math.h>
#include "Message.h"

extern int GENERATETYPE;

Event::Event(Allrouting *rout1)
{
	consumed = 0;
	totalcir = 0;
	messarrive = 0;
	rout = rout1;
	mes = rout1->mesh;
	k = rout1->k;
}


// 根据外部变量 GENERATETYPE 的值生成不同类型的消息：
/**********************
GENERATETYPE == 1：生成具有随机源和目的地的消息，源和目的地在3D Mesh网络中随机选择，确保源和目的地不同。
GENERATETYPE == 2 和 3：这两种类型看起来是从处理2D Torus网络迁移过来的代码片段。它们在计算目标位置时使用特定的几何关系，但因为使用了 torus 对象（在这段代码中并未定义），这部分代码在3D Mesh设置中可能不适用或需要重写。
GENERATETYPE == 4：混合了随机生成和特定模式生成消息的方法，一部分消息根据一定的概率在特定位置生成，另一部分则完全随机。

3.	Event中的genMes()函数用于产生消息，实现了不同流量模式下产生消息的情况；forWardMes()函数实现flit如何在节点之间向前移动，这个函数有点复杂，但不需要你们修改它。
大概讲一下，每个周期，

如果flit是header flit会根据路由算法选择的下一跳节点，占据当前节点和下一跳节点之间的链路，占据下一跳节点的buffer。
如果flit是body flit会跟着前面的flit向前移动一步。
如果是tail flit，会向前移动一步并释放buffer，之后统一释放链路。

代码150-172表示当header flit没有到达目的节点时，并且header flit发生阻塞的情况，其后的flit会向前移动一步，到每个flit的前一个flit所在的位置。
代码173-194表示当header flit没有到达目的节点时，并且header flit没有发生阻塞的情况，所有flit都向前移动一步，包括header flit。
代码198-223表示当header flit到达目的节点。
 * *******************************/
Message *Event::genMes()
{ // generate a message
	Q3DMesh *mesh = NULL;
	if (rout != NULL)
		mesh = rout->mesh;
	// uniform流量模式
	if (GENERATETYPE == 1)
	{
		int tempRand;
		int src, dest;
		tempRand = abs(rand()) % (k * k * k);
		src = tempRand;
		while (1)
		{
			tempRand = abs(rand()) % (k * k * k);
			if (tempRand != src)
			{
				dest = tempRand;
				break;
			}
		}
		return new Message(src, dest);
	}
	if (GENERATETYPE == 2)
	{
		int tempRand;
		int src, dest;
		int x;
		int y;
		int z;
		int destX, destY, destZ;

		// 希望源和目的地不在同一个XY平面、XZ平面或YZ平面上：
		while (1)
		{
			tempRand = abs(rand()) % (k * k * k);
			x = (*mesh)[tempRand]->x;
			y = (*mesh)[tempRand]->y;
			z = (*mesh)[tempRand]->z;
			
			// 避免源和目的地在同一个XY, XZ, YZ平面
            do {
                dest = abs(rand()) % (k * k * k);
                destX = dest % k;
                destY = (dest / k) % k;
                destZ = dest / (k * k);
            } while ((x == destX && y == destY) || (x == destX && z == destZ) || (y == destY && z == destZ));
			// if (x != (k - 1 - y) || y != (k - 1 - x) )
			// 	break;
		}
		src = tempRand;
        dest = destX + destY * k + destZ * k * k;	//现在我们是增加变量，来得到一个源和目的地不在同一个XY平面、XZ平面或YZ平面上的实现。
		// dest = (k - 1 - y) + (k - 1 - x) * k;	//原来这个是直接交换x和y乘以k的位置，从而设计一个排除对角线位置的x和y的2D坐标。
		// src = tempRand;

		return new Message(src, dest);
	}

	//实现在3D Mesh网络中生成消息，其中源节点和目的地节点不在同一个 z 坐标平面上：
	if (GENERATETYPE == 3)
	{
		int tempRand;
        int srcIndex, destIndex;
        int x, y, z;
        int destX, destY, destZ;

		// while (1)
		// {
		// 	// 生成一个随机的源节点索引
        //     tempRand = abs(rand()) % (k * k * k);
        //     x = tempRand % k;
        //     y = (tempRand / k) % k;
        //     z = tempRand / (k * k);

		// 	if (x != y)
		// 		break;
		// }
		// dest = y + x * k;
		// src = tempRand;

		while (true)
        {
            // 生成一个随机的源节点索引
            tempRand = abs(rand()) % (k * k * k);
            x = tempRand % k;
            y = (tempRand / k) % k;
            z = tempRand / (k * k);
			// x = (*mesh)[tempRand]->x;
			// y = (*mesh)[tempRand]->y;
			// z = (*mesh)[tempRand]->z;

            // 确保目的地节点不与源节点在同一z坐标平面
            do {
                destIndex = abs(rand()) % (k * k * k);
                destX = destIndex % k;
                destY = (destIndex / k) % k;
                destZ = destIndex / (k * k);
            } while (z == destZ);

            srcIndex = tempRand;
            destIndex = destX + destY * k + destZ * k * k;

            break;
        }
		return new Message(srcIndex, destIndex);
	}

	// 用于在大多数情况下随机选择源和目的地，但在某些特定情况下（10%的几率），将目的地设置为网格的特定角落位置。
	if (GENERATETYPE == 4)
    {
        int tempRand;
        int srcIndex, destIndex;
        int x, y, z;

        tempRand = abs(rand()) % 10;
        if (tempRand != 0)
        {
            // 通常情况下，随机生成源和目的地
            tempRand = abs(rand()) % (k * k * k);
            srcIndex = tempRand;
            while (1)
            {
                tempRand = abs(rand()) % (k * k * k);
                if (tempRand != srcIndex)
                {
                    destIndex = tempRand;
                    break;
                }
            }
        }
        else
        {
            // 10%的几率，选择特定的目的地位置
            int temprand2 = abs(rand()) % 2;
            switch (temprand2)
            {
            case 0:
                destIndex = (int)((k - 1 - k / 4) + (k - 1 - k / 4) * k + (k - 1 - k / 4) * k * k); // Corner far from the origin
                break;
            case 1:
                destIndex = (int)(k / 4 + k / 4 * k + k / 4 * k * k); // Corner near to the origin
                break;
            }
            while (1)
            {
                tempRand = abs(rand()) % (k * k * k);
                if (tempRand != destIndex)
                {
                    srcIndex = tempRand;
                    break;
                }
            }
        }

        return new Message(srcIndex, destIndex);
    }
}

/********************** 
此函数负责处理消息的转发逻辑。它包括多个步骤：

消息的延迟管理：首先检查消息是否正在传输过程中（begintrans），并据此管理计数器。
消息路由：调用 rout->forward(s) 获取下一个应转发到的节点信息。
消息阻塞处理：如果返回的下一节点为 -1，表示当前没有可用路径，此时增加消息的超时计数。如果消息在路由路径的初始位置，会进行特殊处理，如尝试重新获取下一跳节点。
消息传输：如果消息成功获得下一节点信息，会更新消息路径并处理相关的缓冲区和链路状态。
消息到达处理：如果消息到达目标节点，会更新统计信息并处理消息结束逻辑。
******************************/
void Event::forwardMes(Message &s)
{
	if (s.begintrans <= 0)
		s.count++;
	if (s.routpath[0].node == s.src)
	{
		if (s.begintrans > 0)
			s.begintrans--; // check whether process time
		else
		{
			s.begintrans--;
			NodeInfo *next;
			assert(rout != NULL);
			next = rout->forward(s);		//TODO: 这里是在allrouting类中实现的路由策略。
			if (next->node == -1)
			{
				s.timeout++;
				//						if(s.timeout == TIMEOUT) consumeMes(s);
			}
			else
			{
				s.timeout = 0;
				assert(s.routpath[0].node != next->node);
				s.routpath[0] = *next;
			}
		}
	}
	else
	{
		if (s.routpath[0].node != s.dst)
		{
			NodeInfo *next = NULL;
			if (rout != NULL)
				next = rout->forward(s); // 调用Allrouting.h中的forward()函数，得到header flit下一跳的节点
			if (next->node == -1)
			{
				int i;
				for (i = 1; i < MESSLENGTH && s.routpath[i].node == s.routpath[0].node; i++)
					; // if blocked insert the flits until all are inserted
				if (i < MESSLENGTH)
				{
					NodeInfo temp1, temp2; // all the flits forward one step
					temp2 = s.routpath[i - 1];
					while (i < MESSLENGTH)
					{
						temp1 = s.routpath[i];
						s.routpath[i] = temp2;
						temp2 = temp1;
						i++;
					}
					if (temp2.node != s.routpath[MESSLENGTH - 1].node)
					{
						assert(s.routpath[MESSLENGTH - 1].buff->linkused);
						s.releaselink = true;
					}
					if (temp2.buff != NULL && temp2.node != s.routpath[MESSLENGTH - 1].node)
						temp2.buff->bufferPlus(temp2.channel, MESSLENGTH);
				}
				s.timeout++;
				//	if(s.timeout == TIMEOUT) consumeMes(s);
			}
			else
			{
				s.timeout = 0;
				NodeInfo temp1, temp2; // all the flits forward one step
				temp2 = s.routpath[0];
				int i = 1;
				while (i < MESSLENGTH)
				{
					temp1 = s.routpath[i];
					s.routpath[i] = temp2;
					temp2 = temp1;
					i++;
				}
				if (temp2.node != s.routpath[MESSLENGTH - 1].node)
				{
					assert(s.routpath[MESSLENGTH - 1].buff->linkused || s.routpath[MESSLENGTH - 1].buff->linkused);
					s.releaselink = true; // the tail shift, release the physical link
				}
				if (temp2.buff != NULL && temp2.node != s.routpath[MESSLENGTH - 1].node) // maybe the last flit do not insert into the network
					temp2.buff->bufferPlus(temp2.channel, MESSLENGTH);

				s.routpath[0] = *next;
			}
		}

		else
		{
			NodeInfo temp1, temp2; // all the flits forward one step
			temp2 = s.routpath[0];
			int i;
			for (i = 1; i < MESSLENGTH && s.routpath[i].node == s.routpath[0].node; i++)
				;
			if (i == MESSLENGTH)
			{
				s.routpath[i - 1].buff->bufferPlus(s.routpath[i - 1].channel, MESSLENGTH); // add buffer
				s.active = false;
				totalcir += s.count;
				messarrive++;
				return;
			}
			while (i < MESSLENGTH)
			{
				temp1 = s.routpath[i];
				s.routpath[i] = temp2;
				temp2 = temp1;
				i++;
			}
			if (temp2.node != s.routpath[MESSLENGTH - 1].node)
				s.releaselink = true; // the tail shift, release the physical link

			if (temp2.buff != NULL && temp2.node != s.routpath[MESSLENGTH - 1].node) // maybe the last flit do not insert
				temp2.buff->bufferPlus(temp2.channel, MESSLENGTH);
		}
	}
	// cout << s.src << "dst: " << s.dst << endl ;
}
