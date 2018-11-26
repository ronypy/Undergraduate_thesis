#include"mac802_11.h"
#include"node.h"
#include"math.h"
//#include"random.h"
#include <ctime>
#include <stdio.h>


#define max(a, b) (a > b ? a : b)


double MAC802_11::aSlotTime=20.0e-6;
double MAC802_11::DIFS=50.0e-6;
double MAC802_11::SIFS=10.0e-6;
double MAC802_11::MaxPropagationDelay=1.0e-6;
double MAC802_11::EIFS=aSlotTime+DIFS+144.0e-6+48.0e-6+8.0*14.0/2.0e6;
//double MAC802_11::BasicRate=1.0e6;
double MAC802_11::BasicRate=2.0e6;

//double MAC802_11::DataRate=2.0e6;
double MAC802_11::DataRate=2.0e6;


MAC802_11::MAC802_11()
{
call=0;
NoisePower=1e-13;//-100.0;
InterferencePower=0.0;
macstate=Idle;
PktRecv1=0;
PktRecv2=0;
cw=0;//resetCW();
CWmax=5*CWmin;
tx_active=0;
ShortRetry=0;
LongRetry=0;
BackOffEndTime=0.0;
ExpireTime=0.0;
Pmac=0;
//DontCare2Xmit=1;


RecvPkts=new int[100];

for (int j=0;j<100;j++)
	for (int fi=0;fi<100;fi++)
      DummyPkt2Send[j][fi]=0;

for (int j=0;j<100;j++)
    RecvPkts[j]=0;
for (int fi=0;fi<100;fi++)
   {
    TotalXmitPkts[fi]=0;
    TotalRecvPkts[fi]=0;
    TotalDroppedPkts[fi]=0;
    AGTReceived[fi]=0;

   }
}


void MAC802_11::setscheduler(Scheduler* S)
{
sp=S;
}
void MAC802_11::setnode(node* x, flow* y, int index,int NodeNumber,int FlowNumber)
{
n=x;
f=y;
N=NodeNumber;
F=FlowNumber;
i=index;
}

int MAC802_11:: checkmedium()
{

double TotalNoise= InterferencePower+ NoisePower+getpower(PktRecv1)+getpower(PktRecv2);
if (TotalNoise>CST)
        return 1;
else return 0;

}


double MAC802_11::getpower(Packet* p)
{
if (p==0) return 0;
else
{
 int id=p->machr.XmitterNodeID;
 int j=0;
  while(n[j].node_id!=id)
         j++;

  double d=sqrt((n[i].X-n[j].X)* (n[i].X-n[j].X)+ (n[i].Y-n[j].Y)* (n[i].Y-n[j].Y) +(n[i].Z-n[j].Z)* (n[i].Z-n[j].Z));
  //double Prdbm=(Pt-30-30.0*log10(d/0.1));
  //return pow(10,Prdbm/10)*1e-3;
  double Pt_watt= pow(10,Pt/10)*1e-3;
  return Pt_watt*pow((d/0.1),-3);
  }
}


void MAC802_11::idle()
{
	int TotalBufferOccupancy=0;
	int fi;
	for (fi=0;fi<F;fi++) TotalBufferOccupancy+=n[i].router.BufferOccupancy[fi];

	if(checkmedium()==0&&macstate==Idle&&TotalBufferOccupancy>0)

      {  sp->schedule(4,DIFS, n[i].node_id, 0);
         macstate=WaitingForDIFS;
       }

}


double MAC802_11::resetCW()
  { //srand ( time(NULL) );

   int x=rand()/double(RAND_MAX)*CWmin;
   if(x<0) x=0;
    CWprev= CWmin;
     return (double(x)*aSlotTime);
   }



double MAC802_11::IncreaseCW(int CWprev)
{ //srand ( time(NULL) );

 if(CWprev<CWmax)
       CWprev=CWprev*2+1;
  int x=rand()/double(RAND_MAX)*CWprev;
    if(x<0) x=0;
 return   (double(x)*aSlotTime);

}

