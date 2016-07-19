#define WIN32_LEAN_AND_MEAN
#pragma once
#include "mkvector.h"

MKV* mkv_create()
{	
	MKV* mkv = new MKV;	
	::InitializeCriticalSection(&(mkv->cs));
	return mkv;
}

bool mkv_delete(PBYTE p)
{	
	bool ret = false;
	if (p != NULL)
	{
		MKV* mkv = (MKV*)p;
		::EnterCriticalSection(&(mkv->cs));
		mkv->v.clear();
		::LeaveCriticalSection(&(mkv->cs));
		::DeleteCriticalSection(&(mkv->cs));
		delete mkv;
		ret = true;
	}
	return ret;
}

void mkv_in (MKV* mkv)
{
	::EnterCriticalSection(&(mkv->cs));
}
void mkv_out(MKV* mkv)
{
	::LeaveCriticalSection(&(mkv->cs));
}
void mkv_clear(MKV* mkv)
{
	::EnterCriticalSection (&(mkv->cs));
	mkv->v.clear();
	::LeaveCriticalSection (&(mkv->cs));
}

