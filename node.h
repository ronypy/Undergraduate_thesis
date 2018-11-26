//node.h

#ifndef INC_node_H
#define INC_node_H
#include"router.h"
#include"mac802_11.h"

class node
{
public:
int node_id;
double X;
double Y;
double Z;
//int node_type;
//int dest_id;
//double packet_rate;
Router router;
MAC802_11 mac;

};

#endif



//flow.h

