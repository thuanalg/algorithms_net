#include "pch.h"
#include "FF_MemDetect.h"
#include <stdio.h>

#define FF_LOGMEM_CONT(__obj__)                                                            \
	{                                                                      \
		fprintf(stdout, "%s:0x%p", __FUNCTION__, __obj__);                                                                    \
	}
#define FF_LOGMEM_DETR(__obj__)                                                \
	{                                                                      \
		fprintf(stdout, "%s:0x%p", __FUNCTION__, __obj__);                                                                     \
	}
FF_MemDetect::FF_MemDetect(){
	FF_LOGMEM_CONT(this);
} 
FF_MemDetect::~FF_MemDetect()
{
	FF_LOGMEM_DETR(this);
}
