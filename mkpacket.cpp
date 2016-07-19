#define WIN32_LEAN_AND_MEAN
#pragma once
#include "mkpacket.h"

DllExport MKPACKETCONTEXT* mkpacket_create (uint32_t size)
{
	MKPACKETCONTEXT* c = new MKPACKETCONTEXT;	
	c->len    = size;
	c->buf    = new BYTE[size];	
	c->start  = c->buf;
	c->end    = c->buf + size;
	c->head   = c->buf;
	c->tail   = c->buf;	
	c->wsabuf.buf = new CHAR[MKPACKET_WSABUFSIZE];
	c->wsabuf.len = MKPACKET_WSABUFSIZE;
	InitializeCriticalSection (&(c->cs));
	return c;
}

DllExport bool mkpacket_delete (PBYTE p)
{
	bool ret = false;
	if (p != NULL)
	{
		MKPACKETCONTEXT* c = (MKPACKETCONTEXT*)p;
		delete[] c->buf;
		c->len = 0;
		c->buf = NULL;
		c->start = NULL;
		c->end = NULL;
		c->head = NULL;
		c->tail = NULL;
		delete[] c->wsabuf.buf;
		c->wsabuf.buf = NULL;
		c->wsabuf.len = 0;
		DeleteCriticalSection(&(c->cs));
		delete c;
		ret = true;
	}
	return ret;
}

DllExport void mkpacket_clear (MKPACKETCONTEXT* c)
{
	::EnterCriticalSection(&(c->cs));

	for (uint32_t i = 0; i < c->packets.size(); i++)
	{
		delete c->packets[i]->buf;
		delete c->packets[i];
		c->packets[i] = NULL;
	}
	c->packets.clear();

	c->head = c->buf;
	c->tail = c->buf;

	::LeaveCriticalSection(&(c->cs));
}

DllExport int32_t mkpacket_put (MKPACKETCONTEXT* c,int32_t len,PBYTE buf)
{	
	int32_t  avail    = 0;
	int32_t  filled   = 0;
	int32_t  shiftval = 0;
	int32_t  ret      = 0;

	if ((c != NULL) && (buf != NULL) && (len > 0))
	{
		::EnterCriticalSection (&(c->cs));
		avail  = (int32_t)(c->end  - c->tail);
		filled = (int32_t)(c->tail - c->head);

		if (avail < len)
		{
			memcpy(c->start, c->head, filled);
			c->head = c->start;
			c->tail = c->head + filled;
			avail = (int32_t)(c->end - c->tail);

			if (avail < len)
			{
				shiftval = len - avail;
				c->head += shiftval;
				filled = (int32_t)(c->tail - c->head);

				memcpy(c->start, c->head, filled);
				c->head = c->start;
				c->tail = c->head + filled;
				avail = (int32_t)(c->end - c->tail);
			}
		}

		if (avail >= len)
		{
			memcpy(c->tail, buf, len);
			c->tail += len;
			avail = (int32_t)(c->end - c->tail);
			ret = len;
		}
		::LeaveCriticalSection (&(c->cs));
	}
	return ret;
}

DllExport int32_t mkpacket_recv (MKPACKETCONTEXT* c,SOCKET sock)
{	
	DWORD     byteio  =  0;
	DWORD     flag    =  0;				
	int32_t   ret     =  0;
	int32_t   alen    =  0;
	int32_t   remaind =  0;
	MKPACKET* pkt     =  NULL;

	ret = WSARecv(sock, &(c->wsabuf), 1, &byteio, &flag, NULL, NULL);
	if ((ret == 0) && (byteio > 0))
	{
		::EnterCriticalSection(&(c->cs));
		try
		{
			mkpacket_put(c, byteio, PBYTE(c->wsabuf.buf));
			while (c->head < c->tail)
			{
				pkt = new MKPACKET;

				pkt->sync = *(c->head); 	           
				c->head += 1;
				if (pkt->sync != MKPACKET_SYNCBYTE) { delete pkt; throw 1; }

				pkt->type = *(c->head);	           
				c->head += 1;

				memcpy(&(pkt->len), (c->head), 4); 
				c->head += 4;
				if (pkt->len >= c->len) { delete pkt; throw 2; }

				pkt->buf = new BYTE[pkt->len];
				alen = min(int32_t(pkt->len), int32_t(c->tail - c->head));
				memcpy(pkt->buf, c->head, alen);
				c->head += alen;

				remaind = pkt->len - alen;
				if (remaind > 0)
				{
					ret = 0;
					byteio = 1;
					while ((remaind > 0) && (ret == 0) && (byteio > 0))
					{
						ret = WSARecv(sock, &(c->wsabuf), 1, &byteio, &flag, NULL, NULL);
						mkpacket_put(c, byteio, PBYTE(c->wsabuf.buf));
						remaind -= byteio;
					}
					remaind = pkt->len - alen;
					memcpy(pkt->buf + alen, c->head, remaind);
					c->head += remaind;
				}
				c->packets.push_back(pkt);
			}
			ret = 0;  // OK return
		}
		catch (int32_t e)
		{
			cerr << "MKPACKET_RECV error:" << e << "  " << WSAGetLastError() << endl << flush;
			ret = e;  // Error return
		}		
		::LeaveCriticalSection(&(c->cs));
	}
	else
		ret = -1;     // Error return

	return ret;
}




DllExport int32_t mkpacket_send (SOCKET sock,const uint8_t type,const uint32_t datalen,const PBYTE data)
{	
	WSABUF   wsabuf;
	DWORD    byteio  = 0;     // For first run
	DWORD    flag    = 0;	  // MUST init to zero
	DWORD    remaind = 0;
	PBYTE    buf     = NULL;
	PBYTE    pos     = NULL;
	BYTE     sync    = MKPACKET_SYNCBYTE;
	uint32_t len     = 0;
	int      ret     = 0;
	int      err     = 0;
	
	len     = 6 + datalen;
	buf     = new BYTE[len];
	pos     = buf;
	
	memcpy(pos,&sync,1)     ; pos += 1;
	memcpy(pos,&type,1)     ; pos += 1;
	memcpy(pos,&datalen,4)  ; pos += 4;
	memcpy(pos,data,datalen); pos += datalen;

	ret     = 0;
	byteio  = 1;
	pos     = buf;
	remaind = len;
	while((remaind > 0) && (ret == 0) && (byteio > 0))
	{
		wsabuf.buf = (CHAR*)pos;
		wsabuf.len = remaind;
		ret        = WSASend(sock,&(wsabuf),1,&byteio,flag,NULL,NULL);
		err        = WSAGetLastError();
		pos       += byteio;
		remaind   -= byteio;
	}
	delete[] buf;

	return ret;
}