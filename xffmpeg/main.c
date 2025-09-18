#include <stdio.h>
#include <stdlib.h>

#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

int save_frame_to_mp4(AVFrame *frame, const char *filename) ;
//int save_frame_to_mp4(AVFrame *frame, const char *filename) ;
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
    const AVInputFormat *ifmt = NULL;
    while ((ifmt = av_input_video_device_next(ifmt))) {
        printf("Video Input format: %s (%s)\n", ifmt->name, ifmt->long_name);
    }    
    fprintf(stdout , "\n\n");
    while ((ifmt = av_input_audio_device_next(ifmt))) {
        printf("Audio Input format: %s (%s)\n", ifmt->name, ifmt->long_name);
    }        
    //return 1;
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

    AVFormatContext *audio_ctx = NULL;
    AVInputFormat *audio_input_fmt = av_find_input_format("alsa");
    const char *audio_device = "default";

    if (avformat_open_input(&audio_ctx, audio_device, audio_input_fmt, NULL) < 0) {
        fprintf(stderr, "Cannot open audio device\n");
        return -1;
    }
 
    if (avformat_find_stream_info(audio_ctx, NULL) < 0) {
        fprintf(stderr, "Cannot get info of the audio stream\n");
        avformat_close_input(&audio_ctx);
        return 1;
    }
       
    // Find the first stream video
    int video_index = -1;
    int audio_index = -1;
    for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            AVCodec *video = fmt_ctx->video_codec;
            if(video) {
                fprintf(stdout, "video codec name: %s\n", video->name);
            }
            break;
        }     
    }
    for (unsigned i = 0; i < audio_ctx->nb_streams; i++) {

        if (audio_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_index = i;
            AVCodec *audio = audio_ctx->audio_codec;
            if(audio) {
                fprintf(stdout, "audio codec name: %s\n", audio->name);
            }
            break;
        }        
    }    
    if (video_index < 0) {
        fprintf(stderr, "Cannnot find the video stream\n");
        avformat_close_input(&fmt_ctx);
        return 1;
    }
    if (audio_index < 0) {
        fprintf(stderr, "Cannnot find the audio stream\n");
        avformat_close_input(&fmt_ctx);
        return 1;
    }
    //Prepare codec to decode
    AVCodecParameters *codecpar = fmt_ctx->streams[video_index]->codecpar;
    AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        fprintf(stderr, "Not found\n");
        avformat_close_input(&fmt_ctx);
        return 1;
    }

     //Prepare codec to decode audio
    AVCodecParameters *codecpar_audio = audio_ctx->streams[audio_index]->codecpar;
    AVCodec *codec_audio = avcodec_find_decoder(codecpar_audio->codec_id);
    if (!codec_audio) {
        fprintf(stderr, "Not found\n");
        avformat_close_input(&audio_ctx);
        return 1;
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codecpar);
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Cannot open codec\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return 1;
    }

    AVCodecContext *audio_codec_ctx = avcodec_alloc_context3(codec_audio);
    avcodec_parameters_to_context(audio_codec_ctx, codecpar_audio);
    if (avcodec_open2(audio_codec_ctx, codec_audio, NULL) < 0) {
        fprintf(stderr, "Cannot open codec\n");
        avcodec_free_context(&audio_codec_ctx);
        avformat_close_input(&audio_ctx);
        return 1;
    }    

    AVPacket *pkt = av_packet_alloc();
    AVPacket *audio_pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    int videoread = -1, audioread = -1;
    while (1) {
    #if 1  
        videoread = av_read_frame(fmt_ctx, pkt);
        if (videoread >= 0 && pkt->stream_index == video_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) 
            {
                //avcodec_receive_packet
                //fmt_ctx->pb
                //avio_alloc_context
			    ret = avcodec_receive_frame(codec_ctx, frame);
                while (ret == 0) {
                #if 0
                    printf("Frame %d: type=%c, size=%d bytes\n",
                           codec_ctx->frame_number,
                           av_get_picture_type_char(frame->pict_type),
                           frame->pkt_size);
                    av_frame_unref(frame);
                #else
                    //AVSampleFormat a = -1; //unknown
                    int pkt_size = frame->pkt_size;
                    int fmt = frame->format;
                    int nb_samples = frame->nb_samples;
                    int mytype = 0;
                    mytype = codec_ctx->codec_type;
                    fprintf(stdout, "nb_side_data: %d, fm: %d, nb_samples: %d, mytype: %d, sze: %d\n", 
                        frame->nb_side_data, fmt, nb_samples, mytype, pkt_size);
                    av_frame_unref(frame);
                #endif
                }
            }
        }

        av_packet_unref(pkt);
