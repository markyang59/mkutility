#pragma once
#ifndef __MK_QUEUE__
#define __MK_QUEUE__

#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <boost/container/deque.hpp>
using namespace std;

//- Thread Safe queue
//- Semaphhore based
struct MKQ
{	
	boost::container::deque<PBYTE>    ptr;		
	int32_t                       sizemax;  // -1 :unlimited
	CRITICAL_SECTION                   cs;	
	HANDLE                       sem_fill;  // fill count
	HANDLE                      sem_empty;  // empty
	HANDLE                       evt_stop;  // stop work, end wait state
};

MKQ*   mkq_create   (int32_t sizemax = -1);                   // Create with size
bool   mkq_delete   (PBYTE p, bool (*destroyer)(PBYTE)=NULL); // delete queue and destroy items
PBYTE  mkq_push     (MKQ*  q, PBYTE p,bool wait=false);       // When full  wait until avail or just return null
PBYTE  mkq_pop      (MKQ*  q, bool wait=false);               // When empty wait until avail or just return null
void   mkq_stop     (MKQ*  q);                                // If it is in wait state. stop waiting/working
PBYTE  mkq_front    (MKQ*  q);
bool   mkq_empty    (MKQ*  q);                                // queue is empty or not 
PBYTE  mkq_nth      (MKQ*  q, uint32_t n);                    // return n th element
bool   mkq_erase    (MKQ*  q, uint32_t n);                    // erase  n th element
bool   mkq_clear    (MKQ*  q, bool (*destroyer)(PBYTE)=NULL); // clear queue and destroy items

#endif