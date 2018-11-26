#ifndef INC_packet_H
#define INC_packet_H

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


//#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

enum PacketType{//RTS,
                //CTS,
                DATA,
                ACK
                };


struct hdr_cmn {
         int     size;          // simulated packet size
         int     packet_uid;           // unique id
         int     error_;         // error flag
         double txtime;
         int dummy;
         int ReceivedByReceiver;
            };

 struct hdr_mac {
          int source_id;
          int dest_id;
          int flow_id;
          int flow_id2;
          double duration;
          int XmitterNodeID;
          int ReceiverNodeID;
          int ReceiverNodeID2;
         };

class Packet{
//private:
 //unsigned char* bits_;   // header bits
  //unsigned char* data_;   // variable size buffer for 'data'
  //unsigned int datalen_;  // length of variable size buffer
 static void init(Packet*);     // initialize pkt hdr
 ///int fflag_;
public:
  Packet* next_;          // for queues and the free list
  // static int hdrlen_;
   static inline Packet* alloc();
   static inline Packet* alloc(int);
   //inline void allocdata(int);
   //inline Packet* copy() const;
   static inline void free(Packet*);
   unsigned long int packet_uid;
  static long int packet_cid;
   PacketType packet_type;
   struct hdr_cmn cmn;
   struct hdr_mac machr;
   //protected:
  static Packet* free_;

};



 inline void Packet::init(Packet* p)
 {
        // bzero(p->bits_, hdrlen_);
         p->cmn.packet_uid=p->packet_uid;
          p->cmn.error_=0;
         // p->cmn.direction_=-1;

 }


inline Packet* Packet::alloc()
{
       Packet* p = free_;
        if (p != 0) {
           // assert(p->fflag_ == 0);
                 free_ = p->next_;
               //  assert(p->data_ == 0);

        } else {
	       p = new Packet;
                //p->bits_ = new unsigned char[hdrlen_];
               // if (p == 0 || p->bits_ == 0)
                 //        abort();
        }
         p->packet_uid =packet_cid++;

        init(p); // Initialize bits_[]
         // p->fflag_ = 1;//TRURE

     p->next_ = 0;
        return (p);


}

inline Packet* Packet::alloc(int n)
 {
         Packet* p = alloc();
        //  p->datalen_ = n;
         //if (n > 0)
           //     p->data_=new unsigned char[n];
             //    else
              // p->data_ = NULL;
         return (p);
 }

inline void Packet::free(Packet* p)
 {
         //if (p->fflag_) {


                         //assert(p->packet_uid <= 0);
                        // Delete user data because we won't need it any more.
                       // if (p->data_ != 0) {
                         //        delete p->data_;
                           //      p->data_ = 0;
                         //}
                         init(p);
                         p->next_ = free_;
                         free_ = p;
                         //p->fflag_ = 0;

 //}
 }

#endif
