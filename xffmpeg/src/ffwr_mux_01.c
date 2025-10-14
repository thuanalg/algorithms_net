/*
 * Copyright (c) 2003 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file libavformat muxing API usage example
 * @example mux.c
 *
 * Generate a synthetic audio and video signal and mux them to a media file in
 * any supported libavformat format. The default codecs are used.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <simplelog.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>

#define STREAM_DURATION   20.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV422P /* default pix_fmt AV_PIX_FMT_YUV422P */
//#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt AV_PIX_FMT_YUV422P */
#define NUMBER_FRAMES           (STREAM_FRAME_RATE * STREAM_DURATION)
#define SCALE_FLAGS SWS_BICUBIC
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int count_frame = 0;
int count_frame_audio = 0;
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

// a wrapper around a single output AVStream
typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;

    AVFrame *frame;
    AVFrame *a_frame;
    AVFrame *tmp_frame;

    AVPacket *tmp_pkt;

    float t, tincr, tincr2;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

typedef struct __FFWR_INSTREAM__ {
    AVFormatContext *fmt_ctx;

    AVStream *v_st;
    AVCodec *v_codec;
    AVCodecContext *v_cctx;
    AVPacket pkt;
    AVFrame *vframe;
    SwsContext *vscale;
    

    AVStream *a_st;
    AVCodec *a_codec;
    AVCodecContext *a_cctx;
    AVPacket a_pkt;
    AVFrame *a_frame;
    AVFrame *a_dstframe;
    SwrContext *a_scale;    

} FFWR_INSTREAM;

FFWR_INSTREAM gb_instream;
FFWR_INSTREAM gb_instream_audio;
static AVFrame *get_audio_frame_trigger(AVPacket *pkt);
int ffwr_open_input(FFWR_INSTREAM *pinput, char *name, int mode);
int ffwr_close_input(FFWR_INSTREAM *pinput);
int convert_frame(AVFrame *src, AVFrame *dst);
int ffwr_close_input(FFWR_INSTREAM *pinput)
{
    int ret = 0;
    do {
        if(pinput->vframe) {
            av_frame_free(&(pinput->vframe));
        }
        if(pinput->v_cctx) {
            avcodec_free_context(&(pinput->v_cctx));
        }
        if(pinput->fmt_ctx) {
            avformat_free_context(pinput->fmt_ctx);
        }
        if(pinput->vscale) {
            sws_freeContext(pinput->vscale);
        }    
        /*-----------------------*/      
    } while(0);
    return ret;
}

