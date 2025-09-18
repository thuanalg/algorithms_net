#include "FFWrapper.h"
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

int
ffwr_all_codecs(FFWR_CODEC **lst, int *count)
{
	int ret = 0;
	const AVCodec *codec = NULL;
	void *iter = NULL;

	while ((codec = av_codec_iterate(&iter))) {
		if (av_codec_is_encoder(codec)) {
#if 0
			printf("Encoder: %-15s | %s\n", codec->name,
			    codec->long_name ? codec->long_name
					     : "no long name");

#else
			if (strstr(codec->name, "26") ||
			    strstr(codec->long_name, "26")) {
				printf("Encoder: %-15s | %s\n", codec->name,
				    codec->long_name ? codec->long_name
						     : "no long name");
			}
#endif
		}
	}
	return ret;
}