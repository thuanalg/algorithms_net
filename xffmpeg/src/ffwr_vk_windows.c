/*#define UNIX_LINUX*/
#define UNIX_LINUX

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <stdio.h>
#include <simplelog.h>
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <simplelog.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <pthread.h>


#ifndef UNIX_LINUX
#include <windows.h>
HWND gb_sdlWindow = 0;
#else
void *gb_sdlWindow = 0;
#endif 
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
#define MEMORY_PADDING      2
#define FFWR_BUFF_SIZE      12000000
#define FFWR_OUTPUT_ARATE   48000
#define FFWR_AUDIO_BUF          (1024 * 1024 * 2)
#define ffwr_malloc(__nn__, __obj__, __type__)                                 \
	{                                                                      \
		(__obj__) = (__type__ *)malloc(__nn__);                        \
		if (__obj__) {                                                 \
			spllog(1, "[ffwr-MEM] Malloc: 0x%p.", (__obj__));           \
			memset((__obj__), 0, (__nn__));                        \
		} else {                                                       \
			spllog(0, "Malloc: error.");                           \
		}                                                              \
	}
#define FFWR_MIN(__a__, __b__)  ((__a__) < (__b__)) ? (__a__) : (__b__)
#define ffwr_free(__obj__)                                                     \
	{                                                                      \
		if (__obj__) {                                                 \
			spllog(1, "[ffwr-MEM] Free: 0x%p.", (__obj__));             \
			free(__obj__);                                         \
			(__obj__) = 0;                                         \
		}                                                              \
	}
#define ffwr_frame_unref(__fr__) if(__fr__) {av_frame_unref(__fr__);}
#define ffwr_frame_free(__fr__) if(__fr__) {av_frame_free(__fr__);}
#define ffwr_packet_unref(__pkt__) if(__pkt__) {av_packet_unref(__pkt__);}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
typedef enum {
    FFWR_DTYPE_VFRAME,
    FFWR_DTYPE_PACKET,

    FFWR_DTYPE_END
} FFWR_DATA_TYPE_E;

typedef struct __FFWR_GENERIC_DATA__ {
	int total; /*Total size*/
	int range; /*data range*/
	int pc; /*Point to the current*/
	int pl; /*Point to the last*/
	char data[0]; /*Generic data */
} ffwr_gen_data_st;

#define ffwr_araw_stream                ffwr_gen_data_st

typedef struct {
    int total;
    int type;    
} FFWR_SIZE_TYPE;

typedef struct __FFWR_VFrame__ {
    FFWR_SIZE_TYPE tt_sz;
    int w;
    int h;
    int fmt;
    int pts;
    int linesize[AV_NUM_DATA_POINTERS];
    int pos[AV_NUM_DATA_POINTERS + 1];   
    int len[AV_NUM_DATA_POINTERS + 1]; 
    uint8_t data[0];
} FFWR_VFrame;

#ifndef __FFWR_INSTREAM_DEF__
#define __FFWR_INSTREAM_DEF__


typedef struct __FFWR_INSTREAM__ {
    AVFormatContext *fmt_ctx;

    AVStream *v_st;
    AVCodec *v_codec;
    AVCodecContext *v_cctx;
    AVPacket pkt;
    AVFrame *vframe;
    SwsContext *vscale;
    

    AVStream *a_st;
    AVCodec *a_codec;
    AVCodecContext *a_cctx;
    AVPacket a_pkt;
    AVFrame *a_frame;
    AVFrame *a_dstframe;
    SwrContext *a_scale;    

} FFWR_INSTREAM;
#endif
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
struct SwrContext *gb_aConvertContext;
AVFrame *gb_dst_draw;
AVFrame *gb_src_draw;
pthread_mutex_t gb_FRAME_MTX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gb_AFRAME_MTX = PTHREAD_MUTEX_INITIALIZER;
FFWR_INSTREAM gb_instream;
FFWR_VFrame *gb_transfer_avframe = 0;
ffwr_gen_data_st *gb_frame;
ffwr_gen_data_st *gb_tsplanVFrame;
int scan_all_pmts_set;
ffwr_araw_stream *gb_shared_astream;
ffwr_araw_stream *gb_in_astream;
SDL_AudioSpec gb_want;
char *gb_input_fmt;
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
static void set_sdl_yuv_conversion_mode(AVFrame *frame);
int ffwr_fill_vframe(FFWR_VFrame *dst, AVFrame *src);
int ffwr_update_vframe(FFWR_VFrame **dst, AVFrame *src);
int ffwr_create_rawvframe(FFWR_VFrame **dst, AVFrame *src);
int ffwr_convert_vframe(AVFrame *src, AVFrame *dst);
int ffwr_open_input(FFWR_INSTREAM *pinput, char *name, int mode);
int ffwr_create_a_swrContext(AVFrame *src, AVFrame *dst);
int convert_audio_frame( AVFrame *src, AVFrame **outfr);
int fwr_open_audio_output(int sz);
int fwr_clode_audio_output();
void ffwr_clear_gb_var();
int init_gen_buff(ffwr_gen_data_st *obj, int sz);
int ffwr_set_running(int v);
int ffwr_get_running();
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

