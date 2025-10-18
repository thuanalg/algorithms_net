#include <SDL.h>
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

#ifndef UNIX_LINUX
#include <windows.h>
HWND gb_sdlWindow = 0;
#else
#endif 


#ifndef __FFWR_INSTREAM_DEF__
#define __FFWR_INSTREAM_DEF__
int ffwr_open_input(FFWR_INSTREAM *pinput, char *name, int mode);

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

int ffwr_open_input(FFWR_INSTREAM *pinput, char *name, int mode) {
    int ret = 0;
    int result = 0;
    AVInputFormat *iformat = 0; 
    avdevice_register_all();
    AVDictionary *options = 0;
    do {
        if(!pinput) {
            ret = 1;
            spllog(4, "--");
            break;
        }
        iformat = av_find_input_format("dshow");
        if(!iformat) {
            ret = 1;
            spllog(4, "--");
            break;
        }

		av_dict_set(&options, "rtbufsize", "50M", 0);       

        result = avformat_open_input(&(pinput->fmt_ctx), 
            "video=Integrated Webcam:"
            "audio=Microphone (2- Realtek(R) Audio)", 
            iformat, &options);

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
        pinput->v_codec = avcodec_find_decoder(AV_CODEC_ID_RAWVIDEO);
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
        pinput->a_codec = avcodec_find_decoder(AV_CODEC_ID_PCM_S16LE);
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
        pinput->a_dstframe = av_frame_alloc(); 
        if(!pinput->a_dstframe) {
            ret = 1;
            spllog(4, "--");
            break;
        }   
           
    } while(0);
    return ret;
}
#endif

#ifndef UNIX_LINUX
int WinMain
#else
int main
#endif
(int argc, char *argv[]) {	
	int ret = 0;
	SDL_SysWMinfo info = {0};
	SDL_Window *win = 0;
	int running = 1;
	SDL_Event e = {0};
	
	char cfgpath[1024] = {0};
	SPL_INPUT_ARG input = {0};
	snprintf(cfgpath, 1024, "z.cfg");
	snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
	snprintf(input.id_name, 100, "vk_window");
	ret = spl_init_log_ext(&input);
	if(ret) {
		exit(1);
	}
	
	ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret) {
        spllog(4, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    win = SDL_CreateWindow(
        "SDL2 Window",            // title
        SDL_WINDOWPOS_CENTERED,   // x
        SDL_WINDOWPOS_CENTERED,   // y
        800,                      // width
        600,                      // height
        SDL_WINDOW_SHOWN           // flags
    );
	SDL_GetWindowWMInfo(sdl_window, &info);
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
		spllog(1, "--");
        // fill background
        SDL_SetRenderDrawColor(ren, 0, 128, 255, 255); // blue
        SDL_RenderClear(ren);

        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
	spl_finish_log();
    return 0;
}