void MAC802_11::sendDATA()
{    //srand ( time(NULL) );

    //newly added:flow and link (node selection)
	 double u=rand()/double(RAND_MAX);
	 int IndexID=n[i].router.FindSelectedIndex(u);
	 int FlowID=int(IndexID/N);
	 int NextNodeID=IndexID%N;
	 if (n[i].router.BufferOccupancy[FlowID]>0&&n[i].router.RouteCache[FlowID][NextNodeID]>0)
	   { //int datasize=546;//1058;
	      int datasize=1058;
	     n[i].router.pop(FlowID);
         Packet* Pmac=Packet::alloc(datasize);
         //Pmac->cmn.size=546;//1058;
         Pmac->cmn.size=1058;
         Pmac->cmn.ReceivedByReceiver=0;
         Pmac->cmn.dummy=0;
         Pmac->cmn.error_=0;
         if(DummyPkt2Send[NextNodeID][FlowID]==1) Pmac->cmn.dummy=1;
         Pmac->packet_type=DATA;
         Pmac->machr.flow_id=FlowID ;

         Pmac->machr.ReceiverNodeID=NextNodeID;
         Pmac->machr.XmitterNodeID=n[i].node_id;
         //Pmac->cmn.txtime=txtime(546,DataRate)+txtime(24,BasicRate);
         Pmac->cmn.txtime=txtime(1058,DataRate)+txtime(28,BasicRate);
         Pmac->packet_type=DATA;
         Pmac->machr.duration=SIFS+txtime(14,DataRate)+txtime(28,BasicRate);
         macstate=XmittingDATA;
        double timeout=txtime(Pmac)+MaxPropagationDelay+SIFS+ txtime(14,DataRate)+txtime(28,BasicRate)+ MaxPropagationDelay;
        transmit(Pmac,timeout);
        sp->schedule(19, timeout+SIFS,n[i].node_id,(unsigned long  int)Pmac); //to delete p;
        for(int j=0;j<N;j++)
          { if(n[j].node_id!=n[i].node_id)
           n[j].mac.recv(Pmac);
          }

	 }
	 else
	 {macstate=Idle;
      cw=resetCW();// newly added as if the selected flow is not available then choose new CW
	  idle();
	 }

}



void MAC802_11::sendACK(int ReceiverNodeID, int ReceivedFlow)
{
	call++;
   Packet* p=Packet::alloc();
   p->machr.XmitterNodeID=n[i].node_id;
   p->cmn.size=42;
   p->cmn.txtime=txtime(14,DataRate)+txtime(28,BasicRate);
   p->packet_type=ACK;
   p->machr.duration=0.0;

   if(call==1)
   {
	   p->machr.ReceiverNodeID= ReceiverNodeID;
	   p->machr.flow_id=ReceivedFlow;
	   if(macstate==WaitingToXmitACK)
	   {
		   macstate=XmittingACK;
		      double timeout=MaxPropagationDelay;
		      transmit(p,timeout);
		      for(int j=0;j<N;j++)
		          { if(n[j].node_id!=n[i].node_id)
		            n[j].mac.recv(p);
		           }
		      sp->schedule(19, txtime(p)+1.1*SIFS,n[i].node_id,(unsigned long  int)p); //to delete p;
		      call=0;
	   }
   }
   if(call==2)
   {
	   p->machr.ReceiverNodeID2= ReceiverNodeID;
	   p->machr.flow_id2=ReceivedFlow;

	   macstate=XmittingACK;
	      double timeout=MaxPropagationDelay;
	      transmit(p,timeout);
	      for(int j=0;j<N;j++)
	          { if(n[j].node_id!=n[i].node_id)
	            n[j].mac.recv(p);
	           }
	      sp->schedule(19, txtime(p)+1.1*SIFS,n[i].node_id,(unsigned long  int)p); //to delete p;
	      call=0;
   }
}


double MAC802_11::txtime(int PacketSize, double DataRate)
{
return (8.0*PacketSize/DataRate);
}

double MAC802_11::txtime(Packet* p)
{
return p->cmn.txtime;
}



