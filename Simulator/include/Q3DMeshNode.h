//  3 dimension mesh nodes.
//  20/3/2009, Luo Wei.

#ifndef Q3DMESHNODE_H
#define Q3DMESHNODE_H
#include <iostream>
#include <string>
#include <assert.h>
using namespace std;

#define R1 1 // 定义R1虚拟通道标识
#define R2 2 // 定义R2虚拟通道标识
#define MESSLENGTH 16 // 消息长度定义

// #include "Q3DMesh.h"	//TODO: 循环包含,已经解决

class Q3DMesh; // 声明三维网格类  // 只需前向声明

// 缓冲区类定义
class Buffer
{ // R1 and R2 reserve some buffers separately,they can also use sharebuffer.
public:
	bool linkused; // in a physical circle，检查物理链接在一个周期中是否被使用，每个周期只允许一个flit通过
	int r1; // R1缓冲区大小
	int r2; // R2缓冲区大小
	void bufferMin(int chn, int n); // 减少指定通道的缓冲量
	void bufferPlus(int chn, int n); // 增加指定通道的缓冲量

	int c; // 用于流量控制的缓冲区
	int s; // 特殊用途的流量控制缓冲区
};
/**************************************************

	this class define the info of a flit in the mesh node
	including nodeid, buffer
	************************************************/

// 定义网格节点中flit的信息类
class NodeInfo
{
public:
	int node; // 下一个节点编号
	int channel;  // 使用的虚拟通道：R1或R2
	Buffer *buff; // 指向占用的缓冲区，如bufferxneg等
	NodeInfo() : node(-1) {} // 构造函数
};

// 三维MESH网格节点类
class Q3DMeshNode
{
private:
	int nodeid;

public:
	Buffer *bufferxneg; // x轴负方向的缓冲区
	Buffer *bufferxpos; // x轴正方向的缓冲区
	Buffer *bufferyneg; // y轴负方向的缓冲区
	Buffer *bufferypos; // y轴正方向的缓冲区
	Buffer *bufferzneg; // z轴负方向的缓冲区
	Buffer *bufferzpos; // z轴正方向的缓冲区

	int linkxneg; // x轴负方向链接的节点
	int linkxpos; // x轴正方向链接的节点
	int linkyneg; // y轴负方向链接的节点
	int linkypos; // y轴负方向链接的节点
	int linkzneg; // z轴正方向链接的节点
	int linkzpos; // z轴正方向链接的节点

	Q3DMesh *mesh;	//mesh网络指针

	Buffer *bufferxneglink; // x轴负方向链接的节点的缓冲区
	Buffer *bufferxposlink; // x轴正方向链接的节点的缓冲区
	Buffer *bufferyneglink; // y轴负方向链接的节点的缓冲区
	Buffer *bufferyposlink; // y轴正方向链接的节点的缓冲区
	Buffer *bufferzneglink; // z轴负方向链接的节点的缓冲区
	Buffer *bufferzposlink; // z轴正方向链接的节点的缓冲区

	int x; // x coordinate
	int y; // y coordinate
	int z; // z coordinate

	void setCoordinate(int _nodeid, int _x, int _y, int _z);

	void setbuffer(int buff1, int buff2); //   num is the share buffer number

	void setLinkBuffer(int x1, int x2, int y1, int y2, int z1, int z2);

	void setMesh(Q3DMesh *mesh);

	void bufferPlus(Buffer *buff, int chn, int n);

	void bufferMin(Buffer *buff, int chn, int n);

	void clearBuffer();

	~Q3DMeshNode()
	{
		delete bufferxneg;
		delete bufferyneg;
		delete bufferzneg;
		delete bufferxpos;
		delete bufferypos;
		delete bufferzpos;
	}
};
#endif