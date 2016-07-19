#pragma once
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <boost/container/vector.hpp>
using namespace std;

struct MKV
{	
	boost::container::vector<PBYTE> v;
	CRITICAL_SECTION cs;		
};

MKV*  mkv_create  ();
bool  mkv_delete  (PBYTE p);
void  mkv_in      (MKV* mkv);
void  mkv_out     (MKV* mkv);
void  mkv_clear   (MKV* mkv);


