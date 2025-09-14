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

#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

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

/* DLL_API_FF_WRAPPER */
DLL_API_FF_WRAPPER int
ffwr_all_codecs();
#ifdef __cplusplus
}
#endif
#endif
