#define WIN32_LEAN_AND_MEAN

#include "mksecurity.h"
#include "mkbitset.h"

uint32_t GetVolume()
{
	wchar_t  tszVolumeName[_MAX_PATH];
	wchar_t  tszFileSystemName[_MAX_PATH];
	DWORD    dwVolumeSerialNumber;
	DWORD    dwMaximumComponentLength;
	DWORD    dwFileSystemFlag;	
	GetVolumeInformation(L"C:\\", tszVolumeName, _MAX_PATH, &dwVolumeSerialNumber, &dwMaximumComponentLength, &dwFileSystemFlag, tszFileSystemName, _MAX_PATH );
	return   (uint32_t)dwVolumeSerialNumber;	
}

uint32_t GetMAC()
{	
	ULONG             len      = 0;
	DWORD             tag      = 0;
	uint32_t          rst      = 0;
	PIP_ADAPTER_INFO  pAdapter = NULL;
	PIP_ADAPTER_INFO  curr     = NULL; // adapter traversal

	// Get All Adapter info
	GetAdaptersInfo(NULL,&len);
	pAdapter =(PIP_ADAPTER_INFO) new BYTE[len];
	tag      = GetAdaptersInfo(pAdapter,&len);
	if(tag!=ERROR_SUCCESS) return 0;
	
	// MAC ADDRESS:  00-00-00-00-00-00   
	// 6 BYTES
	// Traverse Adapter
	curr = pAdapter;
	while (curr != NULL)
	{
		for (uint32_t i = 0; i < curr->AddressLength; i++)
			rst += curr->Address[i];
		curr = curr->Next;
	}

	delete[] pAdapter;
	return (uint32_t)rst;
}

void bs_swap(MKBITSET* bs,int32_t a,int32_t b)
{
	uint8_t va = mkbitset_get(bs,a);
	uint8_t vb = mkbitset_get(bs,b);
	uint8_t vt = 0;
	vt = va;
	va = vb;
	vb = vt;
	mkbitset_set(bs,a,va);
	mkbitset_set(bs,b,vb);
}
uint32_t key_encode(uint32_t seed) //Key must have 16 byte space
{	
	uint32_t  key  = 0;
	uint32_t  mask = 0x00000001;
	uint8_t   val  = 0;
	MKBITSET* bs   = mkbitset_create(32);	

	mkbitset_clear(bs,0);
	for(int i=0;i<32;i++)
	{
		val = (seed>>i & mask); 		
		mkbitset_set(bs,i,val);
	}

	bs_swap(bs,4,21);
	bs_swap(bs,30,2);
	bs_swap(bs,17,3);
	bs_swap(bs,11,15);
	bs_swap(bs,20,6);
	bs_swap(bs,0,18);

	for(int i=0;i<32;i++)
	{
		val = mkbitset_get(bs,i);
		key = key | ((uint32_t) val)<<i;	
	}

	mkbitset_delete((PBYTE) bs);
	return key;
}

uint32_t GetSeed()
{	 
	return (GetVolume() + GetMAC());	
}

uint32_t GetKey(uint32_t seed)
{
	return key_encode(seed);
}

bool RegisterValify()
{
	bool  ret      = false;
    DWORD seed     = GetSeed();
	DWORD validKey = GetKey(seed);

	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,regKey,0,KEY_QUERY_VALUE,&hKey)==ERROR_SUCCESS)
	{
		DWORD key=0;
		DWORD dwType=0;
		DWORD cbData = sizeof(key);
		RegQueryValueEx(hKey,L"KEY",0,&dwType,(PBYTE)&key,&cbData);
		RegCloseKey(hKey);
		
		if(key==validKey) ret = true;
	}	
	return ret;
}