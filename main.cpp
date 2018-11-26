/*
 * main.cpp
 *
 *  Created on: Oct 13, 2012
 *      Author: RKB
 */

// This version modified on August 06,2010. It simulates basic (No RTS/CTS) IEEE 802.11 with probabilistic flow and link selection.
//We can control CWmin of each node. After finishing back-off, a node choose a flow and then one of the nodes i.e. link for transmission

#include <iostream>
#include <fstream>
using namespace std;
#include "scheduler.h"
#include "node.h"
#include "flow.h"
//#include "random.h"
#include "math.h"
#include "ctime"
#define NumberOfIteration 1
#define NodeNumber 20
#define FlowNumber 38

int main()
{

	double sim_time;

	sim_time=10;
	cout<<"SimulationTime:"<<sim_time<<endl;
	//cin>>sim_time;


	// Iterations start
			//int NumberOfIteration=1;
			//cout<<"Iterations:"<<endl;
			//cin>>NumberOfIteration;
	        //double PktRate[2]={.01, .02};
			double PktRate[NumberOfIteration];
			int iter;
			for(iter=0;iter<NumberOfIteration;iter++)
			   {cout<<"PktRate:";//<<endl;
			     //cin>>PktRate[iter];
			   PktRate[iter]=10;
			   cout<<PktRate[iter]<<endl;
			   }

	for(iter=0;iter<NumberOfIteration;iter++)
	{

	   // INPUT Start
		int _NodeNumber;
		int _FlowNumber;
		ifstream Params("C:\\Users\\Rakib\\Downloads\\sic\\datacpp.dat", ios::in);

		Params>>_NodeNumber;
		Params>>_FlowNumber;

		double NodePosition[NodeNumber][3];
		int Source[FlowNumber];
		int Destination[FlowNumber];
		double TransmissionPower;
		double CarrierSensingThreshold;
		double SourceRate;
		double SimulationTime;
		int MinimumCW [NodeNumber];
		double RoutingParameters[NodeNumber][FlowNumber][NodeNumber];

		//Reading the other params
	   // NodePosition
		int i, j, k;

		for(i=0;i<NodeNumber;i++)
		 {
		 Params>>NodePosition[i][0];
		  }

		for(i=0;i<NodeNumber;i++)
		 {
		 Params>>NodePosition[i][1];
		  }
		for(i=0;i<NodeNumber;i++)
		  {
		  Params>>NodePosition[i][2];
		   }

		// Source and destination of the flows


		for(j=0;j<FlowNumber;j++)
		{
			 Params>>Source[j];
		}
		for(j=0;j<FlowNumber;j++)
		{
			 Params>>Destination[j];
		}

		//PHY
		Params>>TransmissionPower;
		Params>>CarrierSensingThreshold;
	    Params>>SourceRate;
		Params>>SimulationTime;

		for(i=0;i<NodeNumber;i++)
		  	  Params>>MinimumCW[i];


		for(i=0;i<NodeNumber;i++)
			for(j=0;j<FlowNumber;j++)
	    		for(k=0;k<NodeNumber;k++)
		 		  Params>>RoutingParameters[i][j][k];

		Params.close();

		cout<<"PT: "<<TransmissionPower<<endl;
		/*for(i=0;i<NodeNumber;i++)
					{for(j=0;j<FlowNumber;j++)
			    		{for(k=0;k<NodeNumber;k++)
				 		  cout<<RoutingParameters[i][j][k]<<" ";
			    		cout<<endl;}
					cout<<endl;}
		cout<<endl;*/


		// Input End


//sim_time=100;//SimulationTime;

static int N=NodeNumber;
static int F=FlowNumber;
node *nodes=new node[N];
flow *flows=new flow[F];

//double PktRate;
//cout<<"Packet Generation Rate"<<endl;
//cin>> PktRate;


int PG[FlowNumber];

int TotalPG=0;
srand ( time(NULL) );


for (j=0;j<F;j++)
{
	PG[j]=0;
flows[j].flow_id=j;
flows[j].source_id=Source[j]-1;
flows[j].dest_id=Destination[j]-1;
flows[j].packet_rate=PktRate[iter];//SourceRate;
//PG[j]=0;
}

for(i=0;i<N;i++)
{
	         nodes[i].node_id=i;
	         nodes[i].X=NodePosition[i][0];
	         nodes[i].Y=NodePosition[i][1];
	         nodes[i].Z=NodePosition[i][2];
}

/// RoutingParameters should be same as route cache
for(i=0;i<N;i++)
		for(j=0;j<F;j++)
    		for(k=0;k<N;k++)
	 		    nodes[i].router.RouteCache[j][k]= RoutingParameters[i][j][k];

//for(k=0;k<N;k++)
//cout<<nodes[1].router.RouteCache[0][k]<<endl;
//for(k=0;k<N;k++)
//cout<<nodes[1].router.RouteCache[1][k]<<endl;

// MAC and PHY controlling

for(i=0;i<N;i++)
{nodes[i].mac.CWmin=MinimumCW[i];
nodes[i].mac.CWprev=MinimumCW[i];
nodes[i].mac.CST=CarrierSensingThreshold;
nodes[i].mac.Pt=TransmissionPower;

}


Scheduler s(nodes,flows);
 s.EQ=s.push_data(1,0,N,F);/// First event
 s.EQ->next_=0;

for(i=0;i<N;i++)
{nodes[i].mac.setscheduler(&s);
 nodes[i].mac.setnode(nodes,flows,i,N,F);
 nodes[i].router.SetFlowNode(F,N);
 }



ofstream fout("test2");

while(s.get_clock()<=sim_time)
{
Event* e=s.pop();



//if (e->first_data==1)

fout<<""<<endl;
fout<<e->first_data<<'\t';
fout<<e->event_type<<'\t';
fout<<e->ex_time<<'\t';

if(e->first_data==0||e->first_data==1)
fout<<nodes[e->first_data].mac.macstate<<'\t';
fout<< (nodes[e->first_data].mac.cw)/20.0e-6<< '\t';
fout<< nodes[e->first_data].mac.AGTReceived[e->first_data]<< '\t';
fout<<endl;
//}

/*
int fi=0;
//for (int fi=0;fi<F;fi++)
{
fout<< nodes[e->first_data].mac.TotalRecvPkts[fi]<< '\t';
  fout<< nodes[e->first_data].mac.TotalXmitPkts[fi]<< '\t';
  fout<<nodes[e->first_data].router.BufferOccupancy[fi]<<'\t';
  fout<<nodes[e->first_data].mac.TotalDroppedPkts[fi]<<'\t';
}
fout<<endl;
}

*/

//cout<<e->first_data<<'\t';

//if (int(e->ex_time)%2==0)
	//{cout<<""<<endl;
	// cout<<e->ex_time<<'\t';
//	}

//cout<<nodes[e->first_data].mac.cw<<endl;
//cout<<nodes[e->first_data].mac.checkmedium()<<'\t';
//cout<<e->event_type<<'\t';

//for (i=0;i<N;i++)
//cout<<nodes[i].mac.cw<<'\t';

//cout<<endl;

if(e->event_type==2)
       {  PG[e->second_data]++;
         TotalPG++;
        }
/*

//if (e->event_type==13)
{
fout<<""<<endl;
fout<<e->first_data<<'\t';
fout<<e->ex_time<<'\t';
fout<<e->event_type<<endl;
}

fout<<nodes[0].mac.AGTReceived<<'\t';
fout<<TotalPG<<endl;

for(i=0;i<N;i++)
 fout<<nodes[i].mac.macstate<<'\t';
 fout<<""<<endl;

 for(i=0;i<N;i++)
   fout<<nodes[i].router.BufferOccupancy<<'\t';
   fout<<""<<endl;
 for(i=0;i<N;i++)
 fout<< nodes[0].mac.RecvPkts[i]<< '\t';
  fout<<""<<endl;


for(i=0;i<N;i++)
   fout<<nodes[i].mac.getpower(nodes[i].mac.PktInter)<<'\t';
   fout<<""<<endl;
for(i=0;i<N;i++)
   fout<<nodes[i].mac.checkmedium()<<'\t';
   fout<<""<<endl;

for(i=0;i<N;i++)
   fout<<nodes[i].mac.cw<<'\t';
   fout<<""<<endl;

for(i=0;i<N;i++)
   fout<<nodes[i].mac.BackOffEndTime<<'\t';
   fout<<""<<endl;

for(i=0;i<N;i++)
   fout<<nodes[i].mac.NavEndTime<<'\t';
   fout<<""<<endl;

for(i=0;i<N;i++)
   fout<<nodes[i].mac.ShortRetry<<'\t';
   fout<<""<<endl;
for(i=0;i<N;i++)
   fout<<nodes[i].mac.LongRetry<<'\t';
   fout<<""<<endl;
 */
s.action(e);


delete e;
  for(i=0;i<N;i++)
   {nodes[i].mac.CheckBackOff();
   }


}

//cout<<nodes[0].mac.AGTReceived<<endl;

//cout<<TotalPG<<endl;


for(j=0;j<F;j++)
 {cout<<j<<'\t'<< PG[j]<< '\t';
 cout<< nodes[flows[j].dest_id].mac.AGTReceived[flows[j].source_id]<< " ";

 cout<<""<<endl;}

 cout<<""<<endl;

int Stable=1;
 for(i=0;i<N;i++)
 {
  for(int f=0;f<F;f++)
    {
	  //if((nodes[i].mac.TotalRecvPkts[f]>0)&&(nodes[i].mac.TotalXmitPkts[f]>0)&&(nodes[i].mac.TotalRecvPkts[f]-nodes[i].mac.TotalXmitPkts[f])>0)
		//   cout<< float(nodes[i].mac.TotalRecvPkts[f]-nodes[i].mac.TotalXmitPkts[f])/float(nodes[i].mac.TotalRecvPkts[f])<<'\t';
  //fout<< i<<'\t'<<nodes[i].mac.TotalRecvPkts[f]<< '\t';
  //fout<< nodes[i].mac.TotalXmitPkts[f]<< '\t';
  //fout<<nodes[i].router.BufferOccupancy[f]<<'\t';
  //fout<<nodes[i].mac.TotalDroppedPkts[f]<<'\t';
  //cout<<f<<'\t';
  //cout<<Destination[f]<<endl;
   //cout<<endl;
	 float LossRate;
	 LossRate=float(nodes[i].mac.TotalDroppedPkts[f])/float(nodes[i].mac.TotalRecvPkts[f]);
	 if(LossRate>.01&&(Destination[f]-1)!=i) Stable=0;
	 //fout<<endl;
    }
fout<<""<<endl;
 }
 cout<<""<<endl;
 cout<<"StableRate:";
 if (Stable==1)cout<<PktRate[iter]<<endl;
   cout<<""<<endl;
delete nodes;
delete flows;

} // end iterations


int value;
cout<<"end simulation"<<endl;
cin>>value;

return 0;
}

