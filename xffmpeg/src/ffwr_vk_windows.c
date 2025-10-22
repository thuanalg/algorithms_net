
#include <SDL.h>
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

#define PADDING_MEMORY  0
#define FFWR_BUFF_SIZE 12000000
#ifndef UNIX_LINUX
#include <windows.h>
HWND gb_sdlWindow = 0;
#else
#endif 

#define MEMORY_PADDING   2

typedef enum {
    FFWR_FRAME,
    FFWR_PACKET,

    FFWR_END
} FFWR_DATA_TYPE;
typedef struct __FFWR_GENERIC_DATA__ {
	int total; /*Total size*/
	int range; /*Total size*/
	int pc; /*Point to the current*/
	int pl; /*Point to the last*/
	char data[0]; /*Generic data */
} ffwr_gen_data_st;

typedef struct {
    int total;
    int type;    
} FFWR_SIZE_TYPE;

typedef struct __FFWR_AvFrame__ {
    FFWR_SIZE_TYPE tt_sz;
    int w;
    int h;
    int fmt;
    int pts;
    int linesize[AV_NUM_DATA_POINTERS];
    int pos[AV_NUM_DATA_POINTERS + 1];   
    int len[AV_NUM_DATA_POINTERS + 1]; 
    uint8_t data[0];
} FFWR_AvFrame;


//int ffwr_mv2_rawframe(FFWR_AvFrame **dst, AVFrame *src);
FFWR_AvFrame *gb_transfer_avframe = 0;

ffwr_gen_data_st *gb_frame;
ffwr_gen_data_st *gb_tsplanVFrame;
static void set_sdl_yuv_conversion_mode(AVFrame *frame);
int get_buff_size(ffwr_gen_data_st **dst, AVFrame *src);
int ffwr_fill_vframe(FFWR_AvFrame *dst, AVFrame *src);
int ffwr_update_vframe(FFWR_AvFrame **dst, AVFrame *src);
int ffwr_create_rawframe(FFWR_AvFrame **dst, AVFrame *src);
AVFrame *gb_dst_draw;
AVFrame *gb_src_draw;

pthread_mutex_t gb_FRAME_MTX = PTHREAD_MUTEX_INITIALIZER;

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

FFWR_INSTREAM gb_instream;
int
convert_frame(AVFrame *src, AVFrame *dst);
int ffwr_open_input(FFWR_INSTREAM *pinput, char *name, int mode);
int scan_all_pmts_set;
int ffwr_open_input(FFWR_INSTREAM *pinput, char *name, int mode) {
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
    

        result = avformat_open_input(&(pinput->fmt_ctx), "tcp://127.0.0.1:12345",  iformat, &options);

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

void *demux_routine(void *arg);
#ifndef UNIX_LINUX
int WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main (int argc, char *argv[])
#endif
{	
	int ret = 0;
	SDL_SysWMinfo info = {0};
	SDL_Window *win = 0;
	int running = 1;
	SDL_Event e = {0};
    pthread_t thread_id = 0;
    FFWR_AvFrame *p = 0;
    SDL_Texture *gb_texture = NULL;
    FFWR_SIZE_TYPE *it = 0;
     int step = 0;
	
	char cfgpath[1024] = {0};
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	snprintf(input.id_name, 100, "vk_window");
	ret = spl_init_log_ext(&input);
    avdevice_register_all();
    
    gb_tsplanVFrame = malloc(FFWR_BUFF_SIZE);
    if(!gb_tsplanVFrame) {
        exit(1);
    }
    memset(gb_tsplanVFrame, 0, FFWR_BUFF_SIZE);
    gb_tsplanVFrame->total = FFWR_BUFF_SIZE;
    gb_tsplanVFrame->range = gb_tsplanVFrame->total -sizeof(ffwr_gen_data_st);

    gb_frame = malloc(FFWR_BUFF_SIZE);
    if(!gb_frame) {
        exit(1);
    }
    memset(gb_frame, 0, FFWR_BUFF_SIZE);
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

    ret = pthread_create(&thread_id, 0,
                                  demux_routine, 0);

    win = SDL_CreateWindow(
        "SDL2 Window",            // title
        SDL_WINDOWPOS_UNDEFINED,   // x
        SDL_WINDOWPOS_UNDEFINED,   // y
        640,                      // width
        480,                      // height
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE          // flags
    );
	SDL_GetWindowWMInfo(win, &info);
	gb_sdlWindow = info.info.win.window;
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

    // main loop
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
        p = (FFWR_AvFrame *)(gb_frame->data + gb_frame->pc);
       
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

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
	spl_finish_log();
    return 0;
}

void *demux_routine(void *arg) {
    int ret = 0;
    int result = 0;
    AVFrame *tmp = 0;
    FFWR_AvFrame *ffwr_vframe = 0;
    
    ret = ffwr_open_input(&gb_instream, 
        "tcp://127.0.0.1:12345", 
        0);
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

            convert_frame(tmp, gb_instream.vframe);


            spl_vframe(gb_instream.vframe);
            if(!ffwr_vframe) {
                ffwr_create_rawframe(&ffwr_vframe, gb_instream.vframe);
            }
            else {
                ffwr_update_vframe(&ffwr_vframe, gb_instream.vframe);
            }
            pthread_mutex_lock(&gb_FRAME_MTX);
            do {
                if(!gb_tsplanVFrame) {
                    break;
                }
                if(!ffwr_vframe) {
                    break;
                }  
                if(ffwr_vframe->tt_sz.total < 1) {
                    break;
                }    
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
            av_frame_unref(gb_instream.a_frame);          
        }    
    }
    
    return 0;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int
convert_frame(AVFrame *src, AVFrame *dst)
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
    int k = 0;
    int m = 0;
    int i = 0;
    if(!src) {
        return ret;
    }
    if(src->format == 0) {
        while(src->linesize[i]) {
            k = src->linesize[i];
            m = (i == 0) ? src->height : ((src->height)/2);
            ret += k * (m + MEMORY_PADDING);
            spllog(1, "ret: %d", ret);
            ++i;
        }           
    }
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int ffwr_update_vframe(FFWR_AvFrame **dst, AVFrame *src) {
    int ret = 0;
    int total = 0;
    int shouldupdate = 1;
    FFWR_AvFrame *tmp = 0;
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

    do {
        if(shouldupdate) {
            tmp = realloc(*dst, total);
            if(!tmp) {
                ret = 1;
                break;
            }
        }
        tmp->tt_sz.total = total;
        tmp->tt_sz.type = FFWR_FRAME;
        ret = ffwr_fill_vframe(tmp, src);
        *dst = tmp;
    } while(0);

    return ret  = 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int ffwr_fill_vframe(FFWR_AvFrame *dst, AVFrame *src) {
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
int ffwr_create_rawframe(FFWR_AvFrame **dst, AVFrame *src) {
    int ret = 0;
    FFWR_AvFrame *tmp = 0;
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
            total = sizeof(FFWR_FRAME) + len;
            //total += len + PADDING_MEMORY;
            if(!tmp) {    
                tmp = malloc(total);
                if(!tmp) {
                    ret = 1;
                    break;
                }
                memset(tmp, 0, total);

                tmp->tt_sz.total = total;
                tmp->tt_sz.type = FFWR_FRAME;
                ffwr_fill_vframe(tmp, src);
            }
            *dst = tmp;
            //ffwr_update_vframe(dst, src);  
            break;
        }
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