void MAC802_11::transmit(Packet* p, double timeout)
 {
  tx_active=1;
 switch(p->packet_type)
   {
     case 0:
                 { sp->schedule(15,txtime(p),n[i].node_id,0);///Finishing data transmission
                   sp->schedule(16,timeout,n[i].node_id,(unsigned long  int)p);//Time out of reciving ack
                   break;
                  }
     case 1:
                { sp->schedule(18,txtime(p),n[i].node_id,0);///Finishing ack transmission
                 break;
                }
      default: exit(1);
     }

  }

void MAC802_11::RetransmitDATA(int NodeID, int FlowID,double Rate)
  {    macstate=Idle;
       LongRetry++;
       if(LongRetry>5)
          {
    	   LongRetry=0;
           cw=resetCW();
           //Pmac=0;
           ShortRetry=0;//newly added
           //TotalDroppedPkts[FlowID]++;//it is not drop for queue overflow
           //n[NodeID].router.BufferOccupancy[FlowID]--;/// free the buffer, ie, drop it
           double u=rand()/double(RAND_MAX);
           double delay =-1/Rate*log(u);
           sp->schedule(20,delay,NodeID,(unsigned long int)FlowID);// generate the same packet after a delay

          }
           else
        	   {cw=IncreaseCW(CWprev);
        	   sp->schedule(20,MaxPropagationDelay,NodeID,(unsigned long int)FlowID);// generate the same packet after a small delay
        	   }



        // idle();
  }


