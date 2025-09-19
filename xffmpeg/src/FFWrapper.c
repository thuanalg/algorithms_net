#include "FFWrapper.h"
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    AVFormatContext *fmt_ctx;
    AVStream *stream;
    AVCodecContext *codec_ctx;
    AVIOContext *avio_ctx;
    uint8_t *avio_buffer;
    FILE *file;
    int64_t pts;
} MP4Writer;
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_all_codecs(FFWR_CODEC **lst, int *count)
{
	int ret = 0;
	const AVCodec *codec = NULL;
	void *iter = NULL;

	while ((codec = av_codec_iterate(&iter))) {
		if (av_codec_is_encoder(codec)) {
			printf("Encoder: %-15s | %s\n", codec->name,
			    codec->long_name ? codec->long_name
					     : "no long name");

		}
	}
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_all_devices(FFWR_CODEC **, int *count) {
	return 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if 0
    const AVInputFormat *ifmt = NULL;
    while ((ifmt = av_input_video_device_next(ifmt))) {
        printf("Video Input format: %s (%s)\n", ifmt->name, ifmt->long_name);
    }    
    fprintf(stdout , "\n\n");
    while ((ifmt = av_input_audio_device_next(ifmt))) {
        printf("Audio Input format: %s (%s)\n", ifmt->name, ifmt->long_name);
    }    
#endif