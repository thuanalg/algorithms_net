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
				    &options, "video_size", "640x480", 0);
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
	AVPacket *pkt = 0;
	AVFrame *frame = 0;
	int readindex = -1;
	int i = 0;
	int rs = 0;
	int count_frame = 0;
	do {
		pkt = av_packet_alloc();
		frame = av_frame_alloc();
	} while (0);

	if (ret) {
		if (pkt) {
			av_packet_free(&pkt);
		}
		if (frame) {
			av_frame_free(&frame);
		}
		return ret;
	}

	do {
		for (i = 0 ; i < count ; ++i) {
			readindex = av_read_frame(
				devs[i].in_ctx, pkt);
			if (readindex < 0) {
				continue;
			}
			rs = avcodec_send_packet(
				devs[i].in_codec_ctx, pkt);
			if (rs) {
				spllog(3, "in_ctx, send packet: %s.", 
					(devs[i].av == FFWR_VIDEO) ? 
					"Video" : "Audio");
				continue;
			}
			count_frame = 0;
			while (1) {
				rs = avcodec_receive_frame(
				    devs[i].in_codec_ctx, frame);
				if (rs) {
					if (count_frame) {
						break;
					}
					spllog(3, "in_ctx, recv fame: %s.",
					    (devs[i].av == FFWR_VIDEO)
						? "Video" : "Audio");
					break;
				}
				++count_frame;
				/*
				* Have to init output AVFormatContex
				* Processing
				*/
				av_frame_unref(frame);
			}
			
		}
		if (ret) {
			break;
		}
		av_packet_unref(pkt);
	} while (1);
	if (pkt) {
		av_packet_free(&pkt);
	}
	if (frame) {
		av_frame_free(&frame);
	}
	return ret;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
ffwr_open_output(FFWR_DEVICE *devs, int count)
{
	int ret = 0;
	do {
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
		avformat_close_input(&devs[i].in_ctx);
	}
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
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