#else
        audioread = av_read_frame(audio_ctx, audio_pkt);
        int stream_index = audio_pkt->stream_index ;
        if (audioread >= 0 && stream_index == audio_index) {
            stream_index = avcodec_send_packet(audio_codec_ctx, audio_pkt);
            if (stream_index == 0) 
            {
			    ret = avcodec_receive_frame(audio_codec_ctx, frame);
                while (ret == 0) {
                #if 0
                    printf("Frame %d: type=%c, size=%d bytes\n",
                           codec_ctx->frame_number,
                           av_get_picture_type_char(frame->pict_type),
                           frame->pkt_size);
                    av_frame_unref(frame);
                #else
                    //AVSampleFormat a = -1; //unknown
                    int fmt = frame->format;
                    int nb_samples = frame->nb_samples;
                    int mytype = 0;
                    mytype = audio_codec_ctx->codec_type;
                    fprintf(stdout, "nb_side_data: %d, fmt: %d, nb_samples: %d, mytype: %d\n", 
                        frame->nb_side_data, fmt, nb_samples, mytype);
                    av_frame_unref(frame);                    
#if 0                    
                    enum AVSampleFormat {
    AV_SAMPLE_FMT_NONE = -1,
    AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
    AV_SAMPLE_FMT_S32,         ///< signed 32 bits
    AV_SAMPLE_FMT_FLT,         ///< float
    AV_SAMPLE_FMT_DBL,         ///< double

    AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    AV_SAMPLE_FMT_FLTP,        ///< float, planar
    AV_SAMPLE_FMT_DBLP,        ///< double, planar
    AV_SAMPLE_FMT_S64,         ///< signed 64 bits
    AV_SAMPLE_FMT_S64P,        ///< signed 64 bits, planar

    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};
enum AVMediaType {
    AVMEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
    AVMEDIA_TYPE_VIDEO,
    AVMEDIA_TYPE_AUDIO,
    AVMEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
    AVMEDIA_TYPE_SUBTITLE,
    AVMEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
    AVMEDIA_TYPE_NB
};

#endif

                #endif
                }
            }
        }
        av_packet_unref(pkt);
#endif
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
    //avio_alloc_context
    //init_push_encoder

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

#if 0
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

// Global static context (only for demo, thread-unsafe)
static AVFormatContext *format_ctx = NULL;
static AVCodecContext  *codec_ctx = NULL;
static AVStream        *video_stream = NULL;
static AVIOContext     *avio_ctx = NULL;
static uint8_t         *avio_buffer = NULL;
static FILE            *output_file = NULL;
static int64_t         frame_pts = 0;

static int write_packet(void *opaque, uint8_t *buf, int buf_size) {
    FILE *f = (FILE *)opaque;
    return fwrite(buf, 1, buf_size, f);
}

int init_push_encoder(FILE *f, int width, int height) {
    output_file = f;

    // Allocate format context
    avformat_alloc_output_context2(&format_ctx, NULL, "mp4", NULL);
    if (!format_ctx) {
        fprintf(stderr, "Could not create format context\n");
        return -1;
    }

    // Allocate AVIOContext to write to FILE*
    avio_buffer = av_malloc(4096);
    avio_ctx = avio_alloc_context(avio_buffer, 4096, 1, f, NULL, write_packet, NULL);
    format_ctx->pb = avio_ctx;
    format_ctx->flags |= AVFMT_FLAG_CUSTOM_IO;

    // Find encoder
    const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "H264 codec not found\n");
        return -1;
    }

    // Create new video stream
    video_stream = avformat_new_stream(format_ctx, NULL);
    if (!video_stream) {
        fprintf(stderr, "Could not create stream\n");
        return -1;
    }

    codec_ctx = avcodec_alloc_context3(codec);
    codec_ctx->codec_id = AV_CODEC_ID_H264;
    codec_ctx->bit_rate = 400000;
    codec_ctx->width = width;
    codec_ctx->height = height;
    codec_ctx->time_base = (AVRational){1, 25};
    codec_ctx->framerate = (AVRational){25, 1};
    codec_ctx->gop_size = 10;
    codec_ctx->max_b_frames = 1;
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (format_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return -1;
    }

    avcodec_parameters_from_context(video_stream->codecpar, codec_ctx);
    video_stream->time_base = codec_ctx->time_base;

    // Write MP4 header
    if (avformat_write_header(format_ctx, NULL) < 0) {
        fprintf(stderr, "Error writing header\n");
        return -1;
    }

    return 0;
}

int push_avframe(AVFrame *frame, FILE *f) {
    if (!format_ctx || !codec_ctx) {
        fprintf(stderr, "Encoder not initialized. Call init_push_encoder() first.\n");
        return -1;
    }

    // Set proper PTS
    frame->pts = frame_pts++;

    // Send frame to encoder
    int ret = avcodec_send_frame(codec_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending frame to encoder\n");
        return -1;
    }

    // Receive packet(s)
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    while (avcodec_receive_packet(codec_ctx, &pkt) == 0) {
        pkt.stream_index = video_stream->index;
        av_interleaved_write_frame(format_ctx, &pkt);
        av_packet_unref(&pkt);
    }

    return 0;
}

