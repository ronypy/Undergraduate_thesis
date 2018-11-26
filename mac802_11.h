#ifndef INC_mac802_11_H
#define INC_mac802_11_H



#include"scheduler.h"
#include"packet.h"


class node;
class flow;
enum MacState{Idle,//0
              WaitingForDIFS,//1
              BackingOff,//2
              WaitingToXmitDATA,//3
              XmittingDATA,//4
              WaitingForACK,//5
              WaitingToXmitACK,//6
              XmittingACK,//7
              WaitingToXmitACK2//8
              };

class MAC802_11
{
node* n;
flow* f;
Scheduler* sp;
int i;
int N;
int F;

public:
//variables
 double CST;
 double Pt;
static double DIFS;
static double SIFS;
static double EIFS;
static double aSlotTime;
static double MaxPropagationDelay;
static double InterleavingTime;
int CWmin;
int CWmax;
int CWprev;
//int LastRecvPktID[16][30];
int DummyPkt2Send[100][100];
int *RecvPkts;
int TotalXmitPkts[100];
int TotalRecvPkts[100];
int TotalDroppedPkts[100];


double cw;
double BackOffEndTime;
double InterferencePower;
double NoisePower;
//double NavEndTime;
double ExpireTime;
double NextRTSXmissionTime;
double NextDATAXmissionTime;
static double BasicRate;
static double DataRate;
int tx_active;
int ShortRetry;
int LongRetry;
int AGTReceived[100];
int call;
//int DontCare2Xmit;


//functions
MAC802_11();
void setscheduler(Scheduler* S);
void setnode(node* x, flow* y, int index,int N, int F);
void idle();
int  checkmedium();
double getpower(Packet* p);
double resetCW();
double IncreaseCW(int CWprev);
//void sendRTS();
//void sendCTS(int ReceiverNodeID,double RTSduration);
void sendDATA();
void sendACK(int ReceiverNodeID, int ReceivedFlow);
double txtime(int PacketSize,double DataRate);
double txtime(Packet* p);
void transmit(Packet* p, double timeout);
//void RetransmitRTS();
void RetransmitDATA(int NodeID,int FlowID, double Rate);
void recv(Packet* p);
void recv_timer(Packet* p);
//void recvRTS(Packet* p);
//void recvCTS(Packet* p);
void recvDATA(Packet* p);
void recvACK(Packet* p);
void ProcessReceivedDATA(Packet* p);
double SINRThreshold(Packet* p);
void CheckBackOff();
//void CheckNav();

//classes
MacState macstate;
Packet* Pmac;
Packet* PktRecv1;
Packet* PktRecv2;

};
#endif
