#define WIN32_LEAN_AND_MEAN
#pragma once
#include "mkqueue.h"
#include "mkutil.h"

MKQ* mkq_create(int32_t sizemax)
{
	MKQ* q = new MKQ;
	q->sizemax   = (sizemax == -1) ? MAXINT : sizemax;
	q->sem_fill  = ::CreateSemaphore(NULL, 0, q->sizemax, NULL);
	q->sem_empty = ::CreateSemaphore(NULL, q->sizemax, q->sizemax, NULL);
	q->evt_stop  = ::CreateEvent    (NULL, TRUE, FALSE,NULL);
	::InitializeCriticalSection(&(q->cs));
	return q;
}

bool mkq_delete(PBYTE p, bool (*destroyer)(PBYTE))
{
	bool ret = false;
	if (p != NULL)
	{
		MKQ* q = (MKQ*) p;

		mkq_stop(q);

		::EnterCriticalSection(&(q->cs));
		while (!(q->ptr.empty()))
		{
			PBYTE ret = q->ptr.front();
			q->ptr.pop_front();
			if (destroyer != NULL)(*destroyer)(ret);
		}
		::LeaveCriticalSection(&(q->cs));

		::CloseHandle(q->sem_fill);
		::CloseHandle(q->sem_empty);
		::CloseHandle(q->evt_stop);
		::DeleteCriticalSection(&(q->cs));
		q->sizemax = -1;
		delete q;
		ret = true;
	}
	return ret;
}

PBYTE mkq_push(MKQ* q, PBYTE p, bool wait)
{
	PBYTE ret = NULL;
	if (q != NULL && p != NULL)
	{
		if (wait && GetEvent(q->evt_stop) == false)
		{
			HANDLE hs[] = { q->sem_empty, q->evt_stop };
			DWORD  sig = ::WaitForMultipleObjects(2, hs, false, INFINITE);
		}

		::EnterCriticalSection(&(q->cs));
		if ((q->sizemax > 0) && ((int32_t)(q->ptr.size()) > q->sizemax))
		{
			ret = q->ptr.front();
			q->ptr.pop_front();
		}
		q->ptr.push_back(p);
		::LeaveCriticalSection(&(q->cs));
		::ReleaseSemaphore(q->sem_fill, 1, NULL);  // inc sem
	}
	return ret;
}

PBYTE mkq_pop(MKQ* q, bool wait)
{
	PBYTE ret = NULL;
	if (q != NULL)
	{
		if (wait && GetEvent(q->evt_stop) == false)
		{
			HANDLE hs[] = { q->sem_fill, q->evt_stop };
			DWORD  sig = ::WaitForMultipleObjects(2, hs, false, INFINITE);
		}

		::EnterCriticalSection(&(q->cs));
		if (!(q->ptr.empty()))
		{
			ret = q->ptr.front();
			q->ptr.pop_front();
		}
		::LeaveCriticalSection(&(q->cs));
		::ReleaseSemaphore(q->sem_empty, 1, NULL);
	}
	return ret;
}

void mkq_stop(MKQ* q)
{
	::SetEvent(q->evt_stop);
}

PBYTE mkq_front(MKQ* q)
{
	PBYTE ret = NULL;
	if (!(q->ptr.empty()))
	{
		::EnterCriticalSection(&(q->cs));
		ret = q->ptr.front();
		::LeaveCriticalSection(&(q->cs));
	}
	return ret;
}

bool  mkq_empty(MKQ* q)
{
	return q->ptr.empty();
}

PBYTE mkq_nth(MKQ* q, uint32_t n)
{
	PBYTE ret = NULL;
	if ((q != NULL) && (!(q->ptr.empty())) && (n < q->ptr.size()))
	{
		::EnterCriticalSection(&(q->cs));
		ret = q->ptr.at(n);
		::LeaveCriticalSection(&(q->cs));
	}
	return ret;
}

bool  mkq_erase(MKQ* q, uint32_t n)
{
	bool ret = false;
	if ((q != NULL) && (!(q->ptr.empty())) && (n < q->ptr.size()))
	{
		::EnterCriticalSection(&(q->cs));
		q->ptr.erase(q->ptr.begin() + n);
		::LeaveCriticalSection(&(q->cs));
	}
	return ret;
}

bool mkq_clear(MKQ* q, bool (*destroyer)(PBYTE))
{	
	PBYTE item  = mkq_pop(q,false);
	while(item != NULL)
	{
		if (destroyer != NULL)(*destroyer)(item);
		item = mkq_pop(q,false);
	}	
	return true;
}