void MAC802_11::recv(Packet* p)
{
double Pr=getpower(p);

/*if(i==N-1){
for(int j=0;j<N-1;j++)
{
	if(n[j].mac.tx_active==1)
		cout<<p->machr.ReceiverNodeID<<" "<<p->machr.XmitterNodeID<<" "<<j<<endl;
}
cout<<endl;}*/
if(PktRecv1==0 && PktRecv2==0)
    { if(10*log10(Pr/(InterferencePower+NoisePower))<SINRThreshold(p))
         {InterferencePower+=Pr;
          sp->schedule(12,txtime(p),n[i].node_id,(unsigned long int)p);
          }
      else
          {PktRecv1=p;
            if(tx_active) PktRecv1->cmn.error_=1;
             sp->schedule(13,txtime(p),n[i].node_id,(unsigned long  int)p);
             ExpireTime=sp->get_clock()+txtime(p);
           }
     }

 else if(PktRecv1==0 || PktRecv2==0)
 {if(PktRecv2==0)
   { double Precv1=getpower(PktRecv1);
     if(10*log10(Precv1/(InterferencePower+NoisePower+Pr))<SINRThreshold(PktRecv1))
         { InterferencePower+=Precv1;
           //sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv);
           if(10*log10(Pr/(InterferencePower+NoisePower))<SINRThreshold(p))
               { InterferencePower+=Pr;
                  PktRecv1=0;
                  sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv1);
                  sp->schedule(12,txtime(p),n[i].node_id, (unsigned long  int)p);
                }
          else
              { PktRecv2=p;
               if(tx_active) PktRecv2->cmn.error_=1;
                sp->schedule(13,txtime(p),n[i].node_id,(unsigned long  int)p);
                ExpireTime=sp->get_clock()+txtime(p);

                InterferencePower-=Precv1;
                if(10*log10(Precv1/(InterferencePower+NoisePower))<SINRThreshold(PktRecv1))
                {
                	InterferencePower+=Precv1;
                	sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv1);
                }
                /*else
                {
                	if(tx_active) PktRecv1->cmn.error_=1;
                	sp->schedule(13,ExpireTime-sp->get_clock(),n[i].node_id,(unsigned long  int)PktRecv1);
                	ExpireTime=sp->get_clock()+txtime(p);
                }*/

               }
         }
     else
        {
    	 if(10*log10(Pr/(InterferencePower+NoisePower))<SINRThreshold(p))
    	 {
    		 InterferencePower+=Pr;
    		 sp->schedule(12,txtime(p),n[i].node_id,(unsigned long  int)p);
    	 }
    	 else
    	 {
    		PktRecv2=p;
			if(tx_active) PktRecv2->cmn.error_=1;
			sp->schedule(13,txtime(p),n[i].node_id,(unsigned long  int)p);
			ExpireTime=sp->get_clock()+txtime(p);
    	 }


        }
   }
 if(PktRecv1==0)
    { double Precv2=getpower(PktRecv2);
      if(10*log10(Precv2/(InterferencePower+NoisePower+Pr))<SINRThreshold(PktRecv2))
          { InterferencePower+=Precv2;
            //sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv);
            if(10*log10(Pr/(InterferencePower+NoisePower))<SINRThreshold(p))
                { InterferencePower+=Pr;
                   PktRecv2=0;
                   sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv2);
                   sp->schedule(12,txtime(p),n[i].node_id, (unsigned long  int)p);
                 }
           else
               { PktRecv1=p;
                if(tx_active) PktRecv1->cmn.error_=1;
                 sp->schedule(13,txtime(p),n[i].node_id,(unsigned long  int)p);
                 ExpireTime=sp->get_clock()+txtime(p);

                 InterferencePower-=Precv2;
                 if(10*log10(Precv2/(InterferencePower+NoisePower))<SINRThreshold(PktRecv2))
                 {
                 	InterferencePower+=Precv2;
                 	sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv2);
                 }
                 /*else
                 {
                 	if(tx_active) PktRecv2->cmn.error_=1;
                 	sp->schedule(13,ExpireTime-sp->get_clock(),n[i].node_id,(unsigned long  int)PktRecv2);
                 	ExpireTime=sp->get_clock()+txtime(p);
                 }*/

                }
          }
      else
         {
			 if(10*log10(Pr/(InterferencePower+NoisePower))<SINRThreshold(p))
			 {
				 InterferencePower+=Pr;
				 sp->schedule(12,txtime(p),n[i].node_id,(unsigned long  int)p);
			 }
			 else
			 {
				PktRecv1=p;
				if(tx_active) PktRecv1->cmn.error_=1;
				sp->schedule(13,txtime(p),n[i].node_id,(unsigned long  int)p);
				ExpireTime=sp->get_clock()+txtime(p);
			 }


         }
    }

 }
 else
 {
	 double Precv1=getpower(PktRecv1);
	 double Precv2=getpower(PktRecv2);
	 if(Precv1>Precv2)
	 {
		 if(10*log10(Precv1/(InterferencePower+Precv2+NoisePower+Pr))<SINRThreshold(PktRecv1))
		 {
			InterferencePower=InterferencePower+Precv1+Precv2;
			if(10*log10(Pr/(InterferencePower+NoisePower))<SINRThreshold(p))
			{
				InterferencePower+=Pr;
			   PktRecv1=0;
			   PktRecv2=0;
			   sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv1);
			   sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv2);
			   sp->schedule(12,txtime(p),n[i].node_id, (unsigned long  int)p);
			}
			else
			{
				//sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv2);
				PktRecv2=p;
				if(tx_active) PktRecv2->cmn.error_=1;
				sp->schedule(13,txtime(p),n[i].node_id,(unsigned long  int)p);
				ExpireTime=sp->get_clock()+txtime(p);

				InterferencePower-=Precv1;
				if(10*log10(Precv1/(InterferencePower+NoisePower))<SINRThreshold(PktRecv1))
				{
					InterferencePower+=Precv1;
					sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv1);
				}
				/*else
				{
					if(tx_active) PktRecv1->cmn.error_=1;
					sp->schedule(13,ExpireTime-sp->get_clock(),n[i].node_id,(unsigned long  int)PktRecv1);
					ExpireTime=sp->get_clock()+txtime(p);
				}*/
			}
		 }
		 else
		 {
			 if(10*log10(Pr/(InterferencePower+NoisePower+Precv2))<SINRThreshold(p))
			 {
				 InterferencePower+=Pr;
				 sp->schedule(12,txtime(p),n[i].node_id,(unsigned long  int)p);
				 if(10*log10(Precv2/(InterferencePower+NoisePower))<SINRThreshold(p))
				 {
					 InterferencePower+=Precv2;
					 sp->schedule(12,txtime(p),n[i].node_id,(unsigned long  int)PktRecv2);
				 }
			 }
			 else
			 {
				PktRecv2=p;
				if(tx_active) PktRecv2->cmn.error_=1;
				sp->schedule(13,txtime(p),n[i].node_id,(unsigned long  int)p);
				ExpireTime=sp->get_clock()+txtime(p);
			 }
		 }
	 }
	 //////////////
	 else
	 {
	 		 if(10*log10(Precv2/(InterferencePower+Precv1+NoisePower+Pr))<SINRThreshold(PktRecv2))
	 		 {
	 			InterferencePower=InterferencePower+Precv2+Precv1;
	 			if(10*log10(Pr/(InterferencePower+NoisePower))<SINRThreshold(p))
	 			{
	 				InterferencePower+=Pr;
	 			   PktRecv2=0;
	 			   PktRecv1=0;
	 			   sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv2);
	 			   sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv1);
	 			   sp->schedule(12,txtime(p),n[i].node_id, (unsigned long  int)p);
	 			}
	 			else
	 			{
	 				//sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv1);
	 				PktRecv1=p;
	 				if(tx_active) PktRecv1->cmn.error_=1;
	 				sp->schedule(13,txtime(p),n[i].node_id,(unsigned long  int)p);
	 				ExpireTime=sp->get_clock()+txtime(p);

	 				InterferencePower-=Precv2;
	 				if(10*log10(Precv2/(InterferencePower+NoisePower))<SINRThreshold(PktRecv2))
	 				{
	 					InterferencePower+=Precv2;
	 					sp->schedule(12,ExpireTime-sp->get_clock(),n[i].node_id, (unsigned long  int)PktRecv2);
	 				}
	 				/*else
	 				{
	 					if(tx_active) PktRecv2->cmn.error_=1;
	 					sp->schedule(13,ExpireTime-sp->get_clock(),n[i].node_id,(unsigned long  int)PktRecv2);
	 					ExpireTime=sp->get_clock()+txtime(p);
	 				}*/
	 			}
	 		 }
	 		 else
	 		 {
	 			 if(10*log10(Pr/(InterferencePower+NoisePower+Precv1))<SINRThreshold(p))
	 			 {
	 				 InterferencePower+=Pr;
	 				 sp->schedule(12,txtime(p),n[i].node_id,(unsigned long  int)p);
	 				 if(10*log10(Precv1/(InterferencePower+NoisePower))<SINRThreshold(p))
	 				 {
	 					 InterferencePower+=Precv1;
	 					 sp->schedule(12,txtime(p),n[i].node_id,(unsigned long  int)PktRecv1);
	 				 }
	 			 }
	 			 else
	 			 {
	 				PktRecv1=p;
	 				if(tx_active) PktRecv1->cmn.error_=1;
	 				sp->schedule(13,txtime(p),n[i].node_id,(unsigned long  int)p);
	 				ExpireTime=sp->get_clock()+txtime(p);
	 			 }
	 		 }
	 	 }
 }


    if(macstate==1)
                            if (checkmedium())
                         	   macstate=Idle;
   /* if(macstate==3)
                           if (checkmedium())
                          	 { if (NextDATAXmissionTime>(sp->get_clock()+aSlotTime))
                        	        macstate=Idle;
                          	   if (NextDATAXmissionTime<=(sp->get_clock()+aSlotTime)&&PktRecv!=0)
                        	     	 PktRecv->cmn.error_=1;
                          	   //else DontCare2Xmit=1;

                          	 }
 */
    if( macstate==2)
                  if (checkmedium())
                   { //macstate=Idle;
                	  double TxRxTime=0;//2.5e-6;
                	  double CCASlotBoundaryTime=aSlotTime-TxRxTime;
                    if((BackOffEndTime-sp->get_clock())>=CCASlotBoundaryTime)
                      {
                    	macstate=Idle;
                    	//double ContentionWindow= ceil((BackOffEndTime-sp->get_clock())/aSlotTime);// Since carrier sensing start at the starting of each slot                       cw=max(0,RemainingTime);
                    	//cw=abs(ContentionWindow)*aSlotTime;

                      }

                    if(((BackOffEndTime-sp->get_clock())<CCASlotBoundaryTime))
                     {
                    	if(PktRecv1!=0) PktRecv1->cmn.error_=1;
                    	if(PktRecv2!=0) PktRecv2->cmn.error_=1;
                    	/*if(PktRecv1!=0 && PktRecv1!=0)
                    	{
                    		PktRecv1->cmn.error_=1;
                    		PktRecv1->cmn.error_=1;
                    	}*/
                     }
                   }



  }


