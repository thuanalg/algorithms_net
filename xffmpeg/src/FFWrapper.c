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
				    &options, "video_size", "1920×1080", 0);
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
		vcodec_ctx->height = 1080;
		vcodec_ctx->width = 1920;
		vcodec_ctx->time_base = (AVRational){1, 25};
		vcodec_ctx->framerate = (AVRational){25, 1};
		vcodec_ctx->gop_size = 12;
		vcodec_ctx->max_b_frames = 2;
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
	AVFrame * frame = 0;
	AVStream *st = 0;
	FFWR_OUT_GROUP outobj = {0};
	enum AVMediaType type = AVMEDIA_TYPE_UNKNOWN;
	const AVCodec *vcodec = avcodec_find_decoder(AV_CODEC_ID_RAWVIDEO);
	const AVCodec *acodec = avcodec_find_decoder(AV_CODEC_ID_PCM_S16LE);
	vctx_raw  = avcodec_alloc_context3(vcodec);
	actx_raw  = avcodec_alloc_context3(acodec);
	avdevice_register_all();
	do {
		vframe = av_frame_alloc(); 
		aframe = av_frame_alloc(); 


		iformat = av_find_input_format(inp->type);

		if (!iformat) {
			break;
		}
		rs = avformat_open_input(&fctx, inp->name, iformat, 0);

		if (rs < 0) {
			break;
		}
		if (avformat_find_stream_info(fctx, NULL) < 0) {
			break;
		}
		st = fctx->streams[0];
		type = st->codecpar->codec_type;
		avcodec_parameters_to_context(vctx_raw, st->codecpar);
		rs = avcodec_open2(vctx_raw, vcodec, 0);
		if (rs < 0) {
			break;
		}
		st = fctx->streams[1];
		avcodec_parameters_to_context(actx_raw, st->codecpar);
		rs = avcodec_open2(actx_raw, acodec, 0);
		if (rs < 0) {
			break;
		}
		ffwr_open_out_fmt(&outobj, fctx->nb_streams + 1);
		while (1) {
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
				frame = vframe;
			} else {
				frame = aframe;
			}
			rs = avcodec_receive_frame(cctx, frame);
			if (rs < 0) {
				av_frame_unref(vframe);
				av_packet_unref(&pkt);
				continue;
			}


			if (type == AVMEDIA_TYPE_VIDEO) {
				cctx = (AVCodecContext *)outobj.vctx;

				vframe->format = cctx->pix_fmt;
				vframe->width = cctx->width;
				vframe->height = cctx->height;
				//vframe->format = AV_PIX_FMT_YUV420P;
				//vframe->width = 1920;
				//vframe->height = 1080;


				frame = vframe;

			} else {
				frame = aframe;
				cctx = (AVCodecContext *)outobj.actx;
				aframe->format = cctx->pix_fmt;
				aframe->width = cctx->width;
				aframe->height = cctx->height;
				
			}
			spllog(1, "fr::sample_rate: %d (w,h) = (%d, %d)",
			    frame->sample_rate, frame->width, frame->height);

			rs = av_frame_get_buffer(frame, 0);
			rs = av_frame_make_writable(frame);

			rs = avcodec_send_frame(cctx, frame);
			if (rs < 0) {
				int b = 0;
			} else {
				int a = 0;
			}
			av_packet_unref(&pkt);

			av_frame_unref(frame);
			

		}
	} while (0);
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
	do {
		rs = avformat_alloc_output_context2(&fmt_ctx, 0, "mp4", 0);
		if (!fmt_ctx) {
			spllog(4, "Could not allocate output context\n");
			break;
		}
		output->fmt_ctx = fmt_ctx;
		fp = fopen("output.mp4", "w+");
		if (!fp) {
			break;
		}
		output->fp = fp;
		output->cb_write = write_packet;
		output->cb_seek = my_seek;
		vcodec = avcodec_find_encoder(AV_CODEC_ID_H264);
		
		vcodec_ctx = avcodec_alloc_context3(vcodec);

		//vcodec_ctx->codec_id = AV_CODEC_ID_H264;
		vcodec_ctx->bit_rate = 400000;
		/* Golden rate 1:1.618 */
		vcodec_ctx->width = 1920;
		vcodec_ctx->height = 1080;
		//vcodec_ctx->time_base = (AVRational){1, 25};
		//vcodec_ctx->framerate = (AVRational){25, 1};
		vcodec_ctx->time_base = (AVRational){1, 25};
		vcodec_ctx->framerate = (AVRational){25, 1};
		vcodec_ctx->gop_size = 10;
		vcodec_ctx->max_b_frames = 1;
		vcodec_ctx->pix_fmt = AV_PIX_FMT_YUV422P;
		//av_opt_set(cctx->priv_data, "preset", "slow", 0);
		av_opt_set(vcodec_ctx->priv_data, "preset", "slow", 0);

		if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
			vcodec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		}
		vstream = avformat_new_stream(fmt_ctx, vcodec);
		rs = avcodec_parameters_from_context(
		    vstream->codecpar, vcodec_ctx);
		if (rs < 0) {
			break;
		}
		rs = avcodec_open2(vcodec_ctx, vcodec, 0);
		if (rs < 0) {
			break;
		}
		output->vctx = vcodec_ctx;
		/*---------------*/
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
		rs = avcodec_open2(acodec_ctx, acodec, 0);
		if (rs < 0) {
			break;
		}
		output->actx = acodec_ctx;
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
		rs = avformat_write_header(fmt_ctx, 0);
		if (rs < 0) {
			char buf[AV_ERROR_MAX_STRING_SIZE] = {0};
			av_strerror(rs, buf, sizeof(buf));
			spllog(4, "Cannot be written by : %s.\n", buf);
			ret = FFWR_WRITE_HEADER;
			break;
		}
