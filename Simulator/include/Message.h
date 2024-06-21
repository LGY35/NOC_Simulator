/*********
		9/11/2007,Luo Wei.
		The definition of message.

消息（Message）在网络中的路由和传输处理。
此类定义了在仿真环境中的网络消息，特别是针对3D mesh网络的适应。

********/


#ifndef MESSAGE_H
#define MESSAGE_H
#define PROCESSTIME 16  // 消息处理开始前的等待时间
#define TIMEOUT 50000  // 消息在等待通道时的时间限制	//TODO: 是否需要改这个
#include <iostream>
#include <string>
#include <assert.h>
#include "Q3DMeshNode.h"
using namespace std;
class Buffer;
class NodeInfo;

/************************
    消息信息定义
******************************/
class Message {
public:
    int length;  // 以flits为单位的消息长度
    int src;  // 消息源节点
    int dst;  // 消息目的节点
    int timeout;  // 消息等待通道的时间限制（以周期计）
    int begintrans;  // 消息生成到开始传输的时间
    int step;  // 消息已走的步数
    bool active;  // 消息是否在网络中活跃（未到达目的地）
    NodeInfo *routpath;  // 路由路径：每个flit当前所在的节点、通道和缓冲区
    int count;  // 消息消耗的总时间
    bool releaselink;  // 消息移动后是否需要释放占用的物理链路

    bool turn;  // 在bubble flow control中使用，如果为true，则请求逃逸通道需要2个缓冲区

    Message() {
        src = -1;
        dst = -1;
    }

    Message(int src1, int dst1) 
	{
        begintrans = PROCESSTIME;
        src = src1;
        dst = dst1;
        routpath = new NodeInfo[MESSLENGTH];
        assert(routpath);
        for (int i = 0; i < MESSLENGTH; i++) 
		{
            routpath[i].node = src;
            routpath[i].channel = 0;
            routpath[i].buff = NULL;
        }
        step = 0;
        active = true;
        length = MESSLENGTH;
        count = 0;
        releaselink = false;
        turn = true;
        timeout = 0;
    }

    ~Message() 
	{
        delete[] routpath;
    }

    void setLength(int n) 
	{
        length = n;
    }
};

#endif




// #ifndef MESS_AGE
// #define MESS_AGE
// #define PROCESSTIME 16
// #define TIMEOUT 50000
// #include <iostream>
// #include <string>
// #include <assert.h>
// #include "Q3DMeshNode.h"
// using namespace std;
// class Buffer;
// class NodeInfo;

// /************************
// Message info
//   ******************************/
// class Message
// {
// public:
// 	int length; // measured by flits;
// 	int src;	// The source of the message
// 	int dst;
// 	int timeout;		// measured by circle ,the time limit of message waits for a channel
// 	int begintrans;		// when a message is generated ,it needs some time until transmitting,begintrans record this.
// 	int step;			// how many steps this message has walked;
// 	bool active;		// check this message whether consumed or arrive at dst
// 	NodeInfo *routpath; // : the ith flit now at routpath[i].node and take routpath[i].buffer
// 	int count;			// the total time a message  consumed
// 	bool releaselink;	// if the tail of a message shifts , the physical link the message occupied should release.

// 	bool turn; //  used in bubble flow control, if true, then the request escape channel need 2 buffers.

// 	Message()
// 	{
// 		src = -1;
// 		dst = -1;
// 	}

// 	Message(int src1, int dst1)
// 	{
// 		begintrans = PROCESSTIME;
// 		src = src1;
// 		dst = dst1;
// 		routpath = new NodeInfo[MESSLENGTH];
// 		assert(routpath);
// 		for (int i = 0; i < MESSLENGTH; i++)
// 		{
// 			routpath[i].node = src;
// 			routpath[i].channel = 0;
// 			routpath[i].buff = NULL;
// 		}
// 		step = 0;
// 		active = true;
// 		length = MESSLENGTH;
// 		count = 0;
// 		releaselink = false;
// 		turn = true;
// 		timeout = 0;
// 	}

// 	~Message()
// 	{
// 		delete[] routpath;
// 	}

// 	void setLength(int n)
// 	{
// 		length = n;
// 	}
// };

// #endif