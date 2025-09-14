#include <stdio.h>
#include <stdlib.h>

#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

int
get_all_encoders()
{
	const AVCodec *codec = NULL;
	void *iter = NULL;

	while ((codec = av_codec_iterate(&iter))) {
		if (av_codec_is_encoder(codec)) {
#if 1
			printf("Encoder: %-15s | %s\n", codec->name,
			    codec->long_name ? codec->long_name
					     : "no long name");
#else
			if (strstr(codec->name, "26") ||
			    strstr(codec->long_name, "26")) {
				printf("Encoder: %-15s | %s\n", codec->name,
				    codec->long_name ? codec->long_name
						     : "no long name");
#endif
		}
	}

	return 0;
}

int
get_encoder(char *remind)
{
	const AVCodec *codec = NULL;
	void *iter = NULL;

	while ((codec = av_codec_iterate(&iter))) {
		if (av_codec_is_encoder(codec)) {
#if 0
			printf("Encoder: %-15s | %s\n", codec->name,
			    codec->long_name ? codec->long_name
					     : "no long name");
#else
			if (strstr(codec->name, remind) ||
			    strstr(codec->long_name, remind)) {
				printf("Encoder: %-15s | %s\n", codec->name,
				    codec->long_name ? codec->long_name
						     : "no long name");
#endif
			}
		}
	}

	return 0;
}


int main(int argc, char* argv[]) {
	int ret = 0;
#ifndef UNIX_LINUX	
    const char *device_name = "video=Integrated Webcam"; 
    const char *input_format_name = "dshow"; 
#else
    const char *device_name = "/dev/video0"; 
    const char *input_format_name = "dshow"; 
#endif
    
    avdevice_register_all();
    avformat_network_init();
    get_all_encoders();
    get_encoder("pus");

    AVInputFormat *input_fmt = av_find_input_format(input_format_name);
    if (!input_fmt) {
        fprintf(stderr, "Cannot see the input format '%s'\n", input_format_name);
        return 1;
    }

    AVFormatContext *fmt_ctx = NULL;
    AVDictionary *options = NULL;
    av_dict_set(&options, "framerate", "30", 0);
    av_dict_set(&options, "video_size", "640x480", 0);

    if (avformat_open_input(&fmt_ctx, device_name, input_fmt, &options) < 0) {
        fprintf(stderr, "Cannot open device %s\n", device_name);
        return 1;
    }

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Cannot get info of the stream\n");
        avformat_close_input(&fmt_ctx);
        return 1;
    }

    // Find the first stream video
    int video_index = -1;
    for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            break;
        }
    }
    if (video_index < 0) {
        fprintf(stderr, "Cannnot find the stream\n");
        avformat_close_input(&fmt_ctx);
        return 1;
    }

    //Prepare codec to decode
    AVCodecParameters *codecpar = fmt_ctx->streams[video_index]->codecpar;
    AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        fprintf(stderr, "Không tìm thấy codec phù hợp\n");
        avformat_close_input(&fmt_ctx);
        return 1;
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codecpar);
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Không thể mở codec\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return 1;
    }

    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) 
            {
			    ret = avcodec_receive_frame(codec_ctx, frame);
                while (ret == 0) {
                #if 0
                    printf("Frame %d: type=%c, size=%d bytes\n",
                           codec_ctx->frame_number,
                           av_get_picture_type_char(frame->pict_type),
                           frame->pkt_size);
                    av_frame_unref(frame);
                #endif
                }
            }
        }
        av_packet_unref(pkt);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    return 0;
}
