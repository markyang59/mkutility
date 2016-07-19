#pragma once
//#define WINVER 0x0600
//#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define DllExport extern "C" _declspec(dllexport)

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
using namespace std;

#include "Mmdeviceapi.h"
#include "Audioclient.h"
#include "Endpointvolume.h"
#define REFTIMES_PER_SEC      10000000
#define REFTIMES_PERMILLISEC     10000


struct WASAPICONTEXT
{
	REFERENCE_TIME        buf_duration_request; 
	REFERENCE_TIME        buf_duration_actual; 
	IMMDeviceEnumerator  *pEnumerator;         
	IMMDevice            *pDevice;              
	IAudioClient         *pAudioClient;        
	IAudioRenderClient   *pRenderClient;        
	IAudioEndpointVolume *endpoint_vol;        
	WAVEFORMATEX         *fmt;                  
	WAVEFORMATEX         *closest_format;      
	UINT32                frame_total;    // total frames in buffer
	UINT32                frame_avail;    // available frame number in buffer
	UINT32                frame_fill;     // filled frames
	BYTE                 *pData;              
	FILE                 *file_audio;         
	DWORD                 flags;              
	int8_t               *buf;      	
};


//-- Functions
DllExport WASAPICONTEXT* wasapi_open  ();
DllExport int            wasapi_close (WASAPICONTEXT* ctx);