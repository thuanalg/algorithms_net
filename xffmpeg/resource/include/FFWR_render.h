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

#define LLU                             unsigned long long

#define FFWR_LOG_BASE                    0
#define FFWR_LOG_DEBUG                   1
#define FFWR_LOG_INFO                    2
#define FFWR_LOG_WARNING                 3
#define FFWR_LOG_ERROR                   4
#define FFWR_LOG_FATAL                   5
#define FFWR_LOG_PEAK                    6

#if 0
#ifndef FFWR_RL_BUF
#define FFWR_RL_BUF                      50
#endif
#endif

#define FFWR_RL_BUF                      256
#define FFWR_PATH_FOLDER                 (256 + 16)
#define FFWR_IDD_NAME                    32
#define FFWR_TOPIC_SIZE                  32
#define FFWR_MEMO_PADDING                2048
#define FFWR_SHARED_KEY_LEN              32
#define FFWR_SHARED_NAME_LEN             64
#define FFWR_FNAME_LEN                   (FFWR_IDD_NAME + 32)
#define FFWR_TEMPLATE_LEN                (FFWR_PATH_FOLDER + FFWR_FNAME_LEN + 32)
#define FFWR_FULLPATH_LEN                (FFWR_TEMPLATE_LEN + 32 + 16)
#define FFWR_MILLION                     1000000
#define FFWR_RANGE_YEAR                  10000
#define FFWR_RANGE_MONTH                 13
#define FFWR_RANGE_DAY                   32

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

typedef enum __FFWR_LOG_ERR_CODE__ {
	FFWR_NO_ERROR,


	FFWR_END_ERROR,
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

#define FFWR_uchar                       unsigned char
#define FFWR_uint                        unsigned int


#if 0
DLL_API_FFWR_RENDER int
FFWR_local_time_now(FFWR_local_time_st *st_time);
#endif
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef __cplusplus
}
#endif
#endif
