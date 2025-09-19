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


#if 1
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

typedef struct __FFWR_CODEC__{
    char *name;
    char *detail;
} FFWR_CODEC;

typedef struct __FFWR_DEVICE__{
    char *name;
    char *detail;
} FFWR_DEVICE;

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

/* Find all codecs. */
DLL_API_FF_WRAPPER int
ffwr_all_codecs(FFWR_CODEC **, int *count);
/* Find all codecs. */
DLL_API_FF_WRAPPER int
ffwr_clear_all_codecs(FFWR_CODEC **, int *count);

/* Find a codec. */
DLL_API_FF_WRAPPER int
ffwr_find_codec(char *name, int *outout);

/* Find all audio/video devices. */
DLL_API_FF_WRAPPER int
ffwr_all_devices(FFWR_DEVICE **, int *count);

DLL_API_FF_WRAPPER int
ffwr_clear_all_devices(FFWR_DEVICE **, int *count);

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifdef __cplusplus
}
#endif
#endif