double MAC802_11::SINRThreshold(Packet* p)
 {// if(p->packet_type==0||p->packet_type==1)
   // return 12.0;
	//   return 3.4;
  // else return 15.0;
     //else
    	 return 6.4;
}



void MAC802_11::recv_timer(Packet* p)
  {
  if(PktRecv1==p || PktRecv2==p)
  {
	   if(p->cmn.error_)
           {//NavEndTime=sp->get_clock()+EIFS;
           goto done;
            }
         if(tx_active)
           {//drop(p,MacBusy )
            goto done;
            }
         else
             {
        	  switch(p->packet_type)
                 { case 0:
                        recvDATA(p);
                        break;
                 case 1:
                      recvACK(p);
                        break;
                 default:
                    exit(1);
                 }
             }
done:
	if(p==PktRecv1)
	{
		PktRecv1=0;
		idle();
	}
	if(p==PktRecv2)
	{
		PktRecv2=0;
		idle();
	}



     }

  }


void MAC802_11::recvDATA(Packet* p)
{    if(macstate==Idle && p->machr.ReceiverNodeID==n[i].node_id)
	  {//printf("\nReceived flow:%d \n",p->machr.flow_id);
	      sp->schedule(17,SIFS,n[i].node_id,(unsigned long  int)p);// xmitACK
	     macstate=WaitingToXmitACK;
         ProcessReceivedDATA(p);
         }
	else if((macstate==WaitingToXmitACK) && p->machr.ReceiverNodeID==n[i].node_id)
	  {//printf("\nReceived flow:%d \n",p->machr.flow_id);
	      sp->schedule(17,SIFS,n[i].node_id,(unsigned long  int)p);// xmitACK
	     macstate=WaitingToXmitACK2;
         ProcessReceivedDATA(p);
         }
  //EIFS implementation

   if(macstate==Idle && p->machr.ReceiverNodeID!=n[i].node_id)
   {
	   sp->schedule(18,EIFS,n[i].node_id,0);

   }

 }