int ffwr_open_input(FFWR_INSTREAM *pinput, char *name, int mode) {
    int ret = 0;
    int result = 0;
    AVInputFormat *iformat = 0; 
    avdevice_register_all();
    AVDictionary *options = 0;
    do {
        if(!pinput) {
            ret = 1;
            spllog(4, "--");
            break;
        }
        iformat = av_find_input_format("dshow");
        if(!iformat) {
            ret = 1;
            spllog(4, "--");
            break;
        }

		av_dict_set(&options, "rtbufsize", "50M", 0);       

        result = avformat_open_input(&(pinput->fmt_ctx), 
            "video=Integrated Webcam:"
            "audio=Microphone (2- Realtek(R) Audio)", 
            iformat, &options);

        if(result < 0) {
            ret = 1;
            spllog(4, "--");
            break;
        }


        result = avformat_find_stream_info(pinput->fmt_ctx, 0);
        if(result < 0) {
            ret = 1;
            spllog(4, "--");
            break;
        }
        if(pinput->fmt_ctx->nb_streams < 1) {
            ret = 1;
            spllog(4, "--");
            break;
        }
        pinput->v_st = pinput->fmt_ctx->streams[0];
        if(!pinput->v_st) {
            ret = 1;
            spllog(4, "--");
            break;
        }
        pinput->v_codec = avcodec_find_decoder(AV_CODEC_ID_RAWVIDEO);
        if(!pinput->v_codec) {
            ret = 1;
            spllog(4, "--");
            break;
        }        
        pinput->v_cctx  = avcodec_alloc_context3(pinput->v_codec);
        if(!pinput->v_cctx) {
            ret = 1;
            spllog(4, "--");
            break;
        }    
        result = avcodec_parameters_to_context(pinput->v_cctx, pinput->v_st->codecpar);
        if(result < 0) {
            ret = 1;
            spllog(4, "--");
            break;
        }
		result = avcodec_open2(pinput->v_cctx, pinput->v_codec, 0);
		if (result < 0) {
            spllog(4, "avcodec_open2, result: %d", result);
			break;
		}        
        pinput->vframe = av_frame_alloc(); 
        if(!pinput->vframe) {
            ret = 1;
            spllog(4, "--");
            break;
        }
            
        /*------------------------------*/

        if(pinput->fmt_ctx->nb_streams > 1) {
            pinput->a_st = pinput->fmt_ctx->streams[1];
        }
        if(!pinput->a_st) {
            ret = 1;
            spllog(1, "---");
            break;
        }
        pinput->a_codec = avcodec_find_decoder(AV_CODEC_ID_PCM_S16LE);
        if(!pinput->a_codec) {
            ret = 1;
            spllog(4, "--");
            break;
        }      
        pinput->a_cctx  = avcodec_alloc_context3(pinput->a_codec);
        if(!pinput->a_cctx) {
            ret = 1;
            spllog(4, "--a_cctx");
            break;
        }   
        result = avcodec_parameters_to_context(pinput->a_cctx, pinput->a_st->codecpar);
        if(result < 0) {
            ret = 1;
            spllog(4, "--");
            break;
        }   
		result = avcodec_open2(pinput->a_cctx, pinput->a_codec, 0);
		if (result < 0) {
            spllog(4, "--");
			break;
		} 
        pinput->a_frame = av_frame_alloc(); 
        if(!pinput->a_frame) {
            ret = 1;
            spllog(4, "--");
            break;
        }      
        pinput->a_dstframe = av_frame_alloc(); 
        if(!pinput->a_dstframe) {
            ret = 1;
            spllog(4, "--");
            break;
        }   
           
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
convert_frame(AVFrame *src, AVFrame *dst)
{
    int ret = 0;
    //av_frame_get_buffer(src, 32);
    av_frame_get_buffer(dst, 32);
    if (!dst->data[0]) {
        ret = av_frame_get_buffer(dst, 32);
        if (ret < 0) {
            spllog(4, "Error allocating dst buffer\n");
            return ret;
        }
        spllog(1, "av_frame_get_buffer");
    }
    if(! (gb_instream.vscale)) {
        gb_instream.vscale = sws_getContext(
	    	src->width, 
	    	src->height, 
	    	src->format, 
	    	dst->width, 
	    	dst->height, 
	    	dst->format,
            SWS_BILINEAR, NULL, NULL, NULL
        );
    }
    if (!gb_instream.vscale) {
        spllog(4, "Error: cannot create sws context\n");
        return -1;
    }

    ret = sws_scale(
        gb_instream.vscale,
        (const uint8_t * const *)src->data,
        src->linesize,
        0,
        src->height,
        dst->data,
        dst->linesize
    );

    return 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

int
convert_audio_frame(OutputStream *ost, AVFrame *src, AVFrame *dst);

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    spllog(1, "pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}

static int write_frame(AVFormatContext *fmt_ctx, AVCodecContext *c,
                       AVStream *st, AVFrame *frame, AVPacket *pkt)
{
    int ret;

    // send the frame to the encoder
    ret = avcodec_send_frame(c, frame);

    if(frame) {
        if(frame->width) {
            spllog(1, "---");
#if 0            
            av_frame_unref(frame);
#endif            
        }
    }

    if (ret < 0) {
        spllog(4, "Error sending a frame to the encoder: %s\n",
                av_err2str(ret));
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(c, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        else if (ret < 0) {
            spllog(4, "Error encoding a frame: %s\n", av_err2str(ret));
            exit(1);
        }

        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(pkt, c->time_base, st->time_base);
        pkt->stream_index = st->index;

        /* Write the compressed frame to the media file. */
        log_packet(fmt_ctx, pkt);
        ret = av_interleaved_write_frame(fmt_ctx, pkt);
        
        /* pkt is now blank (av_interleaved_write_frame() takes ownership of
         * its contents and resets pkt), so that no unreferencing is necessary.
         * This would be different if one used av_write_frame(). */
        if (ret < 0) {
            spllog(4, "Error while writing output packet: %s\n", av_err2str(ret));
            exit(1);
        }
        av_packet_unref(pkt);
    }

    return ret == AVERROR_EOF ? 1 : 0;
}

/* Add an output stream. */
static void add_stream(OutputStream *ost, AVFormatContext *oc,
                       const AVCodec **codec,
                       enum AVCodecID codec_id)
{
    AVCodecContext *c;
    int i;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        spllog(4, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }

    ost->tmp_pkt = av_packet_alloc();
    if (!ost->tmp_pkt) {
        spllog(4, "Could not allocate AVPacket\n");
        exit(1);
    }

    ost->st = avformat_new_stream(oc, NULL);
    if (!ost->st) {
        spllog(4, "Could not allocate stream\n");
        exit(1);
    }
    ost->st->id = oc->nb_streams-1;
    c = avcodec_alloc_context3(*codec);
    if (!c) {
        spllog(4, "Could not alloc an encoding context\n");
        exit(1);
    }
    ost->enc = c;

    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        c->sample_fmt  = (*codec)->sample_fmts ?
            (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
        c->bit_rate    = 64000;
        c->sample_rate = 44100;
        if ((*codec)->supported_samplerates) {
            c->sample_rate = (*codec)->supported_samplerates[0];
            for (i = 0; (*codec)->supported_samplerates[i]; i++) {
                if ((*codec)->supported_samplerates[i] == 44100)
                    c->sample_rate = 44100;
            }
        }

        av_channel_layout_copy(&c->ch_layout, &(AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO);

        ost->st->time_base = (AVRational){ 1, c->sample_rate };
        break;

    case AVMEDIA_TYPE_VIDEO:
        c->codec_id = codec_id;
        //c->pix_fmt = 4;
        c->bit_rate = 400000;
        /* Resolution must be a multiple of two. */
        c->width    = 640;
        c->height   = 480;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
        ost->st->time_base = (AVRational){ 1, STREAM_FRAME_RATE };
        c->time_base       = ost->st->time_base;

        c->gop_size      = 12; /* emit one intra frame every twelve frames at most */
        c->pix_fmt       = STREAM_PIX_FMT;
        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            /* just for testing, we also add B-frames */
            c->max_b_frames = 2;
        }
        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
            /* Needed to avoid using macroblocks in which some coeffs overflow.
             * This does not happen with normal video, it just happens here as
             * the motion of the chroma plane does not match the luma plane. */
            c->mb_decision = 2;
        }
        break;

    default:
        break;
    }

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

/**************************************************************/
/* audio output */

static AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt,
                                  const AVChannelLayout *channel_layout,
                                  int sample_rate, int nb_samples)
{
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Error allocating an audio frame\n");
        exit(1);
    }

    frame->format = sample_fmt;
    av_channel_layout_copy(&frame->ch_layout, channel_layout);
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_samples;

    if (nb_samples) {
        if (av_frame_get_buffer(frame, 0) < 0) {
            fprintf(stderr, "Error allocating an audio buffer\n");
            exit(1);
        }
    }

    return frame;
}

static void open_audio(AVFormatContext *oc, const AVCodec *codec,
                       OutputStream *ost, AVDictionary *opt_arg)
{
    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = NULL;

    c = ost->enc;

    /* open it */
    av_dict_copy(&opt, opt_arg, 0);
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        fprintf(stderr, "Could not open audio codec: %s\n", av_err2str(ret));
        exit(1);
    }

    /* init signal generator */
    ost->t     = 0;
    ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        nb_samples = 10000;
    else
        nb_samples = c->frame_size;

    ost->frame     = alloc_audio_frame(c->sample_fmt, &c->ch_layout,
                                       c->sample_rate, nb_samples);
    ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, &c->ch_layout,
                                       c->sample_rate, nb_samples);

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }

    /* create resampler context */
    ost->swr_ctx = swr_alloc();
    if (!ost->swr_ctx) {
        fprintf(stderr, "Could not allocate resampler context\n");
        exit(1);
    }

    /* set options */
    av_opt_set_chlayout  (ost->swr_ctx, "in_chlayout",       &c->ch_layout,      0);
    av_opt_set_int       (ost->swr_ctx, "in_sample_rate",     c->sample_rate,    0);
    av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
    av_opt_set_chlayout  (ost->swr_ctx, "out_chlayout",      &c->ch_layout,      0);
    av_opt_set_int       (ost->swr_ctx, "out_sample_rate",    c->sample_rate,    0);
    av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt",     c->sample_fmt,     0);

    /* initialize the resampling context */
    if ((ret = swr_init(ost->swr_ctx)) < 0) {
        fprintf(stderr, "Failed to initialize the resampling context\n");
        exit(1);
    }
}

