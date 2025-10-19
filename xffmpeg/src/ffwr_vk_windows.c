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

#define PADDING_MEMORY  1024

#ifndef UNIX_LINUX
#include <windows.h>
HWND gb_sdlWindow = 0;
#else
#endif 
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
    int type;
	char data[0]; /*Generic data */
} ffwr_gen_data_st;

typedef struct __FFWR_AvFrame__ {
    int total;
    int w;
    int h;
    int fmt;
    int linesize[AV_NUM_DATA_POINTERS];
    int len[AV_NUM_DATA_POINTERS];
    uint8_t data[0];
} FFWR_AvFrame;

ffwr_gen_data_st *gb_frame;
ffwr_gen_data_st *planVFrame;

int get_buff_size(ffwr_gen_data_st **dst, AVFrame *src);

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

    //pinput->fmt_ctx->interrupt_callback.callback = decode_interrupt_cb;
    //pinput->fmt_ctx->interrupt_callback.opaque = is;
    if (!av_dict_get(options, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
        av_dict_set(&options, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
        scan_all_pmts_set = 1;
    }        
#if 0        
        iformat = av_find_input_format("dshow");
        if(!iformat) {
            ret = 1;
            spllog(4, "--");
            break;
        }
#endif
		//av_dict_set(&options, "rtbufsize", "50M", 0);       

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
    int frame_ready = 0;
    SDL_Texture *gb_texture = NULL;
	
	char cfgpath[1024] = {0};
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	snprintf(input.id_name, 100, "vk_window");
	ret = spl_init_log_ext(&input);
    avdevice_register_all();
    

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
        SDL_WINDOW_SHOWN           // flags
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

    // main loop
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
		//spllog(1, "--");
        // fill background
#if 0
        SDL_SetRenderDrawColor(ren, 0, 128, 255, 255); // blue
        SDL_RenderClear(ren);
        SDL_RenderPresent(ren);
#endif
        pthread_mutex_lock(&gb_FRAME_MTX);
        do {
            //get_buff_size(&planVFrame, gb_instream.vframe);
            if(!planVFrame) {
                break;
            }
            p = (FFWR_AvFrame *)planVFrame->data;
            if(!p->total) {
                break;
            }
            if(!gb_frame) {
                gb_frame = malloc(planVFrame->total * 2);
                gb_frame->total = planVFrame->total * 2;
            } else {
                if(gb_frame->total < planVFrame->total) {
                    exit(1);
                }
            }
            if(!gb_frame) {
                break;
            }
            frame_ready = 1;
            memcpy(gb_frame->data, p, p->total);
            p->total = 0;
        } while(0);
        pthread_mutex_unlock(&gb_FRAME_MTX);
#if 1
        if (frame_ready) {
            //p = (FFWR_AvFrame *)gb_frame->data;
            if(!gb_texture) {
                gb_texture = SDL_CreateTexture( ren,
                    SDL_PIXELFORMAT_IYUV,
                    SDL_TEXTUREACCESS_STREAMING,
                    p->w,
                    p->h);
            }
            if(!gb_texture) {
                exit(1);
            }
            p = (FFWR_AvFrame *)gb_frame->data;
            SDL_UpdateYUVTexture(
                gb_texture,
                NULL,
                p->data, p->linesize[0],
                p->data + p->len[0], p->linesize[1],
                p->data + p->len[1], p->linesize[2]
            );

            SDL_RenderClear(ren);
            SDL_RenderCopy(ren, gb_texture, NULL, NULL);
            SDL_RenderPresent(ren);

            frame_ready = 0;
        } else {
            SDL_Delay(10);
        }
#endif        
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
	spl_finish_log();
    return 0;
}
FFWR_INSTREAM gb_instream;
void *demux_routine(void *arg) {
    int ret = 0;
    int result = 0;
    
    ret = ffwr_open_input(&gb_instream, 
        "tcp://127.0.0.1:12345", 
        0);
    if(ret) {
        return 0;
    }


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
		    result = avcodec_receive_frame(gb_instream.v_cctx, gb_instream.vframe);
		    if (result < 0) {
		    	break;
		    } 
            pthread_mutex_lock(&gb_FRAME_MTX);
            do {
                get_buff_size(&planVFrame, gb_instream.vframe);
            } while(0);
            pthread_mutex_unlock(&gb_FRAME_MTX);
            spl_vframe(gb_instream.vframe);
        }   
        else if (gb_instream.pkt.stream_index == 1) {
            result = avcodec_send_packet(gb_instream.a_cctx, &(gb_instream.pkt));
            if(result < 0) {
                spllog(1, "v_cctx: 0x%p", gb_instream.a_cctx);
                break;
            }
		    result = avcodec_receive_frame(gb_instream.a_cctx, gb_instream.a_frame);
		    if (result < 0) {
		    	break;
		    }  
            spl_vframe(gb_instream.a_frame);           
        }    
    }
    
    return 0;
}
int get_buff_size(ffwr_gen_data_st **dst, AVFrame *src) {
    FFWR_AvFrame *p = 0;
    int ret = 0;
    int len = 0;
    int i = 0;
    int k = 0;
    ffwr_gen_data_st *tmp = 0;
    int total = 0;
    int t = 0;
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
        if(src->format == 4) {
            /* AV_PIX_FMT_YUV420P */
            /* YUV, Y: luminance, U/chrominance: Color (Cr), V/chrominance: Color (Cb)*/
            while(src->linesize[i]) {
                k = (i == 0) ? src->linesize[i] : (src->linesize[i]/2);
                len += src->height * k;
                ++i;
            }
            total = sizeof(ffwr_gen_data_st) + sizeof(FFWR_FRAME);
            total += len + PADDING_MEMORY;
            if(!tmp) {
                tmp = (ffwr_gen_data_st*) malloc(total);
                if(!tmp) {
                    exit(1);
                }
                memset(tmp, 0, total);
                tmp->total = total;
                tmp->range = total - sizeof(ffwr_gen_data_st);
                //p = (FFWR_AvFrame *) tmp->data;
            } else {
                if(tmp->total < total) {
                    tmp = (ffwr_gen_data_st*) realloc(tmp, total);
                    if(!tmp) {
                        exit(1);
                    }      
                    tmp->total = total;
                    tmp->range = total - sizeof(ffwr_gen_data_st);
                    //p = (FFWR_AvFrame *) tmp->data;                                  
                }
            }
            p = (FFWR_AvFrame *) tmp->data;  
            tmp->pc = 0;
            tmp->pl = sizeof(FFWR_FRAME) + len;
            tmp->type = FFWR_FRAME;
            p->total = len + sizeof(FFWR_FRAME);
            p->w = src->width;
            p->h = src->height;
            p->fmt = src->format;
            i = 0;
            //av_get_pix_fmt_name(p->fmt);
            while(src->linesize[i] && i < AV_NUM_DATA_POINTERS) 
            {
                p->linesize[i] = src->linesize[i];
                k = (i == 0) ? src->linesize[i] : (src->linesize[i]/2);
                k *= src->height;
                p->len[i] = k;               
                memcpy( p->data + t, src->data[i], p->len[i]);
                t += p->len[i];
                ++i;
            }   
            while(i < AV_NUM_DATA_POINTERS) {
                p->linesize[i] = 0;
                ++i;
            }
            *dst = tmp;         
            break;
        }
    } while(0);
    return ret;
}

#if 0
    if (frame_ready) {
        SDL_UpdateYUVTexture(
            texture,
            NULL,
            frame->data[0], frame->linesize[0],
            frame->data[1], frame->linesize[1],
            frame->data[2], frame->linesize[2]
        );

        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, texture, NULL, NULL);
        SDL_RenderPresent(ren);

        frame_ready = 0;
    } else {
        SDL_Delay(10);
    }
#endif