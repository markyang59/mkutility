#ifndef __MKGC__
#define __MKGC__

#pragma once
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <process.h>
#include "mkutil.h"
#include "mkqueue.h"
using namespace std;

struct MKGC
{
	MKQ*     q; 
	HANDLE   th_collector;		
	HANDLE   evt_stop;
	uint64_t delay;       // millisec : every garbage wait delay before delete
};

struct MKGCITEM
{
	PBYTE p;
	uint64_t ct_push;
	bool (*destroyer)(PBYTE);
};

MKGC*    mkgc_create (uint64_t delay);
bool     mkgc_delete (PBYTE p);
int32_t  mkgc_push   (MKGC* gc,PBYTE g,bool (*destroyer)(PBYTE));
static uint32_t WINAPI mkgc_thread_collector (LPVOID param);

#endif