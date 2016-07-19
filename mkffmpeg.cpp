#pragma once
#include "mkffmpeg.h"


// Utility functions only for mkffmpeg
int32_t alloc_samples_array_and_data(uint8_t*** data, int32_t *linesize, int32_t nb_channels,int32_t nb_samples, enum AVSampleFormat sample_fmt, int32_t align)
{
	int32_t nb_planes = av_sample_fmt_is_planar(sample_fmt) ? nb_channels : 1;
	*data = (uint8_t**)av_malloc(sizeof(*data) * nb_planes);
	return av_samples_alloc(*data, linesize, nb_channels,nb_samples, sample_fmt, align);
}
int32_t ffmpeg_stream_reset(FFMPEG_STREAM* st)//,bool close)
{
	int32_t ret = 1;
	if (st != NULL)
	{
		st->num = -1;
		st->type = AVMEDIA_TYPE_UNKNOWN;		
		if (st->ctx_codec != NULL) avcodec_free_context(&st->ctx_codec);
		st->ctx_codec = NULL;
		st->codec = NULL;
		st->stream = NULL;
		ret = 0;
	}
	return ret;
}
int64_t frame_to_timestamp (AVStream* ctx,int64_t frame)
{
	AVRational f = av_make_q ((int)frame,1);
	AVRational d = av_div_q  (av_div_q(f,ctx->r_frame_rate),ctx->time_base);
	return int64_t(av_q2d(d));
}
int64_t timestamp_to_frame (AVStream* ctx,int64_t timestamp)
{
	AVRational d = av_make_q ((int)timestamp,1);
	AVRational r = av_mul_q  (av_mul_q(d,ctx->time_base),ctx->r_frame_rate);
	return int64_t(av_q2d(r));		
}
int64_t get_maxframe(AVStream* ctx)
{
	int64_t frames = 0;
	if (ctx->nb_frames != 0) frames = ctx->nb_frames;
	else   		             frames = timestamp_to_frame (ctx,ctx->duration);
	return frames;
}


// Initialize Codecs
DllExport void ffmpeg_init()
{
	av_register_all(); 
	setlocale(LC_ALL, "korean");
}

// Create Context
DllExport FFMPEGCONTEXT* ffmpeg_create ()
{
	FFMPEGCONTEXT* ctx = NULL;
	try
	{
		ctx = new FFMPEGCONTEXT;  if (ctx==NULL) throw 1;
		ctx->fn_in				 = NULL;
		ctx->fn_out				 = NULL;
		ctx->format_out			 = NULL;
		ctx->ctx_format_in		 = NULL;
		ctx->ctx_format_out		 = NULL;
		for (int32_t i = 0; i < FFMPEG_MAX_STREAM; i++)
		{
			memset(&(ctx->stream_in [i]),0,sizeof(FFMPEG_STREAM));
			memset(&(ctx->stream_out[i]),0,sizeof(FFMPEG_STREAM));
			ffmpeg_stream_reset(&(ctx->stream_in[i]));
			ffmpeg_stream_reset(&(ctx->stream_out[i]));
		}
		ctx->vstream_in_0        = -1;
		ctx->astream_in_0        = -1;
		ctx->astream_in_1        = -1;
		ctx->width_in		     = 0;
		ctx->height_in			 = 0;
		ctx->pixfmt_in			 = AV_PIX_FMT_YUV422P;
		ctx->video_time_base_in  = {0,0};
		ctx->sample_aspect_ratio_in = {0,0};
		ctx->sample_rate_in		 = 0;
		ctx->sample_fmt_in		 = AV_SAMPLE_FMT_S32;
		ctx->channels_in		 = 0;
		ctx->width_out			 = 0;
		ctx->height_out			 = 0;
		ctx->pixfmt_out			 = AV_PIX_FMT_YUV422P;
		ctx->sample_rate_out	 = 0;
		ctx->sample_fmt_out		 = AV_SAMPLE_FMT_S32;
		ctx->channels_out		 = 0;
		ctx->frame_video_src	 = NULL;
		ctx->frame_video_rgb	 = NULL;
		ctx->frame_video_dst     = NULL;
		ctx->frame_audio_src     = NULL;
		ctx->frame_audio_dst     = NULL;
		ctx->ctx_sws_src_to_rgb  = NULL;
		ctx->ctx_sws_rgb_to_dst  = NULL;
		ctx->dib_rgb			 = NULL;
		ctx->dib_dst			 = NULL;

		// Audio resample (Encoding)
		ctx->ctx_swr			 = NULL;
		ctx->data_src			 = NULL;
		ctx->data_dst			 = NULL;
		ctx->bufsize_src		 = 0;
		ctx->bufsize_dst		 = 0;
		ctx->linesize_src		 = 0;
		ctx->linesize_dst	     = 0;
		ctx->nb_channels_src	 = 0;
		ctx->nb_channels_dst	 = 0;
		ctx->nb_samples_src		 = 1024;
		ctx->nb_samples_dst		 = 0;
		ctx->nb_samples_dst_max  = 0;

		// Audio resample (WASAPI)
		ctx->ctx_swr2			 = NULL;
		ctx->data_src2			 = NULL;
		ctx->data_dst2			 = NULL;
		ctx->bufsize_src2		 = 0;
		ctx->bufsize_dst2		 = 0;
		ctx->linesize_src2		 = 0;
		ctx->linesize_dst2       = 0;
		ctx->nb_channels_src2    = 0;
		ctx->nb_channels_dst2	 = 0;
		ctx->nb_samples_src2	 = 1024;
		ctx->nb_samples_dst2     = 0;
		ctx->nb_samples_dst_max2 = 0;

		// Audio resample (DECKLINK)
		ctx->ctx_swr3			 = NULL;
		ctx->data_src3			 = NULL;
		ctx->data_dst3			 = NULL;
		ctx->bufsize_src3		 = 0;
		ctx->bufsize_dst3		 = 0;
		ctx->linesize_src3		 = 0;
		ctx->linesize_dst3       = 0;
		ctx->nb_channels_src3    = 0;
		ctx->nb_channels_dst3	 = 0;
		ctx->nb_samples_src3	 = 1024;
		ctx->nb_samples_dst3     = 0;
		ctx->nb_samples_dst_max3 = 0;

		// Audio resample (XDCAM MXF)
		ctx->ctx_swr4			 = NULL;
		ctx->data_src4			 = NULL;
		ctx->data_dst4			 = NULL;
		ctx->bufsize_src4		 = 0;
		ctx->bufsize_dst4		 = 0;
		ctx->linesize_src4		 = 0;
		ctx->linesize_dst4       = 0;
		ctx->nb_channels_src4    = 0;
		ctx->nb_channels_dst4	 = 0;
		ctx->nb_samples_src4	 = 1024;
		ctx->nb_samples_dst4     = 0;
		ctx->nb_samples_dst_max4 = 0;

		// Packets
		ctx->read_video          = 0;
		ctx->read_audio          = 0;
		ctx->st_index            = -1;
		ctx->frame_finished      = 0;
		ctx->st_in               = NULL;
		ctx->st_out              = NULL;

		// Play control	
		ctx->currframe           = 0;
		ctx->maxframe            = 0;
		ctx->decode_ready        = false;
		ctx->encode_ready        = false;
		ctx->record              = false;

		ctx->frame_video_src     = av_frame_alloc();
		ctx->frame_video_rgb     = av_frame_alloc();
		ctx->frame_video_dst     = av_frame_alloc();
		ctx->frame_video_filtered= av_frame_alloc();
		ctx->frame_audio_src     = av_frame_alloc();
		ctx->frame_audio_dst     = av_frame_alloc();

		av_init_packet(&(ctx->packet_read));
		ctx->packet_read.data    = NULL;
		ctx->packet_read.size    = 0;
		av_init_packet(&(ctx->packet_write));
		ctx->packet_write.data   = NULL;
		ctx->packet_write.size   = 0;
		ctx->read_packet         = 0;
		ctx->read_video          = 0;
		ctx->read_audio          = 0;
		ctx->encode_ready        = false;
		ctx->decode_ready        = false;

		// Audio play (WASAPI)
		ctx->wasapi = NULL;
		//ctx->wasapi = wasapi_open();

		// Filter
		ctx->filter_graph        = NULL;		
		ctx->ctx_buffersink      = NULL;
		ctx->ctx_buffersrc       = NULL;	

		// Clock
		ctx->clk_base_video      = -1;
		ctx->clk_base_audio      = -1;
	}
	catch (int32_t e)
	{
		cerr << "FFCONTEXT create failed:" << e << flush;
	}
	return ctx;
}

