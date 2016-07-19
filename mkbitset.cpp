#define WIN32_LEAN_AND_MEAN
#pragma once
#include "mkbitset.h"


MKBITSET* mkbitset_create (uint32_t size)
{
	MKBITSET* bs   = NULL;	
	ldiv_t    rst;

	bs       = new MKBITSET;
	bs->size = size;
	rst      = ldiv(size,32);
	bs->bnum = rst.quot + ((rst.rem > 0) ? 1 : 0);
	bs->bits = new uint32_t[bs->bnum];	
	memset(bs->bits,0,bs->bnum*4);    // set 0 for default
	return bs;
}

bool  mkbitset_delete (PBYTE p)
{
	bool ret = false;
	if (p != NULL)
	{
		MKBITSET* bs = (MKBITSET*)p;
		delete bs->bits;
		delete bs;
		ret = true;
	}
	return ret;
}

int32_t   mkbitset_get    (MKBITSET* bs,int32_t pos)
{
	ldiv_t   rst;
	uint32_t index  = 0;
	uint32_t offset = 0;
	uint32_t val    = 0;
	uint32_t mask   = 0;
	int32_t  ret    = 0;

	rst    = ldiv(pos,32);
	index  = rst.quot;
	offset = rst.rem;
	val    = bs->bits[index];
	mask   = 0x80000000 >> offset;
	ret    = ((val & mask)>0) ? 1 : 0;
	
	return ret;
}

void      mkbitset_set    (MKBITSET* bs,int32_t pos,uint8_t newval)
{
	ldiv_t   rst;
	uint32_t index  = 0;
	uint32_t offset = 0;
	uint32_t val    = 0;
	uint32_t mask   = 0;
	
	if(pos>=0 && pos<(int32_t)(bs->size))
	{
		rst    = ldiv(pos,32);
		index  = rst.quot;
		offset = rst.rem;
		val    = bs->bits[index];
		mask   = 0x80000000 >> offset;
		switch(newval)
		{
			case  0: val = val & (~mask);break;
			case  1: val = val |   mask ;break;
			default: val = 0x00; break;
		}
		bs->bits[index] = val;
	}
}

void      mkbitset_clear  (MKBITSET* bs,uint8_t newval)
{
	if (bs != NULL && (newval == 0 || newval == 1))
	{
		uint32_t bv = (newval==1) ? 0xFFFFFFFF : 0x00000000;
		for (uint32_t i = 0; i < bs->bnum; i++)
		{
			bs->bits[i] = bv;
		}
	}
}

int32_t   mkbitset_all    (MKBITSET* bs,uint8_t newval)
{
	int32_t  ret    = 0;
	ldiv_t   rst;
	uint32_t index  = 0;
	uint32_t offset = 0;
	uint32_t i      = 0;	
	uint32_t val    = 0;
	uint32_t mask   = 0;
	uint32_t bcmp   = 0;
	
	ret    = 1;
	rst    = ldiv(bs->size,32);
	index  = rst.quot;
	offset = rst.rem;	
	i      = 0;
	
	for(i=0;i<index;i++)
	{
		if(bs->bits[i] != ((newval==1) ? 0xFFFFFFFF : 0x00000000))
		{
			ret = 0;
			break;
		}				
	}

	if(ret==1)
	{
		val  = bs->bits[index];
		mask = 0x80000000; 
		for(i=0;i<offset;i++)
		{
			bcmp = val & mask;			
			if (!((newval==0 && bcmp==0)||(newval==1 && bcmp!=0)))									
			{
				ret = 0;
				break;
			}
			mask = mask >> 1;
		}
	}
	return  ret;
}

int32_t  mkbitset_first  (MKBITSET* bs,int32_t start,uint8_t newval)
{
	int32_t  ret    =-1;	
	ldiv_t   rst;
	uint32_t index  = 0;
	uint32_t offset = 0;
	uint32_t val    = 0;
	uint32_t mask   = 0;	
	uint8_t  bv     = 0;
		
	ret    = -1;
	if(start>=0 && start< (int32_t)(bs->size))
	{
		rst    = ldiv(start,32);
		index  = rst.quot;
		offset = rst.rem;
		val    = bs->bits[index];

		for(int32_t i=start;i<(int32_t)(bs->size);i++)
		{						
			mask  =  0x80000000 >> offset;
			bv    =  ((val & mask)>0) ? 1 : 0;
			if(bv == newval)
			{
				ret = i;
				break;
			}

			offset++;
			if(offset>=32)
			{
				index++;
				offset=0;
				val = bs->bits[index];
			}
		}
	}
	return ret;
}


void mkbitset_print (MKBITSET* bs)
{
	for(uint32_t i=0;i<bs->size;i++)
	{
		cout<<"["<<i<<"] : "<<(int)(mkbitset_get(bs,i));
	}
}



int32_t mkbitset_encode (MKBITSET* bs,PBYTE* buf,uint32_t* len)
{
	int32_t  ret   = 0;
	PBYTE    curr  = NULL;
	PBYTE    bin   = NULL;
	uint32_t size  = 0;

	try
	{
		if (bs == NULL) throw 1;
		size = 4 + 4 + (bs->bnum) * 4;
		bin = new BYTE[size]; if (bin == NULL) throw 2;
		curr = bin;

		memcpy(curr, &(bs->size), 4); curr += 4;
		memcpy(curr, &(bs->bnum), 4); curr += 4;
		memcpy(curr, bs->bits, (bs->bnum) * 4);
	}
	catch (int32_t e)
	{
		ret = e;
	}

	*buf = bin;
	*len = size;
	return ret;
}

MKBITSET* mkbitset_decode (PBYTE buf,uint32_t len)
{
	MKBITSET* bs   = NULL;
	PBYTE     curr = buf;

	if(buf!=NULL && len>0)
	{
		bs = new MKBITSET;
		memcpy(&(bs->size),curr,4); curr+=4;
		memcpy(&(bs->bnum),curr,4); curr+=4;
		memcpy(bs->bits,curr,bs->bnum*4); curr+=(bs->bnum*4);
	}

	return bs;
}
