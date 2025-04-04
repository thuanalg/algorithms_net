#ifndef ___SIMPLE_ALGORITHMS_NET__
#define ___SIMPLE_ALGORITHMS_NET__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef __SP_FILLE__
	#define __SP_FILLE__(__p__)	do { __p__ = strrchr(__FILE__, '/'); if(__p__) {++__p__;break;} \
		__p__ = strrchr(__FILE__, '\\'); if(__p__) {++__p__;break;}\
		__p__ = __FILE__;} while(0);
#endif

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

#define sp_alg_free(__obj__)   { if(__obj__) { spllog(0, "Free: 0x%p.", (__obj__)); free(__obj__); (__obj__) = 0; } }

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
		int 	sz;
		char 	data[0];
	} SP_ALGORITHMS_NET_GENERIC_ST;

typedef int (*SPL_VECTOR_CMP_CALLBACK)(void*, int l, int h);

/*
* __target__: generic object
* __type__: any type: int, char, ... a structure
* __src__: pointer of instance: int*, char*, ...
* __nitem__: how many items
* __nstep__: Memory step
*
*/
#define __sp_apl_vector_append__(__target__, __type__, __src__, __nitem__, __nstep__)  \
do {\
	;int __total__ = 0;\
	;int __range__ = (__nstep__) * sizeof(__type__) * (__nitem__);\
	;if(!(__target__)) {\
	;	__total__ = __range__ + sizeof(SP_ALGORITHMS_NET_GENERIC_ST);;\
	;	sp_alg_malloc(__total__, __target__, SP_ALGORITHMS_NET_GENERIC_ST);;\
	;	(__target__)->total += __total__;;\
	;	(__target__)->range += __range__;;\
	;	if(!(__target__)) {break;};\
	;	;spllog(0, "New size: total: %d, __nitem__: %d.", __total__, (__nitem__));;\
	;	;(__target__)->sz = sizeof(__type__);\
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
	;;	;;\
	;memcpy((__target__)->data + (__target__)->pl, (__src__), sizeof(__type__) * (__nitem__));	;;\
	;(__target__)->pl += sizeof(__type__) * (__nitem__);;	;;\
	;spllog(0, "New size: ---->total: %d, range: %d, pl: %d.", (__target__)->total, __range__, (__target__)->pl);;	;;\
} while(0);

/*
* __target__: generic object
* __type__: any type: int, char, ... a structure
* __index__: index of item
* __nitem__: how many items
* __nstep__: Memory step
*
*/
#define __sp_apl_vector_rem__(__target__, __type__, __index__, __nitem__, __nstep__) \
do{\
	;char *__po__ = 0;\
	;if((__index__) < 0) break;\
	;if((__nitem__) < 1) break;\
	;if ((__index__) * sizeof(__type__)  >= (__target__)->pl) break;\
	;if ((__nitem__) * sizeof(__type__) > (__target__)->pl) break;\
	;if ( ( sizeof(__type__) * ((__nitem__) + (__index__))) > (__target__)->pl) break;\
	;;;\
	;;\
	;;;\
	;;\
	;;\
	;__po__ = (__target__)->data + sizeof(__type__) * (__index__);\
	;memmove(__po__, __po__ + sizeof(__type__) * (__nitem__), (__target__)->pl - sizeof(__type__) * (__nitem__));\
	;(__target__)->pl -= sizeof(__type__) * (__nitem__);;\
	;;;\
	;if((__nstep__) * sizeof(__type__) + (__target__)->pl < (__target__)->range) {\
		;int __dta__ = (__target__)->range - (__nstep__) * sizeof(__type__);\
		;(__target__)->range -= __dta__;\
		;(__target__)->total -= __dta__;\
		;;\
		;__target__ = realloc(__target__, __target__->total);;\
		;if(!__target__) { spllog(5, "FATAL Error realloc.");exit(1); };;\
	};;\
	;;\
} while (0);

/*
* __target__: generic object
*
*/
#define __sp_apl_vector_clear__(__target__) \
do{\
	;(__target__) = (SP_ALGORITHMS_NET_GENERIC_ST *) realloc((__target__), sizeof(SP_ALGORITHMS_NET_GENERIC_ST));;;\
	;;if(!(__target__)) {\
	;	;spllog(5, "FATAL Error realloc.");\
	;	;break;\
	;};\
	;__target__->total = sizeof(SP_ALGORITHMS_NET_GENERIC_ST);;\
	;__target__->range = 0;;\
	;__target__->pl = 0;;\
	;sp_alg_malloc(__target__->total, __target__, SP_ALGORITHMS_NET_GENERIC_ST);;\
} while (0);


/*
* __target__: generic object
* __type__: any type: int, char, ... a structure
* __src__: pointer of instance: int*, char*, ... 
* __index__: index of item
* __nitem__: how many items
* __nstep__: Memory step
*
*/
#define __sp_apl_vector_insert__(__target__, __type__, __src__, __index__, __nitem__, __nstep__)  \
do {\
	;int __total__ = 0;\
	;char * __po__ = 0;\
	;int __range__ = (__nstep__) * sizeof(__type__) * (__nitem__);\
	;int __adjust__ = (__index__) < 0 ? 0 : (__index__);\
	;\
	;if(!(__target__)) {\
	;	__total__ = __range__ + sizeof(SP_ALGORITHMS_NET_GENERIC_ST);;\
	;	sp_alg_malloc(__total__, __target__, SP_ALGORITHMS_NET_GENERIC_ST);;\
	;	(__target__)->total += __total__;;\
	;	(__target__)->range += __range__;;\
	;	if(!(__target__)) {break;};\
	;	;spllog(0, "New size: total: %d, __nitem__: %d.", __total__, (__nitem__));;\
	;	;(__target__)->sz = sizeof(__type__);\
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
	;__adjust__ = __index__ > (__target__)->pl/sizeof(__type__) ? ((__target__)->pl/sizeof(__type__)) : __index__;	;;\
	;;;\
	;__po__ = (__target__)->data + (__adjust__) * sizeof(__type__);	;;\
	;memmove(__po__ + sizeof(__type__) * (__nitem__ ), __po__,  (__target__)->pl - sizeof(__type__) * (__adjust__));	;;\
	;;;\
	;memcpy(__po__, (__src__), sizeof(__type__) * (__nitem__));	;;\
	;(__target__)->pl += sizeof(__type__) * (__nitem__);;	;;\
	;spllog(0, "New size: ---->total: %d, range: %d, pl: %d.", (__target__)->total, __range__, (__target__)->pl);;	;;\
} while(0);

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

DLL_API_SIMPLE_ALGORITHMS_NET int 
sp_apl_vector_quicksort(SP_ALGORITHMS_NET_GENERIC_ST *v, SPL_VECTOR_CMP_CALLBACK f);

DLL_API_SIMPLE_ALGORITHMS_NET int 
sp_apl_quicksort_int_cmp(void* arr, int i, int j);

DLL_API_SIMPLE_ALGORITHMS_NET int
sp_apl_quicksort_int(void* arr, int i, int j);

#define		sp_apl_vector_append		__sp_apl_vector_append__
#define		sp_apl_vector_rem			__sp_apl_vector_rem__
#define		sp_apl_vector_insert		__sp_apl_vector_insert__
#define		sp_apl_vector_clear			__sp_apl_vector_clear__

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifdef __cplusplus
}
#endif
#endif