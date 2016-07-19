#include "mkwasapi.h"

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID   IID_IMMDeviceEnumerator  = __uuidof(IMMDeviceEnumerator);
const IID   IID_IAudioClient         = __uuidof(IAudioClient);
const IID   IID_IAudioRenderClient   = __uuidof(IAudioRenderClient);

DllExport WASAPICONTEXT* wasapi_open ()
{
	WASAPICONTEXT* ctx        = new WASAPICONTEXT;
	HRESULT hr;

	ctx->buf_duration_request = REFTIMES_PER_SEC;
	ctx->buf_duration_actual  = REFTIMES_PER_SEC;
	ctx->pEnumerator          = NULL;
	ctx->pDevice              = NULL;
	ctx->pAudioClient         = NULL;
	ctx->pRenderClient        = NULL;
	ctx->endpoint_vol         = NULL;
	ctx->fmt                  = NULL;
	ctx->closest_format       = NULL;
	ctx->frame_total          = 0;   // total frames in buffer
	ctx->frame_avail          = 0;   // available frame number in buffer
	ctx->frame_fill           = 0;   // filled frames
	ctx->pData                = NULL;
	ctx->file_audio           = NULL;
	ctx->flags                = 0;
	ctx->buf                  = NULL;		

	
	//-- Initialize
	hr = CoInitializeEx   (NULL,COINIT_MULTITHREADED);  // W A R N I N G .. could re-init already init dcom
	hr = CoCreateInstance (CLSID_MMDeviceEnumerator,NULL,CLSCTX_ALL,IID_IMMDeviceEnumerator, (void**)&(ctx->pEnumerator));
	ctx->pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &(ctx->pDevice));
	if(ctx->pDevice==NULL) return NULL;

	ctx->pDevice->Activate (IID_IAudioClient             , CLSCTX_ALL,NULL,(void**)&(ctx->pAudioClient));
	ctx->pDevice->Activate (__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void**)&(ctx->endpoint_vol));
	ctx->pAudioClient->GetMixFormat((WAVEFORMATEX**)&(ctx->fmt));
			
	ctx->fmt->wFormatTag      = WAVE_FORMAT_PCM;
	ctx->fmt->nChannels       = 2 ; 
	ctx->fmt->nSamplesPerSec  = 44100;
	ctx->fmt->wBitsPerSample  = 32;
	ctx->fmt->nBlockAlign     = ctx->fmt->nChannels * (ctx->fmt->wBitsPerSample/8);
	ctx->fmt->nAvgBytesPerSec = ctx->fmt->nSamplesPerSec * ctx->fmt->nBlockAlign; 	
	ctx->fmt->cbSize          = 0;
		
	hr = ctx->pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, ctx->fmt ,&(ctx->closest_format));
	if(ctx->closest_format)
	{
		memcpy(&(ctx->fmt),&(ctx->closest_format),sizeof(ctx->fmt));
		ctx->fmt->wFormatTag      = WAVE_FORMAT_PCM;
		ctx->fmt->nChannels       = 2 ; 
		ctx->fmt->nBlockAlign     = ctx->fmt->nChannels      * (ctx->fmt->wBitsPerSample/8);
	    ctx->fmt->nAvgBytesPerSec = ctx->fmt->nSamplesPerSec * (ctx->fmt->nBlockAlign); 
		ctx->fmt->cbSize          = 0;		
	}	
	ctx->pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, NULL ,ctx->buf_duration_request,0, ctx->fmt ,NULL);
	ctx->pAudioClient->GetBufferSize(&(ctx->frame_total));
	ctx->pAudioClient->GetService(IID_IAudioRenderClient,(void**)&(ctx->pRenderClient));
	
	ctx->buf_duration_actual = REFERENCE_TIME((double)REFTIMES_PER_SEC * (double)(ctx->frame_total) / (double)(ctx->fmt->nSamplesPerSec));
	ctx->pAudioClient->Start();

	return ctx;
}


DllExport int wasapi_close (WASAPICONTEXT* ctx)
{
	if(ctx != NULL)
	{
		ctx->pAudioClient->Stop();
		CoTaskMemFree(ctx->fmt);
		ctx->pEnumerator->Release();
		ctx->pDevice->Release();
		ctx->pAudioClient->Release();
		ctx->pRenderClient->Release();
		CoUninitialize();
		ctx = NULL;
	}
	return 1;
}