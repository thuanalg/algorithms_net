#include "pch.h"
#include "FF_ListCtrl.h"
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#pragma comment(lib, "avcodec.lib")

FF_ListCtrl::FF_ListCtrl()
{
	this->load_codecs();
}

FF_ListCtrl::~FF_ListCtrl()
{
}
// https://ffmpeg.org/doxygen/0.6/avcodec_8h.html
int FF_ListCtrl::load_codecs()
{
	int ret = 0;
	const AVCodec *codec = NULL;
	void *iter = NULL;
	codecs = 0;
	size = 0;

		//	while ((codec = av_codec_iterate(&iter))) {
//		if (av_codec_is_encoder(codec)) {
//#if 1
//			printf("Encoder: %-15s | %s\n", codec->name,
//			    codec->long_name ? codec->long_name
//					     : "no long name");
//#else
//			if (strstr(codec->name, "26") ||
//			    strstr(codec->long_name, "26")) {
//				printf("Encoder: %-15s | %s\n", codec->name,
//				    codec->long_name ? codec->long_name
//						     : "no long name");
//#endif
//		}
//	}
	return ret;
}
