#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
using namespace std;

struct MKBUF
{
	uint32_t  len;   // buf length
	PBYTE     buf;   // buf pointer
	uint64_t  pts;   
	uint64_t  framenum;	
	int32_t   var;   // aux value
};
MKBUF*  mkbuf_create (uint32_t len,PBYTE buf);
bool    mkbuf_delete (PBYTE  p);
int32_t mkbuf_put    (MKBUF* b,uint32_t len,PBYTE buf); // write into buf return awrite
int32_t mkbuf_get    (MKBUF* b,uint32_t len,PBYTE buf); // read  from buf,len 0 is all
MKBUF*  mkbuf_merge  (MKBUF* a,MKBUF* b,uint32_t span); // interleave packed merge