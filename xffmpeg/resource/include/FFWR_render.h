/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* Email:
 *		<nguyenthaithuanalg@gmail.com> - Nguyễn Thái Thuận
 * Mobile:
 *		<+084.799.324.179>
 * Skype:
 *		<nguyenthaithuanalg>
 * Date:
 *		<2024-July-14>
 * The lasted modified date:
 *		<2024-Sep-14>
 *		<2024-Dec-18>
 *		<2024-Dec-20>
 *		<2024-Dec-22>
 *		<2024-Dec-23>
 *		<2024-Dec-30>
 *		<2024-Jan-06>
 *		<2025-Jan-08>
 *		<2025-Jan-10>
 *		<2025-Feb-01>
 *		<2025-Feb-16>
 *		<2025-Apr-11>
 *		<2025-Apr-22>
 *		<2025-Jun-01>
 *		<2025-Jun-11>
 * Decription:
 *		The (only) main header file to export 4 APIs: [FFWR_init_log_ext, spllog, spllogtopic, FFWR_finish_log].
 */
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef ___FFWR_RENDER_LOG__
#define ___FFWR_RENDER_LOG__                 
#include <stdio.h>
#include <string.h>
/*strrchr*/

#define FFWR_MIN_AB(a, b) ((a) < (b)) ? (a) : (b)
#define FFWR_MAX_AB(a, b) ((a) > (b)) ? (a) : (b)

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

#ifndef LLU
	#define LLU                             unsigned long long
#endif

#define FFWR_uchar                       	unsigned char
#define FFWR_uint                        	unsigned int
#define FFWR_InitFlags						FFWR_uint


#ifndef UNIX_LINUX
#ifndef __FFWR_RENDER_STATIC_LOG__
#ifdef EXPORT_DLL_API_FFWR_RENDER
#define DLL_API_FFWR_RENDER              __declspec(dllexport)
#else
#define DLL_API_FFWR_RENDER              __declspec(dllimport)
#endif
#else
#define DLL_API_FFWR_RENDER              
#endif
#else
#define DLL_API_FFWR_RENDER              
#endif /*! UNIX_LINUX */

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#define FFWR_INIT_TIMER          0x00000001u
#define FFWR_INIT_AUDIO          0x00000010u
#define FFWR_INIT_VIDEO          0x00000020u  /**< FFWR_INIT_VIDEO implies FFWR_INIT_EVENTS */
#define FFWR_INIT_JOYSTICK       0x00000200u  /**< FFWR_INIT_JOYSTICK implies FFWR_INIT_EVENTS */
#define FFWR_INIT_HAPTIC         0x00001000u
#define FFWR_INIT_GAMECONTROLLER 0x00002000u  /**< FFWR_INIT_GAMECONTROLLER implies FFWR_INIT_JOYSTICK */
#define FFWR_INIT_EVENTS         0x00004000u
#define FFWR_INIT_SENSOR         0x00008000u
#define FFWR_INIT_NOPARACHUTE    0x00100000u  /**< compatibility; this flag is ignored. */
#define FFWR_INIT_EVERYTHING ( \
                FFWR_INIT_TIMER | FFWR_INIT_AUDIO | FFWR_INIT_VIDEO | FFWR_INIT_EVENTS | \
                FFWR_INIT_JOYSTICK | FFWR_INIT_HAPTIC | FFWR_INIT_GAMECONTROLLER | FFWR_INIT_SENSOR \
            )
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
typedef enum __FFWR_LOG_ERR_CODE__ {
	FFWR_NO_ERR,
	FFWR_INIT_ERR,


	FFWR_END_ERR,
} FFWR_LOG_ERR_CODE;
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

typedef struct __FFWR_CALLBACL_DATA__ {
	int total;
	int eventid;
	/*int range;*/
	int pc;
	int pl;
	char data[0];
} FFWR_CALLBACL_DATA;

typedef int (*FFWR_CALLBACL_FUNCTION)(void *);

typedef struct __FFWR_GENERIC_DATA__ {
	int total; /*Total size*/
	int range; /*Total size*/
	int pc; /*Point to the current*/
	int pl; /*Point to the last*/
	char data[0]; /*Generic data */
} FFWR_gen_data_st;




#if 0
DLL_API_FFWR_RENDER int
ffwr_hello();

DLL_API_FFWR_RENDER int
ffwr_init(FFWR_InitFlags flags);

#endif
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef __cplusplus
}
#endif
#endif