/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
 * 'nb_channels' channels. */
static AVFrame *get_audio_frame_trigger(AVPacket *pkt) {
    FFWR_INSTREAM *p = &gb_instream;
    int result = 0;
    do {        
        result = avcodec_send_packet(p->a_cctx, pkt);
        if(result < 0) {
            spllog(4, "---");
            break;
        }   
        result = avcodec_receive_frame(p->a_cctx, p->a_frame);     
        if(result < 0) {
            spllog(4, "---");
            break;
        }   
        spl_vframe(p->a_frame);              
    } while(0);

    return p->a_dstframe;    
}
static AVFrame *get_audio_frame(OutputStream *ost)
{
    int result = 0;
#if 1    
    AVFrame *frame = ost->tmp_frame;
    int j, i, v;
    int16_t *q = (int16_t*)frame->data[0];

    /* check if we want to generate more frames */
#if 0    
    result = (av_compare_ts(ost->next_pts, ost->enc->time_base,
                      STREAM_DURATION, (AVRational){ 1, 1 }) > 0);
#else
    result = (count_frame > NUMBER_FRAMES);
#endif                      
    if (result)
        return NULL;

    for (j = 0; j <frame->nb_samples; j++) {
        v = (int)(sin(ost->t) * 10000);
        for (i = 0; i < ost->enc->ch_layout.nb_channels; i++)
            *q++ = v;
        ost->t     += ost->tincr;
        ost->tincr += ost->tincr2;
    }

    frame->pts = ost->next_pts;
    ost->next_pts  += frame->nb_samples;
    spl_vframe(frame); 

    return frame;
#else
    
#endif    
}

