#include"router.h"
#include <stdio.h>


int Router::BufferSize=10000;

Router::Router()
{


int j;
int i;
for (j=0;j<100;j++)
  {	PQ[j]=new Packet;
    PQ[j]=0;
    BufferOccupancy[j]=0;
    for (i=0;i<100;i++)
		RouteCache[j][i]=0;
   }
}


void Router::SetFlowNode(int F,int N)
{NumberofFlows=F;
NumberofNodes=N;
}

/*
void Router::push(Packet* p)
{ int FlowID=p->machr.flow_id;

 if(PQ[FlowID]==0)
  { PQ[FlowID]=p;
   }
 else
  {Packet* q=PQ[FlowID];
  int BO;
  for(BO=1;BO<BufferOccupancy[FlowID];BO++)
    q=q->next_;
     q->next_=p;
 }
BufferOccupancy[FlowID]=BufferOccupancy[FlowID]+1;
}
*/

void Router::push(int FlowID)
{
 BufferOccupancy[FlowID]++;
}

/*
void Router::pushforward(Packet* p)
{int FlowID=p->machr.flow_id;
 p->next_=PQ[FlowID];
 PQ[FlowID]=p;
 BufferOccupancy[FlowID]=BufferOccupancy[FlowID]+1;
}
*/

void Router::pushforward(int FlowID)
{
 BufferOccupancy[FlowID]++;
}


int Router::FindSelectedIndex(double RandomValue)
{int j;
int Index=0;
int length=NumberofFlows*NumberofNodes;
double sum=0.0;
//printf("\n Rand: %d; length:%d \n",RandomValue, length);

for (j=0;j<length;j++)
  {  int Flow=int(j/NumberofNodes);
    int Node=j%NumberofNodes;

   /* if(RouteCache[Flow][Node]>0)
    	{printf("\n Node:%d \n",this);
    	printf("\n Flow:%d \n",Flow);
    	printf("\n Node:%d \n", Node);
    	printf("\n RC:%d \n", RouteCache[Flow][Node]);
    	}
*/
	  if(sum<=RandomValue&&RouteCache[Flow][Node]!=0)Index=j;
	  sum+=RouteCache[Flow][Node];

   }
//printf("\n Index:%d \n", Index);

return Index;
}

/*
Packet* Router::pop(int FlowID)
{Packet* p=PQ[FlowID];
PQ[FlowID]=PQ[FlowID]->next_;
p->next_=0;
BufferOccupancy[FlowID]=BufferOccupancy[FlowID]-1;
;
return p;
}
*/


void Router::pop(int FlowID)
{BufferOccupancy[FlowID]--;
}

/*
int Router::nexthop(int FlowID)
{int i=0;
  while(RouteCache[FlowID][i]!=1) {
        i++;
       // if (i>=30) printf("\nFlowID=%d\n",FlowID);

  }

  return i;
}
*/
