

#include"packet.h"

//int Packet::hdrlen_ = 32;                // size of a packet's header
Packet* Packet::free_=0;                  // free list
long int Packet::packet_cid=1;
 //int hdr_cmn::offset_=0;                   // static offset of common header
// int hdr_flags::offset_=0;                 // static offset of flags header*/