/*
 * encode one audio frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
#define MINNN(a__, __b) ((a__) < (__b)) ? (a__) : (__b)
#define NUM_STEP 1024
static int write_audio_frame(AVFormatContext *oc, OutputStream *ost)
{
    AVCodecContext *c = 0;
    AVFrame *frame = 0;
    AVFrame *aframe = 0;
    int ret = 0;
    int dst_nb_samples = 0;
    int step = 0;
    int k = 0;
    uint8_t *p = 0;
    c = ost->enc;
    int bytes_per_sample = 0;
#if 0
    frame = get_audio_frame(ost);
#else    
    frame = gb_instream.a_frame;
#endif    
    spl_vframe(frame);
    bytes_per_sample = av_get_bytes_per_sample(frame->format);
    if (frame && frame->pts) {
        while(step < frame->nb_samples) {
#if 0            
            /* convert samples from native format to destination codec format, using the resampler */
            /* compute destination number of samples */
            k = MINNN(NUM_STEP, (frame->nb_samples - step));
            dst_nb_samples = swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples;
            av_assert0(dst_nb_samples == frame->nb_samples);

            /* when we pass a frame to the encoder, it may keep a reference to it
             * internally;
             * make sure we do not overwrite it here
             */
            ret = av_frame_make_writable(ost->frame);
            if (ret < 0)
                exit(1);

        /* convert to destination format */
            ret = swr_convert(ost->swr_ctx, ost->frame->data, dst_nb_samples, 
                (const uint8_t **)frame->data, frame->nb_samples);

            if (ret < 0) {
                spllog(1, "Error while converting\n");
                exit(1);
            }

            frame = ost->frame;   
            frame->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
            ost->samples_count += dst_nb_samples; 
            ret = write_frame(oc, c, ost->st, frame, ost->tmp_pkt);   
            step += NUM_STEP;                           
