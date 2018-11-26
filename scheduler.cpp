//scheduler.cpp

#include "scheduler.h"
//#include "random.h"
#include "node.h"
#include <time.h>
#include "flow.h"
#include<math.h>
#include <ctime>
#include <cstdlib>
#include <stdio.h>

double Scheduler::clock_=0.0;
Event* Scheduler::EQ=0;
long int Scheduler::event_uid=0;


#define max(a, b) (a > b ? a : b)

Scheduler::Scheduler(node* a, flow* b)
{
n=a;
f=b;
}

 Event* Scheduler::pop()
 {
 Event* e=EQ;

 return e;
 }

Event* Scheduler::push_data(int event_type,double delay, int node_id,int packet_uid)
{
double current_time=get_clock();
double t=current_time+delay;
Event* e=new Event;
 e->event_type=event_type;
 e->event_uid=event_uid++;
 e->first_data=node_id;
 e->ex_time=t;
 e->second_data=packet_uid;
return e;
}

void Scheduler::schedule(int event_type,double delay, int node_id,int packet_uid)
{
Event* e=push_data(event_type,delay, node_id,packet_uid);
insert(e);
}

void Scheduler::insert(Event* e)
 {
         double t = e->ex_time;
         Event* p;
         Event* q;

         for (p = EQ; p != 0; p = p->next_)
               { if (t < p->ex_time)
                  break;
                   q=p;
               }
         e->next_ = p;
         p = e;
         q->next_=p;

 }


void Scheduler::action(Event* e)
{
set_clock(e->ex_time);
switch(e->event_type){

//Reset Mac parameters and schedule for packet generation
case 1:
{
int i;
int j;
for (i=0;i<e->first_data;i++)
   {
	n[i].mac.cw=n[i].mac.resetCW();

	n[i].router.BufferOccupancy[0]=0;
    }

  for(j=0;j<e->second_data;j++)
    {//srand ( time(NULL) );

	  double u=rand()/double(RAND_MAX);
      double delay =-1/f[j].packet_rate*log(u);
       schedule(2,delay,f[j].source_id,f[j].flow_id);//packet generation by all nodes
    }
    EQ=EQ->next_;
    break;
}


// Packet generation by a source Node
case 2:

{
int i= e->first_data;//source node
int j= e->second_data;//flow id
n[i].mac.TotalRecvPkts[j]++;
if(n[i].router.BufferOccupancy[f[j].flow_id]< n[i].router.BufferSize)
  {
   n[i].router.push(f[j].flow_id);
   //if (p->machr.flow_id>15)printf("FlowID=%d",p->machr.flow_id);
    n[i].mac.idle();
   }
else n[i].mac.TotalDroppedPkts[e->second_data]++;
//srand ( time(NULL) );

double u=rand()/double(RAND_MAX);
double delay =-1/f[j].packet_rate*log(u);
schedule(2,delay,i,j);
EQ=EQ->next_;
break;
}

case 4:

{ int i= e->first_data;// node ID
   if (n[i].mac.checkmedium()==0&&n[i].mac.macstate==WaitingForDIFS)
       {  schedule(5,n[i].mac.cw,i,0);
          n[i].mac.macstate=BackingOff;
          n[i].mac.BackOffEndTime=get_clock()+n[i].mac.cw;

        }
EQ=EQ->next_;
break;
}


case 5:

{ int i= e->first_data;// node ID
   if (n[i].mac. BackOffEndTime==get_clock()&&n[i].mac.macstate==BackingOff)
       { n[i].mac.sendDATA();//new
	   //schedule(6,n[i].mac.DIFS,i,0);
         //  n[i].mac.NextDATAXmissionTime=get_clock()+n[i].mac.DIFS;
          //n[i].mac.cw=0.0;
          //n[i].mac.macstate=WaitingToXmitDATA;
        }
      //else//
        //  {n[i].mac.macstate=Idle;//new
         //  n[i].mac.idle();//new
         // }
EQ=EQ->next_;
break;
}

/*
case 6:

{ int i= e->first_data;// node ID

        if (n[i].mac.macstate==WaitingToXmitDATA)
       {
    		 n[i].mac.sendDATA();
        }
       else
       {n[i].mac.macstate=Idle;
        n[i].mac.idle();
       }
EQ=EQ->next_;
break;
}
*/
case 12://Interference finishing--7a

{ int i= e->first_data;
  Packet* p= (Packet*)e->second_data;
  double Pr=n[i].mac.getpower(p);
  n[i].mac.InterferencePower-=Pr;
  n[i].mac.InterferencePower=max(0,n[i].mac.InterferencePower);
  n[i].mac.idle();
  EQ=EQ->next_;
  break;
}

case 13://Finishing the reception of packet--7b
{ int i= e->first_data;
  Packet* p=(Packet*)e->second_data;
  n[i].mac.recv_timer(p);
  EQ=EQ->next_;
  break;
  }


case 15://Finishing xmitting data---10

{ int i= e->first_data;
  n[i].mac.tx_active=0;
  n[i].mac.macstate=WaitingForACK;

EQ=EQ->next_;
break;
}


case 16://timeout for reception of ack--11

{ int i=e->first_data;
 Packet* p=(Packet*)e->second_data;

  if(n[i].mac.macstate==WaitingForACK)
   {

	  if(p->cmn.ReceivedByReceiver==1)
	    {n[i].mac.DummyPkt2Send[p->machr.ReceiverNodeID][p->machr.flow_id]=1;
	    //cout<<i<<"\t";
	    //cout<<p->machr.ReceiverNodeID<<endl;
	    }

	 // n[i].router.pushforward(p->machr.flow_id);
      n[i].mac.RetransmitDATA(i, p->machr.flow_id, f[p->machr.flow_id].packet_rate);
    }

EQ=EQ->next_;
break;
}


case 17://xmit ack--15

{ int i= e->first_data;
  Packet* Preceived=(Packet*)e->second_data;//new add
  int Xmitter=Preceived->machr.XmitterNodeID;
  int ReceivedFlow=Preceived->machr.flow_id;
  if(n[i].mac.macstate==WaitingToXmitACK || n[i].mac.macstate==WaitingToXmitACK2)// Changed
    n[i].mac.sendACK(Xmitter,ReceivedFlow);

EQ=EQ->next_;
break;
}

case 18://finishing ack xmission--16

{ int i= e->first_data;
 if(n[i].mac.macstate==XmittingACK)
 { n[i].mac.tx_active=0;
   n[i].mac.macstate=Idle;
   n[i].mac.idle();

 }
EQ=EQ->next_;
break;
}


case 19:
{int i= e->first_data;
Packet* Pack=(Packet*)e->second_data;
delete Pack;
EQ=EQ->next_;
break;
}


case 20:
{int i= e->first_data;
int j= e->second_data;


if(n[i].router.BufferOccupancy[f[j].flow_id]< n[i].router.BufferSize)
  {
   n[i].router.push(f[j].flow_id);
   n[i].mac.idle();

   }
else n[i].mac.TotalDroppedPkts[f[j].flow_id]++;


EQ=EQ->next_;
break;
}


default:
int def =0;

}
}

