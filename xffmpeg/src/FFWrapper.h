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
typedef enum {
    FFWR_OK,
    FFWR_MALLOC,
    FFWR_REALLOC, 
    FFWR_NULL_ARG,
	FFWR_NO_FORMAT,
    FFWR_NO_DEVICE,

    
    
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
} FFWR_DEVICE;

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

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifdef __cplusplus
}
#endif
#endif
