#include <simplelog.h>
#include "FFWrapper.h"
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

typedef struct {
    AVFormatContext *fmt_ctx;
    AVStream *stream;
    AVCodecContext *codec_ctx;
    AVIOContext *avio_ctx;
    uint8_t *avio_buffer;
    FILE *file;
    int64_t pts;
} MP4Writer;

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
static int ffwr_clone_str(char **dst, char *str);
static int convert_frame(AVFrame *src, AVFrame *dst);
static int convert_audio_frame(AVFrame *src, AVFrame *dst);
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
#define FFWR_STEP           5
int
ffwr_all_codecs(FFWR_CODEC **lst, int *count)
{
	int ret = 0;
	const AVCodec *codec = NULL;
	void *iter = NULL;
    FFWR_CODEC *p = 0;
    int size = 0;
    int i = 0;
    int step = FFWR_STEP;
    size = (step + 1) * sizeof(FFWR_CODEC);
    do {
        if(!count) {
            ret = FFWR_NULL_ARG;
            break;
        }
        if(!lst) {
            ret = FFWR_NULL_ARG;
            break;
        }        
        ffwr_malloc(size, p, FFWR_CODEC);
        if(!p) {
            ret = FFWR_MALLOC;
            break;
        }
        
	    while (1)
        {
            codec = av_codec_iterate(&iter);
            if(!codec) {
                break;
            }
            if (!av_codec_is_encoder(codec)) {
                continue;
            }
	    	spllog(2, "codec : (%s, %s).", codec->name,
	    	    codec->long_name ? codec->long_name
	    			     : "no long name");
            ffwr_clone_str(&p[i].name, codec->name);
            ffwr_clone_str(&p[i].detail, codec->long_name);
            ++i;
            if(i < step) {
                continue;
            }
            step += FFWR_STEP;
            size = (step + 1) * sizeof(FFWR_CODEC);
            ffwr_realloc(size, p, FFWR_CODEC);
            if(p) {
                continue;
            }
            ret = FFWR_REALLOC;
            break;
	    }
	    
        if(ret) {
            break;
        }
        *lst = p;
        *count = i;
    } while(0);
    if(ret) {
        ffwr_clear_all_codecs(&p, i);
    }
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
ffwr_clear_all_codecs(FFWR_CODEC **lst, int count) {
    int ret = 0;
    int i = 0;
    FFWR_CODEC *p = 0;
    do {
        if(!lst) {
            ret = FFWR_NULL_ARG;
            break;
        }
        p = *lst;
        if(!(p)) {
            ret = FFWR_NULL_ARG;
            break;
        }
        for(i = 0; i < count; ++i) {
            ffwr_free(p[i].name);
            ffwr_free(p[i].detail);
        }
        ffwr_free(p);
        *lst = 0;

    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

int
ffwr_find_codec(char *name, int *outout)
{
	int ret = 0;
	const AVCodec *codec = 0;
	void *iter = 0;

	while ((codec = av_codec_iterate(&iter))) 
    {
		if (av_codec_is_encoder(codec)) {
			printf("Encoder: %-15s | %s\n", codec->name,
			    codec->long_name ? codec->long_name
					     : "no long name");

		}
	}
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

int
ffwr_all_devices(FFWR_DEVICE **lst, int *count) {
	int ret = 0;
	void *iter = 0;
    FFWR_DEVICE *p = 0;
    int size = 0;
    int i = 0;
    const AVInputFormat *ifmt = 0;
    int step = FFWR_STEP;
    size = (step + 1) * sizeof(FFWR_DEVICE); 
    
    do {
        if(!count) {
            ret = FFWR_NULL_ARG;
            break;
        }
        if(!lst) {
            ret = FFWR_NULL_ARG;
            break;
        }        
        ffwr_malloc(size, p, FFWR_DEVICE);
        if(!p) {
            ret = FFWR_MALLOC;
            break;
        }
        while (1) {
            ifmt = av_input_video_device_next(ifmt);
            if(!ifmt) {
                break;
            }
	        ffwr_clone_str(&p[i].name, ifmt->name);
	        ffwr_clone_str(&p[i].detail, ifmt->long_name);    
            p[i].av = FFWR_VIDEO;        
            ++i;
            spllog(2, "Video dev Input : (%s, %s).", 
                ifmt->name, ifmt->long_name);
	        if (i < step) {
		        continue;
	        }
	        step += FFWR_STEP;
		    size = (step + 1) * sizeof(FFWR_DEVICE);
		    ffwr_realloc(size, p, FFWR_DEVICE);
	        if (p) {
		        continue;
	        }
	        ret = FFWR_REALLOC;
	        break;
        }    
        ifmt = 0;
        while (1) {
            ifmt = av_input_audio_device_next(ifmt);
            if(!ifmt) {
                break;
            }
            
            ffwr_clone_str(&p[i].name, ifmt->name);
	        ffwr_clone_str(&p[i].detail, ifmt->long_name);   
            p[i].av = FFWR_AUDIO;
            ++i;
	        spllog(2, "Audio dev Input : (%s, %s).", 
                ifmt->name, ifmt->long_name);
	        if (i < step) {
		        continue;
	        }
	        step += FFWR_STEP;
		    size = (step + 1) * sizeof(FFWR_DEVICE);
		    ffwr_realloc(size, p, FFWR_DEVICE);
	        if (p) {
		        continue;
	        }
	        ret = FFWR_REALLOC;
	        break;
        } 
        *lst = p; 
        *count = i;
    } while(0);
    if(ret) {
        ffwr_clear_all_devices(&p, i);
    }
	return 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
ffwr_clear_all_devices(FFWR_DEVICE **devs, int count) {
    int ret = 0;
    int i = 0;
    FFWR_DEVICE *p = 0;
    do {
        if(!devs) {
            ret = FFWR_NULL_ARG;
            break;
        }
        p = *devs;
        if(!(p)) {
            ret = FFWR_NULL_ARG;
            break;
        }
        for(i = 0; i < count; ++i) {
            ffwr_free(p[i].name);
            ffwr_free(p[i].detail);
        }
        ffwr_free(p);
        *devs = 0;

    } while(0);
    return ret;
}
/* Find all audio/video devices by name. */
int
ffwr_devices_by_name(FFWR_DEVICE **devs, int *count, char *name)
{
	int ret = 0;
	int result = 0;
	AVDeviceInfo *dev = 0;
	AVInputFormat *iformat = 0;
	AVDeviceInfoList *dev_list = NULL;
	enum AVMediaType type;
	int i = 0;
	int zize = 0;
	int step = FFWR_STEP;
	FFWR_DEVICE *p = 0;
	zize = (step + 1) * sizeof(FFWR_DEVICE); 
	avdevice_register_all();
	do {
		iformat = av_find_input_format(name);
		if (!iformat) {
			ret = FFWR_NO_FORMAT;
			break;
		}
		result = avdevice_list_input_sources(
            iformat, NULL, NULL, &dev_list);
		if (result < 0) {
			ret = FFWR_NO_DEVICE;
			break;
		}
		ffwr_malloc(zize, p, FFWR_DEVICE);
		if (!p) {
			ret = FFWR_MALLOC;
			break;
		}
		for ( ; i < dev_list->nb_devices; i++) {
			dev = dev_list->devices[i];
			type = dev->media_types ? dev->media_types[0] : 0;
			spllog(2, "Device %d: (%s, %s)\n",
                i, dev->device_name,
			    dev->device_description);
			ffwr_clone_str(&p[i].name, dev->device_name);
			ffwr_clone_str(&p[i].detail, dev->device_description);
			p[i].av = dev->media_types ? dev->media_types[0]
						   : AVMEDIA_TYPE_UNKNOWN;

			if (i < step) {
				continue;
			}
			step += FFWR_STEP;
			zize = (step + 1) * sizeof(FFWR_DEVICE);
			ffwr_realloc(zize, p, FFWR_DEVICE);
			if (p) {
				continue;
			}
			ret = FFWR_REALLOC;
			break;
		}
		if (ret) {
			break;
		}
		avdevice_free_list_devices(&dev_list);
		*devs = p;
		*count = i;
	} while (0);
	if (ret) {
		ffwr_clear_devices_by_name(&p, i);
	}
	return ret;
}

int
ffwr_clear_devices_by_name(FFWR_DEVICE **devs, int count)
{
	int ret = 0;
	ret = ffwr_clear_all_devices(devs, count);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/* Find all demuxer format. */
int
ffwr_all_demuxers(FFWR_DEMUXER_FMT **fmts, int *count)
{
	int ret = 0;
	void *opaque = 0;
    FFWR_DEMUXER_FMT *p = 0;
    int size = 0;
    int i = 0;
    const AVInputFormat *ifmt = 0;
    int step = FFWR_STEP;
    size = (step + 1) * sizeof(FFWR_DEMUXER_FMT); 
#if LIBAVFORMAT_VERSION_MAJOR < 58    
    av_register_all();
#endif    
    do {
        if(!count) {
            ret = FFWR_NULL_ARG;
            break;
        }
        if(!fmts) {
            ret = FFWR_NULL_ARG;
            break;
        }        
        ffwr_malloc(size, p, FFWR_DEMUXER_FMT);
        if(!p) {
            ret = FFWR_MALLOC;
            break;
        }
        while (1) {
		    ifmt = av_demuxer_iterate(&opaque);
            if(!ifmt) {
                break;
            }
	        ffwr_clone_str(&p[i].name, ifmt->name);
	        ffwr_clone_str(&p[i].detail, ifmt->long_name);    
               
            ++i;
		    spllog(2, "Demuxer: (%s, %s).", 
                ifmt->name, ifmt->long_name);
	        if (i < step) {
		        continue;
	        }
	        step += FFWR_STEP;
		    size = (step + 1) * sizeof(FFWR_DEMUXER_FMT);
		    ffwr_realloc(size, p, FFWR_DEMUXER_FMT);
	        if (p) {
		        continue;
	        }
	        ret = FFWR_REALLOC;
	        break;
        }   
        *fmts = p;     
        *count = i;
    } while (0);
    if(ret) {
	    ffwr_clear_all_demuxers(&p, i);
    }
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
DLL_API_FF_WRAPPER int
ffwr_clear_all_demuxers(FFWR_DEMUXER_FMT **fmts, int count)
{
    int ret = 0;
    int i = 0;
    FFWR_DEMUXER_FMT *p = 0;
    do {
        if(!fmts) {
            ret = FFWR_NULL_ARG;
            break;
        }
        p = *fmts;
        if(!(p)) {
            ret = FFWR_NULL_ARG;
            break;
        }
        for(i = 0; i < count; ++i) {
            ffwr_free(p[i].name);
            ffwr_free(p[i].detail);
        }
        ffwr_free(p);
        *fmts = 0;

    } while(0);
    return ret;    
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/* Find all demuxer format. */
int
ffwr_all_muxers(FFWR_MUXER_FMT **fmts, int *count)
{
	int ret = 0;
	void *opaque = 0;
	FFWR_MUXER_FMT *p = 0;
	int size = 0;
	int i = 0;
	const AVInputFormat *ifmt = 0;
	int step = FFWR_STEP;
	size = (step + 1) * sizeof(FFWR_MUXER_FMT);
#if LIBAVFORMAT_VERSION_MAJOR < 58
	av_register_all();
#endif
	do {
		if (!count) {
			ret = FFWR_NULL_ARG;
			break;
		}
		if (!fmts) {
			ret = FFWR_NULL_ARG;
			break;
		}
		ffwr_malloc(size, p, FFWR_MUXER_FMT);
		if (!p) {
			ret = FFWR_MALLOC;
			break;
		}
		while (1) {
			ifmt = av_muxer_iterate(&opaque);
			if (!ifmt) {
				break;
			}
			ffwr_clone_str(&p[i].name, ifmt->name);
			ffwr_clone_str(&p[i].detail, ifmt->long_name);

			++i;
			spllog(2, "Muxer: (%s, %s).", ifmt->name,
			    ifmt->long_name);
			if (i < step) {
				continue;
			}
			step += FFWR_STEP;
			size = (step + 1) * sizeof(FFWR_MUXER_FMT);
			ffwr_realloc(size, p, FFWR_MUXER_FMT);
			if (p) {
				continue;
			}
			ret = FFWR_REALLOC;
			break;
		}
		*fmts = p;
		*count = i;
	} while (0);
	if (ret) {
		ffwr_clear_all_muxers(&p, i);
	}
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
DLL_API_FF_WRAPPER int
ffwr_clear_all_muxers(FFWR_MUXER_FMT **fmts, int count)
{
	int ret = 0;
	int i = 0;
	FFWR_MUXER_FMT *p = 0;
	do {
		if (!fmts) {
			ret = FFWR_NULL_ARG;
			break;
		}
		p = *fmts;
		if (!(p)) {
			ret = FFWR_NULL_ARG;
			break;
		}
		for (i = 0; i < count; ++i) {
			ffwr_free(p[i].name);
			ffwr_free(p[i].detail);
		}
		ffwr_free(p);
		*fmts = 0;

	} while (0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int ffwr_clone_str(char **dst, char *src) {
    int ret = 0;
    char *p = 0;
    int n = 0;
    do {
        if(!src) {
            ret = FFWR_NULL_ARG;
            break;
        }
        n = strlen(src) + 1;
        if(!dst) {
            ret = FFWR_NULL_ARG;
            break;
        }
        ffwr_malloc(n, p, char);
        if(!p) {
            ret = FFWR_MALLOC;
            break;
        }
        memcpy(p, src, n);
        *dst = p;
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
ffwr_open_devices(FFWR_DEVICE *devs, int count, char *name)
{
	int ret = 0;
	int i = 0;
	AVInputFormat *iformat = 0; 
	char buf[1024]; 
	int result = 0;
	AVPacket *pkt = 0;

	do {
		iformat = av_find_input_format(name);
#ifndef UNIX_LINUX
		for (i = count -1; i >= 0; --i) {
        #if 0
			AVDictionary *opts = 0;
			av_dict_set(&opts, "rtbufsize", "100M", 0); //
			av_dict_set(&opts, "framerate", "30", 0); //
        #endif
			/*Open Video context.*/
			/*Open Audio context*/
			const char *pfmt = 0;
			pfmt = (devs[i].av == FFWR_VIDEO) ? 
                "video=%s" : "audio=%s";
			snprintf(buf, 1024, pfmt, devs[i].detail);
            //AVFormatContext
			if (devs[i].av == FFWR_VIDEO) { 
                AVDictionary *options = NULL;
				av_dict_set(&options, "framerate", "30", 0);
				av_dict_set(
				    &options, "video_size", "640×480", 0);
				av_dict_set(&options, "rtbufsize", "100M", 0);
				ret = avformat_open_input(
				    &(devs[i].in_ctx), buf, iformat, &options);
			} else {
				ret = avformat_open_input(
				    &(devs[i].in_ctx), buf, iformat, 0);
			}

			if (ret) {
				break;
			}
			//AVFormatContext *v = devs[i].context;
			//int a = 0;
		}
#else
#endif
		if (ret) {
			break;
		}
		for (i = count - 1; i >= 0; --i) {
			result = 0;
			if (devs[i].in_ctx) {
				if (devs[i].av == FFWR_VIDEO) {
					result = avformat_find_stream_info(
					    devs[i].in_ctx, 0);
					if (result >= 0) continue;
					spllog(4, "Cannot get info of the "
					    "video stream\n");
					ret = FFWR_OPEN_STREAM_VIDEO;
					break;
				} 
				else if (devs[i].av == FFWR_AUDIO) {
					result = avformat_find_stream_info( 
						devs[i].in_ctx, 0);
					if (result >= 0) continue;
					spllog(4,
					    "Cannot get info of the "
					    "audio stream\n");
					ret = FFWR_OPEN_STREAM_AUDIO;
					break;
				}
			}
		}
		if (ret) {
			break;
		}
		
		for (i = count - 1; i >= 0; --i) {
			AVCodecParameters *codecpar = 0;
			AVCodec *codec = 0;
			AVFormatContext *p = 0;
			int index = 0;
			if (!devs[i].in_ctx)
				continue;
			p = devs[i].in_ctx;
			if (!p->streams) {
				continue;
			}				
			codecpar = p->streams[index]->codecpar;
			if (!codecpar)
				continue;
			codec = avcodec_find_decoder(codecpar->codec_id);
			if (!codec) {
				if (devs[i].av == FFWR_VIDEO) {
					ret = FFWR_NO_VIDEO_CODEC_FOUND;
					break;
				} 
				else if (devs[i].av == FFWR_AUDIO) {
					ret = FFWR_NO_AUDIO_CODEC_FOUND;
					break;
				}
			} 
			/*AVCodecContext *codec_ctx*/
			devs[i].in_codec_ctx = avcodec_alloc_context3(codec);
			if (!devs[i].in_ctx) {
				ret = (devs[i].av == FFWR_VIDEO)
					  ? FFWR_ALLOC_VIDEO_CONTEXT
					  : FFWR_ALLOC_AUDO_CONTEXT;
				break;
			}
			avcodec_parameters_to_context(
			    devs[i].in_codec_ctx, codecpar);
			result = avcodec_open2(devs[i].in_codec_ctx, codec, 0);
			if (result < 0) {
				ret = (devs[i].av == FFWR_VIDEO)
					  ? FFWR_OPEN_VIDEO_CONTEXT
					  : FFWR_OPEN_AUDO_CONTEXT;
				break;
			}
			/*
			else {
				int a = AV_CODEC_ID_RAWVIDEO;
			}
			//AV_CODEC_ID_RAWVIDEO, AV_CODEC_ID_FIRST_AUDIO
			*/
		}
		for (i = count - 1; i >= 0; --i) {

		}
#if 0
		AVCodecParameters *codecpar =
		    fmt_ctx->streams[video_index]->codecpar;
		AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
		if (!codec) {
			fprintf(stderr, "Not found\n");
			avformat_close_input(&fmt_ctx);
			return 1;
		}
#endif
	} while (0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
ffwr_devices_operate(FFWR_DEVICE *devs, int count)
{
	int ret = 0;
	AVPacket pkt = {0};
	AVPacket out_pkt = {0};
	AVFrame frame = {0};
	AVCodecContext *avCodec_ctx = 0;
	int readindex = -1;
	int i = 0;
	int rs = 0;
	int count_frame = 0;
	int stream_index = -1;
	AVStream *out_steam = 0;
	AVFormatContext *fctx = 0;

	//av_init_packet(&pkt);
	//av_init_packet(&out_pkt);


	do {
		for (i = count - 1; i >= 0; --i) {
			avCodec_ctx = (devs[i].av == FFWR_VIDEO)
					  ? devs[i].out_vcodec_context
					  : devs[i].out_acodec_context;
			stream_index = (devs[i].av == FFWR_VIDEO)
					  ? 0
					  : 1;
			out_steam = (AVStream *)(devs[i].out_stream);
			fctx = (AVFormatContext *)devs[i].in_ctx;
			//fctx->v
			readindex = av_read_frame(fctx, &pkt);
			if (readindex < 0) {
				continue;
			}

			rs = avcodec_send_packet(
				devs[i].in_codec_ctx, &pkt);
			if (rs) {
				spllog(3, "in_ctx, send packet: %s, codec_id: %d.", 
					(devs[i].av == FFWR_VIDEO) ? 
					"Video" : "Audio",
				    ((AVCodecContext *)devs[i].in_codec_ctx)->codec_id);
				continue;
			}
			count_frame = 0;
			while (1) {
				/*AV_CODEC_ID_PCM_S16LE: 65536, AV_CODEC_ID_RAWVIDEO: 13*/
				rs = avcodec_receive_frame(
				    devs[i].in_codec_ctx, &frame);
				if (rs) {
					if (count_frame) {
						break;
					}
					spllog(3, "in_ctx, recv fame: %s.",
					    (devs[i].av == FFWR_VIDEO)
						? "Video" : "Audio");
					break;
				}

				
#if 0
				int ret = avcodec_send_frame(video_enc, raw_video_frame);
				while (ret >= 0) {
				    AVPacket pkt = {0};
				    av_init_packet(&pkt);
				    ret = avcodec_receive_packet(video_enc, &pkt);
				    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
				    pkt.stream_index = video_st->index;
				    av_packet_rescale_ts(&pkt, video_enc->time_base, video_st->time_base);
				    av_interleaved_write_frame(out_ctx, &pkt);
				    av_packet_unref(&pkt);
				}
				int ret = avcodec_send_frame(audio_enc, raw_audio_frame);
				while (ret >= 0) {
				    AVPacket pkt = {0};
				    av_init_packet(&pkt);
				    ret = avcodec_receive_packet(audio_enc, &pkt);
				    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
				    pkt.stream_index = audio_st->index;
				    av_packet_rescale_ts(&pkt, audio_enc->time_base, audio_st->time_base);
				    av_interleaved_write_frame(out_ctx, &pkt);
				    av_packet_unref(&pkt);
				}
#else

#if 0				//AV_PIX_FMT_YUV420P
				frame.format = AV_PIX_FMT_YUV420P;
				enum AVPixelFormat {
				    AV_PIX_FMT_NONE = -1,
				    AV_PIX_FMT_YUV420P,   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
				    AV_PIX_FMT_YUYV422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
				    AV_PIX_FMT_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
#endif
				//if (devs[i].av == FFWR_AUDIO) {
				//	frame.nb_samples =
				//	    avCodec_ctx->frame_size;
				//}
				/*AV_CODEC_ID_H264: 27, AV_CODEC_ID_AAC: 86018*/
					/*Ignore*/
				rs = avcodec_send_frame(avCodec_ctx, &frame);
				if (rs < 0) {
					continue;
				}
				while (rs >= 0) {
					
					rs = avcodec_receive_packet( avCodec_ctx, &out_pkt);
					if (rs == AVERROR(EAGAIN) ||
					    rs == AVERROR_EOF) {
						break;
					}
					out_pkt.stream_index = out_steam->index;
					av_packet_rescale_ts(&out_pkt,
					    avCodec_ctx->time_base,
					    out_steam->time_base);
					rs = av_interleaved_write_frame(
					    devs[0].out_ctx, &out_pkt);
					//av_write_frame
					av_packet_unref(&out_pkt);
				}
#endif
				++count_frame;
				/*
				* Have to init output AVFormatContex
				* Processing
				*/
				av_frame_unref(&frame);
			}
			
		}
		if (ret) {
			break;
		}
		av_packet_unref(&pkt);
	} while (1);

	return ret;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
static int
write_packet(void *opaque, uint8_t *buf, int buf_size)
{
#if 1
	int n = fwrite(buf, 1, buf_size, (FILE *)opaque);
	fflush((FILE *)opaque);
	spllog(1, "buf_size: %d", buf_size);
	return n;
#else
	return fwrite(buf, 1, buf_size, (FILE *)opaque);
#endif
}

FFLL
my_seek(void *opaque, int64_t offset, int whence)
{
	FILE *f = (FILE *)opaque;

	if (whence == AVSEEK_SIZE) {
		long cur = ftell(f);
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		fseek(f, cur, SEEK_SET);
		return size;
	}

	return fseek(f, (long)offset, whence) == 0 ? ftell(f) : -1;
}
int
ffwr_open_output(FFWR_DEVICE *devs, int count)
{
	int ret = 0;
	int i = 0;
	int rs = 0;
	AVFormatContext *fmt_ctx = 0;
	AVCodecContext *vcodec_ctx = 0;;
	AVCodecContext *acodec_ctx = 0;;
	const AVCodec *acodec = 0;
	const AVCodec *vcodec = 0;
	uint8_t *avio_buffer = 0;
	AVIOContext *avio_ctx = 0;
	FILE *fp = 0;
	AVOutputFormat *fmt = 0;
	AVStream *video_st = 0;
	AVStream *audio_st = 0;
	AVChannelLayout layout = AV_CHANNEL_LAYOUT_STEREO;
	//AV_CHANNEL_LAYOUT_STEREO

	do {
		fp = fopen("d:/z.mp4", "w+");
		if (!fp) {
			ret = FFWR_OPEN_FILE;
			break;
		}
		devs[0].out_cb_obj = fp;
		devs[0].avio_cb_fn = write_packet;
		rs = avformat_alloc_output_context2(&fmt_ctx, 0, "mp4", 0);
		if (rs < 0) {
			ret = FFWR_CREATE_OUTPUT_CONTEXT;
			break;
		}
		devs[0].out_ctx = fmt_ctx;
		fmt = fmt_ctx->oformat;
		/*------------*/
	#if 0
		vcodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	#else
		vcodec = avcodec_find_encoder(fmt->video_codec);
	#endif
		if (!vcodec) {
			ret = FFWR_H264_NOT_FOUND;
			break;
		} 
		vcodec_ctx = avcodec_alloc_context3(vcodec);
		vcodec_ctx->codec_id = AV_CODEC_ID_H264;
		vcodec_ctx->bit_rate = 400000;
		/*Golden rate 1:1.618*/
		vcodec_ctx->height = 480;
		vcodec_ctx->width = 640;
		vcodec_ctx->time_base = (AVRational){333333, 10000000};
		vcodec_ctx->framerate = (AVRational){10000000, 333333};
		vcodec_ctx->gop_size = -1;
		vcodec_ctx->max_b_frames = 0;
		vcodec_ctx->pix_fmt = AV_PIX_FMT_YUVJ422P;
		if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
			vcodec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		}			
		for (i = 0; i < count; ++i) {
			if (devs[i].av == FFWR_VIDEO) {
				devs[i].out_vcodec_context = vcodec_ctx;
				break;
			}
		}
		/*------------*/
		/* Create Video stream */
		video_st = avformat_new_stream(fmt_ctx, 0);
		if (!video_st) {
			spllog(4, "Cannot create video stream\n");
			ret = FFWR_CREATE_VIDEO_STREAM;
			break;
		}
		rs = avcodec_parameters_from_context(
		    video_st->codecpar, vcodec_ctx);

		video_st->id = fmt_ctx->nb_streams - 1;
		if (devs[i].av == FFWR_VIDEO) {
			devs[i].out_stream = video_st;
		}
		/*------------*/
		/* Create audio stream */
		audio_st = avformat_new_stream(fmt_ctx, 0);
		if (!audio_st) {
			spllog(4, "Cannot create audio stream\n");
			ret = FFWR_CREATE_AUDIO_STREAM;
			break;
		}
		for (i = 0; i < count; ++i) {
			if (devs[i].av == FFWR_AUDIO) {
				devs[i].out_stream = audio_st;
				break;
			}
		}
		audio_st->id = fmt_ctx->nb_streams - 1;
		/* Declare code for audio */
	#if 1
		acodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	#else
		acodec = avcodec_find_encoder(fmt->audio_codec);
	#endif
		if (!acodec) {
			ret = FFWR_ACC_NOT_FOUND;
			break;
		} 
		acodec_ctx = avcodec_alloc_context3(acodec);
		acodec_ctx->codec_id = fmt_ctx->oformat->audio_codec;
		//acodec_ctx->codec_id = acodec;
		acodec_ctx->sample_rate = 44100;
		acodec_ctx->sample_fmt = acodec->sample_fmts[0]; 
		acodec_ctx->time_base = (AVRational){1, 44100};
		av_channel_layout_copy(&acodec_ctx->ch_layout, &layout);
		rs = avcodec_parameters_from_context(
		    audio_st->codecpar, acodec_ctx);
		avcodec_open2(acodec_ctx, acodec, 0);
		// copy params into stream
		

		for (i = 0; i < count; ++i) {
			if (devs[i].av == FFWR_AUDIO) {
				devs[i].out_acodec_context = acodec_ctx;
				break;
			}
		}
		/*------------*/
		/*------------*/
	#if 1
		avio_buffer = av_malloc(4096);
		if (!avio_buffer) {
			ret = FFWR_AVIO_MALLOC_BUFF;
			break;
		}

		devs[0].out_avio_buff = avio_buffer;

		avio_ctx = avio_alloc_context(
		    avio_buffer, 4096, 1, 
			devs[0].out_cb_obj, 0, devs[0].avio_cb_fn, my_seek);
		if (!avio_ctx) {
			ret = FFWR_AVIO_CTX_NULL;
			break;
		}
		fmt_ctx->pb = avio_ctx;
		fmt_ctx->flags |= AVFMT_FLAG_CUSTOM_IO;
	#else
		avio_open(&fmt_ctx->pb, "test.mp4", AVIO_FLAG_READ_WRITE);
	#endif
		/* Next step: write file, write header, 
		write packet audio/video*/
		rs = avformat_write_header(fmt_ctx, 0);
		if (rs < 0) {
			char buf[AV_ERROR_MAX_STRING_SIZE] = {0};
			av_strerror(rs, buf, sizeof(buf));
			spllog(4, "Cannot be written by : %s.\n", buf);
			ret = FFWR_WRITE_HEADER;
			break;
		}
		/**/

		int kk = fmt_ctx->nb_streams;
		spllog(0, "---");
	} while (0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
ffwr_close_devices(FFWR_DEVICE *devs, int count)
{
	int ret = 0;
	int i = 0;
	for (i = 0; i < count; ++i) {
		if (!devs[i].in_ctx) {
			continue;
		}
		if (devs[i].out_cb_obj)
		{
			fclose(devs[i].out_cb_obj);
			devs[i].out_cb_obj = 0;
		}
		avformat_close_input(&devs[i].in_ctx);
	}
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
#define FRAME_NUBER__ 1000
int
ffwr_open_in_fmt(FFWR_FMT_DEVICES *inp)
{
	int ret = 0;
	int rs = 0;
	AVFormatContext *fctx = 0;
	AVInputFormat *iformat = 0; 
	AVPacket pkt = {0};
	int readindex = 0;
	AVCodec *avcode = 0;
	AVCodecContext *actx_raw = 0;
	AVCodecContext *vctx_raw = 0;
	AVCodecContext *cctx = 0;
	int count = 0;
	AVFrame * vframe = 0;
	AVFrame * aframe = 0;
	AVFrame * inframe = 0;
	AVFrame * outframe = 0;
	AVStream *st = 0;
	FFWR_OUT_GROUP outobj = {0};
	enum AVMediaType type = AVMEDIA_TYPE_UNKNOWN;
	const AVCodec *vcodec = avcodec_find_decoder(AV_CODEC_ID_RAWVIDEO);
	const AVCodec *acodec = avcodec_find_decoder(AV_CODEC_ID_PCM_S16LE);
	vctx_raw  = avcodec_alloc_context3(vcodec);
	actx_raw  = avcodec_alloc_context3(acodec);
	avdevice_register_all();
	int nnnn = 0;
	static int64_t vframe_index = 0;
	static int64_t aframe_index = 0;
	AVFormatContext *outfmt_ctx = 0;
	
	do {
		vframe = av_frame_alloc(); 
		aframe = av_frame_alloc(); 
		outframe = av_frame_alloc();


		iformat = av_find_input_format(inp->type);

		if (!iformat) {
			break;
		}
		AVDictionary *options = NULL;
		//av_dict_set(&options, "framerate", "30", 0);
		//av_dict_set(&options, "video_size", "640×480", 0);
		av_dict_set(&options, "rtbufsize", "100M", 0);

		rs = avformat_open_input(&fctx, inp->name, iformat, &options);

		if (rs < 0) {
			break;
		}
		if (avformat_find_stream_info(fctx, NULL) < 0) {
			break;
		}
		st = fctx->streams[0];
		type = st->codecpar->codec_type;
		avcodec_parameters_to_context(vctx_raw, st->codecpar);
		//av_opt_set(vctx_raw->priv_data, "profile", "high422", 0);
		rs = avcodec_open2(vctx_raw, vcodec, 0);
		if (rs < 0) {
			break;
		}
#if 0
		st = fctx->streams[1];
		avcodec_parameters_to_context(actx_raw, st->codecpar);
		rs = avcodec_open2(actx_raw, acodec, 0);
		if (rs < 0) {
			break;
		}
#endif
		ffwr_open_out_fmt(&outobj, fctx->nb_streams + 1);
		while (nnnn < FRAME_NUBER__) {
			cctx = 0;
			readindex = av_read_frame(fctx, &pkt);
			if (readindex < 0) {
				break;
			}
			//spllog(1, "pkt::size: %d", pkt.size);
			st = fctx->streams[pkt.stream_index];
			type = st->codecpar->codec_type;

			if (type == AVMEDIA_TYPE_VIDEO) 
			{
				if (fctx->data_codec_id == AV_CODEC_ID_NONE) {
					cctx = vctx_raw;
				}
			} else if (type == AVMEDIA_TYPE_AUDIO) {
				if (fctx->data_codec_id == AV_CODEC_ID_NONE) {
					cctx = actx_raw;
				}
			}
			if (!cctx) {
				continue;
			}
			rs = avcodec_send_packet(cctx, &pkt);
			if (rs < 0) {
				av_packet_unref(&pkt);
				continue;
			}
			if (type == AVMEDIA_TYPE_VIDEO) {
				inframe = vframe;
			} else {
				inframe = aframe;
			}
			rs = avcodec_receive_frame(cctx, inframe);
			if (rs < 0) {
				av_frame_unref(vframe);
				av_packet_unref(&pkt);
				continue;
			}


			if (type == AVMEDIA_TYPE_VIDEO) {
				cctx = (AVCodecContext *)outobj.vctx;

				outframe->format = (int)cctx->pix_fmt;
				outframe->width = cctx->width;
				outframe->height = cctx->height;
				//vframe->format = AV_PIX_FMT_YUV420P;
				//vframe->width = 1920;
				//vframe->height = 1080;


				convert_frame(inframe, outframe);
				//frame = vframe;
				outframe->pts = vframe_index++;


			} else {
				//For audio
				cctx = (AVCodecContext *)outobj.actx;
				aframe->format = 8;
				//cctx->codec_id
				aframe->nb_samples = 1024;
				convert_audio_frame(inframe, aframe);
				outframe = aframe;
				outframe->pts = aframe_index++;
			}
			spllog(1, "fr::sample_rate: %d (w,h) = (%d, %d)",
			    outframe->sample_rate, outframe->width,
			    outframe->height);

			rs = av_frame_get_buffer(outframe, 0);
			rs = av_frame_make_writable(outframe);

			outfmt_ctx = outobj.fmt_ctx;
			avtry_set_profile(
			     outfmt_ctx->streams[0], 122, cctx);
			
			rs = avcodec_send_frame(cctx, outframe);
			if (rs < 0) {
				int b = 0;
			} else {
				int a = 0;
			}
			while (1) {
				ret = avcodec_receive_packet(cctx, &pkt);
				if (ret < 0) {
					break;
				} else {
					int k = 0;
				}

				pkt.stream_index = (type == AVMEDIA_TYPE_VIDEO) ? 0 :1 ;

				//outfmt_ctx->streams[0]->time_base =
				//    (AVRational){1, 30};
				//cctx->time_base = (AVRational){1, 30};

				av_packet_rescale_ts(&pkt, cctx->time_base,
				    outfmt_ctx->streams[0]->time_base);

				ret = av_interleaved_write_frame(
				    outobj.fmt_ctx, &pkt); 

				spl_avpkt(&pkt);
				if (ret < 0) { 	
					break;
				} else {
					++nnnn;
				}

				av_packet_unref(&pkt);
			}
			av_packet_unref(&pkt);

			av_frame_unref(outframe);
			av_frame_unref(vframe);
			av_frame_unref(aframe);
			av_frame_unref(inframe);
		}
	} while (nnnn < FRAME_NUBER__);
	outfmt_ctx = outobj.fmt_ctx;
	if (outfmt_ctx && outfmt_ctx->streams[0]) {
		avtry_set_profile(
		    outfmt_ctx->streams[0], 122, outobj.vctx);
	}
	//spl_milli_sleep(10);
	ret  = av_write_trailer(outobj.fmt_ctx);
	ffwr_close_out_fmt(&outobj);
	return ret;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
ffwr_open_out_fmt(FFWR_OUT_GROUP *output, int nstream)
{
	int ret = 0;
	AVFormatContext *fmt_ctx = 0;
	int rs = 0;
	FILE *fp = 0;
	AVCodec *vcodec =0;
	AVCodec *acodec =0;
	AVStream *vstream = 0;
	AVStream *astream = 0;
	AVCodecContext *vcodec_ctx = 0;
	AVCodecContext *acodec_ctx = 0;
	AVChannelLayout layout = AV_CHANNEL_LAYOUT_STEREO;
	AVIOContext *avio_ctx = 0;
	uint8_t *avio_buffer = 0;
	//FFStream *ff = 0;
	//(const FFStream *)st;
	//AVOutputFormat outfmt = {0};
	//outfmt.video_codec = AV_CODEC_ID_H264;
	do {
		//rs = avformat_alloc_output_context2(&fmt_ctx, &outfmt, "mp4", 0);
		rs = avformat_alloc_output_context2(&fmt_ctx, 0, "mp4", 0);
		if (!fmt_ctx) {
			spllog(4, "Could not allocate output context\n");
			break;
		}
		output->fmt_ctx = fmt_ctx;
		fp = fopen("d:/output.mp4", "w+");
		if (!fp) {
			break;
		}
		output->fp = fp;
		output->cb_write = write_packet;
		output->cb_seek = my_seek;
		vcodec = avcodec_find_encoder(AV_CODEC_ID_H264);
		
		vcodec_ctx = avcodec_alloc_context3(vcodec);

		vcodec_ctx->codec_id = AV_CODEC_ID_H264;
		vcodec_ctx->bit_rate = 400000;
		/* Golden rate 1:1.618 */
		vcodec_ctx->width = 640;
		vcodec_ctx->height = 480;
		//vcodec_ctx->time_base = (AVRational){1, 25};
		//vcodec_ctx->framerate = (AVRational){25, 1};

		vcodec_ctx->time_base = (AVRational){1, 30};
		vcodec_ctx->framerate = (AVRational){30, 1};
		

		vcodec_ctx->gop_size = 10;
		vcodec_ctx->max_b_frames = 0;
		//vcodec_ctx->pix_fmt = 1;
		vcodec_ctx->pix_fmt = AV_PIX_FMT_YUV422P;
		//av_opt_set(cctx->priv_data, "preset", "slow", 0);
		//av_opt_set(vcodec_ctx->priv_data, "preset", "slow", 0);
		//av_opt_set(vcodec_ctx->priv_data, "profile", "high422", 0);

		

		if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
			vcodec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		}
		vstream = avformat_new_stream(fmt_ctx, vcodec);
		//rs = avcodec_parameters_from_context(
		//    vstream->codecpar, vcodec_ctx);
		//if (rs < 0) {
		//	break;
		//}
		vstream->time_base = vcodec_ctx->time_base;
		//vstream->r_frame_rate
		vstream->codecpar->profile = 122;

		// Cấu hình màu (nếu bạn muốn metadata đầy đủ)
		vcodec_ctx->color_primaries = AVCOL_PRI_BT709;
		vcodec_ctx->color_trc = AVCOL_TRC_BT709;
		vcodec_ctx->colorspace = AVCOL_SPC_BT709;
		vcodec_ctx->chroma_sample_location = AVCHROMA_LOC_LEFT;
		vcodec_ctx->level = 30;

		// Thiết lập profile
		av_opt_set(vcodec_ctx->priv_data, "profile", "high422", 0);

		// (tùy chọn) preset & tune để dễ encode
		av_opt_set(vcodec_ctx->priv_data, "preset", "medium", 0);
		av_opt_set(vcodec_ctx->priv_data, "tune", "film", 0);

        AVCodecDescriptor const *output_descriptor =
		    avcodec_descriptor_get(vcodec->id);
		vcodec_ctx->profile = 
		vstream->codecpar->profile = 122;

		

		rs = avcodec_open2(vcodec_ctx, vcodec, 0);
		if (rs < 0) {
			break;
		}

		//vstream->codecpar->level = 30;
		rs = avcodec_parameters_from_context(
		    vstream->codecpar, vcodec_ctx);
		if (rs < 0) {
			break;
		}
		vstream->time_base = vcodec_ctx->time_base;
		//avtry_set_profile(vstream, 122);
		//avtry_set_profile(vstream, 122, vcodec_ctx);
		output->vctx = vcodec_ctx;
		/*---------------*/
#if 0
		acodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
		astream = avformat_new_stream(fmt_ctx, vcodec);
		acodec_ctx = avcodec_alloc_context3(acodec);
		acodec_ctx->sample_rate = 44100;
		acodec_ctx->sample_fmt = acodec->sample_fmts[0];
		acodec_ctx->time_base = (AVRational){1, 44100};
		av_channel_layout_copy(&acodec_ctx->ch_layout, &layout);
		rs = avcodec_parameters_from_context(
		    astream->codecpar, acodec_ctx);
		if (rs < 0) {
			break;
		}
		astream->time_base = acodec_ctx->time_base;
		rs = avcodec_open2(acodec_ctx, acodec, 0);
		if (rs < 0) {
			break;
		}
		output->actx = acodec_ctx;
#endif
		/*---------------*/
		
		avio_buffer = av_malloc(4096);
		if (!avio_buffer) {
			ret = FFWR_AVIO_MALLOC_BUFF;
			break;
		}
		avio_ctx = avio_alloc_context(avio_buffer, 4096, 1, output->fp,
		    0, output->cb_write, output->cb_seek);
		if (!avio_ctx) {
			ret = FFWR_AVIO_CTX_NULL;
			break;
		}
		fmt_ctx->pb = avio_ctx;
		fmt_ctx->flags |= AVFMT_FLAG_CUSTOM_IO;

		/* Next step: write file, write header,
		write packet audio/video*/
		AVDictionary *options_header = 0;
		rs = avformat_write_header(fmt_ctx, &options_header);
		//rs = avformat_write_header(fmt_ctx, 0);
		if (rs < 0) {
			char buf[AV_ERROR_MAX_STRING_SIZE] = {0};
			av_strerror(rs, buf, sizeof(buf));
			spllog(4, "Cannot be written by : %s.\n", buf);
			ret = FFWR_WRITE_HEADER;
			break;
		}
	} while (0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
ffwr_close_out_fmt(FFWR_OUT_GROUP *output)
{
	int ret = 0;
	AVFormatContext *fmt_ctx = 0;
	int rs = 0;
	FILE *fp = 0;
	AVCodec *vcodec = 0;
	AVCodec *acodec = 0;
	AVStream *vstream = 0;
	AVStream *astream = 0;
	AVCodecContext *vcodec_ctx = 0;
	AVCodecContext *acodec_ctx = 0;
	AVChannelLayout layout = AV_CHANNEL_LAYOUT_STEREO;
	AVIOContext *avio_ctx = 0;
	uint8_t *avio_buffer = 0;
	// AVOutputFormat outfmt = {0};
	// outfmt.video_codec = AV_CODEC_ID_H264;
	do {


		if (!output) {
			ret = 1;
			break;
		}
		fmt_ctx = output->fmt_ctx;
		avio_context_free(&(fmt_ctx->pb));
		if (output->vctx) {
			avcodec_free_context(&(output->vctx));
		}
		if (output->actx) {
			avcodec_free_context(&(output->actx));
		}
		if (output->fmt_ctx) {
			//avformat_free_context(output->fmt_ctx);
			output->fmt_ctx = 0;
		}
		if (output->fp) {
			fclose(output->fp);
		}
	} while (0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#if 1
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <stdio.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <stdio.h>

int
convert_frame(AVFrame *src, AVFrame *dst)
{

    int ret = 0;



    //AVFrame *src = av_frame_alloc();
    //src->format = src_fmt;
    //src->width  = src_w;
    //src->height = src_h;
    av_frame_get_buffer(src, 32);

    // TODO: copy hoặc điền dữ liệu thật vào src->data[0]

    //dst->format = dst_fmt;
    //dst->width  = dst_w;
    //dst->height = dst_h;
    av_frame_get_buffer(dst, 32);

    struct SwsContext *sws_ctx = sws_getContext(
		src->width, 
		src->height, 
		src->format, 
		dst->width, 
		dst->height, 
		dst->format,
        SWS_BILINEAR, NULL, NULL, NULL
    );
    if (!sws_ctx) {
        fprintf(stderr, "Error: cannot create sws context\n");
        return -1;
    }

    ret = sws_scale(
        sws_ctx,
        (const uint8_t * const *)src->data,
        src->linesize,
        0,
        src->height,
        dst->data,
        dst->linesize
    );



    sws_freeContext(sws_ctx);
    //av_frame_free(&src);
    //av_frame_free(&dst);
    return 0;
}
int
convert_audio_frame(AVFrame *src, AVFrame *dst)
{
	int ret = 0;
	struct SwrContext *swr_ctx = NULL;
	AVChannelLayout src_layout, dst_layout;

	av_channel_layout_default(&src_layout, src->ch_layout.nb_channels);
	av_channel_layout_default(&dst_layout, dst->ch_layout.nb_channels);
	// 🔧 Tạo SwrContext cho chuyển đổi audio
	swr_alloc_set_opts2(&swr_ctx, &dst_layout,
	    (enum AVSampleFormat)dst->format, dst->sample_rate, &src_layout,
	    (enum AVSampleFormat)src->format, src->sample_rate, 0, NULL);

	if (!swr_ctx) {
		fprintf(stderr, "❌ Error: cannot allocate SwrContext\n");
		return -1;
	}

	if ((ret = swr_init(swr_ctx)) < 0) {
		fprintf(stderr, "❌ Error: cannot init SwrContext (%d)\n", ret);
		swr_free(&swr_ctx);
		return ret;
	}

	// 🔄 Tính số mẫu đầu ra (có thể khác đầu vào do sample_rate thay đổi)
	//int dst_nb_samples = av_rescale_rnd(
	//    swr_get_delay(swr_ctx, src->sample_rate) + src->nb_samples,
	//    dst->sample_rate, src->sample_rate, AV_ROUND_UP);
	//
	//// 🔧 Cấp buffer cho frame đầu ra
	//dst->nb_samples = dst_nb_samples;

	if ((ret = av_frame_get_buffer(dst, 0)) < 0) {
		fprintf(
		    stderr, "❌ Error: cannot allocate dst buffer (%d)\n", ret);
		swr_free(&swr_ctx);
		return ret;
	}

	// 🚀 Thực hiện chuyển đổi audio
	ret = swr_convert(swr_ctx, dst->data, dst->nb_samples,
	    (const uint8_t **)src->data, src->nb_samples);

	if (ret < 0) {
		fprintf(stderr, "❌ Error: swr_convert failed (%d)\n", ret);
		swr_free(&swr_ctx);
		return ret;
	}

	// ✅ Cập nhật số mẫu thực tế đầu ra
	dst->nb_samples = ret;

	// 🧹 Giải phóng context
	swr_free(&swr_ctx);

	printf("✅ Audio convert done: %d samples -> %d samples\n",
	    src->nb_samples, dst->nb_samples);

	return 0;
}
#else

convert_audio_frame(src, dst);
| Chức năng | Video                 | Audio                           |
| --------- | --------------------- | ------------------------------- |
| Thư viện  | libswscale            | libswresample                   |
| Hàm chính | `sws_scale()`         | `swr_convert()`                 |
| Context   | `SwsContext`          | `SwrContext`                    |
| Mục đích  | Pixel format / Resize | Sample format / Rate / Channels |

#endif
