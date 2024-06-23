/**********

12/11/2007  Luo wei

模拟信息（message）在网络中的路由过程。
通过设置不同的路由算法和网络结构，测试不同条件下的网络性能，如延迟（latency）和吞吐量（throughput）。
  *********************/

/**************************************************************
arrive是到达的信息
in the network是滞留在网络中的信息

里面的虚拟通道只是给了buffer，但是实际路由的过程中没有用到，所以增加一下虚拟通道会有用的。
***************************************************************/

#include "common.h"

int ALGORITHM;	  // 实现多个路由算法的时候执行的是哪一个，该程序只实现了一个，用于Routing.cpp中
int GENERATETYPE; // 用于选择linkrate的增长方式
int flowalg;	  // 未在代码中使用，可能是用来指定特定的流控算法。
long totalcircle; // 程序总的运行周期
int knode;		 // 二维网络中每维的节点数

int getsize(vector<Message *> *mess);

int main()
{
	for (int allgen = 1; allgen < 2; allgen++)	// 从 allgen = 1 开始，只执行一次，
	{
		int threshold = 8000+2000; 					// 设置一个阈值，用于控制网络中的消息数量，当滞留的消息达到阈值时就会饱和
        Allrouting *rout1 = NULL; 				// 路由器对象指针，初始化为NULL
        GENERATETYPE = allgen; 					// 设置消息生成类型			//TODO: 更改消息生成类型，即流量模式  为1表示 uniform流量模式
        flowalg = 1; 							// 流控算法标识符
        totalcircle = 15000+2000; 					// 设置模拟的总周期数
        knode = 4; 								// 设置网络中每个维度的节点数
        Q3DMesh *mes = NULL; 		 			// 网络结构，3D mesh
        Event *s = NULL; 						// 事件处理对象指针，初始化为NULL
        int r1, r2; 							// 用于存储虚拟通道的缓存大小
        string gen[5] = {"0", "1", "2", "3", "4"}; 	// 生成类型标识符数组
        string filename[5] = {
			"./data/Bubble Flow",
			"./data/clue-WF", 	
			"./data/clue-DOR",	//是相对于makefile的路径，所以只需要1个.即可
			"./data/FCclue-DOR",
			"./data/FCclue-WF",
		}; // 相对于 main.cpp 的路径(应该不是相对于main，而是相对于makefile) // 结果的输出文件路径，因为以前实现了5种路由算法，所以这里有5个文件路径

		for (int lop = 0; lop < 5; lop++)	 // 循环更新文件名中的生成类型
		{
			;
			filename[lop].replace(10, 1, gen[GENERATETYPE]);
		}

		// r1buffer 和 r2buffer 分别存储不同虚拟通道的缓冲区大小。
		int r1buffer[5] = {1, 2, 1, 2, 2}; // 虚拟信道1缓存大小，以message个数为基本单位   数字为缓冲区大小 //TODO: 5个分别对应5种路由算法的buffer的大小
		int r2buffer[5] = {2, 1, 0, 1, 1}; // 虚拟信道2缓存大小，若无虚拟通道不使用r2
		int alg[5] = {0, 1, 2, 1, 2};	   // 路由算法标识符数组	

		/***
		round = 2 : Dimension Order Routing，该程序只实现了xy路由，所以round的值不改变，只为2，循环只在实现多种路由算法时才有意义
	 ***/
		//TODO: 更改为其他路由算法时，把2和3更改即可
		for (int round = 2; round < 3; round++) // 仅执行一次循环，用于指定路由算法
		{

			ofstream out = ofstream(filename[round].c_str()); 	// 打开文件准备写入结果
			float linkrate = 0;									// 链路利用率，初始化为0
			double max = 0;										// 用于存储最大吞吐量

			/************************************************************************************

								start simulate

		  ***********************************************************************************/
		// 开始模拟 		//通过改变 linkrate 来测试网络在不同负载下的表现。
			// linkrate控制消息产生速率，消息生成过程根据 linkrate 计算每个周期每个节点生成的 message 数量。
			for (linkrate = 0.01; linkrate < 1;)				 // 逐步增加链路利用率进行测试
			{

				r1 = r1buffer[round] * MESSLENGTH; // 以flit个数为基本单位	 // 计算虚拟通道1的总缓冲区大小 
				r2 = r2buffer[round] * MESSLENGTH;
				mes = new Q3DMesh(knode, r1, r2); // 初始化网络结构

				switch (round)	// 根据round选择并初始化路由算法
				{

				case 1:
				case 2:
					ALGORITHM = alg[round];			// TODO: 路由算法的更改。此处round = 2 为 DOR算法
					rout1 = new Routing(mes);
					break;
				}
				
				s = new Event(rout1);			// 初始化事件处理对象

				float msgpercir = (float)(linkrate * 2 * 2 * knode / (MESSLENGTH * 10)); // 计算每个周期每个节点产生的message数，还要除以10是因为allvecmess有10个元素
				// saturationRate = (double)(knode * 2 * 2) / (double)(knode * knode); 在mesh网络中的饱和吞吐量
				// msgpercir = linkrate * saturationRate * knode * knode; 每个周期每个节点产生的flit数

				vector<Message *> allvecmess[10];		// 存储消息的数组，每个元素是一个消息向量
				float k = 0;
				int allmess = 0;

				/************************************************************************************

								genarate message

			  ***********************************************************************************/
			// 进行模拟，直到达到指定周期或消息数量超过阈值
			// 网络的主体模拟在 totalcircle 指定的周期内进行，通过调用 Event 类的方法来处理消息的生成和转发。
				// 执行totalcircle个周期，getsize(allvecmess) < threshold只是自己加的限制条件，可以有也可以删除，具体的threshold和totalcircle值也可以在前面修改
				for (int i = 0; i < totalcircle && getsize(allvecmess) < threshold; i++)
				{
					vector<Message *> &vecmess = allvecmess[i % 10];	// 获取对应周期的消息向量
					for (k += msgpercir; k > 0; k--)					// 生成新消息
					{
						allmess++;										// 更新消息计数  // 总的产生消息数加一  
						vecmess.push_back(s->genMes()); 		// 生成并存储新消息		// 产生消息放入	allvecmess的某个元素中
					}

					/************************************************************************************

									release link

				  ***********************************************************************************/
				// 释放占用的链路资源
					for (vector<Message *>::iterator it = vecmess.begin(); it != vecmess.end(); it++)
					{

						/* if the tail of a message shifts ,
						the physical link the message  occupied should release.
						  */

						if ((*it)->releaselink == true)
						{
							assert((*it)->routpath[MESSLENGTH - 1].buff->linkused);
							(*it)->routpath[MESSLENGTH - 1].buff->linkused = false; // 释放链路
							(*it)->releaselink = false;
						}
					}

					/************************************************************************************

									forward message

				  ***********************************************************************************/
				// 转发消息
					for (vector<Message *>::iterator it = vecmess.begin(); it != vecmess.end();)
					{
						if ((*it)->active == false)	// 消息到达目的地
						{ // when a message arrive at its destination, it is not active
							delete (*it);			// 删除消息
							it = vecmess.erase(it); // 消息到达目的节点，将它从vecmess中删除
						}
						else
							s->forwardMes(*(*it++)); // 转发活跃消息  // 调用Routing.cpp中函数转发消息
					}
				}

				/*****************************************************************************

								output results
				  ****************************************************************************/
				// 输出结果，计算并输出平均延迟和吞吐量
				int size = getsize(allvecmess);

				// s->totalcir/s->messarrive 平均延迟的计算公式；linkrate * ((float) s->messarrive / allmess)吞吐量的计算公式
				cout << endl
					<< endl
					<< "linkrate:" << linkrate << "    arrive:  " << s->messarrive << "    in the network : "
					<< size << endl
					<< "average latency: "
					<< (s->totalcir / s->messarrive) << "  nomalized accepted traffic: "
					<< linkrate * ((float)s->messarrive / allmess)
					<< endl
					<< endl;

				out << linkrate * ((float)s->messarrive / allmess)
					<< " " << (s->totalcir / s->messarrive) << endl;

				/************************************************************************************

									whether arrive at saturation point

			  ***********************************************************************************/
			// 判断是否达到饱和点
				if (linkrate * ((float)s->messarrive / allmess) > max && ((linkrate * ((float)s->messarrive / allmess) - max) / max) > 0.01 )
				// && getsize(allvecmess) < threshold)		//如果产生的消息大于阈值，直接饱和。  对于mesh可能没什么影响，但是对于torus承载流量大的，就可能会限制，滞留的消息达不到阈值也会饱和
					max = linkrate * ((float)s->messarrive / allmess);

				else
				{
					cout << "Saturation point, drain......." << endl;
					drain(allvecmess, mes, s);		 // 当达到饱和点时，清空网络并输出相关信息
					int size = 0;
					for (int j = 0; j < 10; j++)
					{
						if (!allvecmess[j].empty())
						{
							size += allvecmess[j].size();
							// printf("ponit------- 1111 --------------------\r\n"); //for debug
						}
					}
					cout << "in the network:      " << size << endl;
					outtotest(allvecmess, mes);
					bufferleft(mes, knode);
					cout << "max:" << max << endl;
					// printf("ponit------- 2222 successful--------------------\r\n");	//for debug
					break;
				}

				/************************************************************************************
								clean
			  *******************************************************************************************/
			// 清理资源，准备下一轮模拟
				for (int m = 0; m < 10; m++)
				{
					//正常运行每次都输出10个这行代码
					// printf("ponit------- 3333 --------------------\r\n");
					for (vector<Message *>::iterator it = allvecmess[m].begin();
						it != allvecmess[m].end(); it++)
						delete (*it);	// 删除所有消息
				}
				delete rout1;			// 删除路由对象
				delete mes;				// 删除网络结构对象
				delete s;				// 删除事件处理对象

				// 根据生成类型调整linkrate的增长
				switch (GENERATETYPE)
				{
				case 1:
					if (linkrate < 0.5)
						linkrate += 0.05;
					else
						linkrate += 0.02;
					break;

				case 2:
				case 3:
					if (linkrate < 0.3)
						linkrate += 0.1;
					else
						linkrate += 0.02;
					break;
				case 4:
					if (linkrate < 0.4)
						linkrate += 0.1;
					else
						linkrate += 0.02;
					break;
				}

			} // each linkrate end
		} // round end
	}	// end for allgen
	// printf("ponit------- 4444 --------------------\r\n");	//for debug
	return 0;	// return 1
}
