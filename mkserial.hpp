#ifndef __MK_SERIAL_COM__
#define __MK_SERIAL_COM__

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>
#include <wx/wx.h>
	
	
struct MKSERIAL
{
	HANDLE  hCOM;
	DCB     cominfo;
};

MKSERIAL* mkserial_create (uint32_t port,uint32_t rate)
{
	MKSERIAL* c = NULL;
	try
	{
		c = new MKSERIAL;
		c->hCOM  = INVALID_HANDLE_VALUE;
		memset(&(c->cominfo),0,sizeof(DCB));
	
		BOOL ret = FALSE;
		wxString strport = wxString::Format("\\\\.\\COM%d",port);
		c->hCOM  = CreateFileW(strport.wc_str(),GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		c->cominfo.DCBlength = sizeof(DCB);
		ret = GetCommState(c->hCOM,&(c->cominfo));
		c->cominfo.BaudRate = rate;
		c->cominfo.ByteSize = 8;
		ret = SetCommState(c->hCOM,&(c->cominfo));
	}
	catch(...)
	{
		cerr<<"MKSERIAL Create Error"<<endl<<flush;
	}
	return c;
}


void  mkserial_delete(MKSERIAL* c)
{
	try
	{
		CloseHandle(c->hCOM);
		delete c;
	}
	catch (...)
	{
		cerr<<"MKSERIAL Delete error"<<endl<<flush;
	}
}


uint32_t mkserial_send (MKSERIAL* c,PBYTE buf,uint32_t len)
{
	DWORD towrite = 0;
	DWORD awrite  = 0;
	BOOL  ret     = false;
	
	try
	{
		// send size first
		towrite = 4;
		awrite  = 0;
		ret     = WriteFile(c->hCOM, &len, towrite, &awrite, NULL);

		// send actual data
		towrite = len;
		awrite  = 0;
		ret     = WriteFile(c->hCOM, (void*)buf, towrite, &awrite, NULL);
	}
	catch (...)
	{
		cerr << "SERIAL SEND ERROR" << endl << flush;
	}
	return awrite;
}

uint32_t mkserial_recv (MKSERIAL* c,PBYTE* buf,uint32_t* len)
{
	BOOL  ret     = false;
	DWORD evtmask = 0;
	DWORD toread  = 0;
	DWORD aread   = 0;
	PBYTE tbuf    = NULL;
	
	try
	{
		ret  = WaitCommEvent(c->hCOM,&evtmask,NULL);     // Wait until someing comes
		ret  = ReadFile(c->hCOM,&toread,4,&aread,NULL);
		tbuf = new BYTE [toread];
		memset(tbuf,0,toread);
		ret  = ReadFile(c->hCOM,tbuf,toread,&aread,NULL);
		*buf = tbuf;
		*len = aread;
	}
	catch(...)
	{
		cerr << "SERIAL RECV ERROR"<<endl<<flush;
		*buf = NULL;
		*len = 0;
	}
	
	return aread;
}


#endif