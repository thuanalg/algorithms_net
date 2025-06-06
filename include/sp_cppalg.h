#ifndef ___SIMPLE_CPPALGORITHMS_NET__
#define ___SIMPLE_CPPALGORITHMS_NET__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef  UNIX_LINUX
	#ifndef __STATIC_SIMPLE_CPPALGORITHMS_NET__
		#ifdef EXPORT_DLL_API_SIMPLE_CPPALGORITHMS_NET
			#define DLL_API_SIMPLE_CPPALGORITHMS_NET		__declspec(dllexport)
		#else
			#define DLL_API_SIMPLE_CPPALGORITHMS_NET		__declspec(dllimport)
		#endif
	#else
		#define DLL_API_SIMPLE_CPPALGORITHMS_NET
	#endif
#else
	#define DLL_API_SIMPLE_CPPALGORITHMS_NET
#endif /*! UNIX_LINUX */ 
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
typedef int (*SP_CppCALLBACK_FUNCTION)(void*);
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

DLL_API_SIMPLE_CPPALGORITHMS_NET int 
sp_cppalg_hello();

DLL_API_SIMPLE_CPPALGORITHMS_NET int
sp_cppalg_greedy(int a, int b,
	SP_CppCALLBACK_FUNCTION f, void *obj);
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifdef __cplusplus
}
#endif
#endif