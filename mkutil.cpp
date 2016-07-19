#define WIN32_LEAN_AND_MEAN
#pragma once
#include "mkutil.h"
#include <emmintrin.h>
#include <boost/container/vector.hpp>

//-- TIME
LARGE_INTEGER    g_cputime_freq;
DllExport void     cputime_init()
{
	QueryPerformanceFrequency(&g_cputime_freq);
}
DllExport uint64_t cputime_get()  // Microsecond  10^-6
{
	LARGE_INTEGER cputime_cnt;
	QueryPerformanceCounter(&cputime_cnt);
	return (uint64_t)((double_t)(cputime_cnt.QuadPart) / (double_t)(g_cputime_freq.QuadPart) * 1000000.0);
}
DllExport wchar_t* current_datetime_str()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	wchar_t* str = new wchar_t[30];
	swprintf_s(str, 30, L"%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return str;
}
DllExport void str_to_systemtime(SYSTEMTIME* st, const wchar_t* str)
{
	memset(st, 0, sizeof(SYSTEMTIME));
	int year, month, day, hour, minute, second;

	swscanf_s(str, L"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	st->wYear = year;
	st->wMonth = month;
	st->wDay = day;
	st->wHour = hour;
	st->wMinute = minute;
	st->wSecond = second;
}
wxString now_fname()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return wxString::Format(wxT("%04d-%02d-%02d_%02d%02d%02d"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}
wxString now_fname_ms()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return wxString::Format(wxT("%04d-%02d-%02d_%02d_%02d_%02d_%03d"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}
wxString now_string()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return wxString::Format(wxT("%04d-%02d-%02d %02d:%02d:%02d.%03d"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

//-- Thread Control
DllExport inline bool GetEvent(HANDLE event)
{
	return (::WaitForSingleObjectEx(event, 0, FALSE) == WAIT_OBJECT_0);
}

//-- ARITHMATIC
DllExport DWORD   HIGH64(uint64_t v) { return ((v & 0xFFFFFFFF00000000) >> 32); }
DllExport DWORD   LOW64(uint64_t v) { return  (v & 0x00000000FFFFFFFF); }
DllExport int32_t bound32(int32_t val, int32_t minval, int32_t maxval)
{
	int32_t ret = val;
	if (val < minval) ret = minval;
	if (val > maxval) ret = maxval;
	return  ret;
}
DllExport int64_t bound64(int64_t val, int64_t minval, int64_t maxval)
{
	int64_t ret = val;
	if (val < minval) ret = minval;
	if (val > maxval) ret = maxval;
	return  ret;
}
DllExport double_t boundf(double_t val, double_t minval, double_t maxval)
{
	double  ret = val;
	if (val < minval) ret = minval;
	if (val > maxval) ret = maxval;
	return  ret;
}
DllExport uint32_t divroundup(const uint32_t x, const uint32_t y) { return (1 + ((x - 1) / y)); }
DllExport uint32_t checksum_calc(const PBYTE buf, uint32_t len)
{
	uint32_t checksum = 0;
	uint32_t remaind = len;
	PBYTE    pos = buf;

	if (buf != NULL && len > 0)
	{
		while (remaind >= 4)
		{
			checksum += *((uint32_t*)pos);
			pos += 4;
			remaind -= 4;
		}
	}
	return checksum;
}
DllExport uint32_t mkmin(uint32_t a, uint32_t b) { return (a < b) ? a : b; }
DllExport bool     diff (double_t a, double_t b, double_t delta) { return ((fabs(a - b)) < delta); }
DllExport double_t MIN3 (double_t a, double_t b, double_t c)
{
	double_t   v = a;
	if (b < v) v = b;
	if (c < v) v = c;
	return v;
}
DllExport double_t MAX3(double_t a, double_t b, double_t c)
{
	double_t   v = a;
	if (b > v) v = b;
	if (c > v) v = c;
	return v;
}
DllExport double_t SDIV(double_t a, double_t b)
{
	double_t    v = 0.0;
	if (b != 0.0) v = a / b;
	return v;
}

DllExport float_t  int_to_float(int32_t i){return (float_t)(i / (8.0 * 0x10000000));}
DllExport double_t dist360(double_t a, double_t b)
{
	double_t v1 = fabs(a - b);
	double_t v2 = fabs(360.0 - v1);
	return fmin(v1, v2);
}

//-- Graphics
wxRect fill_rect(wxRect dst, wxRect src, bool full)
{
	float_t ix = 0.0;
	float_t iy = 0.0;
	float_t iw = 0.0;
	float_t ih = 0.0;
	float_t sw = src.width;
	float_t sh = src.height;
	float_t dw = dst.width;
	float_t dh = dst.height;

	if (full == false)  //-- Into dest  
	{
		if ((sw / sh * dh) > dw)
		{
			iw = dw;
			ih = sh / sw * dw;
		}
		else
		{
			iw = sw / sh * dh;
			ih = dh;
		}
		ix = (dw - iw) / 2.0;
		iy = (dh - ih) / 2.0;
		if (ix < 0) ix = 0;
		if (iy < 0) iy = 0;
	}
	else // Fill dest with clipping  
	{
		if ((sw / sh * dh) < dw)
		{
			iw = dw;
			ih = sh / sw * dw;
		}
		else
		{
			iw = sw / sh * dh;
			ih = dh;
		}
		ix = (dw - iw) / 2.0;
		iy = (dh - ih) / 2.0;
	}

	return wxRect((int)ix, (int)iy, (int)iw, (int)ih);
}

wxRect fill_grid(int32_t num, int32_t sw, int32_t sh, int32_t tw, int32_t th, int32_t xslot, int32_t yslot, int32_t lgap, int32_t tgap)
{
	int32_t w = tw / xslot;
	int32_t h = th / yslot;
	ldiv_t  d = ldiv(num, xslot);
	int32_t x = w * d.rem;
	int32_t y = h * d.quot;
	x += lgap;
	y += tgap;
	w -= lgap;
	h -= tgap;
	wxRect rdst = wxRect(x, y, w, h);
	wxRect rsrc = wxRect(0, 0, sw, sh);
	wxRect rrst = fill_rect(rdst, rsrc, false);
	rrst.x += x;
	rrst.y += y;
	return rrst;
}

//-- String
DllExport wchar_t* strconcat(wchar_t* a, wchar_t* b)
{
	size_t alen = wcslen(a);
	size_t blen = wcslen(b);

	wchar_t* rst = new wchar_t[alen + blen + 1];
	wchar_t* curr = rst;
	memcpy(curr, a, alen*sizeof(wchar_t)); curr += alen;
	memcpy(curr, b, blen*sizeof(wchar_t)); curr += blen;
	*curr = L'\0';

	return rst;
}

DllExport wchar_t* strfilename(const wchar_t* pf)
{
	int32_t  len = (int32_t)wcslen(pf);
	wchar_t* rst = NULL;

	for (int32_t i = len - 1; i >= 0; i--)
	{
		if (pf[i] == L'\\')
		{
			rst = new wchar_t[len - i];
			memcpy(rst, pf + i + 1, (len - i)*sizeof(wchar_t));
			break;
		}
	}
	return rst;
}

DllExport wchar_t* strpathname(const wchar_t* pf)
{
	int32_t  len = (int32_t)wcslen(pf);
	wchar_t* rst = NULL;

	for (int32_t i = len - 1; i >= 0; i--)
	{
		if (pf[i] == L'\\')
		{
			rst = new wchar_t[i + 2];
			memcpy(rst, pf, (i + 1)*sizeof(wchar_t));
			rst[i + 1] = L'\0';
			break;
		}
	}
	return rst;
}

DllExport int32_t strreplace(wchar_t** dst, const wchar_t* src)  // Delete Previous string and make new string 
{
	int32_t ret = 0;
	try
	{
		if (dst == NULL || src == NULL) throw 1; // parameter error
		mkdelete((void**)dst);
		*dst = new wchar_t[wcslen(src) + 1];
		wcscpy(*dst, src);
	}
	catch (int e)
	{
		ret = e;
	}
	return ret;
}

DllExport int32_t  mkdelete(void** pp)
{
	int32_t ret = 0;
	try
	{
		if (pp == NULL) throw 1;
		if (*pp == NULL) throw 2;
		delete *pp;
		*pp = NULL;
	}
	catch (int e)
	{
		ret = e;
	}
	return ret;
}

//-- Network
DllExport BOOL DisconnectSock(SOCKET sock)
{
	BOOL  ret = TRUE;
	GUID  GuidDisconnectEx = WSAID_DISCONNECTEX;
	LPFN_DISCONNECTEX lpfnDisconnectEx;
	DWORD copied = 0;
	DWORD err = 0;
	if (WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidDisconnectEx, sizeof(GuidDisconnectEx), &lpfnDisconnectEx, sizeof(lpfnDisconnectEx), &copied, 0, 0) == 0)
	{
		ret = lpfnDisconnectEx(sock, NULL, 0, 0);
		err = WSAGetLastError();
	}
	return ret;
}

DllExport bool ip_string_p(wchar_t* str)  // string is valid ip string
{
	bool     ret = true;
	enum     ipmask { INIT, OK_A, OK_B, OK_C, OK_D };
	ipmask   stat = INIT;
	wchar_t* token = NULL;
	wchar_t* nt = NULL;
	int      num = 0;
	wchar_t  buffer[128];
	try
	{
		// character check
		for (uint32_t i = 0; i < wcslen(str); i++)
		{
			switch (str[i])
			{
				case L'0': case L'1': case L'2': case L'3': case L'4': case L'5': case L'6': case L'7':	case L'8': case L'9':
				case L'.':
					break;
				default:throw 1; break;
			}
		}

		// grammar check
		wcsncpy_s(buffer, str, 128);
		token = wcstok_s(buffer, L".", &nt);
		while (token != NULL)
		{
			switch (stat)
			{
				case INIT:
					num = _wtoi(token);
					if (num < 0 || num>255) throw 3;
					stat = OK_A;
					break;
				case OK_A:
					num = _wtoi(token);
					if (num < 0 || num>255) throw 4;
					stat = OK_B;
					break;
				case OK_B:
					num = _wtoi(token);
					if (num < 0 || num>255) throw 5;
					stat = OK_C;
					break;
				case OK_C:
					num = _wtoi(token);
					if (num < 0 || num>255) throw 6;
					stat = OK_D;
					break;
				case OK_D:
					break;
				default: throw 2; break;
			}
			token = wcstok_s(NULL, L".", &nt);
		}
	}
	catch (int e)
	{
		cerr << "ip string error :" << e;
		ret = false;
	}
	return ret;
}

DllExport bool port_avail_udp(int32_t port)
{
	bool   ret = false;
	int    rst = 0;
	int    err = 0;
	SOCKET s = INVALID_SOCKET;

	try
	{
		s = socket(AF_INET, SOCK_DGRAM, 0); if (s == INVALID_SOCKET) throw 1;
		SOCKADDR_IN sd;
		sd.sin_family = AF_INET;
		sd.sin_port = htons(port);
		sd.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		rst = ::bind(s, (PSOCKADDR)&sd, sizeof(SOCKADDR_IN)); if (rst != 0) throw 2;
		ret = true;
	}
	catch (int e)
	{
		err = WSAGetLastError();   //if (err == WSAEADDRINUSE)
		cerr << "Port Availity Error :" << e << "  " << err << endl << flush;
		ret = false;
	}
	closesocket(s);

	return ret;
}

wxString numstr(uint64_t num, uint32_t unit)
{
	switch (unit)
	{
		case FFT_UNIT_BYTE: break;
		case FFT_UNIT_KB: num = num / 1000;       break;
		case FFT_UNIT_MB: num = num / 1000000;    break;
		case FFT_UNIT_GB: num = num / 1000000000; break;
		default: break;
	}

	wxString buf = wxString::Format(wxT("%llu"), num);
	int  cnt = 0;
	auto  it = buf.end();
	while (it > buf.begin())
	{
		if (cnt == 3)
		{
			it = buf.insert(it, ',');
			cnt = 0;
		}
		else
		{
			it--;
			cnt++;
		}
	}
	return buf;
}

//-- File Handling
DllExport bool path_exist(LPCWSTR szPath)
{
	bool  ret = TRUE;
	DWORD dwAttrib = 0;
	try
	{
		if (szPath == NULL)    throw 1;
		if (wcslen(szPath) == 0) throw 2;
		dwAttrib = GetFileAttributes(szPath);
		if (dwAttrib == INVALID_FILE_ATTRIBUTES) throw 3;
		ret = TRUE;
	}
	catch (int e)
	{
		cerr << "path error:" << e;
		ret = FALSE;
	}
	return ret;
}

wxString wxstrfilename(wxString& pf) { return pf.AfterLast(L'\\'); }
wxString wxstrpathname(wxString& pf) { return pf.BeforeLast(L'\\'); }
wxString remslash(wxString pf)
{
	wxString ret = pf;
	if (pf.Last() == '\\')
		ret = pf.RemoveLast();
	return ret;
}
wxString wildpath(wxString pf)
{
	wxString ret = pf;
	wxString a = pf.AfterLast(L'\\');

	if (a.length() == 0)  ret = pf + "*";
	else if (a != L"*") ret = pf + "\\*";

	return ret;
}

//-- System
DllExport void dump_memory()
{
	//MEMORYSTATUSEX statex;
	//statex.dwLength = sizeof(statex);
	//GlobalMemoryStatusEx(&statex);
	//_tprintf(TEXT("PERCENT   : %*ld   \n"), 10, statex.dwMemoryLoad);
	//_tprintf(TEXT("PHY TOTAL : %*I64d \n"), 10, statex.ullTotalPhys / 1024);
	//_tprintf(TEXT("PHY FREE  : %*I64d \n"), 10, statex.ullAvailPhys / 1024);
	//_tprintf(TEXT("PG  TOTAL : %*I64d \n"), 10, statex.ullTotalPageFile / 1024);
	//_tprintf(TEXT("PG  FREE  : %*I64d \n"), 10, statex.ullAvailPageFile / 1024);
	//_tprintf(TEXT("VR  TOTAL : %*I64d \n"), 10, statex.ullTotalVirtual / 1024);
	//_tprintf(TEXT("VR  FREE  : %*I64d \n"), 10, statex.ullAvailVirtual / 1024);
	//_tprintf(TEXT("EX  FREE  : %*I64d \n"), 10, statex.ullAvailExtendedVirtual / 1024);

	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (hProcess == NULL) return;

	pmc.cb = sizeof(pmc);
	if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, pmc.cb))
	{
		//printf("\tPageFaultCount             : %8I64d\n", pmc.PageFaultCount);
		//printf("\tPeakWorkingSetSize         : %8I64d\n", pmc.PeakWorkingSetSize);
		//printf("\tWorkingSetSize             : %8I64d\n", pmc.WorkingSetSize);
		//printf("\tQuotaPeakPagedPoolUsage    : %8I64d\n", pmc.QuotaPeakPagedPoolUsage);
		//printf("\tQuotaPagedPoolUsage        : %8I64d\n", pmc.QuotaPagedPoolUsage);
		//printf("\tQuotaPeakNonPagedPoolUsage : %8I64d\n", pmc.QuotaPeakNonPagedPoolUsage);
		//printf("\tQuotaNonPagedPoolUsage     : %8I64d\n", pmc.QuotaNonPagedPoolUsage);
		printf("\t\t\tPagefileUsage              : %8I64d\n", int64_t(pmc.PagefileUsage));
		printf("\t\t\tPeakPagefileUsage          : %8I64d\n", int64_t(pmc.PeakPagefileUsage));
		//printf("\tPrivateUsage               : %8I64d\n", pmc.PrivateUsage);
	}
	CloseHandle(hProcess);
}

wxString getexepath()
{
	wchar_t path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	return wxString(path);
}

// retrive file in path, path has no '*' at the end
int32_t getfilelist(const wchar_t* path, PBYTE* buf, uint32_t* len, bool incdot)
{
	int32_t          ret = 0;
	HANDLE           hFind = INVALID_HANDLE_VALUE;
	PBYTE            bin = NULL;
	int32_t          size = 0;  // total size in BYTE
	int32_t          sizep = 0;  // path  size in BYTE (no null terminate)
	int32_t          sizef = 0;  // filelist size
	int32_t          sizefi = 0;  // fileitem size
	PBYTE            curr = 0;
	wxString         cpath = wxString(path);   // no '*'
	wxString         vpath = wildpath(cpath);  // with '*'
	WIN32_FIND_DATA  ffd;
	boost::container::vector<WIN32_FIND_DATA> ffds;

	try
	{
		// Get File infos
		hFind = FindFirstFile(vpath.wc_str(), &ffd);
		if (hFind == INVALID_HANDLE_VALUE) throw 1;
		do
		{
			WIN32_FIND_DATA t_ffd;
			wxString        t_name;

			memcpy((void*)(&t_ffd), (void*)(&ffd), sizeof(t_ffd));
			t_name = wxString::Format("%s", t_ffd.cFileName);

			if (!(!incdot && (t_name == "." || t_name == "..")))  // exclude . .. dir									
				ffds.push_back(t_ffd);

		} while (FindNextFile(hFind, &ffd) != 0);
		FindClose(hFind);

		// make binary data
		sizefi = sizeof(WIN32_FIND_DATA);
		sizef = ffds.size() * sizefi;
		sizep = wcslen(path) * sizeof(wchar_t);
		size = 4 + sizep + 4 + sizef;
		bin = new BYTE[size]; if (bin == NULL) throw 2;
		curr = bin;

		memcpy(curr, &sizep, 4);            curr += 4;
		memcpy(curr, (void*)path, sizep);   curr += sizep;
		memcpy(curr, &sizef, 4);            curr += 4;
		for (uint32_t i = 0; i < ffds.size(); i++)
		{
			memcpy(curr, &ffds[i], sizefi); curr += sizefi;
		}
	}
	catch (int32_t e)
	{
		ret = e;
	}

	*buf = bin;
	*len = size;
	return ret;
}