#else

            k = MINNN(NUM_STEP, (frame->nb_samples - step));
            //if(k < NUM_STEP) {
            //    break;
            //}
            //dst_nb_samples = swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples;
            dst_nb_samples = k;

            //av_assert0(dst_nb_samples == frame->nb_samples);

            /* when we pass a frame to the encoder, it may keep a reference to it
             * internally;
             * make sure we do not overwrite it here
             */
            ret = av_frame_make_writable(ost->frame);
            if (ret < 0)
                exit(1);
            
            if(!p) {
                p = frame->data[0];
            } else {
                p += (bytes_per_sample * k * frame->ch_layout.nb_channels);
            }
            //p = frame->data[0] + step;
        /* convert to destination format */
            ret = swr_convert(ost->swr_ctx, ost->frame->data, dst_nb_samples, 
                (const uint8_t **) &p, k);
            spllog(1, "(p, ret, bytes_per_sample)=(0x%p, %d, %d)", p, ret, bytes_per_sample); 
            if (ret < 0) {
                spllog(1, "Error while converting\n");
                exit(1);
            }

            aframe = ost->frame;   
            aframe->pts = ost->next_pts;
            ost->next_pts  += k;        
            aframe->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
            ost->samples_count += dst_nb_samples; 
            ret = write_frame(oc, c, ost->st, aframe, ost->tmp_pkt);  
           
            step += NUM_STEP;    
            //break;
#endif
        }
    }
    av_frame_unref(gb_instream.a_frame);
    
    spl_vframe(frame);
    return ret;
}

/**************************************************************/
/* video output */

static AVFrame *alloc_frame(enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *frame;
    int ret;

    frame = av_frame_alloc();
    if (!frame)
        return NULL;

    frame->format = pix_fmt;
    frame->width  = width;
    frame->height = height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }

    return frame;
}

static void open_video(AVFormatContext *oc, const AVCodec *codec,
                       OutputStream *ost, AVDictionary *opt_arg)
{
    int ret;
    AVCodecContext *c = ost->enc;
    AVDictionary *opt = NULL;

    av_dict_copy(&opt, opt_arg, 0);

    /* open the codec */
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
        exit(1);
    }

    /* allocate and init a reusable frame */
    ost->frame = alloc_frame(c->pix_fmt, c->width, c->height);
    if (!ost->frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
    ost->tmp_frame = NULL;
    if (c->pix_fmt != STREAM_PIX_FMT) {
        ost->tmp_frame = alloc_frame(STREAM_PIX_FMT, c->width, c->height);
        if (!ost->tmp_frame) {
            fprintf(stderr, "Could not allocate temporary video frame\n");
            exit(1);
        }
    }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }
}

/* Prepare a dummy image. */
static void fill_yuv_image(void *sourceInput, AVFrame *pict, int frame_index,
                           int width, int height)
{
#if 0  
    int x, y, i;

    i = frame_index;

    /* Y */
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;

    /* Cb and Cr */
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
#else
    int result = 0;
    //spllog(1, "frame_index: %d", frame_index);
    do {
        if(!(gb_instream.vframe)) {
            gb_instream.vframe = av_frame_alloc();
        }
        if(!(gb_instream.vframe)) {
            break;
        }
        av_frame_unref(gb_instream.vframe);
        result = av_read_frame(gb_instream.fmt_ctx, &(gb_instream.pkt));     
        if(result < 0) {
            break;
        }
        if(gb_instream.pkt.stream_index) {
            spllog(1, "For audio, stream_index: %d, count_frame_audio: %d, count__frame: %d", 
                gb_instream.pkt.stream_index, (++count_frame_audio), count_frame);
            get_audio_frame_trigger(&(gb_instream.pkt));
            break;
        }
        result = avcodec_send_packet(gb_instream.v_cctx, &(gb_instream.pkt));
        if(result < 0) {
            spllog(1, "v_cctx: 0x%p", gb_instream.v_cctx);
            break;
        }
		result = avcodec_receive_frame(gb_instream.v_cctx, gb_instream.vframe);
		if (result < 0) {
			break;
		}
        spl_vframe(gb_instream.vframe);
        //pict->format = 4;
        convert_frame(gb_instream.vframe, pict);
        count_frame++;
        spl_vframe(pict);
    } while(0);
    av_packet_unref(&(gb_instream.pkt));
#endif    
}

