#define WIN32_LEAN_AND_MEAN
#pragma once
#include "mkgc.h"

MKGC* mkgc_create (uint64_t delay)
{
	MKGC* gc = new MKGC;
	gc->q = mkq_create();
	gc->evt_stop     = CreateEvent(NULL,TRUE,FALSE,NULL);	
	gc->th_collector = (HANDLE)_beginthreadex(NULL,0,mkgc_thread_collector,gc,0,NULL);
	gc->delay        = delay;
	return gc;
}

bool mkgc_delete (PBYTE p)
{	
	bool ret = false;
	if (p != NULL)
	{
		MKGC* gc = (MKGC*)p;
		mkq_stop  (gc->q);
		::SetEvent(gc->evt_stop);		
		::WaitForSingleObjectEx(gc->th_collector, INFINITE, TRUE);

		mkq_delete((PBYTE)(gc->q));
		gc->q = NULL;
		::CloseHandle(gc->evt_stop);
		::CloseHandle(gc->th_collector);
		delete gc;
		ret = true;
	}
	return ret;
}

int32_t mkgc_push(MKGC* gc, PBYTE g, bool(*destroyer)(PBYTE))
{
	int32_t ret = 1;
	if (gc != NULL && g != NULL)
	{
		MKGCITEM* item = new MKGCITEM;
		item->p = g;
		item->destroyer = destroyer;
		item->ct_push = cputime_get();
		mkq_push(gc->q, (PBYTE)item);
	}
	return 0;
}


static uint32_t WINAPI mkgc_thread_collector (LPVOID param)
{
	MKGC*     gc      = (MKGC*)param;
	MKGCITEM* qitem   = NULL;
	uint64_t  ct_curr = 0;
	int64_t   ct_diff = 0;
	int64_t   wt      = 0;
	
	while(GetEvent(gc->evt_stop)==false)
	{			
		qitem = (MKGCITEM*) mkq_pop (gc->q,true);		
		while(qitem != NULL)
		{
			ct_curr = cputime_get();
			ct_diff = (ct_curr - (qitem->ct_push)) / 1000;  // ms
			wt = bound64(gc->delay - ct_diff,0,gc->delay);
			::WaitForSingleObjectEx(gc->evt_stop,(DWORD)wt,TRUE);

			(*(qitem->destroyer))((PBYTE)qitem->p);
			delete qitem;
			qitem = (MKGCITEM*) mkq_pop (gc->q,false);
		}		
	}
	return 0;
}