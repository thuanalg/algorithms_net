/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/


/* Email:
 *		<nguyenthaithuanalg@gmail.com> - Nguyễn Thái Thuận
 * Mobile:
 *		<+084.799.324.179>
 * Skype:
 *		<nguyenthaithuanalg>
 * Date:
 *		<2025-Aug-14>
 * The lasted modified date:
 * Decription:
 *		
*/

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifndef ___FF_WRAPPER__
#define ___FF_WRAPPER__

#include <stdlib.h>


#if 0
#ifndef UNIX_LINUX
    #define UNIX_LINUX                      
#endif
#endif

#if 0
#ifndef __MACH__
    #define __MACH__                        
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif



#ifndef UNIX_LINUX
    #ifndef __STATIC_FF_WRAPPER__
        #ifdef EXPORT_DLL_API_FF_WRAPPER
            #define DLL_API_FF_WRAPPER              __declspec(dllexport)
        #else
            #define DLL_API_FF_WRAPPER              __declspec(dllimport)
        #endif
    #else
        #define DLL_API_FF_WRAPPER              
    #endif
#else
    #define DLL_API_FF_WRAPPER              
#endif /*! UNIX_LINUX */


/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifndef FFLLU
    #define FFLLU                           unsigned long long
#endif

#ifndef FFLL
    #define FFLL                           long long
#endif

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
typedef int (*FFWR_WRITE_PACKET_CB)( void *obj, unsigned char *buf, int buf_size);
#ifndef FFLL
	#define FFLL long long
#endif
typedef FFLL (*FFWR_SEEK_PACKET_CB)(void *opaque, FFLL offset, int whence);

typedef enum {
    FFWR_OK,
    FFWR_MALLOC,
    FFWR_REALLOC, 
    FFWR_NULL_ARG,
	FFWR_NO_FORMAT,
    FFWR_NO_DEVICE,
	FFWR_OPEN_STREAM_VIDEO,
	FFWR_OPEN_STREAM_AUDIO,
	FFWR_NO_VIDEO_CODEC_FOUND,
	FFWR_NO_AUDIO_CODEC_FOUND,
	FFWR_ALLOC_VIDEO_CONTEXT,
	FFWR_ALLOC_AUDO_CONTEXT,
	FFWR_OPEN_VIDEO_CONTEXT,
	FFWR_OPEN_AUDO_CONTEXT,
	FFWR_SEND_PACKET_FAILED,
	FFWR_CREATE_OUTPUT_CONTEXT,
	FFWR_H264_NOT_FOUND,
	FFWR_ACC_NOT_FOUND,
	FFWR_AVIO_MALLOC_BUFF,
	FFWR_OPEN_FILE,
	FFWR_AVIO_CTX_NULL,
	FFWR_CREATE_VIDEO_STREAM,
	FFWR_CREATE_AUDIO_STREAM,
	FFWR_WRITE_HEADER,
	FFWR_SEND_OUTPUT_CONTEXT,
    
    
    FFWR_ERR_END
} FFWR_ERR_CODE;

typedef enum {
	FFWR_UNKNOWN = -1,
    FFWR_VIDEO,
	FFWR_AUDIO,
} FFWR_AUDIO_VIDEO;


typedef struct __FFWR_CODEC__{
    char *name;
    char *detail;
} FFWR_CODEC;


typedef struct __FFWR_DEVICE__{
    char *name;
    char *detail;
    FFWR_AUDIO_VIDEO av;
    void *in_ctx; /*AVFormatContext, input context*/
    void *in_stream; /*AVStream, input context*/
    void *in_codec_ctx; /*AVCodecContext, input context*/



    void *out_ctx; /*AVFormatContext, output context*/
    void *out_vcodec_context; /*AVCodecContext, output context*/
    void *out_acodec_context; /*AVCodecContext, output context*/
    void *out_avio;
    void *out_avio_buff;
    void *out_stream;
    FFWR_WRITE_PACKET_CB avio_cb_fn;
    void *out_cb_obj;
    char filename[512];
    void *filestream;
} FFWR_DEVICE;

typedef struct __FFWR_OUT_GROUP__ {
	int n; /*n AVCodecContext, n AVStream*/
	void *fmt_ctx;
	void **c_ctx; /*n AVCodecContext*/
	void *fp; /*FILE *stream*/
	FFWR_WRITE_PACKET_CB cb_write;
	FFWR_SEEK_PACKET_CB cb_seek;
	void *vctx;
	void *actx;
} FFWR_OUT_GROUP;

