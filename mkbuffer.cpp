#include "mkbuffer.h"

MKBUF* mkbuf_create (uint32_t len,PBYTE buf)
{
	MKBUF* b = NULL;
	if (buf != NULL && len > 0)
	{
		b = new MKBUF;
		b->len = len;
		b->buf = new BYTE[b->len];
		b->pts = 0;
		b->framenum = 0;
		b->var = 0;
		if (buf != NULL) memcpy(b->buf, buf, b->len);
	}
	return b;
}

bool mkbuf_delete (PBYTE p)
{
	bool ret = false;
	if (p != NULL)
	{
		MKBUF* b = (MKBUF*)p;
		delete[] b->buf;
		delete   b;
		ret = true;
	}
	return ret;
}

// write into buf return awrite
int32_t mkbuf_put(MKBUF* b, uint32_t len, PBYTE buf)
{
	uint32_t awrite = 0;
	if (b != NULL && buf != NULL && len > 0)
	{
		awrite = min(b->len, len);
		memcpy(b->buf, buf, awrite);
		b->len = awrite;
	}
	return awrite;
}

// read  from buf,len 0 is all
int32_t mkbuf_get(MKBUF* b, uint32_t len, PBYTE buf)
{
	return 0;
}

// interleave packed merge
MKBUF*  mkbuf_merge(MKBUF* a, MKBUF* b, uint32_t span)
{
	MKBUF* c = NULL;
	if (a != NULL && b != NULL)
	{
		c = new MKBUF;
		c->len = min(a->len, b->len) * 2;
		c->buf = new BYTE[c->len];
		c->pts = 0;
		c->framenum = 0;
		c->var = 0;

		PBYTE ca = a->buf;
		PBYTE cb = b->buf;
		PBYTE cc = c->buf;
		while (cc < c->buf + c->len)
		{
			memcpy(cc, ca, span); ca += span; cc += span;
			memcpy(cc, cb, span); cb += span; cc += span;
		}
	}
	return c;
}


