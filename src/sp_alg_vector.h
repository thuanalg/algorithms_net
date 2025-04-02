#ifndef ___SIMPLE_ALGORITHMS_NET__
#define ___SIMPLE_ALGORITHMS_NET__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define __SP_FILLE__(__p__)	do { __p__ = strrchr(__FILE__, '/'); if(__p__) {++__p__;break;} \
	__p__ = strrchr(__FILE__, '\\'); if(__p__) {++__p__;break;}\
	__p__ = __FILE__;} while(0);

#ifndef __UNIX_LINUX_CPP11_AND_NEWERS__
	#define sp_alg_console_log(__lv__, ___fmttt___, ...)		{; const char *pfn = 0; __SP_FILLE__(pfn);;\
		fprintf(stdout, "[%d - %s - %s] [%s:%s:%d] "___fmttt___"\n" , (__lv__), __DATE__, __TIME__, (char *)pfn, (char*)__FUNCTION__, (int)__LINE__, ##__VA_ARGS__);}
#else
	#define sp_alg_console_log(__lv__, ___fmttt___, ...)		{ ;std::string __c11fmt__="[%d - %s - %s] [%s:%s:%d] ";\
		__c11fmt__+=___fmttt___;__c11fmt__+="\n"; const char *pfn = 0; __FILLE__(pfn);;\
		fprintf(stdout, __c11fmt__.c_str(), (__lv__), __DATE__, __TIME__, (char *)pfn, (char *)__FUNCTION__, (int)__LINE__, ##__VA_ARGS__);}
#endif

#ifdef __ADD_SIMPLE_LOG__
	#include <simplelog.h>
#else
	#define spllog		sp_alg_console_log
#endif

#define sp_alg_malloc(__nn__, __obj__, __type__) { (__obj__) = (__type__*) malloc(__nn__); if(__obj__) \
	{spllog(0, "Malloc: 0x%p, size: %d.", (__obj__), (__nn__)); memset((__obj__), 0, (__nn__));} \
	else {spllog(0, "Malloc: error.");}} 

#define sp_alg_free(__obj__)   { if(__obj__) { spllog(0, "Free: 0x%p.", (__obj__)); free(__obj__); } }

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

	typedef struct __SP_ALGORITHMS_NET_GENERIC_ST__ {
		int 	total;
		int 	range;
		int 	pl;
		int 	pc;
		int 	type;
		char 	data[0];
	} SP_ALGORITHMS_NET_GENERIC_ST;

#define sp_apl_vector_add(__target__, __type__, __src__, __nitem__, __nstep__)  \
do {\
	;int __total__ = 0;\
	;int __range__ = (__nstep__) * sizeof(__type__) * (__nitem__);\
	;if(!(__target__)) {\
	;	__total__ = __range__ + sizeof(SP_ALGORITHMS_NET_GENERIC_ST);;\
	;	sp_alg_malloc(__total__, __target__, SP_ALGORITHMS_NET_GENERIC_ST);;\
	;	(__target__)->total += __total__;;\
	;	(__target__)->range += __range__;;\
	;	if(!(__target__)) {break;};\
	;	;;\
	;} \
	else {\
	;	if((__target__)->pl + sizeof(__type__) * (__nitem__) > (__target__)->range) {\
	;		;__total__ = (__target__)->total + __range__;\
	;		;(__target__) = (SP_ALGORITHMS_NET_GENERIC_ST *) realloc((__target__), __total__);\
	;		;if(!(__target__)) {\
	;			;spllog(5, "FATAL Error realloc.");\
	;			;break;\
			;};\
	;		spllog(0, "New size: total: %d, __nitem__: %d.", __total__, (__nitem__));;\
	;		(__target__)->total += __range__;;\
	;		(__target__)->range += __range__;;\
	;		;;\
		};\
	;	;;\
	;};\
	;spllog(0, "New size: ---->total: %d, range: %d.", (__target__)->total, __range__);	;;\
	;memcpy((__target__)->data + (__target__)->pl, (__src__), sizeof(__type__) * (__nitem__));	;;\
	;(__target__)->pl += sizeof(__type__);;	;;\
} while(0);
#ifdef __cplusplus
}
#endif
#endif