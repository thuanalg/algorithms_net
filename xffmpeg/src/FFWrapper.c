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
#define FFWR_STEP           200
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
	        size = (step + 1) * sizeof(FFWR_CODEC);
	        ffwr_realloc(size, p, FFWR_CODEC);
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
	        size = (step + 1) * sizeof(FFWR_CODEC);
	        ffwr_realloc(size, p, FFWR_CODEC);
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

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/* Find all input format. */
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
	        size = (step + 1) * sizeof(FFWR_CODEC);
	        ffwr_realloc(size, p, FFWR_CODEC);
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