#if 0
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
#endif
	} while (0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#if 0
Got it 👍 Here’s the explanation of **MP4 structure** rewritten in English:

---

## 🧩 Basic Structure of an MP4 File

An MP4 file (MPEG-4 Part 14, ISO/IEC 14496-12) is a **container format** based on the **ISO Base Media File Format (ISOBMFF)**.
It is organized into nested **boxes (atoms)**, each with the format: `[size][type][data]`.

### Main boxes:

1. **ftyp (File Type Box)**

   * Located at the beginning of the file.
   * Declares the file type (MP4, MOV, 3GP…) and compatibility brands.

2. **moov (Movie Box)**

   * Contains **all the metadata** of the movie.
   * Inside are several sub-boxes:

     * **mvhd** (Movie Header) – global information: duration, timescale, etc.
     * **trak** (Track Box) – one for each stream (video, audio, subtitle).

       * **tkhd** – track header.
       * **mdia** (Media Box) – media-specific info.

         * **mdhd** – duration and timescale of the track.
         * **hdlr** – handler type (vide, soun, subt).
         * **minf** – media information.

           * **stbl (Sample Table)** – index of frames: sample offsets, timestamps, sizes.

3. **mdat (Media Data Box)**

   * Holds the raw media data (video frames, audio samples).
   * The `moov` box describes *how to interpret* the data in `mdat`.

4. **free / skip**

   * Optional padding boxes, sometimes used for editing or streaming.

---

## 🔎 Example of a simple MP4 structure

```
ftyp
moov
 ├─ mvhd
 ├─ trak (video)
 │   ├─ tkhd
 │   └─ mdia
 │       ├─ mdhd
 │       ├─ hdlr
 │       └─ minf
 │           └─ stbl
 └─ trak (audio)
     ├─ tkhd
     └─ mdia
         ├─ mdhd
         ├─ hdlr
         └─ minf
             └─ stbl
mdat
```

---

## 📌 Summary

* **ftyp** → identifies the file type.
* **moov** → metadata: duration, codecs, tracks, timestamps, indexes.
* **mdat** → actual raw media data (video/audio).

👉 If `moov` is missing → the player can’t interpret the file.
👉 If `mdat` is missing → only metadata remains, nothing to play.

---

Do you want me to also give you an **FFmpeg command** that dumps the actual MP4 box structure so you can see it in action?

#endif
