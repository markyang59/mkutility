#define WIN32_LEAN_AND_MEAN
#pragma once
#include "mkmap.h"

MKMAP* mkmap_create()
{	
	MKMAP* m = new MKMAP;	
	::InitializeCriticalSection(&(m->cs));
	return m;
}

int mkmap_delete(MKMAP* m)
{	
	::EnterCriticalSection (&(m->cs));
	m->pair.clear();
	::LeaveCriticalSection (&(m->cs));
	::DeleteCriticalSection(&(m->cs));			
	return 1;
}

void mkmap_put (MKMAP* m,PBYTE p,bool b)
{	
	::EnterCriticalSection(&(m->cs));
	m->pair[p] = b;
	::LeaveCriticalSection(&(m->cs));	
}

// true : valid  false: invalid/unknown
bool mkmap_get (MKMAP* m,PBYTE p)
{
	bool ret = false;	
	::EnterCriticalSection(&(m->cs));	
	if(m->pair.find(p) != m->pair.end())
	{
		ret = m->pair[p];
	}
	::LeaveCriticalSection(&(m->cs));
	return ret;
}

int  mkmap_erase (MKMAP* m,PBYTE p)
{
	::EnterCriticalSection(&(m->cs));
	auto it = m->pair.find(p);
	if(it != m->pair.end())
	{
		m->pair.erase(it);
	}
	::LeaveCriticalSection(&(m->cs));
	return 0;
}

