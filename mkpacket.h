#pragma once
#define DllExport extern "C" _declspec(dllexport)

#include <windows.h>
#include <stdint.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <boost/container/vector.hpp>
using namespace std;

#define MKPACKET_WSABUFSIZE 400000
#define MKPACKET_SYNCBYTE     0x47

struct MKPACKET
{
	uint8_t   sync;     // 0x47	
	uint8_t   type;
	uint32_t  len;
	PBYTE     buf;
};

struct MKPACKETCONTEXT
{
	uint32_t  len;
	PBYTE     buf;
	PBYTE     start;
	PBYTE     end;
	PBYTE     head;
	PBYTE     tail;	
	WSABUF    wsabuf;
	boost::container::vector<MKPACKET*> packets;	
	CRITICAL_SECTION cs;
};

DllExport  MKPACKETCONTEXT* mkpacket_create (uint32_t size);
DllExport  bool             mkpacket_delete (PBYTE p);
DllExport  void             mkpacket_clear  (MKPACKETCONTEXT* c);
DllExport  int32_t          mkpacket_put    (MKPACKETCONTEXT* c,int32_t len,PBYTE buf);
DllExport  int32_t          mkpacket_recv   (MKPACKETCONTEXT* c,SOCKET sock);
DllExport  int32_t          mkpacket_send   (SOCKET sock,const uint8_t type,const uint32_t len,const PBYTE buf);

