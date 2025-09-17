#include <stdio.h>
#include <stdlib.h>

#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

int save_frame_to_mp4(AVFrame *frame, const char *filename) ;

#define UNIX_LINUX

int main(int argc, char* argv[]) {
	int ret = 0;
#ifndef UNIX_LINUX	
    const char *device_name = "video=Integrated Webcam"; 
    const char *input_format_name = "dshow"; 
#else
    const char *device_name = "/dev/video0"; 
    const char *input_format_name = "v4l2"; 
#endif
    
    avdevice_register_all();
    avformat_network_init();
    //get_all_encoders();
    //get_encoder("pus");
    

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
                #else
                    fprintf(stdout, "nb_side_data: %d\n", frame->nb_side_data);
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

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

int save_frame_to_mp4(AVFrame *frame, const char *filename) {
    AVFormatContext *fmt_ctx = NULL;
    AVStream *stream = NULL;
    AVCodecContext *codec_ctx = NULL;
    AVCodec *codec = NULL;
    AVPacket *pkt = NULL;
    int ret;

    // 1. Allocate format context (MP4)
    avformat_alloc_output_context2(&fmt_ctx, NULL, NULL, filename);
    if (!fmt_ctx) {
        fprintf(stderr, "Could not allocate format context\n");
        return -1;
    }

    // 2. Find encoder (H.264)
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        return -1;
    }

    // 3. Create new stream
    stream = avformat_new_stream(fmt_ctx, NULL);
    if (!stream) {
        fprintf(stderr, "Could not create stream\n");
        return -1;
    }

    // 4. Allocate and configure codec context
    codec_ctx = avcodec_alloc_context3(codec);
    codec_ctx->codec_id = AV_CODEC_ID_H264;
    codec_ctx->bit_rate = 400000;
    codec_ctx->width = frame->width;
    codec_ctx->height = frame->height;
    codec_ctx->time_base = (AVRational){1, 25};
    codec_ctx->framerate = (AVRational){25, 1};
    codec_ctx->gop_size = 10;
    codec_ctx->max_b_frames = 1;
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // 5. Open codec
    if ((ret = avcodec_open2(codec_ctx, codec, NULL)) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return -1;
    }

    // 6. Copy codec params to stream
    ret = avcodec_parameters_from_context(stream->codecpar, codec_ctx);

    // 7. Open output file
    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE)) < 0) {
            fprintf(stderr, "Could not open output file\n");
            return -1;
        }
    }

    // 8. Write header
    avformat_write_header(fmt_ctx, NULL);

    // 9. Encode the frame
    pkt = av_packet_alloc();
    if (!pkt) return -1;

    frame->pts = 0; // Very important

    avcodec_send_frame(codec_ctx, frame);
    while (avcodec_receive_packet(codec_ctx, pkt) == 0) {
        pkt->stream_index = stream->index;
        pkt->duration = 1;
        av_interleaved_write_frame(fmt_ctx, pkt);
        av_packet_unref(pkt);
    }

    // 10. Flush encoder
    avcodec_send_frame(codec_ctx, NULL);
    while (avcodec_receive_packet(codec_ctx, pkt) == 0) {
        pkt->stream_index = stream->index;
        av_interleaved_write_frame(fmt_ctx, pkt);
        av_packet_unref(pkt);
    }

    // 11. Write trailer and cleanup
    av_write_trailer(fmt_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_free_context(fmt_ctx);
    av_packet_free(&pkt);

    return 0;
}
