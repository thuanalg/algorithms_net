#pragma once
#include <afxcmn.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include "FF_MemDetect.h"

class FF_ListCtrl : 
	public virtual FF_MemDetect, 
	public CListCtrl
{
public:
	FF_ListCtrl();
	virtual ~FF_ListCtrl();
protected:
private:
	AVCodec *codecs;
	int size;
	int load_codecs();
};
