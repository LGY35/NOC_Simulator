#include "Q3DMeshNode.h"
#include "Q3DMesh.h"

// 减少指定通道的缓冲区数量
void Buffer::bufferMin(int chn, int n)
{ 
    // 首先使用独立缓冲区，如果没有共享缓冲区
    if (R1 == chn)
    { 
        r1 -= n; // 减少R1缓冲区的数量
    }
    else
    {
        r2 -= n; // 减少R2缓冲区的数量
    }
    // 下面的代码用于流量控制
    c -= 1;
    if (R2 == chn)
        s++;
}

// 增加指定通道的缓冲区数量
void Buffer::bufferPlus(int chn, int n)
{
    assert(chn == R1 || chn == R2); // 确保通道编号正确
    if (R1 == chn)
    {
        r1 += n; // 增加R1缓冲区的数量
    }
    else
    {
        r2 += n; // 增加R2缓冲区的数量
    }
    // 下面的代码用于流量控制
    c += 1;
    if (R2 == chn)
        s--;
}

// 设置节点的坐标
void Q3DMeshNode::setCoordinate(int _nodeid, int _x, int _y,  int _z)
{
    nodeid = _nodeid; // 节点ID
    x = _x; // x坐标
    y = _y; // y坐标
    z = _z; // z坐标
}

// 设置节点的缓冲区
void Q3DMeshNode::setbuffer(int buff1, int buff2) // num is the share buffer number
{
    bufferxneg = new Buffer();
    bufferxpos = new Buffer();
    bufferyneg = new Buffer();
    bufferypos = new Buffer();
    bufferzneg = new Buffer();
    bufferzpos = new Buffer();

    clearBuffer(); // 清除缓冲区链接状态

    // 初始化x和y方向的缓冲区
    bufferxneg->r1 = buff1;
    bufferxpos->r1 = buff1;
    bufferyneg->r1 = buff1;
    bufferypos->r1 = buff1;
    bufferzneg->r1 = buff1;
    bufferzpos->r1 = buff1;

    bufferxneg->r2 = buff2;
    bufferxpos->r2 = buff2;
    bufferyneg->r2 = buff2;
    bufferyneg->r2 = buff2;
    bufferzpos->r2 = buff2;
    bufferzpos->r2 = buff2;

    // 用于流量控制的初始化
    bufferxneg->c = (buff1 + buff2) / MESSLENGTH;
    bufferxpos->c = (buff1 + buff2) / MESSLENGTH;
    bufferyneg->c = (buff1 + buff2) / MESSLENGTH;
    bufferypos->c = (buff1 + buff2) / MESSLENGTH;
    bufferzneg->c = (buff1 + buff2) / MESSLENGTH;
    bufferzpos->c = (buff1 + buff2) / MESSLENGTH;

    bufferxneg->s = 0;
    bufferxpos->s = 0;
    bufferyneg->s = 0;
    bufferypos->s = 0;
    bufferzneg->s = 0;
    bufferzpos->s = 0;
}

// 设置节点的链接缓冲区
void Q3DMeshNode::setLinkBuffer(int x1, int x2, int y1, int y2, int z1, int z2)
{
    linkxneg = x1;
    linkxpos = x2;
    linkyneg = y1;
    linkypos = y2;
    linkzneg = z1;
    linkzpos = z2;
					
    // 链接对应节点的缓冲区，如果没有链接则为NULL
    // 通常，一个节点在某一维度的正方向应该链接到该维度负方向的缓冲区，反之亦然。
    // 例如，如果节点A在x维度的正方向链接到节点B，那么节点B在x维度的负方向应该链接回节点A。这样的设置确保了消息可以正确地在节点间传递。
    //xneg
    if (linkxneg != -1)		//如果存在，就去链接
        bufferxneglink = (*mesh)[linkxneg]->bufferxpos;
    else
        bufferxneglink = NULL;
    //xpos
    if (linkxpos != -1)
        bufferxposlink = (*mesh)[linkxpos]->bufferxneg;
    else
        bufferxposlink = NULL;

    //yneg
    if (linkyneg != -1)
        bufferyneglink = (*mesh)[linkyneg]->bufferypos;
    else
        bufferyneglink = NULL;
    //ypos
    if (linkypos != -1)
        bufferyposlink = (*mesh)[linkypos]->bufferyneg;
    else
        bufferyposlink = NULL;
    //zneg
    if (linkzneg != -1)
        bufferzneglink = (*mesh)[linkzneg]->bufferzpos;
    else
        bufferzneglink = NULL;
    //zpos
    if (linkzpos != -1)
        bufferzposlink = (*mesh)[linkzpos]->bufferzneg;
    else
        bufferzposlink = NULL;
}


// 增加或减少指定缓冲区的缓冲量
void Q3DMeshNode::bufferPlus(Buffer *buff, int chn, int n)
{
    buff->bufferPlus(chn, n); // 增加缓冲量
}

void Q3DMeshNode::bufferMin(Buffer *buff, int chn, int n)
{
    buff->bufferMin(chn, n); // 减少缓冲量
}

// 清空所有缓冲区的链接状态
void Q3DMeshNode::clearBuffer()
{
    bufferxneg->linkused = false;
    bufferxpos->linkused = false;
    bufferyneg->linkused = false;
    bufferypos->linkused = false;
    bufferzneg->linkused = false;
    bufferzpos->linkused = false;
}


// 设置节点所在的网络
void Q3DMeshNode::setMesh(Q3DMesh *mesh)
{
    assert(mesh != nullptr);  // 确保mesh不是空指针
    this->mesh = mesh;
}//这个函数报错：
// Program received signal SIGSEGV, Segmentation fault.
// 0x0000555555556c1e in Q3DMeshNode::setMesh(Q3DMesh*) ()
//修改在Q3DMesh.cpp中Q3DMesh::Q3DMesh类构造函数中。