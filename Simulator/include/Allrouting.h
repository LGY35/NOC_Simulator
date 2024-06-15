#ifndef ALLROUTING_H
#define ALLROUTING_H

#include "Q3DMesh.h"
#include "Message.h"
class Message;
class Q3DMesh;
class NodeInfo;
class Allrouting
{
public:
	NodeInfo *next;
	Q3DMesh *mesh;
	int k;
	virtual NodeInfo *forward(Message &s) { return NULL; };
	// implemented by child
};

#endif


// #ifndef ALLROUTING_H
// #define ALLROUTING_H

// #include "Q2DTorus.h"
// #include "Message.h"
// class Message;
// class Q2DTorus;
// class NodeInfo;
// class Allrouting
// {
// public:
// 	NodeInfo *next;
// 	Q2DTorus *torus;
// 	int k;
// 	virtual NodeInfo *forward(Message &s) { return NULL; };
// 	// implemented by child
// };

// Q3DMesh
// #endif