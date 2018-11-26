//scheduler.h

#ifndef INC_scheduler_H
#define INC_scheduler_H
//#include "random.h"
 class node;
 class flow;
class Event {
 public:
 Event* next_;           /* event list */
 Event* prev_;
 double ex_time;           /* time at which event is ready */
 unsigned long int event_uid;
 unsigned long int event_type;
 unsigned long int first_data;
 unsigned long int second_data;
  };

class Scheduler{
node* n;
flow* f;
public:
Scheduler(node* a, flow* b);
static double clock_;
static long int event_uid;
//static long int packet_uid;
virtual Event* push_data(int event_type,double delay, int node_id,int packet_uid);
 static Event* EQ;
void schedule( int event_type,double delay, int node_id,int packet_uid);
virtual void insert(Event*);
virtual Event* pop();
static void  set_clock(double t){clock_=t;};
static double get_clock(){return clock_;};
void action(Event* e);
//double random rand_;
};
#endif
