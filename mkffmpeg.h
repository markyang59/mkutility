//-------------------------------------
// FFMPEG Wrapper API
// Mark Yang
// 2016.7.15

#pragma once
#define WIN32_LEAN_AND_MEAN
#define DllExport extern "C" _declspec(dllexport)
#define FFMPEG_MAX_STREAM      9
#define FFMPEG_MXF_XDCAM       0
#define FFMPEG_MOV_XAVC        1

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <boost/container/vector.hpp>
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/intfloat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
}
#include "mkutil.h"
#include "mkdib.h"
#include "mkbuffer.h"
#include "mkstreambuffer.h"
#include "mkwasapi.h"


struct FFMPEG_STREAM
{
	int32_t          num;
	AVMediaType      type;
	AVCodecContext*  ctx_codec;
	AVCodec*         codec;
	AVStream*        stream;
};

struct FFMPEGCONTEXT
{
	// Format and stream	
	char*            fn_in;
	AVFormatContext* ctx_format_in;  // in  file format		
	FFMPEG_STREAM    stream_in [FFMPEG_MAX_STREAM];
	int32_t          vstream_in_0;   // first  video stream
	int32_t          astream_in_0;   // first  audio stream
	int32_t          astream_in_1;   // second audio stream
	uint32_t         width_in;     
	uint32_t         height_in;
	AVPixelFormat    pixfmt_in;
	AVRational       video_time_base_in;
	AVRational       sample_aspect_ratio_in;
	int32_t          sample_rate_in;
	AVSampleFormat   sample_fmt_in;
	int32_t          channels_in;
	AVRational       audio_time_base_in;
	
	char*            fn_out;
	AVFormatContext* ctx_format_out; // out file format
	AVOutputFormat*  format_out;
	FFMPEG_STREAM    stream_out[FFMPEG_MAX_STREAM];
	uint32_t         width_out;
	uint32_t         height_out;
	AVPixelFormat    pixfmt_out;
	AVRational       video_time_base_out;
	AVRational       sample_aspect_ratio_out;
	int32_t          sample_rate_out;
	AVSampleFormat   sample_fmt_out;
	int32_t          channels_out;
			
	AVFrame*         frame_video_src; 
	AVFrame*         frame_video_rgb;
	AVFrame*         frame_video_dst;	
	AVFrame*         frame_video_filtered;  // FilterGraph Output (temporary)
	AVFrame*         frame_audio_src;
	AVFrame*         frame_audio_dst;
	SwsContext*      ctx_sws_src_to_rgb; // scaler SRC->RGB		    
	SwsContext*      ctx_sws_rgb_to_dst; // scaler RGB->DST
	DIB*             dib_rgb;
	DIB*             dib_dst;
								
	// Audio resampler (for Encoding)	
	SwrContext*      ctx_swr;
	uint8_t**        data_src;
	uint8_t**        data_dst;
	int32_t          bufsize_src;
	int32_t          bufsize_dst;
	int32_t          linesize_src;
	int32_t          linesize_dst;
	int32_t          nb_channels_src;
	int32_t          nb_channels_dst;
	int32_t          nb_samples_src; // 1024
	int32_t          nb_samples_dst;
	int32_t          nb_samples_dst_max;

	// Audio resampler (for WASAPI)	
	WASAPICONTEXT*   wasapi;
	SwrContext*      ctx_swr2 ;
	uint8_t**        data_src2;
	uint8_t**        data_dst2;
	int32_t          bufsize_src2;
	int32_t          bufsize_dst2;
	int32_t          linesize_src2;
	int32_t          linesize_dst2;
	int32_t          nb_channels_src2;
	int32_t          nb_channels_dst2;
	int32_t          nb_samples_src2; // 1024
	int32_t          nb_samples_dst2;
	int32_t          nb_samples_dst_max2;
	
	// Audio resampler (for DeckLink)
	SwrContext*      ctx_swr3 ;
	uint8_t**        data_src3;
	uint8_t**        data_dst3;
	int32_t          bufsize_src3;
	int32_t          bufsize_dst3;
	int32_t          linesize_src3;
	int32_t          linesize_dst3;
	int32_t          nb_channels_src3;
	int32_t          nb_channels_dst3;
	int32_t          nb_samples_src3; // 1024
	int32_t          nb_samples_dst3;
	int32_t          nb_samples_dst_max3;

	// Audio resampler (for XDCAM MXF 8ch)
	SwrContext*      ctx_swr4 ;
	uint8_t**        data_src4;
	uint8_t**        data_dst4;
	int32_t          bufsize_src4;
	int32_t          bufsize_dst4;
	int32_t          linesize_src4;
	int32_t          linesize_dst4;
	int32_t          nb_channels_src4;
	int32_t          nb_channels_dst4;
	int32_t          nb_samples_src4; // 1024
	int32_t          nb_samples_dst4;
	int32_t          nb_samples_dst_max4;
	
	// Read/Write packet
	AVPacket         packet_read;
	AVPacket         packet_write;
	int32_t          read_packet;
	int32_t          read_video;
	int32_t          read_audio;			
	int32_t          frame_finished;	
	int32_t          st_index;	
	FFMPEG_STREAM*   st_in;
	FFMPEG_STREAM*   st_out;
			