DllExport bool ffmpeg_delete (PBYTE p)
{
	bool ret = false;
	if (p != NULL)
	{
		FFMPEGCONTEXT* ctx = (FFMPEGCONTEXT*)p;
		av_frame_free  (&(ctx->frame_video_rgb));
		av_frame_free  (&(ctx->frame_video_src));
		av_frame_free  (&(ctx->frame_video_dst));
		av_frame_free  (&(ctx->frame_video_filtered));		
		av_frame_free  (&(ctx->frame_audio_src));
		av_frame_free  (&(ctx->frame_audio_dst));
		av_packet_unref(&(ctx->packet_read));
		av_packet_unref(&(ctx->packet_write));
		ffmpeg_filter_close(ctx);
		//wasapi_close(ctx->wasapi);
		delete ctx;
		ret = true;
	}
	return ret;
}

DllExport HDC	   ffmpeg_rgb_dc        (FFMPEGCONTEXT* ctx) {return ctx->dib_rgb->dc;}
DllExport void*    ffmpeg_rgb_p         (FFMPEGCONTEXT* ctx) {return ctx->dib_rgb->p;}
DllExport int32_t  ffmpeg_rgb_w         (FFMPEGCONTEXT* ctx) {return ctx->dib_rgb->w;}
DllExport int32_t  ffmpeg_rgb_h         (FFMPEGCONTEXT* ctx) {return ctx->dib_rgb->h;}
DllExport int64_t  ffmpeg_currframe     (FFMPEGCONTEXT* ctx) {return ctx->currframe;}
DllExport int64_t  ffmpeg_maxframe      (FFMPEGCONTEXT* ctx) {return ctx->maxframe;}
DllExport int32_t  ffmpeg_readpacket    (FFMPEGCONTEXT* ctx) {return ctx->read_packet;}
DllExport int32_t  ffmpeg_decoder_ready (FFMPEGCONTEXT* ctx) {return (int32_t)(ctx->decode_ready);}
DllExport int32_t  ffmpeg_encoder_ready (FFMPEGCONTEXT* ctx) {return (int32_t)(ctx->encode_ready);}
DllExport int32_t  ffmpeg_framefinished (FFMPEGCONTEXT* ctx) {return ctx->frame_finished;}
DllExport void     ffmpeg_set_record    (FFMPEGCONTEXT* ctx,int32_t num) {ctx->record = (num==0)?false:true;}
DllExport int32_t  ffmpeg_get_record    (FFMPEGCONTEXT* ctx) {return (ctx->record == true) ? 1 : 0;}
DllExport int32_t  ffmpeg_streamtype    (FFMPEGCONTEXT* ctx) {return ctx->st_in->type;} // 0 video , 1 audio