int ffwr_open_input(FFWR_INSTREAM *pinput, char *name, int mode) 
{
    int ret = 0;
    int result = 0;
    AVInputFormat *iformat = 0; 
    AVDictionary *options = 0;
    do {
        if(!pinput) {
            ret = 1;
            spllog(4, "--");
            break;
        }

    if (!av_dict_get(options, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
        av_dict_set(&options, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
        scan_all_pmts_set = 1;
    }        
    

        result = avformat_open_input(&(pinput->fmt_ctx), name,  iformat, &options);

        if(result < 0) {
            ret = 1;
            spllog(4, "--");
            break;
        }


        result = avformat_find_stream_info(pinput->fmt_ctx, 0);
        if(result < 0) {
            ret = 1;
            spllog(4, "--");
            break;
        }
        if(pinput->fmt_ctx->nb_streams < 1) {
            ret = 1;
            spllog(4, "--");
            break;
        }
        pinput->v_st = pinput->fmt_ctx->streams[0];
        if(!pinput->v_st) {
            ret = 1;
            spllog(4, "--");
            break;
        }
#if 1        
        pinput->v_codec = avcodec_find_decoder(
            pinput->v_st->codecpar->codec_id);
        if(!pinput->v_codec) {
            ret = 1;
            spllog(4, "--");
            break;
        }        
        pinput->v_cctx  = avcodec_alloc_context3(pinput->v_codec);
        if(!pinput->v_cctx) {
            ret = 1;
            spllog(4, "--");
            break;
        }    
        result = avcodec_parameters_to_context(pinput->v_cctx, pinput->v_st->codecpar);
        if(result < 0) {
            ret = 1;
            spllog(4, "--");
            break;
        }
		result = avcodec_open2(pinput->v_cctx, pinput->v_codec, 0);
		if (result < 0) {
            spllog(4, "avcodec_open2, result: %d", result);
			break;
		}        
#endif            
        pinput->vframe = av_frame_alloc(); 
        if(!pinput->vframe) {
            ret = 1;
            spllog(4, "--");
            break;
        }
            
        /*------------------------------*/

        if(pinput->fmt_ctx->nb_streams > 1) {
            pinput->a_st = pinput->fmt_ctx->streams[1];
        }
        if(!pinput->a_st) {
            ret = 1;
            spllog(1, "---");
            break;
        }
#if 1        
        pinput->a_codec = avcodec_find_decoder(
            pinput->a_st->codecpar->codec_id);

        if(!pinput->a_codec) {
            ret = 1;
            spllog(4, "--");
            break;
        }      
        pinput->a_cctx  = avcodec_alloc_context3(pinput->a_codec);
        if(!pinput->a_cctx) {
            ret = 1;
            spllog(4, "--a_cctx");
            break;
        }   
        result = avcodec_parameters_to_context(pinput->a_cctx, pinput->a_st->codecpar);
        if(result < 0) {
            ret = 1;
            spllog(4, "--");
            break;
        }   
		result = avcodec_open2(pinput->a_cctx, pinput->a_codec, 0);
		if (result < 0) {
            spllog(4, "--");
			break;
		} 
        pinput->a_frame = av_frame_alloc(); 
        if(!pinput->a_frame) {
            ret = 1;
            spllog(4, "--");
            break;
        }   
#endif               
        pinput->a_dstframe = av_frame_alloc(); 
        if(!pinput->a_dstframe) {
            ret = 1;
            spllog(4, "--");
            break;
        }   
           
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
void *demux_routine(void *arg);
#ifndef UNIX_LINUX
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main(int argc, char *argv[])
#endif
{	
	int ret = 0;
	SDL_SysWMinfo info = {0};
	SDL_Window *win = 0;
	int running = 1;
	SDL_Event e = {0};
    pthread_t thread_id = 0;
    FFWR_VFrame *p = 0;
    SDL_Texture *gb_texture = NULL;
    FFWR_SIZE_TYPE *it = 0;
     int step = 0;
	
	char cfgpath[1024] = {0};
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	snprintf(input.id_name, 100, "vk_window");
	ret = spl_init_log_ext(&input);
#ifndef UNIX_LINUX
    if(argc > 1)
#else
    if(argc > 1) {
        gb_input_fmt = argv[1];
    }
#endif
    if(ret) {
        exit(1);
    }    
    avdevice_register_all();

    //gb_tsplanVFrame = malloc(FFWR_BUFF_SIZE);
	ffwr_malloc(FFWR_BUFF_SIZE, gb_tsplanVFrame, ffwr_gen_data_st);
    if(!gb_tsplanVFrame) {
        exit(1);
    }
    //memset(gb_tsplanVFrame, 0, FFWR_BUFF_SIZE);
    gb_tsplanVFrame->total = FFWR_BUFF_SIZE;
    gb_tsplanVFrame->range = gb_tsplanVFrame->total -sizeof(ffwr_gen_data_st);

    //gb_frame = malloc(FFWR_BUFF_SIZE);
    ffwr_malloc(FFWR_BUFF_SIZE, gb_frame, ffwr_gen_data_st);
    if(!gb_frame) {
        exit(1);
    }
    //memset(gb_frame, 0, FFWR_BUFF_SIZE);

    gb_frame->total = FFWR_BUFF_SIZE;
    gb_frame->range = gb_frame->total -sizeof(ffwr_gen_data_st);    

	if(ret) {
		exit(1);
	}

    ret = avformat_network_init();

	ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret) {
        spllog(4, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    fwr_open_audio_output( 2000000);

    ret = pthread_create(
        &thread_id, 0, demux_routine, 0);
#if 0
    win = SDL_CreateWindow(
        "SDL2 Window",            // title
        SDL_WINDOWPOS_UNDEFINED,   // x
        SDL_WINDOWPOS_UNDEFINED,   // y
        640,                      // width
        480,                      // height
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN         // flags
    );
    //SDL_WINDOW_VULKAN
#else
    win = SDL_CreateWindow(
        "SDL2 Window",            // title
        SDL_WINDOWPOS_UNDEFINED,   // x
        SDL_WINDOWPOS_UNDEFINED,   // y
        640,                      // width
        480,                      // height
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE       // flags
    );
#endif    
	SDL_GetWindowWMInfo(win, &info);
#ifndef UNIX_LINUX    
	gb_sdlWindow = info.info.win.window;
#else
    /*Wyland*/
    gb_sdlWindow = info.info.wl.egl_window;
#endif    
    if (!win) {
        spllog(4, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren) {
        spllog(4, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    gb_dst_draw = av_frame_alloc();

    gb_dst_draw->width = 640;
    gb_dst_draw->height = 480;
    gb_dst_draw->format = 0;
    gb_dst_draw->pts = 0;

    av_frame_get_buffer(gb_dst_draw, 32);
    
    gb_src_draw = av_frame_alloc();

    gb_src_draw->width = 640;
    gb_src_draw->height = 480;
    gb_src_draw->format = 0;
    gb_src_draw->pts = 0;

    av_frame_get_buffer(gb_src_draw, 32);    

    
    while (running) {
        it = 0;
        p = 0;
        
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        if(gb_frame->pl < 1) 
        {
            pthread_mutex_lock(&gb_FRAME_MTX);
            do {           
                if (gb_tsplanVFrame->pl <= gb_tsplanVFrame->pc)
                {
                    break;
                }          
                if(step < 1) 
                {
                    if(gb_tsplanVFrame->pl > 0) 
                    {
                        step++;
                        gb_frame->pl = gb_frame->pc = 0;
                        gb_tsplanVFrame->pl = gb_tsplanVFrame->pc = 0;
                        break;
                    }
                }               
                memcpy(gb_frame->data + gb_frame->pl, 
                    gb_tsplanVFrame->data + gb_tsplanVFrame->pc,
                    gb_tsplanVFrame->pl - gb_tsplanVFrame->pc);

                gb_frame->pl += gb_tsplanVFrame->pl - gb_tsplanVFrame->pc;
                gb_tsplanVFrame->pl = gb_tsplanVFrame->pc = 0;

            } while(0);
            pthread_mutex_unlock(&gb_FRAME_MTX);
        }


        if(gb_frame->pl <= gb_frame->pc) 
        {
            gb_frame->pl = gb_frame->pc = 0;
            SDL_Delay(10);
            continue;
        } 

        it = (FFWR_SIZE_TYPE *) (gb_frame->data + gb_frame->pc);
        p = (FFWR_VFrame *)(gb_frame->data + gb_frame->pc);
       
        if(!gb_texture) {
            gb_texture = SDL_CreateTexture( ren,
                SDL_PIXELFORMAT_IYUV,
                SDL_TEXTUREACCESS_STREAMING,
                640, 480);
        }
        if(!gb_texture) {
            exit(1);
        }
        
        SDL_UpdateYUVTexture( gb_texture, NULL,
            p->data + p->pos[0], p->linesize[0],
            p->data + p->pos[1], p->linesize[1],
            p->data + p->pos[2], p->linesize[2]
           
        );
      
        spllog(1, "pc render: %d, pts: %d", gb_frame->pc, p->pts);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, gb_texture, NULL, NULL);
        SDL_RenderPresent(ren);
        if(it) {
            gb_frame->pc += it->total;
        }
        SDL_Delay(30);
    }
    ffwr_set_running(0);
    SDL_Delay(100);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    fwr_clode_audio_output();
    ffwr_clear_gb_var();
	spl_finish_log();
    return 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
void *demux_routine(void *arg) {
    int ret = 0;
    int result = 0;
    AVFrame *tmp = 0;
    FFWR_VFrame *ffwr_vframe = 0;
    int runnung = 0;
    
    ret = ffwr_open_input(&gb_instream, 
        gb_input_fmt, 0);
    if(ret) {
        return 0;
    }

    tmp = av_frame_alloc();
    tmp->width = 640;
    tmp->height = 480;
    tmp->format = 4;
    tmp->pts = 0;

    av_frame_get_buffer(tmp, 32);
    


    gb_instream.vframe->width = 640;
    gb_instream.vframe->height = 480;
    gb_instream.vframe->format = 4;
    gb_instream.vframe->pts = 0;
    av_frame_get_buffer(gb_instream.vframe, 32);       

    while(1) {
        runnung = ffwr_get_running();
        if(!runnung) {
            break;
        }
        av_packet_unref(&(gb_instream.pkt));
        result = av_read_frame(gb_instream.fmt_ctx, &(gb_instream.pkt)); 
        if(result) {
            continue;
        }
        if(gb_instream.pkt.stream_index == 0) {
            result = avcodec_send_packet(gb_instream.v_cctx, &(gb_instream.pkt));
            if(result < 0) {
                spllog(1, "v_cctx: 0x%p", gb_instream.v_cctx);
                break;
            }
		    result = avcodec_receive_frame(gb_instream.v_cctx, tmp);
		    if (result < 0) {
		    	break;
		    } 

            ffwr_convert_vframe(tmp, gb_instream.vframe);


            spl_vframe(gb_instream.vframe);
            if(!ffwr_vframe) {
                ffwr_create_rawvframe(&ffwr_vframe, gb_instream.vframe);
                if(!ffwr_vframe) {
                    break;
                }                  
            }
            else {
                ffwr_update_vframe(&ffwr_vframe, gb_instream.vframe);
            } 
            if(ffwr_vframe->tt_sz.total < 1) {
                break;
            }  
            if(!gb_tsplanVFrame) {
                break;
            }
            pthread_mutex_lock(&gb_FRAME_MTX);
            do {
                if(gb_tsplanVFrame->range > 
                    gb_tsplanVFrame->pl + ffwr_vframe->tt_sz.total) {                      
                    memcpy(gb_tsplanVFrame->data + gb_tsplanVFrame->pl, 
                        ffwr_vframe, 
                        ffwr_vframe->tt_sz.total);
                    gb_tsplanVFrame->pl += ffwr_vframe->tt_sz.total;
                    spllog(1, "gb_tsplanVFrame->pl: %d", 
                        gb_tsplanVFrame->pl);
                } else {
                    gb_tsplanVFrame->pl = 0;
                    gb_tsplanVFrame->pc = 0;
                }

            } while(0);
            pthread_mutex_unlock(&gb_FRAME_MTX);
            av_frame_unref(tmp);
            av_frame_unref(gb_instream.vframe);
        }   
        else if (gb_instream.pkt.stream_index == 1) {
            result = avcodec_send_packet(
                gb_instream.a_cctx, &(gb_instream.pkt));
            if(result < 0) {
                spllog(1, "v_cctx: 0x%p", gb_instream.a_cctx);
                break;
            }
		    result = avcodec_receive_frame(
                gb_instream.a_cctx, gb_instream.a_frame);
		    if (result < 0) {
		    	break;
		    }  
            convert_audio_frame(gb_instream.a_frame, 
                &(gb_instream.a_dstframe));
            pthread_mutex_lock(&gb_AFRAME_MTX);
            do {
                if(gb_shared_astream->range > 
                    gb_shared_astream->pl + 
                    gb_instream.a_dstframe->linesize[0]) 
                {
                    memcpy(gb_shared_astream->data + 
                            gb_shared_astream->pl, 
                        gb_instream.a_dstframe->data[0], 
                        gb_instream.a_dstframe->linesize[0]
                    );
                    gb_shared_astream->pl += 
                        gb_instream.a_dstframe->linesize[0];
                } else {
                    gb_shared_astream->pl = 0;
                    gb_shared_astream->pc = 0;
                    spllog(1, "over audio range");
                }
                spllog(1, "(pl, pc, range)=(%d, %d, %d)", 
                    gb_shared_astream->pl, 
                    gb_shared_astream->pc, 
                    gb_shared_astream->range);
            } while(0);
            pthread_mutex_unlock(&gb_AFRAME_MTX);
            spl_vframe(gb_instream.a_dstframe);
            av_frame_unref(gb_instream.a_dstframe); 
            av_frame_unref(gb_instream.a_frame);   
                     
        }    
    }

    if(gb_instream.vframe) {
        ffwr_frame_free(&(gb_instream.vframe));
        gb_instream.vframe = 0;
    }
    if(gb_instream.a_dstframe) {
        ffwr_frame_free(&(gb_instream.a_dstframe)); 
        gb_instream.a_dstframe = 0;
    }
    if(gb_instream.a_frame) {
        ffwr_frame_free(&(gb_instream.a_frame));   
        gb_instream.a_frame = 0;
    }
    if(tmp) {
        ffwr_frame_free(&tmp);
        tmp = 0;
    }
    
    ffwr_packet_unref(&(gb_instream.pkt));
    ffwr_free(ffwr_vframe);

    if(gb_instream.vscale) {
        sws_freeContext(gb_instream.vscale);
        gb_instream.vscale = 0;
    }
    if(gb_aConvertContext) {
        swr_free(&gb_aConvertContext);
        gb_aConvertContext = 0;
    }
    
    avcodec_free_context(&(gb_instream.v_cctx));
    gb_instream.v_cctx = 0;
    avcodec_free_context(&(gb_instream.a_cctx));
    gb_instream.a_cctx = 0;
    avformat_close_input(&(gb_instream.fmt_ctx));
    gb_instream.fmt_ctx = 0;
    
    return 0;
}



/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
ffwr_convert_vframe(AVFrame *src, AVFrame *dst)
{
    int ret = 0;
    //av_frame_get_buffer(src, 32);
    dst->format = 0;
    dst->width = src->width;
    dst->height = src->height;
    av_frame_get_buffer(dst, 32);
    if (!dst->data[0]) {
        ret = av_frame_get_buffer(dst, 32);
        if (ret < 0) {
            spllog(4, "Error allocating dst buffer\n");
            return ret;
        }
        spllog(1, "av_frame_get_buffer");
    }
    
    if(! (gb_instream.vscale)) {
        gb_instream.vscale = sws_getContext(
	    	src->width, 
	    	src->height, 
	    	src->format, 
	    	dst->width, 
	    	dst->height, 
	    	dst->format,
            SWS_BILINEAR, NULL, NULL, NULL
        );
    }
    if (!gb_instream.vscale) {
        spllog(4, "Error: cannot create sws context\n");
        return -1;
    }

    ret = sws_scale(
        gb_instream.vscale,
        (const uint8_t * const *)src->data,
        src->linesize,
        0,
        src->height,
        dst->data,
        dst->linesize
    );
    dst->pts = src->pts;

    return 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int ffwr_get_rawsize_vframe(AVFrame *src) {
    int ret = 0;
    int h = 0;
    int i = 0;
    if(!src) {
        return ret;
    }
    if(src->format == 0) {
        while(src->linesize[i]) {
            h = (i == 0) ? src->height : ((src->height)/2);
            ret += src->linesize[i] * (h + MEMORY_PADDING);
            spllog(1, "ret: %d", ret);
            ++i;
        }           
    }
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int ffwr_update_vframe(FFWR_VFrame **dst, AVFrame *src) {
    int ret = 0;
    int total = 0;
    int shouldupdate = 1;
    FFWR_VFrame *tmp = 0;
    do {
        if(!src) {
            ret = 1;
            break;
        }
        if(!dst) {
            ret = 1;
            break;
        } 
        if(! (*dst)) {
            ret = 1;
            break;
        }         
        tmp = *dst;
        total = ffwr_get_rawsize_vframe(src);
        total += sizeof(FFWR_VFrame);
        spllog(1, "check sz (total, tmp->tt_sz.total)=(%d, %d)",
            total, tmp->tt_sz.total);
        if(total != tmp->tt_sz.total) {
            break;
        }
        if(tmp->fmt != src->format) {
            break;            
        }
        if(tmp->w != src->width) {
            break;            
        }
        if(tmp->h != src->height) {
            break;            
        }        
        shouldupdate = 0;
    } while(0);

    if(ret) {
        return ret;
    }
    /*spllog(1, "shouldupdate: %d", shouldupdate);*/
    do {
        if(shouldupdate) {
            tmp = realloc(*dst, total);
            if(!tmp) {
                ret = 1;
                break;
            }
        }
        tmp->tt_sz.total = total;
        tmp->tt_sz.type = FFWR_DTYPE_VFRAME;
        ret = ffwr_fill_vframe(tmp, src);
        *dst = tmp;
    } while(0);

    return ret  = 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int ffwr_fill_vframe(FFWR_VFrame *dst, AVFrame *src) {
    int ret = 0;
    int k = 0;
    int m = 0;
    int i = 0;
    int pos = 0;
    do {
        if(!src) {
            ret = 1;
            break;
        }
        if(!dst) {
            ret = 1;
            break;
        }  
        dst->w = src->width;
        dst->h = src->height;
        dst->fmt = src->format;
        dst->pts = src->pts;
        i = 0;
        memset(dst->pos, 0, sizeof(dst->pos));
        memset(dst->len, 0, sizeof(dst->len));
        memset(dst->linesize, 0, sizeof(dst->linesize));
        while(src->linesize[i] && i < AV_NUM_DATA_POINTERS) {
            dst->linesize[i] = src->linesize[i];
            ++i;
        }              
        if(src->format == 0) {
            i = 0;
            while(src->linesize[i]) {
                dst->pos[i] = pos;
                k = src->linesize[i];
                m = (i == 0) ? src->height : ((src->height)/2);
                dst->len[i] = k * m;
                pos += k * (m + MEMORY_PADDING);
                memcpy(dst->data + dst->pos[i], 
                    src->data[i], dst->len[i]);
                ++i;
            }  
            i = 0;
            
        }
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int ffwr_create_rawvframe(FFWR_VFrame **dst, AVFrame *src) {
    int ret = 0;
    FFWR_VFrame *tmp = 0;
    int total = 0;
    int len = 0;
    int k = 0;
    int i = 0;
    do {
        if(!src) {
            ret = 1;
            break;
        }
        if(!dst) {
            ret = 1;
            break;
        }
        tmp = *dst;
        if(src->format == 0) {
            len = ffwr_get_rawsize_vframe(src);
            total = sizeof(FFWR_VFrame) + len;
            if(!tmp) {    
				ffwr_malloc(total, tmp, FFWR_VFrame);
                if(!tmp) {
                    ret = 1;
                    break;
                }

                tmp->tt_sz.total = total;
                tmp->tt_sz.type = FFWR_DTYPE_VFRAME;
                ffwr_fill_vframe(tmp, src);
            }
            *dst = tmp;
            break;
        }
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int convert_audio_frame( AVFrame *src, AVFrame **outfr)
{
    int ret = 0;
    AVChannelLayout *src_layout = 0;
    int n = 0;
    AVFrame *dst = 0;
    do {
        if(!src) {
            ret = 1;
            break;
        }    
        if(!outfr) {
            ret = 1;
            break;
        }
        dst = *outfr;
        if(!dst) {
            dst = av_frame_alloc();
            if(!dst) {
                break;
            }
          
        }

        av_channel_layout_default(&dst->ch_layout, 2);
        dst->format = AV_SAMPLE_FMT_FLT;
        dst->sample_rate = FFWR_OUTPUT_ARATE;  

        if(!gb_aConvertContext) {
            ret = ffwr_create_a_swrContext(src, dst);
            if(ret) {
                break;
            }
        }
        if(!gb_instream.a_cctx) {
            ret = 1;
            break;            
        }
        src_layout = &(gb_instream.a_cctx->ch_layout);
        if(!src_layout) {
            ret = 1;
            break;                    
        }

        dst->nb_samples = av_rescale_rnd(
            swr_get_delay(gb_aConvertContext, 
                src->sample_rate) + src->nb_samples, 
                FFWR_OUTPUT_ARATE, src->sample_rate, AV_ROUND_UP);

        n = av_frame_get_buffer(dst, 0);
	    if (n < 0) {
	    	spllog(4, "Error: cannot allocate dst buffer (%d)\n", n);
            ret = 1;
	    	break;
	    }
        av_channel_layout_copy(&(dst->ch_layout), src_layout);
	    n = swr_convert(gb_aConvertContext, dst->data, dst->nb_samples,
	        (const uint8_t **)src->data, src->nb_samples);
	    if (ret < 0) {
	    	spllog(4, " Error: swr_convert failed (%d)\n", ret);
            ret = 1;
	    	break;
	    }

	    dst->nb_samples = n;
	    spllog(1, "Audio convert done: %d samples -> %d samples\n",
	        src->nb_samples, dst->nb_samples);
        dst->pts = src->pts;
        *outfr = dst;

    } while(0);


	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/


int ffwr_create_a_swrContext(AVFrame *src, AVFrame *dst)
{
    int ret = 0;
    SwrContext *swr = 0;
    do {
        if(!src) {
            ret = 1;
            break;
        }
        if(!dst) {
            ret = 1;
            break;
        }        
        swr_alloc_set_opts2(
                &swr,                        // NULL → tạo mới
                &(dst->ch_layout),         // kênh đầu ra
                dst->format,          // định dạng sample đầu ra
                dst->sample_rate,                       // sample rate đầu ra
                &(src->ch_layout),           // kênh đầu vào
                src->format,           // định dạng sample đầu vào
                src->sample_rate,                       // sample rate đầu vào
                0, NULL                      // log offset, log context
            );        
        if(!swr) {
            ret = 1;
            break;
        }
        ret = swr_init(swr);
        if(ret < 0) {
            ret = 1;
            break;
        }

        gb_aConvertContext = swr;

    } while(0);
    return ret;
}
#if 0
Bước 1:
SDL_AudioSpec want;
want.freq = FFWR_OUTPUT_ARATE;           // sample rate output
want.format = AUDIO_F32SYS;  // float 32-bit
want.channels = 2;           // stereo
want.samples = 1024;         // buffer size
want.callback = audio_callback; // callback lấy dữ liệu
SDL_OpenAudio(&want, NULL);
want.userdata = &audio_data;  // <<< đây là chỗ truyền
SDL_PauseAudio(0); // start playback

Bước 2: Convert AVFrame sang định dạng SDL chấp nhận

Nếu input khác sample rate, channel, format, dùng SwrContext:

Tạo SwrContext với swr_alloc_set_opts2().

Chuyển đổi: swr_convert(output_frame, input_frame).

Output frame phải có:

Format đúng SDL (AUDIO_F32SYS → AV_SAMPLE_FMT_FLT)

Channel layout đúng (stereo)

Sample rate đúng (48000 Hz)

Bước 3: Đẩy dữ liệu vào SDL buffer

Trong audio_callback của SDL:

Copy out_frame->data[0] vào buffer.

Dùng linesize[0] để biết số byte cần copy.

SDL thread riêng sẽ play audio tự động, main thread vẫn có thể decode frame tiếp theo.
void audio_callback(void *userdata, Uint8 *stream, int len) {
    int copy_len = (len > audio_buf_size - audio_buf_index) ? 
                   audio_buf_size - audio_buf_index : len;
    memcpy(stream, audio_buf + audio_buf_index, copy_len);
    audio_buf_index += copy_len;
}


want.callback = audio_callback;
want.userdata = &audio_data; // buffer của bạn
SDL_OpenAudio(&want, NULL);
SDL_PauseAudio(0); // start playback

void audio_callback(void *userdata, Uint8 *stream, int len) {
    AudioData *ad = (AudioData*)userdata;
    
    if(ad->buf_index >= ad->buf_size) {
        // buffer hết → phát silence
        memset(stream, 0, len);
        return;
    }

    int copy_len = (len > ad->buf_size - ad->buf_index) ?
                   ad->buf_size - ad->buf_index : len;

    memcpy(stream, ad->buf + ad->buf_index, copy_len);
    ad->buf_index += copy_len;

    // nếu SDL muốn nhiều hơn còn lại trong buffer
    if(copy_len < len)
        memset(stream + copy_len, 0, len - copy_len);
}
-------------->>> AVFilterGraph
AVFilterGraph + các filter contexts + push/pop frame từ filtergraph.
SDL_PauseAudio(1); // stop audio
SDL_CloseAudio();  // close, free resources
#endif

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

void fwr_open_audio_output_cb(void *user, Uint8 * stream, int len)
{
    ffwr_gen_data_st *obj = (ffwr_gen_data_st*) user;
    int real_len = 0;
    static int step = 0;

    if(!obj) {
        return;
    }

    if( obj->pl <= obj->pc) 
    {
        obj->pl = obj->pc = 0;
        pthread_mutex_lock(&gb_AFRAME_MTX);
        do {
            if(gb_shared_astream->pl < 1) {
                break;
            }
            if(step == 0) {
                if(gb_shared_astream->pl > 0) {
                    step++;
                    gb_shared_astream->pc = 0;
                    gb_shared_astream->pl = 0;
                    break;
                }
            }
            if(gb_shared_astream->pl < 1) {
                break;
            }

            memcpy(obj->data + obj->pl, 
                gb_shared_astream->data, 
                gb_shared_astream->pl);

            obj->pl += gb_shared_astream->pl;

            gb_shared_astream->pc = 0;
            gb_shared_astream->pl = 0;
        } while(0);
        pthread_mutex_unlock(&gb_AFRAME_MTX);
    }
    

    real_len = FFWR_MIN(len, obj->pl - obj->pc);



    if(real_len < 1) {
        memset(stream , 0, len);
        return;
    }
    if (real_len < len) {
        memset(stream + real_len, 0, len - real_len);
    }
    memcpy(stream, obj->data + obj->pc, real_len);
    obj->pc += real_len;
 
    spllog(1, "(pl, pc, real_len, len)=(%d, %d, %d, %d)", 
        obj->pl, obj->pc, real_len, len);   
    if((obj->pc * 2) > obj->pl) {
        int tlen = obj->pl - obj->pc;
        if(tlen > 0) {
            memcpy(obj->data, 
                obj->data + obj->pc, tlen);
            obj->pc = 0;
            obj->pl = tlen;
            spllog(1, "(pl, pc, real_len, len)=(%d, %d, %d, %d)", 
                obj->pl, obj->pc, real_len, len); 
            pthread_mutex_lock(&gb_AFRAME_MTX);
            do {
                if(obj->range >= obj->pl + gb_shared_astream->pl) {
                    memcpy(obj->data + obj->pl, 
                        gb_shared_astream->data, 
                        gb_shared_astream->pl);

                    obj->pl += gb_shared_astream->pl;
                }

                gb_shared_astream->pc = 0;
                gb_shared_astream->pl = 0;
            } while(0);
            pthread_mutex_unlock(&gb_AFRAME_MTX);
        }
    }
    if(obj->pl > 800000 + obj->pc) {
        obj->pl = obj->pc = 0;
    }
}                                            
//ffwr_araw_stream *gb_shared_astream;
//ffwr_araw_stream *gb_shared_astream;
//ffwr_araw_stream *gb_in_astream;

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int fwr_open_audio_output(int sz)
{
    int ret = 0;
    int insz = 3 * sz;
    do {
        ffwr_malloc(sz, gb_shared_astream, ffwr_araw_stream);
        if(!gb_shared_astream) {
            ret = 1;
            break;
        }
        init_gen_buff(gb_shared_astream, sz);

        ffwr_malloc(insz, gb_in_astream, ffwr_araw_stream);
        if(!gb_in_astream) {
            ret = 1;
            break;
        }        
        init_gen_buff(gb_in_astream, insz);

        gb_want.freq = FFWR_OUTPUT_ARATE;
        gb_want.format = AUDIO_F32SYS;
        gb_want.channels = 2;
        gb_want.samples = 2048;        // kích thước buffer SDL
        gb_want.callback = fwr_open_audio_output_cb;
        gb_want.userdata = gb_in_astream; // buffer chuẩn hóa của bạn

        ret = SDL_OpenAudio(&gb_want, 0);
        spllog(1, "ret-SDL_OpenAudio: %d", ret);
        SDL_PauseAudio(0);           // start audio playback
    } while(0);
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int fwr_clode_audio_output() {
    int ret = 0;
    do {
        SDL_PauseAudio(1); // stop audio
        SDL_CloseAudio();  // close, free resources
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int init_gen_buff(ffwr_gen_data_st *obj, int sz) {
    int ret = 0;
    ffwr_gen_data_st *tmp = 0;
    do {
        if(!obj) {
            ret = 1;
            break;
        }
        obj->total = sz;
        obj->range = sz - sizeof(ffwr_gen_data_st);
        obj->pl = obj->pc = 0;
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
void ffwr_clear_gb_var() {
    ffwr_free(gb_tsplanVFrame);
    ffwr_free(gb_frame);
    ffwr_free(gb_shared_astream);
    ffwr_free(gb_in_astream);
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

int gb_running = 1;
int ffwr_set_running(int v) {
    pthread_mutex_lock(&gb_FRAME_MTX);
        gb_running = v;
    pthread_mutex_unlock(&gb_FRAME_MTX);
    return 0;
}

int ffwr_get_running() {
    int ret = 0;
    pthread_mutex_lock(&gb_FRAME_MTX);
        ret = gb_running;
    pthread_mutex_unlock(&gb_FRAME_MTX);
    return ret;;
}