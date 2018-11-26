#ifndef INC_router_H
#define INC_router_H

#include"packet.h"
class Router
{
Packet* PQ[100];  // Head of packet
public:
static int BufferSize;
int NumberofFlows;
 int NumberofNodes;
Router();
void SetFlowNode(int F,int N);
//void push(Packet* p);
//void pushforward(Packet* p);
//Packet* pop(int FlowID);
void push(int FlowID);
void pushforward(int FlowID);
void pop(int FlowID);

double RouteCache[100][100];//[FlowNumber][NodeNumber]
int BufferOccupancy[100];

//int nexthop(int FlowID);
int FindSelectedIndex(double RandomValue);

};
#endif
