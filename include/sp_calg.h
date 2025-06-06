#ifndef ___SIMPLE_CALGORITHMS_NET__
#define ___SIMPLE_CALGORITHMS_NET__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef  UNIX_LINUX
	#ifndef __STATIC_SIMPLE_CALGORITHMS_NET__
		#ifdef EXPORT_DLL_API_SIMPLE_CALGORITHMS_NET
			#define DLL_API_SIMPLE_CALGORITHMS_NET		__declspec(dllexport)
		#else
			#define DLL_API_SIMPLE_CALGORITHMS_NET		__declspec(dllimport)
		#endif
	#else
		#define DLL_API_SIMPLE_CALGORITHMS_NET
	#endif
#else
	#define DLL_API_SIMPLE_CALGORITHMS_NET
#endif /*! UNIX_LINUX */ 
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
typedef int (*SP_CCALLBACK_FUNCTION)(void*);
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
DLL_API_SIMPLE_CALGORITHMS_NET int 
sp_calg_hello();

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifdef __cplusplus
}
#endif
#endif