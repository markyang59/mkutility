// MKBITSET
// ultra fast and efficient bit set
// Written by
// Mark Yang   
// 2015.1.4
// mkyang@mediamtd.com

#pragma once
#include <Windows.h>
#include <stdint.h>
#include <iostream>
using namespace std;


struct MKBITSET
{
	uint32_t  size;  // number of BIT
	uint32_t  bnum;  // number of 32 bit blocks
	uint32_t* bits;  // 32 bit block array	
};

MKBITSET* mkbitset_create (uint32_t size);
bool      mkbitset_delete (PBYTE p);
int32_t   mkbitset_get    (MKBITSET* bs,int32_t pos);
void      mkbitset_set    (MKBITSET* bs,int32_t pos,uint8_t val);
void      mkbitset_clear  (MKBITSET* bs,uint8_t val);
int32_t   mkbitset_all    (MKBITSET* bs,uint8_t val);
int32_t   mkbitset_first  (MKBITSET* bs,int32_t pos,uint8_t val); // serach val from pos to end
void      mkbitset_print  (MKBITSET* bs);
int32_t   mkbitset_encode (MKBITSET* bs,PBYTE* buf,uint32_t* len);
MKBITSET* mkbitset_decode (PBYTE     buf,uint32_t len);

