#pragma once
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <boost/unordered_map.hpp>
using namespace std;

//-- Thread Safe Map based on boost
struct MKMAP
{	
	boost::unordered_map<PBYTE,bool>  pair;		
	CRITICAL_SECTION cs;	
};

MKMAP* mkmap_create ();
int    mkmap_delete (MKMAP* m);
void   mkmap_put    (MKMAP* m,PBYTE p,bool b);
bool   mkmap_get    (MKMAP* m,PBYTE p);
int    mkmap_erase  (MKMAP* m,PBYTE p);
