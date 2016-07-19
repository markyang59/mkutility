// MKUTIL
// Common Utility Functions for Mark Yang Projects

#ifndef __MK_UTIL__
#define __MK_UTIL__

#pragma once
#define WIN32_LEAN_AND_MEAN
#define DllExport extern "C" _declspec(dllexport)
#include <windows.h>
#include <stdint.h>
#include <inttypes.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <Psapi.h>
#include <wx/wx.h>
using namespace std;

//-- TIME
extern  LARGE_INTEGER cputime_freq;
DllExport void        cputime_init      ();
DllExport uint64_t    cputime_get       ();
DllExport wchar_t*    current_datetime_str();
DllExport void        str_to_systemtime (SYSTEMTIME* st,const wchar_t* str);
wxString              now_fname         ();
wxString              now_fname_ms      ();
wxString              now_string        ();

//-- Thread Control
DllExport inline bool GetEvent          (HANDLE event);

//-- ARITHMATIC
DllExport DWORD       HIGH64            (uint64_t v);
DllExport DWORD       LOW64             (uint64_t v);
DllExport int32_t     bound32           (int32_t  val,int32_t  minval,int32_t  maxval);     
DllExport int64_t     bound64           (int64_t  val,int64_t  minval,int64_t  maxval);
DllExport double      boundf            (double_t val,double_t minval,double_t maxval);
DllExport uint32_t    divroundup        (const uint32_t x,const uint32_t y);
DllExport uint32_t    checksum_calc     (const PBYTE buf,uint32_t len);
DllExport uint32_t    mkmin             (uint32_t     a,uint32_t b);
DllExport bool        diff              (double_t a,double_t b,double_t delta);
DllExport double_t	  MIN3				(double_t a,double_t b,double_t c);
DllExport double_t	  MAX3				(double_t a,double_t b,double_t c);
DllExport double_t    SDIV              (double_t a,double_t b);  // a / b safe
DllExport float_t     int_to_float      (int32_t num);
DllExport double_t    dist360           (double_t a,double_t b);

//-- Graphics
wxRect                fill_rect         (wxRect  dst,wxRect src,bool full);
wxRect                fill_grid         (int32_t num,int32_t sw,int32_t sh, int32_t tw,int32_t th, int32_t xslot,int32_t yslot,int32_t lgap,int32_t tgap);

//-- String
DllExport wchar_t*    strconcat         (wchar_t* a,wchar_t* b);
DllExport wchar_t*    strfilename       (const wchar_t* pf);
DllExport wchar_t*    strpathname       (const wchar_t* pf);
DllExport int32_t     strreplace        (wchar_t** dst,const wchar_t* src);
DllExport int32_t     mkdelete          (void**    pp);

//-- Network
DllExport BOOL        DisconnectSock    (SOCKET sock);
DllExport bool        ip_string_p       (wchar_t*  str);
DllExport bool        port_avail_udp    (int32_t port);

enum     // num size to wxString with comma, unit
{
	FFT_UNIT_BYTE=0,
	FFT_UNIT_KB,
    FFT_UNIT_MB,
	FFT_UNIT_GB
};
wxString              numstr            (uint64_t num,uint32_t unit=FFT_UNIT_BYTE); 

//-- File Handling
DllExport bool        path_exist        (LPCWSTR szPath);
wxString              wxstrfilename     (wxString& pf);
wxString              wxstrpathname     (wxString& pf);
wxString              remslash          (wxString  pf);
wxString              wildpath          (wxString  pf);                 // append '*' at the end

//-- System
DllExport void        dump_memory       (); 
wxString              getexepath        ();
int32_t               getfilelist       (const wchar_t* path , PBYTE* buf,uint32_t* len,bool incdot=true); // FileList , include . .. dir or not

//-- MACROS
#define MK_SQRT_PI_2 1.2533141373155001
#define NTHROW(x) if(x != 0) throw WSAGetLastError();
#define INRANGE(v,s,e) (((v) >= (s)) && ((v) <= (e)))
#define ROLLOVER(v,s,e) {if((v)==((e)-1)) (v)=(s); else (v)++;}
#define MID(l,r) (((l)+(r)) / 2)
#define eif else if
#define CLOSEHANDLE(h) {if(h != INVALID_HANDLE_VALUE) {::CloseHandle(h);h=INVALID_HANDLE_VALUE;}}
#define SDELETE(p)     {if((p) != NULL){delete (p);(p) = NULL;}}
#define ZEROP(v) ((v)==0)


#endif