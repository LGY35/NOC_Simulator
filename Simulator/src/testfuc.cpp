#include "common.h"

//获取消息列表的总大小
int getsize(vector<Message *> *mess)
{
	int size = 0;
	for (int i = 0; i < 10; i++)
	{
		if (!mess[i].empty())
			size += mess[i].size();// 计算10个消息队列中的总消息数
	}
	return size;
}

//将消息输出到测试文件
void outtotest(vector<Message *> *allvecmess, Q3DMesh *mes)
{
    ofstream out = ofstream("test");  // 创建一个输出文件流
    for (int i = 0; i < 10; i++)
    {
        vector<Message *> &vecmess = allvecmess[i];  // 引用每一个消息队列
        for (vector<Message *>::iterator it = vecmess.begin(); it != vecmess.end(); it++)
        {
            // 输出消息的计数、源点和目的地坐标（包括z坐标），以及头部和尾部的信息
            out << "count: " << (*it)->count 
                << "  src: ( " << (*mes)[(*it)->src]->x << " ," << (*mes)[(*it)->src]->y << ", " << (*mes)[(*it)->src]->z << ")"
                << " dst: ( " << (*mes)[(*it)->dst]->x << " ," << (*mes)[(*it)->dst]->y << ", " << (*mes)[(*it)->dst]->z << ")"
                << " head:( " << (*mes)[(*it)->routpath[0].node]->x << " ," << (*mes)[(*it)->routpath[0].node]->y << ", " << (*mes)[(*it)->routpath[0].node]->z 
                << ", R" << (*it)->routpath[0].channel << ")"  
                << " tail:( " << (*mes)[(*it)->routpath[19].node]->x << " ," << (*mes)[(*it)->routpath[19].node]->y << ", " << (*mes)[(*it)->routpath[19].node]->z << ")"
                << endl;
        }
    }
}


//  检查缓冲区剩余空间         此函数统计并输出所有节点的四个缓冲区（X负向，X正向，Y负向，Y正向）的剩余容量。
void bufferleft(Q3DMesh *mes, int knode)
{
	int star1 = 0, star2 = 0, flag = 0, star3 = 0, star4 = 0, star5 = 0, star6 = 0;
	for (int sta = 0; sta < knode * knode * knode; sta++)	// 注意这里遍历范围的改变，适应3D
	{
		// 汇总所有节点的缓冲区剩余空间
		star1 += ((*mes)[sta]->bufferxneg->c);
		star2 += ((*mes)[sta]->bufferxpos->c);
		star3 += ((*mes)[sta]->bufferyneg->c);
		star4 += ((*mes)[sta]->bufferypos->c);
		star5 += ((*mes)[sta]->bufferzneg->c);
		star6 += ((*mes)[sta]->bufferzpos->c);
	}
	cout << "buffer left ---> x-neg: " << star1 << ", x-pos: " << star2 
        << ", y-neg: " << star3 << ", y-pos: " << star4 
        << ", z-neg: " << star5 << ", z-pos: " << star6 << endl;  // 输出包括z方向的缓冲区信息

	// cout << "buffer left --->" << star1 << " " << star2 << " " << star3 << " " << " " << star4 << endl;
}


void drain(vector<Message *> *allvecmess, Q3DMesh *mes, Event *s)
{
    for (int i = 0; i < 10000; i++)
    {
        vector<Message *> &vecmess = allvecmess[i % 10];  // 处理消息队列，循环使用这10个队列
        for (vector<Message *>::iterator it = vecmess.begin(); it != vecmess.end(); it++)
        {
            // 检查消息是否需要释放占用的链路
            if ((*it)->releaselink == true)
            {
                assert((*it)->routpath[MESSLENGTH - 1].buff->linkused);  // 断言最后一个消息片段的缓冲区是被使用的
                (*it)->routpath[MESSLENGTH - 1].buff->linkused = false;  // 释放链路
                (*it)->releaselink = false;
            }
        }

        for (vector<Message *>::iterator it = vecmess.begin(); it != vecmess.end();)
        {
            if ((*it)->active == false || (*it)->routpath[0].node == (*it)->src)
            {  // 如果消息已经到达目的地或者不再活跃
                delete *it;  // 删除消息对象
                it = vecmess.erase(it);  // 从容器中移除
            }
            else
            {
                s->forwardMes(*(*it++)); // 调用forwardMes来处理消息的下一步 // 移动迭代器到下一个元素
            }
        }
    }
}