void flush_and_close_encoder() {
    if (!codec_ctx || !format_ctx) return;

    // Flush encoder
    avcodec_send_frame(codec_ctx, NULL);
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    while (avcodec_receive_packet(codec_ctx, &pkt) == 0) {
        pkt.stream_index = video_stream->index;
        av_interleaved_write_frame(format_ctx, &pkt);
        av_packet_unref(&pkt);
    }

    // Write trailer
    av_write_trailer(format_ctx);

    // Cleanup
    avcodec_free_context(&codec_ctx);
    avformat_free_context(format_ctx);
    av_free(avio_buffer);
    avio_context_free(&avio_ctx);

    codec_ctx = NULL;
    format_ctx = NULL;
    video_stream = NULL;
    avio_ctx = NULL;
    avio_buffer = NULL;
    frame_pts = 0;
}
    //Cách sử dụng
FILE *f = fopen("output.mp4", "wb");

// Gọi 1 lần đầu
init_push_encoder(f, width, height);

// Lặp cho nhiều AVFrame
for (int i = 0; i < N; i++) {
    AVFrame *frame = create_frame_somehow(...);
    push_avframe(frame, f);
    av_frame_free(&frame);
}

// Gọi 1 lần cuối
flush_and_close_encoder();
fclose(f);

Input format: fbdev (Linux framebuffer)
Input format: kmsgrab (KMS screen capture)
Input format: lavfi (Libavfilter virtual input device)
Input format: video4linux2,v4l2 (Video4Linux2 device grab)
Input format: x11grab (X11 screen capture, using XCB)

--------------------------------------------------------------------------
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

typedef struct {
    AVFormatContext *fmt_ctx;
    AVStream *stream;
    AVCodecContext *codec_ctx;
    AVIOContext *avio_ctx;
    uint8_t *avio_buffer;
    FILE *file;
    int64_t pts;
} MP4Writer;

int mp4_writer_init(MP4Writer *w, FILE *fp, int width, int height) {
    memset(w, 0, sizeof(*w));
    w->file = fp;
    w->pts = 0;

    // Allocate format context
    avformat_alloc_output_context2(&w->fmt_ctx, NULL, "mp4", NULL);
    if (!w->fmt_ctx) return -1;

    // Setup AVIO to write into FILE*
    w->avio_buffer = av_malloc(4096);
    w->avio_ctx = avio_alloc_context(w->avio_buffer, 4096, 1, fp, NULL, [](void *opaque, uint8_t *buf, int buf_size) {
        return fwrite(buf, 1, buf_size, (FILE *)opaque);
    }, NULL);
    w->fmt_ctx->pb = w->avio_ctx;
    w->fmt_ctx->flags |= AVFMT_FLAG_CUSTOM_IO;

    // Find encoder
    const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) return -1;

    // Create stream
    w->stream = avformat_new_stream(w->fmt_ctx, NULL);
    if (!w->stream) return -1;

    // Create codec context
    w->codec_ctx = avcodec_alloc_context3(codec);
    w->codec_ctx->codec_id = AV_CODEC_ID_H264;
    w->codec_ctx->bit_rate = 400000;
    w->codec_ctx->width = width;
    w->codec_ctx->height = height;
    w->codec_ctx->time_base = (AVRational){1, 25};
    w->codec_ctx->framerate = (AVRational){25, 1};
    w->codec_ctx->gop_size = 12;
    w->codec_ctx->max_b_frames = 2;
    w->codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (w->fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        w->codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // Open encoder
    if (avcodec_open2(w->codec_ctx, codec, NULL) < 0) return -1;

    avcodec_parameters_from_context(w->stream->codecpar, w->codec_ctx);
    w->stream->time_base = w->codec_ctx->time_base;

    // Write MP4 header
    if (avformat_write_header(w->fmt_ctx, NULL) < 0) return -1;

    return 0;
}

int mp4_writer_push_frame(MP4Writer *w, AVFrame *frame) {
    frame->pts = w->pts++;

    if (avcodec_send_frame(w->codec_ctx, frame) < 0)
        return -1;

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    while (avcodec_receive_packet(w->codec_ctx, &pkt) == 0) {
        pkt.stream_index = w->stream->index;
        pkt.duration = 1;
        av_interleaved_write_frame(w->fmt_ctx, &pkt);
        av_packet_unref(&pkt);
    }

    return 0;
}

void mp4_writer_close(MP4Writer *w) {
    // Flush encoder
    avcodec_send_frame(w->codec_ctx, NULL);
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    while (avcodec_receive_packet(w->codec_ctx, &pkt) == 0) {
        pkt.stream_index = w->stream->index;
        av_interleaved_write_frame(w->fmt_ctx, &pkt);
        av_packet_unref(&pkt);
    }

    av_write_trailer(w->fmt_ctx);

    avcodec_free_context(&w->codec_ctx);
    avformat_free_context(w->fmt_ctx);
    av_free(w->avio_buffer);
    avio_context_free(&w->avio_ctx);
}

#endif