//--------------------------------
//-- Decoder
DllExport int64_t  ffmpeg_decoder_open  (FFMPEGCONTEXT* ctx,char* src,int32_t w,int32_t h)
{
	int64_t ret = 0;
	int32_t rst = 0;
	try 
	{	
		if (ctx->decode_ready == true) throw 1; // already ready
		ctx->fn_in = src;
		rst = avformat_open_input    (&(ctx->ctx_format_in),ctx->fn_in,NULL,NULL); if (rst < 0) throw -1;
		rst = avformat_find_stream_info(ctx->ctx_format_in ,NULL);                 if (rst < 0) throw -2;

		ctx->vstream_in_0 = -1;
		ctx->astream_in_0 = -1;
		ctx->astream_in_1 = -1;
		for(int32_t i=0; i< (int32_t)mkmin((ctx->ctx_format_in->nb_streams),FFMPEG_MAX_STREAM); i++)
		{	
			ffmpeg_stream_reset(&(ctx->stream_in[i]));
			ctx->stream_in[i].num = i;
			ctx->stream_in[i].type = ctx->ctx_format_in->streams[i]->codecpar->codec_type;
			ctx->stream_in[i].ctx_codec = avcodec_alloc_context3(NULL);
			rst = avcodec_parameters_to_context(ctx->stream_in[i].ctx_codec, ctx->ctx_format_in->streams[i]->codecpar);
			if (rst < 0)
			{
				avcodec_free_context(&ctx->stream_in[i].ctx_codec);
				throw - 22;
			}
			ctx->stream_in[i].ctx_codec->thread_count = 0;
			ctx->stream_in[i].codec = avcodec_find_decoder(ctx->stream_in[i].ctx_codec->codec_id);

		
			switch (ctx->ctx_format_in->streams[i]->codecpar->codec_type)
			{				
				case AVMEDIA_TYPE_VIDEO:
				{					
					rst = av_opt_set_int(ctx->stream_in[i].ctx_codec , "refcounted_frames", 1, 0);
					rst = avcodec_open2 (ctx->stream_in[i].ctx_codec, ctx->stream_in[i].codec, NULL); if (rst < 0) throw -3;

					if (ctx->vstream_in_0 == -1)
					{
						ctx->vstream_in_0 = i;
						ctx->maxframe  = get_maxframe(ctx->ctx_format_in->streams[i]);
						ctx->width_in               = ctx->stream_in[i].ctx_codec->width;
						ctx->height_in              = ctx->stream_in[i].ctx_codec->height;
						ctx->pixfmt_in              = ctx->stream_in[i].ctx_codec->pix_fmt;
						ctx->video_time_base_in     = ctx->stream_in[i].ctx_codec->time_base;
						ctx->sample_aspect_ratio_in = ctx->stream_in[i].ctx_codec->sample_aspect_ratio;
					}
					break;
				}
				case AVMEDIA_TYPE_AUDIO:
				{	
					rst = avcodec_open2(ctx->stream_in[i].ctx_codec, ctx->stream_in[i].codec, NULL); if (rst < 0) throw -4;

					if (ctx->astream_in_0 == -1 && ctx->astream_in_1 == -1)
					{
						ctx->astream_in_0 = i;
						ctx->sample_rate_in     = ctx->stream_in[i].ctx_codec->sample_rate;
						ctx->sample_fmt_in      = ctx->stream_in[i].ctx_codec->sample_fmt;
						ctx->channels_in        = ctx->stream_in[i].ctx_codec->channels;
						ctx->audio_time_base_in = ctx->stream_in[i].ctx_codec->time_base;
					}
					else if (ctx->astream_in_0 != -1 && ctx->astream_in_1 == -1)
						ctx->astream_in_1 = i;

					break;
				}
				default: break;
			}
		}
		if (ctx->vstream_in_0==-1) throw -5;


		// video resample
		ctx->frame_video_rgb->width  = w;
		ctx->frame_video_rgb->height = h;
		ctx->frame_video_rgb->format = AVPixelFormat(AV_PIX_FMT_BGRA);
		ctx->dib_rgb = dib_create(w,h);	
		av_image_fill_arrays(ctx->frame_video_rgb->data,ctx->frame_video_rgb->linesize,(uint8_t*)(ctx->dib_rgb->p),AV_PIX_FMT_BGRA, ctx->dib_rgb->w, ctx->dib_rgb->h,1);	
		ctx->ctx_sws_src_to_rgb = sws_getContext(ctx->width_in  , ctx->height_in , ctx->pixfmt_in,
												 ctx->dib_rgb->w, ctx->dib_rgb->h, AV_PIX_FMT_BGRA,
												 SWS_FAST_BILINEAR,
												 NULL, NULL, NULL);
		
		// Audio resample (for WASAPI)
		if (ctx->wasapi && ctx->astream_in_0 != -1)
		{
			ctx->ctx_swr2 = NULL;
			ctx->ctx_swr2 = swr_alloc();
			av_opt_set_int(ctx->ctx_swr2, "in_channel_layout", av_get_default_channel_layout(ctx->channels_in), 0);
			av_opt_set_int(ctx->ctx_swr2, "in_sample_rate", ctx->sample_rate_in, 0);
			av_opt_set_sample_fmt(ctx->ctx_swr2, "in_sample_fmt", ctx->sample_fmt_in, 0);
			av_opt_set_int(ctx->ctx_swr2, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
			av_opt_set_int(ctx->ctx_swr2, "out_sample_rate", ctx->wasapi->fmt->nSamplesPerSec, 0);
			av_opt_set_sample_fmt(ctx->ctx_swr2, "out_sample_fmt", AV_SAMPLE_FMT_S32, 0);
			swr_init(ctx->ctx_swr2);

			ctx->nb_channels_src2 = ctx->channels_in;
			ctx->nb_channels_dst2 = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
			ctx->nb_samples_src2 = ctx->nb_channels_src2 * ctx->sample_rate_in;
			ctx->nb_samples_dst2 = (int)av_rescale_rnd(ctx->nb_samples_src2, ctx->wasapi->fmt->nSamplesPerSec, ctx->sample_rate_in, AV_ROUND_UP);
			ctx->nb_samples_dst_max2 = ctx->nb_samples_src2;
			alloc_samples_array_and_data(&(ctx->data_src2), &(ctx->linesize_src2), ctx->nb_channels_src2, ctx->nb_samples_src2, ctx->sample_fmt_in, 0);
			alloc_samples_array_and_data(&(ctx->data_dst2), &(ctx->linesize_dst2), ctx->nb_channels_dst2, ctx->nb_samples_dst2, ctx->sample_fmt_out, 0);
			ctx->bufsize_src2 = av_samples_get_buffer_size(&(ctx->linesize_src2), ctx->nb_channels_src2, ctx->nb_samples_src2, ctx->sample_fmt_in, 1);
			ctx->bufsize_dst2 = av_samples_get_buffer_size(&(ctx->linesize_dst2), ctx->nb_channels_dst2, ctx->nb_samples_dst2, ctx->sample_fmt_out, 1);
		}

		
		if (ctx->astream_in_0 != -1 && ctx->astream_in_1 == -1)      // Audio resampler (for DeckLink) - Single Audio Stream
		{
			ctx->ctx_swr3 = NULL;
			ctx->ctx_swr3 = swr_alloc();
			av_opt_set_int       (ctx->ctx_swr3, "in_channel_layout", av_get_default_channel_layout(ctx->channels_in), 0);
			av_opt_set_int       (ctx->ctx_swr3, "in_sample_rate", ctx->sample_rate_in, 0);
			av_opt_set_sample_fmt(ctx->ctx_swr3, "in_sample_fmt", ctx->sample_fmt_in, 0);
			av_opt_set_int       (ctx->ctx_swr3, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
			av_opt_set_int       (ctx->ctx_swr3, "out_sample_rate", 48000, 0);
			av_opt_set_sample_fmt(ctx->ctx_swr3, "out_sample_fmt", AV_SAMPLE_FMT_S32, 0);
			swr_init             (ctx->ctx_swr3);

			ctx->nb_channels_src3 = ctx->channels_in;
			ctx->nb_channels_dst3 = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
			ctx->nb_samples_src3 = ctx->nb_channels_src3 * ctx->sample_rate_in;
			ctx->nb_samples_dst3 = (int)av_rescale_rnd(ctx->nb_samples_src3, 48000, ctx->sample_rate_in, AV_ROUND_UP);
			ctx->nb_samples_dst_max3 = ctx->nb_samples_src3;
			alloc_samples_array_and_data(&(ctx->data_src3), &(ctx->linesize_src3), ctx->nb_channels_src3, ctx->nb_samples_src3, ctx->sample_fmt_in, 0);
			alloc_samples_array_and_data(&(ctx->data_dst3), &(ctx->linesize_dst3), ctx->nb_channels_dst3, ctx->nb_samples_dst3, ctx->sample_fmt_out, 0);
			ctx->bufsize_src3 = av_samples_get_buffer_size(&(ctx->linesize_src3), ctx->nb_channels_src3, ctx->nb_samples_src3, ctx->sample_fmt_in, 1);
			ctx->bufsize_dst3 = av_samples_get_buffer_size(&(ctx->linesize_dst3), ctx->nb_channels_dst3, ctx->nb_samples_dst3, ctx->sample_fmt_out, 1);
		}		
		else if (ctx->astream_in_0 != -1 && ctx->astream_in_1 != -1) // Audio resampler (XDCAM MXF) - Multi Audio Stream, Use only first/second audio stream
		{
			ctx->ctx_swr4 = NULL;
			ctx->ctx_swr4 = swr_alloc();
			av_opt_set_int(ctx->ctx_swr4, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
			av_opt_set_int(ctx->ctx_swr4, "in_sample_rate", ctx->sample_rate_in, 0);
			av_opt_set_sample_fmt(ctx->ctx_swr4, "in_sample_fmt", av_get_planar_sample_fmt(ctx->sample_fmt_in), 0);
			av_opt_set_int(ctx->ctx_swr4, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
			av_opt_set_int(ctx->ctx_swr4, "out_sample_rate", 48000, 0);
			av_opt_set_sample_fmt(ctx->ctx_swr4, "out_sample_fmt", AV_SAMPLE_FMT_S32, 0);
			swr_init(ctx->ctx_swr4);

			ctx->nb_channels_src4 = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
			ctx->nb_channels_dst4 = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
			ctx->nb_samples_src4 = ctx->nb_channels_src4 * ctx->sample_rate_in;
			ctx->nb_samples_dst4 = (int)av_rescale_rnd(ctx->nb_samples_src4, 48000, ctx->sample_rate_in, AV_ROUND_UP);
			ctx->nb_samples_dst_max4 = ctx->nb_samples_src4;
			alloc_samples_array_and_data(&(ctx->data_src4), &(ctx->linesize_src4), ctx->nb_channels_src4, ctx->nb_samples_src4, av_get_planar_sample_fmt(ctx->sample_fmt_in), 0);
			alloc_samples_array_and_data(&(ctx->data_dst4), &(ctx->linesize_dst4), ctx->nb_channels_dst4, ctx->nb_samples_dst4, ctx->sample_fmt_out, 0);
			ctx->bufsize_src4 = av_samples_get_buffer_size(&(ctx->linesize_src4), ctx->nb_channels_src4, ctx->nb_samples_src4, av_get_planar_sample_fmt(ctx->sample_fmt_in), 1);
			ctx->bufsize_dst4 = av_samples_get_buffer_size(&(ctx->linesize_dst4), ctx->nb_channels_dst4, ctx->nb_samples_dst4, ctx->sample_fmt_out, 1);
		}


		// Packet Init
		av_init_packet(&(ctx->packet_read));
		ctx->packet_read.data =  NULL;
		ctx->packet_read.size =  0;
		ctx->currframe        = -1;
		ctx->clk_base_video   = -1;
		ctx->clk_base_audio   = -1;
		ctx->decode_ready     =  true;			

		// everything ok
		ret = ctx->maxframe;
	}
	catch (int32_t e)
	{
		cerr<<"ffmpeg decoder open error:"<<e<<endl<<flush;
		ret = e;
	}
	return ret;
}

DllExport int32_t ffmpeg_decoder_close(FFMPEGCONTEXT* ctx)
{
	if(ctx != NULL && ctx->decode_ready == true)
	{			
		// Reset stream
		if (ctx->ctx_format_in != NULL)
		{
			for (uint32_t i = 0; i < mkmin((ctx->ctx_format_in->nb_streams), FFMPEG_MAX_STREAM); i++)			
				ffmpeg_stream_reset(&(ctx->stream_in[i]));//, TRUE);			
			avformat_close_input(&(ctx->ctx_format_in));
			ctx->ctx_format_in = NULL;
		}

		// Video scaler
		sws_freeContext(ctx->ctx_sws_src_to_rgb);
		dib_delete((PBYTE)(ctx->dib_rgb));

		// Audio resampler (WASAPI)
		if (ctx->ctx_swr2 != NULL)
		{
			av_freep(&(ctx->data_src2[0]));
			av_freep(&(ctx->data_src2));
			av_freep(&(ctx->data_dst2[0]));
			av_freep(&(ctx->data_dst2));
			swr_free(&(ctx->ctx_swr2));
		}

		// Audio resampler (DeckLink) - Single Audio Stream
		if (ctx->ctx_swr3 != NULL)
		{
			av_freep(&(ctx->data_src3[0]));
			av_freep(&(ctx->data_src3));
			av_freep(&(ctx->data_dst3[0]));
			av_freep(&(ctx->data_dst3));
			swr_free(&(ctx->ctx_swr3));
		}

		// Audio buffer (XDCAM MXF) - Multi Audio Stream
		if (ctx->ctx_swr4 != NULL)
		{
			av_freep(&(ctx->data_src4[0]));
			av_freep(&(ctx->data_src4));
			av_freep(&(ctx->data_dst4[0]));
			av_freep(&(ctx->data_dst4));
			swr_free(&(ctx->ctx_swr4));
		}

		av_packet_unref(&(ctx->packet_read));
		
		ctx->fn_in        = NULL;
		ctx->decode_ready = false;
	}
	return 0;
}

// decode packet to make frame
// return 0 frame is not complete
// return 1 frame is complete
DllExport int32_t ffmpeg_decode(FFMPEGCONTEXT* ctx)
{
	double  pts = 0.0;
	int32_t frame_finished = 0;
	int32_t ret = -1;

	if (ctx->st_in)
	{
		switch (ctx->st_in->type)
		{
			case AVMEDIA_TYPE_VIDEO:
			{
				ret = avcodec_send_packet  (ctx->st_in->ctx_codec,&(ctx->packet_read));
				ret = avcodec_receive_frame(ctx->st_in->ctx_codec,  ctx->frame_video_src);
				ctx->frame_finished = (ret==0) ? 1 : 0;

				if (ctx->frame_finished)				
				{
					ctx->currframe++;               // Initial value is -1
					if (ctx->filter_graph == NULL)  // No FilterGraph
					{
						sws_scale(ctx->ctx_sws_src_to_rgb,
							ctx->frame_video_src->data, ctx->frame_video_src->linesize, 0, ctx->frame_video_src->height,
							ctx->frame_video_rgb->data, ctx->frame_video_rgb->linesize);
					}
					else                            // With FilterGraph
					{
						int32_t ret = av_buffersrc_add_frame_flags(ctx->ctx_buffersrc, ctx->frame_video_src, AV_BUFFERSRC_FLAG_KEEP_REF);
						while (ret >= 0)
						{
							ret = av_buffersink_get_frame(ctx->ctx_buffersink, ctx->frame_video_filtered);
							ffmpeg_avframe_copy(ctx->frame_video_rgb, ctx->frame_video_filtered);
							av_frame_unref(ctx->frame_video_filtered);
						}
						av_frame_unref(ctx->frame_video_src);
					}
					ctx->dib_rgb->framenum = ctx->currframe;
				}
				break;
			}
			case AVMEDIA_TYPE_AUDIO:
			{
				av_frame_unref(ctx->frame_audio_src);
				ret = avcodec_send_packet  (ctx->st_in->ctx_codec,&(ctx->packet_read));
				ret = avcodec_receive_frame(ctx->st_in->ctx_codec,  ctx->frame_audio_src);
				ctx->frame_finished = (ret==0) ? 1 : 0;
				break;
			}
			default: break;
		}
		frame_finished = ctx->frame_finished;
	}
	return frame_finished;
}

//-------------------------------------------
//-- Encoder
DllExport int32_t ffmpeg_encoder_open (FFMPEGCONTEXT* ctx, char* dest, int32_t file_format)
{
	if (ctx->encode_ready == false && ctx->decode_ready == true)
	{
		// Format
		ctx->fn_out = dest;

		switch (file_format)
		{
			case FFMPEG_MXF_XDCAM:
				avformat_alloc_output_context2(&(ctx->ctx_format_out), NULL, "mxf", NULL);
				ctx->format_out = ctx->ctx_format_out->oformat;

				// Streams setting for output
				// Audio ,For Sony XDCAM Viewer must have at least 2 audio stream	
				for (UINT i = 0; i < mkmin((ctx->ctx_format_in->nb_streams), FFMPEG_MAX_STREAM); i++)
				{
					switch (ctx->ctx_format_in->streams[i]->codecpar->codec_type)
					{
						case AVMEDIA_TYPE_VIDEO: ffmpeg_setstream_xdcam(ctx, i); break;
						case AVMEDIA_TYPE_AUDIO: ffmpeg_setstream_lpcm (ctx, i); break;
						default: break;
					}
				}
				avio_open(&(ctx->ctx_format_out->pb), ctx->fn_out, AVIO_FLAG_WRITE);
				ctx->ctx_format_out->oformat->audio_codec = AV_CODEC_ID_PCM_S24LE;
				break;

			case FFMPEG_MOV_XAVC:
				avformat_alloc_output_context2(&(ctx->ctx_format_out), NULL, "mov", NULL);
				ctx->format_out = ctx->ctx_format_out->oformat;

				for (UINT i = 0; i < mkmin((ctx->ctx_format_in->nb_streams), FFMPEG_MAX_STREAM); i++)
				{
					switch (ctx->ctx_format_in->streams[i]->codecpar->codec_type)
					{
						case AVMEDIA_TYPE_VIDEO: ffmpeg_setstream_xavc(ctx, i); break;
						case AVMEDIA_TYPE_AUDIO: ffmpeg_setstream_lpcm(ctx, i);	break;
						default: break;
					}
				}
				avio_open(&(ctx->ctx_format_out->pb), ctx->fn_out, AVIO_FLAG_WRITE);
				ctx->ctx_format_out->oformat->video_codec = AV_CODEC_ID_H264;
				ctx->ctx_format_out->oformat->audio_codec = AV_CODEC_ID_PCM_S24LE;
				break;
		}

		avformat_write_header (ctx->ctx_format_out,NULL);
		
		// Video scaler					
		if ((ctx->width_in  != ctx->width_out ) || (ctx->height_in != ctx->height_out))
		{
			dib_delete((PBYTE)(ctx->dib_rgb));				
			ctx->dib_rgb = dib_create(ctx->width_out,ctx->height_out);				
			av_image_fill_arrays(ctx->frame_video_rgb->data,ctx->frame_video_rgb->linesize,(uint8_t*)(ctx->dib_rgb->p),AV_PIX_FMT_BGRA, ctx->dib_rgb->w, ctx->dib_rgb->h,1);	
	
			if (ctx->ctx_sws_src_to_rgb != NULL)
				sws_freeContext(ctx->ctx_sws_src_to_rgb);
			ctx->ctx_sws_src_to_rgb = sws_getContext(
				ctx->width_in, ctx->height_in, ctx->pixfmt_in,
				ctx->width_out, ctx->height_out, AV_PIX_FMT_BGRA,
				SWS_FAST_BILINEAR, NULL, NULL, NULL);
		}
										
		ctx->dib_dst = dib_create(ctx->width_out,ctx->height_out);	
		av_image_fill_arrays(ctx->frame_video_dst->data,ctx->frame_video_dst->linesize,(uint8_t*)(ctx->dib_dst->p),ctx->pixfmt_out, ctx->dib_dst->w, ctx->dib_dst->h,1);	
		ctx->ctx_sws_rgb_to_dst = sws_getContext(ctx->dib_rgb->w, ctx->dib_rgb->h, AV_PIX_FMT_BGRA,
												 ctx->dib_dst->w, ctx->dib_dst->h, ctx->pixfmt_out,
												 SWS_FAST_BILINEAR, NULL, NULL, NULL);

		// Audio resample (for Encoding) NEED TO FIX !!				
		ctx->ctx_swr = swr_alloc();
		av_opt_set_int        (ctx->ctx_swr,"in_channel_layout" , AV_CH_LAYOUT_STEREO    ,0);
		av_opt_set_int        (ctx->ctx_swr,"in_sample_rate"    , ctx->sample_rate_in    ,0);
		av_opt_set_sample_fmt (ctx->ctx_swr,"in_sample_fmt"     , ctx->sample_fmt_in     ,0);
		av_opt_set_int        (ctx->ctx_swr,"out_channel_layout", AV_CH_LAYOUT_STEREO    ,0);
		av_opt_set_int        (ctx->ctx_swr,"out_sample_rate"   , ctx->sample_rate_out   ,0);
		av_opt_set_sample_fmt (ctx->ctx_swr,"out_sample_fmt"    , ctx->sample_fmt_out    ,0);
		swr_init(ctx->ctx_swr);

		ctx->nb_channels_src    = ctx->channels_in;
		ctx->nb_channels_dst    = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_MONO);
		ctx->nb_samples_src     = ctx->nb_channels_src * ctx->sample_rate_in;
		ctx->nb_samples_dst     = (int)av_rescale_rnd(ctx->nb_samples_src,48000,ctx->sample_rate_out,AV_ROUND_UP);
		ctx->nb_samples_dst_max = ctx->nb_samples_dst;
		alloc_samples_array_and_data       (&(ctx->data_src),&(ctx->linesize_src),ctx->nb_channels_src,ctx->nb_samples_src,ctx->sample_fmt_in ,0);			
		alloc_samples_array_and_data       (&(ctx->data_dst),&(ctx->linesize_dst),ctx->nb_channels_dst,ctx->nb_samples_dst,ctx->sample_fmt_out,0);
		ctx->bufsize_src        = av_samples_get_buffer_size(&(ctx->linesize_src),ctx->nb_channels_src,ctx->nb_samples_src,ctx->sample_fmt_in ,1);
		ctx->bufsize_dst        = av_samples_get_buffer_size(&(ctx->linesize_dst),ctx->nb_channels_dst,ctx->nb_samples_dst,ctx->sample_fmt_out,1);	
		
		// out frame setting
		ctx->frame_video_dst->format           = ctx->pixfmt_out;
		ctx->frame_video_dst->width            = ctx->width_out;
		ctx->frame_video_dst->height           = ctx->height_out;
		ctx->frame_video_dst->interlaced_frame = 1;
		ctx->frame_video_dst->top_field_first  = 1;		
		ctx->frame_video_dst->pkt_pts          = 0;
		ctx->frame_video_dst->pts              = 0;
		ctx->frame_audio_dst->nb_samples       = ctx->nb_samples_dst;
		ctx->frame_audio_dst->format           = ctx->sample_fmt_out;
		ctx->frame_audio_dst->channel_layout   = AV_CH_LAYOUT_MONO;

		av_init_packet(&(ctx->packet_write));
		ctx->packet_write.data = NULL;
		ctx->packet_write.size = 0;
		ctx->currframe         = 0;
		ctx->encode_ready      = true;
	}
	return 0;
}

DllExport int32_t ffmpeg_encoder_close(FFMPEGCONTEXT* ctx)
{
	int32_t ret = 0;
	if (ctx != NULL && ctx->encode_ready)
	{	
		if (ctx->ctx_format_out != NULL)
		{
			for (UINT i = 0; i < mkmin((ctx->ctx_format_out->nb_streams), FFMPEG_MAX_STREAM); i++)
				ffmpeg_stream_reset(&(ctx->stream_out[i]));//, TRUE);
		
			av_write_trailer     (ctx->ctx_format_out);
			avio_close           (ctx->ctx_format_out->pb);
			avformat_free_context(ctx->ctx_format_out);
		}
		sws_freeContext(ctx->ctx_sws_rgb_to_dst);
		dib_delete((PBYTE)(ctx->dib_dst));

		// Audio reset
		av_freep(&(ctx->data_src[0]));
		av_freep(&(ctx->data_src));
		av_freep(&(ctx->data_dst[0]));
		av_freep(&(ctx->data_dst));
		swr_free(&(ctx->ctx_swr));
				
		ctx->encode_ready   = false;
		ret = 1;
	}
	return ret;
}

DllExport void ffmpeg_encode (FFMPEGCONTEXT* ctx)
{	
	if(ctx->record && ctx->encode_ready && ctx->st_in && ctx->st_out)
	{
		int32_t ret = 0;
		int32_t got_output = 0;

		switch(ctx->st_in->type)
		{
		case AVMEDIA_TYPE_VIDEO: 
			sws_scale(ctx->ctx_sws_rgb_to_dst,
				ctx->frame_video_rgb->data,ctx->frame_video_rgb->linesize,0,ctx->height_out,
				ctx->frame_video_dst->data,ctx->frame_video_dst->linesize);
			
			ctx->frame_video_dst->pts += av_rescale_q(1, ctx->st_out->ctx_codec->time_base, ctx->st_out->stream->time_base);
			//ctx->frame_video_dst->pts = ctx->packet_read.dts;
			ret = avcodec_encode_video2 (ctx->st_out->ctx_codec,&(ctx->packet_write),ctx->frame_video_dst,&got_output); 
			break;

		case AVMEDIA_TYPE_AUDIO: 
			ctx->nb_samples_src = ctx->frame_audio_src->nb_samples;
			ctx->nb_samples_dst = (int32_t)av_rescale_rnd(ctx->nb_samples_src,ctx->sample_rate_in,ctx->sample_rate_out,AV_ROUND_UP);
			ctx->bufsize_src    = av_samples_get_buffer_size(&(ctx->linesize_src),ctx->nb_channels_src,ctx->nb_samples_src,ctx->sample_fmt_in,1);								
			memcpy(ctx->data_src[0],ctx->frame_audio_src->data[0],ctx->bufsize_src);
			ret = swr_convert(ctx->ctx_swr,ctx->data_dst,ctx->nb_samples_dst,(const uint8_t**)(ctx->data_src),ctx->nb_samples_src);												
			ctx->bufsize_dst    = av_samples_get_buffer_size(&(ctx->linesize_dst),ctx->nb_channels_dst,ret,AV_SAMPLE_FMT_S32,1);
			ctx->frame_audio_dst->nb_samples = ctx->nb_samples_dst;
			avcodec_fill_audio_frame(ctx->frame_audio_dst,1,AV_SAMPLE_FMT_S32 ,(uint8_t*) (ctx->data_dst[0]),ctx->bufsize_dst,1);
			ret = avcodec_encode_audio2(ctx->st_out->ctx_codec,&(ctx->packet_write),ctx->frame_audio_dst,&got_output);
			break;

		default: break;
		}
	
		if(!ret && got_output && ctx->packet_write.size)
		{
			ctx->packet_write.stream_index = ctx->st_index;
			ret = av_interleaved_write_frame(ctx->ctx_format_out,&(ctx->packet_write));				
			av_packet_unref(&(ctx->packet_write));
		}
	}	
}

DllExport void ffmpeg_encode_flush (FFMPEGCONTEXT* ctx)
{
	if(ctx->record && ctx->encode_ready && ctx->st_in && ctx->st_out)
	{	
		int32_t ret = 0;
		int32_t got_output = 0;

		for (UINT i = 0; i >= 0 && i < mkmin((ctx->ctx_format_out->nb_streams),FFMPEG_MAX_STREAM); i++)
		{
			for (got_output=1; got_output;) 
			{
				switch (ctx->stream_out[i].type)
				{
				case AVMEDIA_TYPE_VIDEO: ret = avcodec_encode_video2 (ctx->stream_out[i].ctx_codec,&(ctx->packet_write),NULL,&got_output); break;
				case AVMEDIA_TYPE_AUDIO: ret = avcodec_encode_audio2 (ctx->stream_out[i].ctx_codec,&(ctx->packet_write),NULL,&got_output); break;
				default: break;
				}
				if (!ret && got_output && (ctx->packet_write.size)) 
				{
					ctx->packet_write.stream_index = i;
					ret = av_interleaved_write_frame(ctx->ctx_format_out,&(ctx->packet_write));				
					av_packet_unref(&(ctx->packet_write));
				}
			}	
		}
	}
}



// Encoder Setting
// XDCAM
DllExport int32_t ffmpeg_setstream_xdcam(FFMPEGCONTEXT* ctx, int32_t i)
{
	int rst = -1;

	ctx->width_out                = 1920;
	ctx->height_out               = 1080;
	ctx->pixfmt_out               = AV_PIX_FMT_YUV422P;
	ctx->stream_out[i].num        = i;
	ctx->stream_out[i].type       = AVMEDIA_TYPE_VIDEO;
	ctx->stream_out[i].codec      = avcodec_find_encoder(AV_CODEC_ID_MPEG2VIDEO);
	ctx->stream_out[i].stream     = avformat_new_stream(ctx->ctx_format_out, ctx->stream_out[i].codec);
	ctx->stream_out[i].stream->id = i;
	ctx->stream_out[i].ctx_codec = avcodec_alloc_context3(NULL);
	rst = avcodec_parameters_to_context(ctx->stream_out[i].ctx_codec, ctx->stream_out[i].stream->codecpar);
	if (rst < 0) avcodec_free_context(&ctx->stream_in[i].ctx_codec);
	ctx->stream_out[i].ctx_codec->codec_id   = AV_CODEC_ID_MPEG2VIDEO;
	ctx->stream_out[i].ctx_codec->codec_type = AVMEDIA_TYPE_VIDEO;
	ctx->stream_out[i].ctx_codec->flags     |= CODEC_FLAG_INTERLACED_DCT | CODEC_FLAG_INTERLACED_ME;
	av_opt_set_int(ctx->stream_out[i].ctx_codec->priv_data, "intra_vlc", 1, 0);
	av_opt_set_int(ctx->stream_out[i].ctx_codec->priv_data, "non_linear_quant", 1, 0);
	ctx->stream_out[i].ctx_codec->gop_size		         = 12;
	ctx->stream_out[i].ctx_codec->qmin		             = 1;
	ctx->stream_out[i].ctx_codec->qmax					 = 12;
	//ctx->stream_out[i].ctx_codec->me_method			     = 5;
	ctx->stream_out[i].ctx_codec->intra_dc_precision       = 2;// 10-8
	ctx->stream_out[i].ctx_codec->rc_max_available_vbv_use = 1;
	ctx->stream_out[i].ctx_codec->rc_min_vbv_overflow_use  = 1;
	//ctx->stream_out[i].ctx_codec->lmin					 = 1 * FF_QP2LAMBDA;
	//ctx->stream_out[i].ctx_codec->lmax					 = 3 * FF_QP2LAMBDA;
	ctx->stream_out[i].ctx_codec->mb_decision			 = FF_MB_DECISION_SIMPLE;
	ctx->stream_out[i].ctx_codec->pix_fmt				 = AV_PIX_FMT_YUV422P;//   ctx->pixfmt_out;
	ctx->stream_out[i].ctx_codec->bit_rate				 = 50000000;
	ctx->stream_out[i].ctx_codec->rc_max_rate			 = 50000000;
	ctx->stream_out[i].ctx_codec->rc_min_rate			 = 50000000;
	ctx->stream_out[i].ctx_codec->rc_buffer_size		 = 36408333;
	//ctx->stream_out[i].ctx_codec->stream_codec_tag		 = MKTAG('x', 'd', '5', 'c');
	ctx->stream_out[i].ctx_codec->codec_tag				 = MKTAG('x', 'd', '5', 'c');
	ctx->stream_out[i].ctx_codec->color_primaries        = AVCOL_PRI_BT709;
	ctx->stream_out[i].ctx_codec->color_trc              = AVCOL_TRC_BT709;
	ctx->stream_out[i].ctx_codec->colorspace             = AVCOL_SPC_BT709;
	ctx->stream_out[i].ctx_codec->color_range            = AVCOL_RANGE_MPEG;
	ctx->stream_out[i].ctx_codec->chroma_sample_location = AVCHROMA_LOC_LEFT;
	ctx->stream_out[i].ctx_codec->width					 = ctx->width_out;
	ctx->stream_out[i].ctx_codec->height                 = ctx->height_out;
	ctx->stream_out[i].ctx_codec->thread_count			 = 0; //auto ctx->stream_out[i].ctx_codec->thread_count;
	ctx->stream_out[i].ctx_codec->time_base.num			 = 1001;
	ctx->stream_out[i].ctx_codec->time_base.den			 = 30000;
	avcodec_open2(ctx->stream_out[i].ctx_codec, ctx->stream_out[i].codec, NULL);

	return 1;
}

// Set stream i as LPCM 48000
DllExport int32_t ffmpeg_setstream_lpcm(FFMPEGCONTEXT* ctx, int32_t i)
{
	int32_t rst = -1;
	ctx->sample_rate_out					  = 48000;
	ctx->sample_fmt_out						  = AV_SAMPLE_FMT_S32;
	ctx->stream_out[i].num					  = i;
	ctx->stream_out[i].type					  = AVMEDIA_TYPE_AUDIO;
	ctx->stream_out[i].codec				  = avcodec_find_encoder(AV_CODEC_ID_PCM_S24LE);
	ctx->stream_out[i].stream				  = avformat_new_stream(ctx->ctx_format_out, ctx->stream_out[i].codec);
	ctx->stream_out[i].stream->id             = i;
	ctx->stream_out[i].ctx_codec = avcodec_alloc_context3(NULL);
	rst = avcodec_parameters_to_context(ctx->stream_out[i].ctx_codec, ctx->stream_out[i].stream->codecpar);
	if (rst < 0) avcodec_free_context(&ctx->stream_in[i].ctx_codec);
	ctx->stream_out[i].ctx_codec->sample_fmt  = AV_SAMPLE_FMT_S32;
	ctx->stream_out[i].ctx_codec->bit_rate    = 1152000;
	ctx->stream_out[i].ctx_codec->sample_rate = 48000;
	ctx->stream_out[i].ctx_codec->channels    = 1;
	avcodec_open2(ctx->stream_out[i].ctx_codec, ctx->stream_out[i].codec, NULL);
	
	return rst;
}

DllExport int32_t ffmpeg_setstream_xavc(FFMPEGCONTEXT* ctx, int32_t i)
{
	int32_t rst = -1;
	ctx->width_out  = 4096;// 3840;
	ctx->height_out = 2160;
	ctx->pixfmt_out = AV_PIX_FMT_YUV422P ; //AV_PIX_FMT_YUV422P10LE

	ctx->stream_out[i].num                   = i;
	ctx->stream_out[i].type                  = AVMEDIA_TYPE_VIDEO;
	ctx->stream_out[i].codec                 = avcodec_find_encoder(AV_CODEC_ID_H264);
	ctx->stream_out[i].stream                = avformat_new_stream(ctx->ctx_format_out, ctx->stream_out[i].codec);
	ctx->stream_out[i].stream->id            = i;
	ctx->stream_out[i].ctx_codec = avcodec_alloc_context3(NULL);
	rst = avcodec_parameters_to_context(ctx->stream_out[i].ctx_codec, ctx->stream_out[i].stream->codecpar);
	if (rst < 0) avcodec_free_context(&ctx->stream_in[i].ctx_codec);
	ctx->stream_out[i].ctx_codec->codec_id   = AV_CODEC_ID_H264;
	ctx->stream_out[i].ctx_codec->codec_type = AVMEDIA_TYPE_VIDEO;
	//ctx->stream_out[i].ctx_codec->flags     |= CODEC_FLAG_INTERLACED_DCT | CODEC_FLAG_INTERLACED_ME;
	//av_opt_set_int(ctx->stream_out[i].ctx_codec->priv_data, "intra_vlc"       , 1, 0);
	//av_opt_set_int(ctx->stream_out[i].ctx_codec->priv_data, "non_linear_quant", 1, 0);
	//ctx->stream_out[i].ctx_codec->gop_size         = 12;
	//ctx->stream_out[i].ctx_codec->qmin             = 1;
	//ctx->stream_out[i].ctx_codec->qmax             = 12;
	//ctx->stream_out[i].ctx_codec->me_method        = 5;
	//ctx->stream_out[i].ctx_codec->intra_dc_precision       = 0;
	//ctx->stream_out[i].ctx_codec->rc_max_available_vbv_use = 0;
	//ctx->stream_out[i].ctx_codec->rc_min_vbv_overflow_use  = 3;
	//ctx->stream_out[i].ctx_codec->lmin             = 1 * FF_QP2LAMBDA;
	//ctx->stream_out[i].ctx_codec->lmax             = 3 * FF_QP2LAMBDA;
	//ctx->stream_out[i].ctx_codec->mb_decision      = FF_MB_DECISION_SIMPLE;
	ctx->stream_out[i].ctx_codec->pix_fmt          = ctx->pixfmt_out;
	//ctx->stream_out[i].ctx_codec->bit_rate         = 50000000;
	//ctx->stream_out[i].ctx_codec->rc_max_rate      = 50000000;
	//ctx->stream_out[i].ctx_codec->rc_min_rate      = 50000000;
	//ctx->stream_out[i].ctx_codec->rc_buffer_size   = 36408333;
	//ctx->stream_out[i].ctx_codec->stream_codec_tag = 0;
	//ctx->stream_out[i].ctx_codec->codec_tag        = 0;
	//ctx->stream_out[i].ctx_codec->color_primaries  = AVCOL_PRI_UNSPECIFIED;
	//ctx->stream_out[i].ctx_codec->color_range      = AVCOL_RANGE_UNSPECIFIED;
	//ctx->stream_out[i].ctx_codec->color_trc        = AVCOL_TRC_UNSPECIFIED;
	//ctx->stream_out[i].ctx_codec->colorspace       = AVCOL_SPC_UNSPECIFIED;	
	//ctx->stream_out[i].ctx_codec->chroma_sample_location = AVCHROMA_LOC_LEFT;
	ctx->stream_out[i].ctx_codec->width            = ctx->width_out;
	ctx->stream_out[i].ctx_codec->height           = ctx->height_out;
	ctx->stream_out[i].ctx_codec->thread_count     = 0;
	ctx->stream_out[i].ctx_codec->time_base.num    = 1001;
	ctx->stream_out[i].ctx_codec->time_base.den    = 30000;
	avcodec_open2(ctx->stream_out[i].ctx_codec, ctx->stream_out[i].codec, NULL);

	return rst;
}


DllExport int32_t ffmpeg_seek (FFMPEGCONTEXT* ctx,uint32_t frame)
{
	if(frame>=0 && frame<ctx->maxframe)
	{
		ctx->currframe = frame;	
		av_seek_frame(ctx->ctx_format_in, 0, frame_to_timestamp(ctx->ctx_format_in->streams[0],frame), AVSEEK_FLAG_FRAME);										
	}
	return 0;
}

DllExport void ffmpeg_packet_reset(FFMPEGCONTEXT* ctx)
{
	if (ctx != NULL)
	{
		av_init_packet(&(ctx->packet_read));
		ctx->packet_read.data = NULL;
		ctx->packet_read.size = 0;
		av_init_packet(&(ctx->packet_write));
		ctx->packet_write.data = NULL;
		ctx->packet_write.size = 0;
		ctx->read_packet = 0;
		ctx->read_video = 0;
		ctx->read_audio = 0;
	}
}

DllExport int32_t ffmpeg_packet_read (FFMPEGCONTEXT* ctx)
{
	int32_t ret = 0;
	if (ctx != NULL)
	{
		ctx->read_packet = av_read_frame(ctx->ctx_format_in, &(ctx->packet_read));  // 0 is OK , 0 < Error
		ctx->st_in    = NULL;
		ctx->st_out   = NULL;
		ctx->st_index = ctx->packet_read.stream_index;
		if (ctx->st_index >= 0 && ctx->st_index < FFMPEG_MAX_STREAM)
		{
			ctx->st_in  = &(ctx->stream_in [ctx->st_index]);
			ctx->st_out = &(ctx->stream_out[ctx->st_index]);
			ret = 1;
		}
	}
	return ret;
}

DllExport void ffmpeg_packet_free (FFMPEGCONTEXT* ctx)
{
	av_packet_unref(&(ctx->packet_read));
}



// write decoded audio data from FFMPEG to WASAPI
DllExport int32_t ffmpeg_wasapi_feed (FFMPEGCONTEXT* ctx,int32_t stream_src)
{
	if(ctx->wasapi && ctx->packet_read.stream_index == stream_src)
	{
		WASAPICONTEXT* w = ctx->wasapi;
		HRESULT hr;
		int32_t ret = 0;
		w->pAudioClient->GetCurrentPadding(&(w->frame_fill));
		w->frame_avail = (w->frame_total) - (w->frame_fill);

		ctx->nb_samples_src2 = ctx->frame_audio_src->nb_samples;  // block num ?  nb_samples / ch
		ctx->nb_samples_dst2 = w->frame_avail;		              // block num ?  maximum dst sample		
		ctx->bufsize_src2    = av_samples_get_buffer_size(&(ctx->linesize_src2), ctx->nb_channels_src2, ctx->nb_samples_src2, ctx->sample_fmt_in, 1);
		for(uint32_t i=0;i<AV_NUM_DATA_POINTERS;i++)
		{
			uint8_t* s = ctx->frame_audio_src->data[i];
			if (s!=NULL) memcpy(ctx->data_src2[i], ctx->frame_audio_src->data[i], ctx->linesize_src2);
		}
		ret                  = swr_convert(ctx->ctx_swr2,ctx->data_dst2,ctx->nb_samples_dst2,(const uint8_t**)(ctx->data_src2),(ctx->nb_samples_src2));
		ctx->bufsize_dst2    = av_samples_get_buffer_size(&(ctx->linesize_dst2),ctx->nb_channels_dst2,ret,AV_SAMPLE_FMT_S32,1);

		//cout<<"ffile:"<<w->frame_fill<<"  ftotal:"<<w->frame_total;
		//cout<<"  dst_nb:"<<ctx->dst_nb_samples2<<"  src_nb:"<<ctx->src_nb_samples2<<"  ret:"<<ret<<endl;

		hr  = w->pRenderClient->GetBuffer(w->frame_avail, &(w->pData));	
		memcpy(w->pData,ctx->data_dst2[0], min((UINT)(ctx->bufsize_dst2),(w->frame_avail)*8));	
		hr  = w->pRenderClient->ReleaseBuffer(w->frame_avail,w->flags);	
	}
	return 0;
}

// Single Audio Stream (Return Audio resampler buffer for DeckLink)
DllExport MKBUF* ffmpeg_decklink_feed (FFMPEGCONTEXT* ctx)
{
	MKBUF* b = NULL;
	if (ctx != NULL)
	{
		int32_t ret = 0;
		ctx->nb_samples_src3 = ctx->frame_audio_src->nb_samples;
		ctx->nb_samples_dst3 = av_rescale_rnd(swr_get_delay(ctx->ctx_swr3,ctx->sample_rate_in)+ctx->nb_samples_src3,48000,ctx->sample_rate_in,AV_ROUND_UP);
		ctx->bufsize_src3    = av_samples_get_buffer_size(&(ctx->linesize_src3), ctx->nb_channels_src3, ctx->nb_samples_src3, ctx->sample_fmt_in, 1);
		for(uint32_t i=0;i<AV_NUM_DATA_POINTERS;i++)
		{
			uint8_t* s = ctx->frame_audio_src->data[i];
			if (s!=NULL) memcpy(ctx->data_src3[i], ctx->frame_audio_src->data[i], ctx->linesize_src3);
		}
		ret = swr_convert(ctx->ctx_swr3, ctx->data_dst3, ctx->nb_samples_dst3, (const uint8_t**)(ctx->data_src3), ctx->nb_samples_src3);
		ctx->bufsize_dst3    = av_samples_get_buffer_size(&(ctx->linesize_dst3), ctx->nb_channels_dst3, ret, AV_SAMPLE_FMT_S32, 1);

		b = mkbuf_create(ctx->bufsize_dst3, ctx->data_dst3[0]);
		b->var = ret;
	}
	return b;
}

// Multi Audio Stream (XDCAM Left/Right Resampling)
DllExport MKBUF* ffmpeg_xdcam_feed (FFMPEGCONTEXT* ctx)
{
	MKBUF* b = NULL;
	if (ctx != NULL)
	{
		int32_t ret = 0;
		ctx->nb_samples_src4 = ctx->frame_audio_src->nb_samples;
		ctx->nb_samples_dst4 = av_rescale_rnd(swr_get_delay(ctx->ctx_swr4,ctx->sample_rate_in)+ctx->nb_samples_src4,48000,ctx->sample_rate_in,AV_ROUND_UP);
		ctx->bufsize_src4    = av_samples_get_buffer_size(&(ctx->linesize_src4), ctx->nb_channels_src4, ctx->nb_samples_src4, av_get_planar_sample_fmt(ctx->sample_fmt_in), 1);

		if (ctx->st_index == ctx->astream_in_0)      // First Audio Stream
		{
			memcpy(ctx->data_src4[0], ctx->frame_audio_src->data[0], ctx->linesize_src4);
		}
		else if (ctx->st_index == ctx->astream_in_1) // Second Audio Stream
		{
			memcpy(ctx->data_src4[1], ctx->frame_audio_src->data[0], ctx->linesize_src4);
			ret = swr_convert(ctx->ctx_swr4, ctx->data_dst4, ctx->nb_samples_dst4, (const uint8_t**)(ctx->data_src4), ctx->nb_samples_src4);
			ctx->bufsize_dst4 = av_samples_get_buffer_size(&(ctx->linesize_dst4), ctx->nb_channels_dst4, ret, AV_SAMPLE_FMT_S32, 1);

			b = mkbuf_create(ctx->bufsize_dst4, ctx->data_dst4[0]);
			b->var = ret * 2;
		}
	}
	return b;
}

// MISC functions
DllExport int32_t ffmpeg_save_png (const wchar_t* fn,DIB* dib_src)
{
	AVCodecContext* ctx_codec = NULL;
	AVCodec*        codec     = NULL;
	SwsContext*     ctx_sws   = NULL;
	AVFrame*        frame_src = NULL;
	AVFrame*        frame_dst = NULL;	
	DIB*            dib_dst   = NULL;
	AVPacket        packet;	
	int32_t         ret       = 0;
	int32_t         err       = 0;
	int32_t         got_output= 0;
	
	try
	{			
		frame_src = av_frame_alloc();  	                                      if (frame_src==NULL) throw 2;
		frame_dst = av_frame_alloc();                                         if (frame_dst==NULL) throw 21;
		dib_dst   = dib_create(dib_src->w,dib_src->h);
		
		av_image_fill_arrays(frame_src->data,frame_src->linesize,(uint8_t*)(dib_src->p),AV_PIX_FMT_BGRA, dib_src->w, dib_src->h,1);	
		av_image_fill_arrays(frame_dst->data,frame_dst->linesize,(uint8_t*)(dib_dst->p),AV_PIX_FMT_BGRA, dib_dst->w, dib_dst->h,1);	
		ctx_sws   = sws_getContext(dib_src->w,dib_src->h,AV_PIX_FMT_BGRA,dib_dst->w,dib_dst->h,AV_PIX_FMT_RGB24,SWS_BILINEAR, NULL, NULL, NULL);

		sws_scale(ctx_sws, 
				  frame_src->data, frame_src->linesize, 0,frame_src->height,
				  frame_dst->data, frame_dst->linesize);
		 
		av_init_packet(&packet);
		packet.data        = NULL;
		packet.size        = 0;
		codec              = avcodec_find_encoder_by_name("png");	
		ctx_codec          = avcodec_alloc_context3(codec);                   if (ctx_codec==NULL) throw 3;
		ctx_codec->width   = dib_dst->w;
		ctx_codec->height  = dib_dst->h;
		ctx_codec->pix_fmt = AV_PIX_FMT_RGB24;
		err = avcodec_open2(ctx_codec,codec,NULL); 	                          if (err < 0) throw 4;
		err = avcodec_encode_video2(ctx_codec,&packet,frame_dst,&got_output); if (err < 0) throw 5;

		// Write to file
		FILE* fp;
		fp = _wfopen(fn,L"wb");
		fwrite(packet.data,sizeof(uint8_t),packet.size,fp);
		fclose(fp);

		// Close
		avcodec_free_context(&ctx_codec);
		av_packet_unref(&packet);				
		sws_freeContext(ctx_sws);
		av_free(frame_src);
		av_free(frame_dst);
		dib_delete ((PBYTE)dib_dst);
	}
	catch (int32_t e)
	{
		ret = e;
	}


	return ret;
}


// Filter Initialize and Construct filter
DllExport int32_t ffmpeg_filter_open (FFMPEGCONTEXT* ctx,const char* filter_desc)
{
	int32_t ret = 0;
	char    buf[512];
	avfilter_register_all();
	AVFilter*           buffersrc  = avfilter_get_by_name("buffer");
	AVFilter*           buffersink = avfilter_get_by_name("buffersink");
	AVFilterInOut*      outputs    = avfilter_inout_alloc();
	AVFilterInOut*      inputs     = avfilter_inout_alloc();
	AVPixelFormat       pix_fmts[] = {AV_PIX_FMT_BGRA,AV_PIX_FMT_NONE};

	// Filter Graph alloc
	ctx->filter_graph = avfilter_graph_alloc();

	// Source
	sprintf(buf,
			"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",			
			ctx->width_in, ctx->height_in  , ctx->pixfmt_in,
			ctx->video_time_base_in.num    , ctx->video_time_base_in.den,
			ctx->sample_aspect_ratio_in.num, ctx->sample_aspect_ratio_in.den);
	ret = avfilter_graph_create_filter(&(ctx->ctx_buffersrc), buffersrc, "in", buf, NULL, ctx->filter_graph);

	// Sink
	ret = avfilter_graph_create_filter(&(ctx->ctx_buffersink),buffersink, "out",NULL, NULL, ctx->filter_graph);
	ret = av_opt_set_int_list(ctx->ctx_buffersink, "pix_fmts", pix_fmts,AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);

	// In/out filter
	outputs->name       = av_strdup("in");
	outputs->filter_ctx = ctx->ctx_buffersrc;
	outputs->pad_idx    = 0;
	outputs->next       = NULL;
	inputs->name        = av_strdup("out");
	inputs->filter_ctx  = ctx->ctx_buffersink;
	inputs->pad_idx     = 0;
	inputs->next        = NULL;

	// construct filtergraph	
	char filters[1000];
	sprintf(filters,"scale=%d:%d, %s",ctx->frame_video_rgb->width,ctx->frame_video_rgb->height,filter_desc);
	ret = avfilter_graph_parse_ptr (ctx->filter_graph, filters, &inputs, &outputs, NULL);
	ret = avfilter_graph_config    (ctx->filter_graph, NULL);

	avfilter_inout_free(&inputs);
	avfilter_inout_free(&outputs);
	return 0;
}

DllExport int32_t ffmpeg_filter_close(FFMPEGCONTEXT* ctx)
{
	if (ctx->filter_graph != NULL)
	{
		avfilter_graph_free(&(ctx->filter_graph));
	}
	return 0;
}

// BitBlt AVFrame to HDC
void ffmpeg_bitblt(HDC dst,int x,int y,int w,int h,const AVFrame *frame)
{
	int32_t     ret       = 0;
	SwsContext* sws       = sws_getContext(frame->width,frame->height,AVPixelFormat(frame->format),w,h,AV_PIX_FMT_BGRA,SWS_FAST_BILINEAR,NULL, NULL, NULL);
	AVFrame*    frame_rgb = av_frame_alloc();
	DIB*        dib_rgb   = dib_create(w,h);
	av_image_fill_arrays(frame_rgb->data,frame_rgb->linesize,(uint8_t*)(dib_rgb->p),AV_PIX_FMT_BGRA,dib_rgb->w, dib_rgb->h,1);		
	ret = sws_scale(sws, frame->data, frame->linesize, 0,frame->height,frame_rgb->data, frame_rgb->linesize);		 				                         							
	::BitBlt(dst,x,y,w,h,dib_rgb->dc,0,0,SRCCOPY);				
	dib_delete((PBYTE)dib_rgb);
	av_frame_free(&frame_rgb);
	sws_freeContext(sws);
}

// Copy AVFrame to AVFrame
void ffmpeg_avframe_copy (AVFrame* frame_d,AVFrame* frame_s)
{
	SwsContext* sws = sws_getContext(frame_s->width,frame_s->height,AVPixelFormat(frame_s->format),
									 frame_d->width,frame_d->height,AVPixelFormat(frame_d->format),
									 SWS_FAST_BILINEAR,NULL, NULL, NULL);
	sws_scale(sws, frame_s->data, frame_s->linesize, 0,frame_s->height,frame_d->data, frame_d->linesize);
	sws_freeContext(sws);
}

// Get PTS of Video/Audio
double_t ffmpeg_pts_video(FFMPEGCONTEXT* c)
{
	double_t pts = 0.0;
	pts = (c->packet_read.dts == AV_NOPTS_VALUE) ? 0.0 : c->packet_read.dts;
	if (c->clk_base_video < 0) c->clk_base_video = pts;
	pts -= c->clk_base_video;
	pts /= c->packet_read.duration;
	pts *= av_q2d(c->video_time_base_in); // pts -> sec
	pts *= 1000.0;                        // sec -> ms
	return pts;
}

double_t ffmpeg_pts_audio(FFMPEGCONTEXT* c)
{
	double_t pts = 0.0;
	pts = (c->packet_read.dts == AV_NOPTS_VALUE) ? 0.0 : c->packet_read.dts;
	if (c->clk_base_audio < 0) c->clk_base_audio = pts;
	pts -= c->clk_base_audio;
	pts *= av_q2d(c->audio_time_base_in);
	pts *= 1000.0;
	pts *= 0.5;
	return pts;
}



//
////-------------------------------------------------
////-- M A I N
//
//int main()
//{
//	cout << "START PLAY" << endl;
//	FFMPEGCONTEXT* ctx = ffmpeg_open();
//	int ret = 0;
//
//	ffmpeg_decoder_close(ctx);
//	ffmpeg_encoder_close(ctx);
//	ffmpeg_decoder_open(ctx, "E:\\movie\\error\\error_roommate.mp4", 1920, 1080);
//	ffmpeg_encoder_open(ctx, "E:\\movie\\error\\333.mxf", 1);
//	ffmpeg_set_record(ctx, 1);
//
//	while (ffmpeg_readpacket(ctx) == 0)
//	{
//		ret = ffmpeg_packet_read(ctx);
//		if (ret > 0)
//		{
//			ffmpeg_decode(ctx);
//
//			if (ffmpeg_framefinished(ctx) > 0)
//			{
//				switch (ffmpeg_streamtype(ctx))
//				{
//				case AVMEDIA_TYPE_VIDEO:
//					//::BitBlt(GetDC(NULL), 0, 0, 1920, 1080, ctx->dib_rgb.dc, 0, 0, SRCCOPY);
//					cout << "Frame:" << ctx->currframe << endl;
//					break;
//				case AVMEDIA_TYPE_AUDIO:
//					ffmpeg_wasapi_feed(ctx, 1);
//
//					break;
//				default: break;
//				}
//				ffmpeg_encode(ctx);
//			}
//		}
//		ffmpeg_packet_free(ctx);
//
//		//if (ctx->currframe > 1000) break;
//	}
//	ffmpeg_encode_flush(ctx);
//
//	ffmpeg_encoder_close(ctx);
//	ffmpeg_decoder_close(ctx);
//	ffmpeg_close(ctx);
//	cout << "STOP  PLAY" << endl;
//	return 0;
//}