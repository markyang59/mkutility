#include "mkstreambuffer.h"

MKSTREAMBUFFER* mksb_create (int32_t len)
{
	MKSTREAMBUFFER* sb = NULL;
	if(len>0)
	{
		sb = new MKSTREAMBUFFER;
		sb->len   = len;
		sb->buf   = new BYTE[sb->len];
		sb->start = sb->buf;
		sb->end   = sb->buf + sb->len;
		sb->head  = sb->buf;
		sb->tail  = sb->buf;
		InitializeCriticalSection(&(sb->cs));
	}
	return sb;
}

void mksb_delete (MKSTREAMBUFFER* sb)
{
	if (sb != NULL)
	{
		DeleteCriticalSection(&(sb->cs));
		delete sb->buf;
		delete sb;
	}
}

int32_t mksb_put (MKSTREAMBUFFER* sb, int32_t len,PBYTE buf)
{
	::EnterCriticalSection(&(sb->cs));

	int32_t  avail    = (int32_t)(sb->end  - sb->tail); // avail  sample num
	int32_t  filled   = (int32_t)(sb->tail - sb->head); // filled sample num	
	int32_t  shiftval = 0;
	int32_t  ret      = 0;
	
	// make more space..shift to start
	if (avail < len)
	{
		memcpy(sb->start, sb->head, filled);
		sb->head = sb->start;
		sb->tail = sb->head + filled;
		avail = (int32_t)(sb->end - sb->tail);

		// if still need more space than drop oldest
		if (avail < len)
		{
			shiftval = len - avail;
			sb->head += shiftval;
			filled = (int32_t)(sb->tail - sb->head);

			memcpy(sb->start, sb->head, filled);
			sb->head = sb->start;
			sb->tail = sb->head + filled;
			avail = (int32_t)(sb->end - sb->tail);
		}
	}

	if (avail >= len)
	{
		memcpy(sb->tail, buf, len);
		sb->tail += len;
		avail = (int32_t)(sb->end - sb->tail);
		ret = len;
	}

	::LeaveCriticalSection(&(sb->cs));

	return ret;
}

int32_t mksb_get (MKSTREAMBUFFER* sb, int32_t len,PBYTE* buf)
{
	::EnterCriticalSection(&(sb->cs));

	int32_t filled = (int32_t)(sb->tail - sb->head);
	int32_t aread  = min (len,filled); 

	*buf = sb->head;
	sb->head += aread;

	::LeaveCriticalSection(&(sb->cs));

	return aread;
}

void mksb_clear  (MKSTREAMBUFFER* sb)
{
	::EnterCriticalSection(&(sb->cs));
	sb->head = sb->buf;
	sb->tail = sb->buf;
	::LeaveCriticalSection(&(sb->cs));
}

void mksb_skip   (MKSTREAMBUFFER* sb, int32_t len, PBYTE buf)
{
	::EnterCriticalSection(&(sb->cs));

	int32_t cnt  = 0;
	PBYTE   curr = sb->head;
	PBYTE   pat  = buf;
	while(curr < sb->tail && curr < sb->end && cnt < len)
	{
		if(*curr == *pat)
		{
			curr++;
			pat++;
			cnt++;
		}
		else
		{
			curr++;
			pat=buf;
			cnt=0;
		}
	}
	if(cnt==len) sb->head=curr;
	
	::LeaveCriticalSection(&(sb->cs));	
}