static AVFrame *get_video_frame(OutputStream *ost)
{
    AVCodecContext *c = ost->enc;
    int ret = 0;
    int result = 0;

    /* check if we want to generate more frames */
#if 0    
    result = (av_compare_ts(ost->next_pts, c->time_base,
                      STREAM_DURATION, (AVRational){ 1, 1 }) > 0);
#else
    result = (count_frame > NUMBER_FRAMES);
#endif                      
    if (result)
        return NULL;

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */
    if (av_frame_make_writable(ost->frame) < 0)
        exit(1);

    if (c->pix_fmt != STREAM_PIX_FMT) {
        /* as we only generate a YUV420P picture, we must convert it
         * to the codec pixel format if needed */
        if (!ost->sws_ctx) {
            ost->sws_ctx = sws_getContext(c->width, c->height,
                                          STREAM_PIX_FMT,
                                          c->width, c->height,
                                          c->pix_fmt,
                                          SCALE_FLAGS, NULL, NULL, NULL);
            if (!ost->sws_ctx) {
                fprintf(stderr,
                        "Could not initialize the conversion context\n");
                exit(1);
            }
        }
        fill_yuv_image(0, ost->tmp_frame, ost->next_pts, c->width, c->height);
        ret = sws_scale(ost->sws_ctx, (const uint8_t * const *) ost->tmp_frame->data,
                  ost->tmp_frame->linesize, 0, c->height, ost->frame->data,
                  ost->frame->linesize);
        if(ret < 0) {
            spllog(1, "sws_scale, ret: %d", ret);
        }
    } else {
#if 1        
        fill_yuv_image(0, ost->frame, ost->next_pts, c->width, c->height);
#else        
        fill_yuv_image(0, ost->frame, ost->next_pts, c->width, c->height);
#endif        
    }

    ost->frame->pts = ost->next_pts++;
    spl_vframe(ost->frame);
    return ost->frame;
}

/*
 * encode one video frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
static int write_video_frame(AVFormatContext *oc, OutputStream *ost)
{
    return write_frame(oc, ost->enc, ost->st, get_video_frame(ost), ost->tmp_pkt);
}

static void close_stream(AVFormatContext *oc, OutputStream *ost)
{
    avcodec_free_context(&ost->enc);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
    av_packet_free(&ost->tmp_pkt);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}

/**************************************************************/
/* media file output */

