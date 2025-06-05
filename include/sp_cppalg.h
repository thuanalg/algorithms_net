#ifndef ___SIMPLE_ALGORITHMS_NET__
#define ___SIMPLE_ALGORITHMS_NET__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef  UNIX_LINUX
	#ifndef __STATIC_SIMPLE_ALGORITHMS_NET__
		#ifdef EXPORT_DLL_API_SIMPLE_ALGORITHMS_NET
			#define DLL_API_SIMPLE_ALGORITHMS_NET		__declspec(dllexport)
		#else
			#define DLL_API_SIMPLE_ALGORITHMS_NET		__declspec(dllimport)
		#endif
	#else
		#define DLL_API_SIMPLE_ALGORITHMS_NET
	#endif
#else
	#define DLL_API_SIMPLE_ALGORITHMS_NET
#endif /*! UNIX_LINUX */ 
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
DLL_API_SIMPLE_ALGORITHMS_NET int 
sp_cppalg_hello();

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifdef __cplusplus
}
#endif
#endif