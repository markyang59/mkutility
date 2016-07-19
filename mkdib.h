#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
using namespace std;

//-- DIB section
struct DIB
{
	int32_t   w;     
	int32_t   h;		
	HDC       dc;    
	void*     p;     
	HBITMAP   bmp;   	
	bool      flipy; 
	bool      fill;  
	uint64_t  pts;   
	uint64_t  framenum;	
	int32_t   var;  // aux value
};

DIB*  dib_create    (int32_t w,int32_t h);
bool  dib_delete    (PBYTE p);
DIB*  dib_duplicate (DIB*  d);
void  dib_replace   (DIB*  d,DIB* s);