int main(int argc, char **argv)
{
    OutputStream video_st = { 0 }, audio_st = { 0 };
    const AVOutputFormat *fmt = 0;
    const char *filename = 0;
    AVFormatContext *oc = 0;
    const AVCodec *audio_codec = 0, *video_codec = 0;
    int ret = 0;
    int have_video = 0, have_audio = 0;
    int encode_video = 0, encode_audio = 0;
    AVDictionary *opt = NULL;
    int i = 0;

	char cfgpath[1024] = {0};
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	snprintf(input.id_name, 100, "mux");
	ret = spl_init_log_ext(&input);

    ffwr_open_input(&gb_instream, "video=Integrated Webcam:audio=Microphone (2- Realtek(R) Audio)", 0);

    if (argc < 2) {
        printf("usage: %s output_file\n"
               "API example program to output a media file with libavformat.\n"
               "This program generates a synthetic audio and video stream, encodes and\n"
               "muxes them into a file named output_file.\n"
               "The output format is automatically guessed according to the file extension.\n"
               "Raw images can also be output by using '%%d' in the filename.\n"
               "\n", argv[0]);
        return 1;
    }

    filename = argv[1];
    for (i = 2; i+1 < argc; i+=2) {
        if (!strcmp(argv[i], "-flags") || !strcmp(argv[i], "-fflags"))
            av_dict_set(&opt, argv[i]+1, argv[i+1], 0);
    }

    /* allocate the output media context */
    avformat_alloc_output_context2(&oc, NULL, NULL, filename);
    if (!oc) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&oc, NULL, "mpeg", filename);
    }
    if (!oc)
        return 1;

    fmt = oc->oformat;

    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    if (fmt->video_codec != AV_CODEC_ID_NONE) {
        add_stream(&video_st, oc, &video_codec, fmt->video_codec);//264
        //add_stream(&video_st, oc, &video_codec, AV_CODEC_ID_HEVC);
        //add_stream(&video_st, oc, &video_codec, AV_CODEC_ID_AV1);
        have_video = 1;
        encode_video = 1;
    }
    if (fmt->audio_codec != AV_CODEC_ID_NONE) {
        add_stream(&audio_st, oc, &audio_codec, fmt->audio_codec);
#if 1        
        have_audio = 1;
        encode_audio = 1;
#endif        
    }

    /* Now that all the parameters are set, we can open the audio and
     * video codecs and allocate the necessary encode buffers. */
    if (have_video)
        open_video(oc, video_codec, &video_st, opt);

    if (have_audio)
        open_audio(oc, audio_codec, &audio_st, opt);

    av_dump_format(oc, 0, filename, 1);

    /* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open '%s': %s\n", filename,
                    av_err2str(ret));
            return 1;
        }
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(oc, &opt);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file: %s\n",
                av_err2str(ret));
        return 1;
    }

    while (encode_video || encode_audio) {
        /* select the stream to encode */
        if(count_frame > NUMBER_FRAMES) {
            avcodec_send_frame(video_st.enc, 0);
            avcodec_send_frame(audio_st.enc, 0);
            break;
        }
#if 0            
        if (encode_video &&
            (!encode_audio || av_compare_ts(video_st.next_pts, video_st.enc->time_base,
                                            audio_st.next_pts, audio_st.enc->time_base) <= 0)) 
                                            {
            encode_video = !write_video_frame(oc, &video_st);
        } 
    
        else 
        {
            encode_audio = !write_audio_frame(oc, &audio_st);
        }
#else
        write_video_frame(oc, &video_st);
        if(gb_instream.a_frame->nb_samples)  {
            encode_audio = !write_audio_frame(oc, &audio_st);
        }
#endif    
    }
    spl_milli_sleep(100);
    av_write_trailer(oc);

    /* Close each codec. */
    if (have_video)
        close_stream(oc, &video_st);
    if (have_audio)
        close_stream(oc, &audio_st);

    if (!(fmt->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&oc->pb);

    /* free the stream */
    avformat_free_context(oc);
    ffwr_close_input(&gb_instream);
    spl_finish_log();
    return 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
convert_audio_frame(OutputStream *ost, AVFrame *src, AVFrame *dst)
{
    int ret = 0;
    dst->nb_samples = src->nb_samples;
    dst->format = 8;
    dst->sample_rate = src->sample_rate;
    av_channel_layout_copy(&(dst->ch_layout), &(ost->enc->ch_layout));

    ret = av_frame_get_buffer(dst, 0);
	if ((ret) < 0) {
		spllog(4, "Error: cannot allocate dst buffer (%d)\n", ret);
		return ret;
	}

	ret = swr_convert(ost->swr_ctx, dst->data, dst->nb_samples,
	    (const uint8_t **)src->data, src->nb_samples);

	if (ret < 0) {
		spllog(4, " Error: swr_convert failed (%d)\n", ret);
		return ret;
	}

	dst->nb_samples = ret;


	spllog(1, "Audio convert done: %d samples -> %d samples\n",
	    src->nb_samples, dst->nb_samples);

	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/