	int64_t	         currframe;
	int64_t		     maxframe;
	bool             decode_ready;
	bool             encode_ready;
	bool             record;          // record or just play	

	// Filter
	AVFilterGraph*   filter_graph;
	AVFilterContext* ctx_buffersink;
	AVFilterContext* ctx_buffersrc;	

	// Clock
	int64_t          clk_base_video;
	int64_t          clk_base_audio;
};

int32_t   alloc_samples_array_and_data 		    (uint8_t***    data,int32_t *linesize, int32_t nb_channels,int32_t nb_samples, enum AVSampleFormat sample_fmt, int32_t align);
int32_t   ffmpeg_stream_reset					(FFMPEG_STREAM*  st);//,bool close=FALSE);
int64_t   frame_to_timestamp                    (AVStream*      ctx,int64_t frame);
int64_t   timestamp_to_frame					(AVStream*      ctx,int64_t timestamp);
int64_t   get_maxframe							(AVStream*      ctx);

DllExport void           ffmpeg_init            ();
DllExport FFMPEGCONTEXT* ffmpeg_create			();
DllExport bool           ffmpeg_delete			(PBYTE p);
DllExport HDC            ffmpeg_rgb_dc			(FFMPEGCONTEXT* ctx);
DllExport void*          ffmpeg_rgb_p			(FFMPEGCONTEXT* ctx);
DllExport int32_t        ffmpeg_rgb_w			(FFMPEGCONTEXT* ctx);
DllExport int32_t        ffmpeg_rgb_h			(FFMPEGCONTEXT* ctx);
DllExport int64_t        ffmpeg_currframe		(FFMPEGCONTEXT* ctx);
DllExport int64_t		 ffmpeg_maxframe		(FFMPEGCONTEXT* ctx);
DllExport int32_t		 ffmpeg_readpacket		(FFMPEGCONTEXT* ctx);
DllExport int32_t		 ffmpeg_framefinished	(FFMPEGCONTEXT* ctx);
DllExport void			 ffmpeg_set_record		(FFMPEGCONTEXT* ctx,int32_t num);
DllExport int32_t        ffmpeg_get_record      (FFMPEGCONTEXT* ctx);
DllExport int32_t		 ffmpeg_streamtype		(FFMPEGCONTEXT* ctx);// 0 video , 1 audio
DllExport int32_t		 ffmpeg_decoder_ready	(FFMPEGCONTEXT* ctx);
DllExport int64_t		 ffmpeg_decoder_open	(FFMPEGCONTEXT* ctx, char* src, int32_t w,int32_t h);
DllExport int32_t		 ffmpeg_decoder_close	(FFMPEGCONTEXT* ctx);
DllExport int32_t		 ffmpeg_decode			(FFMPEGCONTEXT* ctx);
DllExport int32_t        ffmpeg_encoder_ready   (FFMPEGCONTEXT* ctx);
DllExport int32_t        ffmpeg_encoder_open    (FFMPEGCONTEXT* ctx, char* dest,int32_t file_format);
DllExport int32_t		 ffmpeg_encoder_close   (FFMPEGCONTEXT* ctx);
DllExport void			 ffmpeg_encode			(FFMPEGCONTEXT* ctx);
DllExport void			 ffmpeg_encode_flush	(FFMPEGCONTEXT* ctx);
DllExport void			 ffmpeg_packet_reset	(FFMPEGCONTEXT* ctx);
DllExport int32_t		 ffmpeg_packet_read		(FFMPEGCONTEXT* ctx);
DllExport void			 ffmpeg_packet_free		(FFMPEGCONTEXT* ctx);
DllExport int32_t        ffmpeg_setstream_xdcam (FFMPEGCONTEXT* ctx, int32_t stream);
DllExport int32_t		 ffmpeg_setstream_lpcm  (FFMPEGCONTEXT* ctx, int32_t stream);
DllExport int32_t        ffmpeg_setstream_xavc  (FFMPEGCONTEXT* ctx, int32_t stream);
DllExport int32_t		 ffmpeg_seek			(FFMPEGCONTEXT* ctx,uint32_t frame);
DllExport int32_t		 ffmpeg_wasapi_feed		(FFMPEGCONTEXT* ctx, int32_t stream);
DllExport MKBUF*         ffmpeg_decklink_feed   (FFMPEGCONTEXT* ctx);
DllExport MKBUF*         ffmpeg_xdcam_feed      (FFMPEGCONTEXT* ctx);
DllExport int32_t        ffmpeg_save_png        (const wchar_t*  fn, DIB*    dib);
DllExport int32_t        ffmpeg_filter_open     (FFMPEGCONTEXT* ctx,const char* filter_desc);
DllExport int32_t        ffmpeg_filter_close    (FFMPEGCONTEXT* ctx);
DllExport void           ffmpeg_bitblt          (HDC dst,int x,int y,int w,int h,const AVFrame *frame);
DllExport void           ffmpeg_avframe_copy    (AVFrame* d,AVFrame* s);
DllExport double_t       ffmpeg_pts_video       (FFMPEGCONTEXT* ctx);
DllExport double_t       ffmpeg_pts_audio       (FFMPEGCONTEXT* ctx);
