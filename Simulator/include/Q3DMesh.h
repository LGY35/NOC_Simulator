// 3 dimension torus network .Written by Luo Wei in 3/20/2009.

#ifndef Q3DMESH_H
#define Q3DMESH_H
#include "Q3DMeshNode.h"
class Q3DMeshNode;
class Q3DMesh
{
public:
	Q3DMesh(int n, int R1buffer, int R2buffer); // n*n*n 3DMesh,
	Q3DMeshNode *operator[](int n);
	void clearAll();
	~Q3DMesh()
	{

		delete[] head;
	}
	int getNode() { return k; }

private:
	int k;
	Q3DMeshNode *head;
};
#endif