typedef struct __FFWR_FMT_DEVICES__
{
	/*dshow: direct show, av_find_input_format(type) */
	char type[64]; 
	
    /*dshow: avformat_open_input(name) 
    * video=Integrated Webcam:audio=Microphone (2- Realtek(R) Audio)
    */
	char name[1024]; 
	
    char *detail;
    
    void *in_fmt_ctx; /*AVFormatContext, output context*/

} FFWR_FMT_DEVICES;

typedef struct {
    char *name;
    char *detail;
} FFWR_DEMUXER_FMT;

typedef struct {
	char *name;
	char *detail;
} FFWR_MUXER_FMT;

typedef struct __FFWR_VIDEO_RECV__{
    char *name;
    char *detail;
} FFWR_VIDEO_RECV;

typedef struct __FFWR_AUDIO_RECV__{
    char *name;
    char *detail;
} FFWR_AUDIO_RECV;

typedef struct __FFWR_AUDIO_VIDEO_RECV__{
    char *name;
    char *detail;
    
    FFWR_AUDIO_RECV *au;
    FFWR_VIDEO_RECV *v;
    void *out;
} FFWR_AUDIO_VIDEO_RECV;
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
#define ffwr_malloc(__nn__, __obj__, __type__)                                 \
	{                                                                      \
		(__obj__) = (__type__ *)malloc(__nn__);                        \
		if (__obj__) {                                                 \
			spllog(0, "Malloc [MEM-FFWR] : 0x%p.", (__obj__));           \
			memset((__obj__), 0, (__nn__));                        \
		} else {                                                       \
			spllog(4, "Malloc: error.");                           \
		}                                                              \
	}
#define ffwr_realloc(__nn__, __obj__, __type__)                                 \
	{                                                                      \
		(__obj__) = (__type__ *)realloc((__obj__) , (__nn__));                        \
		if (__obj__) {                                                 \
			spllog(0, "Realloc [MEM-FFWR] : 0x%p.", (__obj__));           \
		} else {                                                       \
			spllog(4, "Realloc: error.");                           \
		}                                                              \
	}


#define ffwr_free(__obj__)                                                     \
	{                                                                      \
		if (__obj__) {                                                 \
			spllog(0, "Free [MEM-FFWR] : 0x%p.", (__obj__));             \
			free(__obj__);                                         \
			(__obj__) = 0;                                         \
		}                                                              \
	}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

/* Find all codecs. */
DLL_API_FF_WRAPPER int
ffwr_all_codecs(FFWR_CODEC **, int *count);
/* Find all codecs. */
DLL_API_FF_WRAPPER int
ffwr_clear_all_codecs(FFWR_CODEC **, int count);

/* Find a codec. */
DLL_API_FF_WRAPPER int
ffwr_find_codec(char *name, int *output);

/* Find all audio/video devices. */
DLL_API_FF_WRAPPER int
ffwr_all_devices(FFWR_DEVICE **, int *count);

DLL_API_FF_WRAPPER int
ffwr_clear_all_devices(FFWR_DEVICE **, int count);

/* Find all audio/video devices by name. */
DLL_API_FF_WRAPPER int
ffwr_devices_by_name(FFWR_DEVICE **, int *count, char *name);

DLL_API_FF_WRAPPER int
ffwr_clear_devices_by_name(FFWR_DEVICE **, int count);

/* Find all demuxer format. */
DLL_API_FF_WRAPPER int
ffwr_all_demuxers(FFWR_DEMUXER_FMT **, int *count);

DLL_API_FF_WRAPPER int
ffwr_clear_all_demuxers(FFWR_DEMUXER_FMT **, int count);


DLL_API_FF_WRAPPER int
ffwr_all_muxers(FFWR_MUXER_FMT **, int *count);

DLL_API_FF_WRAPPER int
ffwr_clear_all_muxers(FFWR_MUXER_FMT **, int count);

DLL_API_FF_WRAPPER int
ffwr_open_devices(FFWR_DEVICE *, int count, char *name);

DLL_API_FF_WRAPPER int
ffwr_open_output(FFWR_DEVICE *devs, int count);

DLL_API_FF_WRAPPER int
ffwr_devices_operate(FFWR_DEVICE *, int count);

DLL_API_FF_WRAPPER int
ffwr_close_devices(FFWR_DEVICE *, int count);

DLL_API_FF_WRAPPER int
ffwr_open_in_fmt(FFWR_FMT_DEVICES * inp);
//FFWR_OUT_GROUP
DLL_API_FF_WRAPPER int
ffwr_open_out_fmt(FFWR_OUT_GROUP *out, int n);

int
ffwr_close_out_fmt(FFWR_OUT_GROUP *output);
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifdef __cplusplus
}
#endif
#endif
