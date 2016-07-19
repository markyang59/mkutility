#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
using namespace std;

struct MKSTREAMBUFFER
{
	int32_t  len;       // MAX number of element
	PBYTE    buf;
	PBYTE    start;
	PBYTE    end;
	PBYTE    head;
	PBYTE    tail;
	CRITICAL_SECTION cs;
};
MKSTREAMBUFFER* mksb_create (int32_t len);  // element number
void            mksb_delete (MKSTREAMBUFFER* sb);
int32_t         mksb_put    (MKSTREAMBUFFER* sb, int32_t len,PBYTE  buf); // return actual write
int32_t         mksb_get    (MKSTREAMBUFFER* sb, int32_t len,PBYTE* buf); // return actual read
void            mksb_clear  (MKSTREAMBUFFER* sb);
void            mksb_skip   (MKSTREAMBUFFER* sb, int32_t len,PBYTE  buf); // skip until pattern appear