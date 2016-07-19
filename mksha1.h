#pragma once
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include "mkutil.h"
using namespace std;

extern "C"
{	    
	extern void    sha1_compress (uint32_t state[5], uint8_t block[64]);
	void           sha1_hash     (uint8_t *message, uint32_t len, uint32_t hash[5]);
}
void sha1_print (PBYTE sha1);