void MAC802_11:: ProcessReceivedDATA(Packet* p)
{       p->cmn.ReceivedByReceiver=1;

       if(p->cmn.dummy==0)
        {

             AGTReceived[p->machr.flow_id]++;


    	   if (n[i].router.BufferOccupancy[p->machr.flow_id]<n[i].router.BufferSize)
               { n[i].router.push(p->machr.flow_id);
                 TotalRecvPkts[p->machr.flow_id]++;
               }
             //else drop(p,BufferOverflow)
              else TotalDroppedPkts[p->machr.flow_id]++;

         }


 }


void MAC802_11::recvACK(Packet* p)
{   if(macstate==WaitingForACK)
        { // printf("\nFlowid=%d",p->machr.flow_id);
		 if(p->machr.ReceiverNodeID==n[i].node_id)
		 {
				TotalXmitPkts[p->machr.flow_id]++;
				LongRetry=0;

				macstate=Idle;
				cw=resetCW();
				DummyPkt2Send[p->machr.XmitterNodeID][p->machr.flow_id]=0;
				idle();
		 }
		 else if(p->machr.ReceiverNodeID2==n[i].node_id)
		 {
		 				TotalXmitPkts[p->machr.flow_id2]++;
		 				LongRetry=0;

		 				macstate=Idle;
		 				cw=resetCW();
		 				DummyPkt2Send[p->machr.XmitterNodeID][p->machr.flow_id2]=0;
		 				idle();
		 		 }
          }

 }

void MAC802_11::CheckBackOff()
{
 if( macstate==BackingOff)
   cw=max(0,BackOffEndTime-sp->get_clock());
}
