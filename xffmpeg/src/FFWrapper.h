/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

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

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef ___FF_WRAPPER__
#define ___FF_WRAPPER__


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


/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
typedef struct __FFWR_CODEC__{
    char *name;
    char *detail;
} FFWR_CODEC;

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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/* DLL_API_FF_WRAPPER */
DLL_API_FF_WRAPPER int
ffwr_all_codecs(FFWR_CODEC **, int *count);
#ifdef __cplusplus
}
#endif
#endif
