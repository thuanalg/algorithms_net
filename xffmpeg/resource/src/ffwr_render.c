#include <simplelog.h>
#include <ffwr_render.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_hello() {
	int ret = 0;

	ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) ;
	spllog(1, "---");
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_init(FFWR_InitFlags flags) {
	int ret  = 0;
    spllog(1, "SDL compiled version: %d.%d.%d.",
        SDL_MAJOR_VERSION,
        SDL_MINOR_VERSION,
        SDL_PATCHLEVEL);	
	do {
		avdevice_register_all();
		ret = avformat_network_init();
		if(ret) {
			ret = FFWR_NETWORK_INIT_ERR;
			spllog(4, "avformat_network_init, ret: %d", ret);
			break;
		}
		ret = SDL_Init(flags);
		if(ret) {
			ret = FFWR_SDL_INIT_ERR;
			spllog(4, "SDL_Init Error: %s\n", SDL_GetError());
			break;
		}		
	} while(0);

	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_CreateWindowFrom(void* hwnd, void **sdl_win) {
	int ret  = 0;
	void *p = 0;
	do {
		if(!hwnd) {
			ret = FFWR_NULL_HWND_ERR;
			spllog(4, "Null input.");
			break;
		}
		if(!sdl_win) {
			ret = FFWR_NULL_INPUT_ERR;
			spllog(4, "Null output.");
			break;
		}	
		p = SDL_CreateWindowFrom(hwnd);
		if(!p) {
			ret = FFWR_CANNOT_CREATE_WIN_ERR;
			spllog(4, "SDL_CreateWindowFrom: %s\n", SDL_GetError());
			break;
		}
		*sdl_win = p;
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_CreateRenderer(void **render, void *window, int index, FFWR_uint flags) {
	int ret = 0;
	SDL_Renderer *p = 0;
	SDL_RendererInfo  info_render = {0};
	do {
		if(!window) {
			ret = FFWR_NULL_WINDOW_ERR;
			spllog(4, "Null input.");
			break;
		}	
		if(!render) {
			ret = FFWR_NULL_RENDER_OUTPUT_ERR;
			spllog(4, "Null output.");
			break;
		}		
		p = SDL_CreateRenderer(window, index, flags);
		if(!p) {
			ret = FFWR_CREATERENDERER_ERR;
			spllog(4, "SDL_CreateRenderer: %s\n", SDL_GetError());
			break;
		}
		SDL_GetRendererInfo(p, &info_render);
		spllog(1, "Renderer: %s", info_render.name);
		*render = p;
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ffwr_CreateTexture(void **texture, 
	void *renderer, 
	FFWR_PixelFormat format, 
	FFWR_TextureAccess access, int w, int h)
{
	int ret = 0;
	SDL_Texture *p = 0;
	do {
		if(!texture) {
			ret = FFWR_NULL_TEXTURE_OUTPUT_ERR;
			spllog(4, "Null output.");
			break;
		}	
		if(!renderer) {
			ret = FFWR_NULL_RENDER_INPUT_ERR;
			spllog(4, "Null input.");
			break;
		}		
		p = SDL_CreateTexture(renderer, format, access, w, h);
		if(!p) {
			spllog(4, "SDL_CreateTexture: %s\n", SDL_GetError());
			break;
		}
		*texture = p;
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ffwr_UpdateYUVTexture(
	void *texture, const FFWR_Rect *rect,  
	const FFWR_uchar *Yplane,  int Ypitch, 
	const FFWR_uchar *Uplane,  int Upitch, 
	const FFWR_uchar *Vplane, int Vpitch) 
{
	int ret = 0;
	int result = 0;
	do {
		if(!texture) {
			ret = FFWR_NULL_TEXTURE_INPUT_ERR;
			break;
		}
		result = SDL_UpdateYUVTexture(
			texture, (SDL_Rect *)rect, 
			Yplane, Ypitch, 
			Uplane, Upitch, 
			Vplane, Vpitch);
		if(!result) {
			ret = FFWR_UPDATE_YUV_TEXTURE_ERR;
			spllog(4, "SDL_UpdateYUVTexture: %s\n", SDL_GetError());
			break;
		}
	} while(0);
	return ret;
}
						 
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ffwr_RenderClear(void *renderer) {
	int ret = 0;
	int result = 0;
	do {
		if(!renderer) {
			ret = FFWR_NULL_RENDER_INPUT_ERR;
			spllog(4, "Null input.");
			break;
		}
		result = SDL_RenderClear(renderer);
		if(result < 0) {
			ret = FFWR_RENDERCLEAR_ERR;
			spllog(4, "SDL_RenderClear: %s\n", SDL_GetError());
			break;
		}
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int	 
ffwr_RenderCopy(void *renderer, 
	void* texture, 
	const FFWR_Rect *srcrect, 
	const FFWR_Rect *dstrect) 
{
	int ret = 0;
	int result = 0;
	do {
		if(!renderer) {
			ret = FFWR_NULL_RENDER_INPUT_ERR;
			spllog(4, "Null input.");
			break;
		}
		if(!texture) {
			ret = FFWR_NULL_TEXTURE_INPUT_ERR;
			spllog(4, "Null input.");
			break;
		}		
		result = SDL_RenderCopy(renderer, texture, 
			(SDL_Rect *)srcrect, (SDL_Rect *)dstrect);
		if(result < 0) {
			ret = FFWR_RENDERCOPY_ERR;
			spllog(4, "SDL_RenderCopy: %s\n", SDL_GetError());		
			break;
		}
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int	 
ffwr_RenderPresent(void *renderer)
{
	int ret = 0;
	do {
		if(!renderer) {
			ret = FFWR_NULL_RENDER_INPUT_ERR;
			spllog(4, "Null input.");
			break;
		}
		SDL_RenderPresent(renderer);
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int	
ffwr_DestroyRenderer(void *renderer)
{
	int ret = 0;
	do {
		if(!renderer) {
			ret = FFWR_NULL_RENDER_INPUT_ERR;
			spllog(4, "Null input.");
			break;
		}
		SDL_DestroyRenderer(renderer);
	} while(0);
	return ret;	
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int	
ffwr_DestroyWindow(void *win)
{
	int ret = 0;
	do {
		if(!win) {
			ret = FFWR_NULL_SDL_WIN_INPUT_ERR;
			spllog(4, "Null input.");
			break;
		}
		SDL_DestroyWindow(win);
	} while(0);
	return ret;		
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int	
ffwr_Quit() {
	SDL_Quit();
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_open_input(FFWR_INPUT_ST *info) 
{
    int ret = 0;
    int result = 0;
    AVInputFormat *iformat = 0; 
    AVDictionary *options = 0;
	char *name = 0;
	int mode = -1;
	int sz = 0;
	FFWR_INSTREAM *pinput = 0;
    do {
        if(!info) {
            ret = FFWR_NULL_FFWR_INPUT_ST_ERR;
            spllog(4, "FFWR_NULL_FFWR_INPUT_ST_ERR");
            break;
        }
		name = info->name;
		mode = info->mode;
		sz = sizeof(FFWR_INSTREAM);
		ffwr_malloc(sz, pinput, FFWR_INSTREAM);
		if(!pinput) {
			ret = FFWR_MEMORY_ERR;
			break;
		}
		info->ffinfo = pinput;
		if (!av_dict_get(options, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
			av_dict_set(&options, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		}        
    

        result = avformat_open_input(&(pinput->fmt_ctx), name,  iformat, &options);

        if(result < 0) {
            ret = FFWR_AV_OPEN_INPUT_ERR;
            spllog(4, "--");
            break;
        }


        result = avformat_find_stream_info(pinput->fmt_ctx, 0);
        if(result < 0) {
            ret = FFWR_AV_FIND_STREAM_INFO_ERR;
            spllog(4, "--");
            break;
        }
        if(pinput->fmt_ctx->nb_streams < 1) {
            ret = FFWR_AV_NB_STREAMS_ERR;
            spllog(4, "--");
            break;
        }
        pinput->v_st = pinput->fmt_ctx->streams[0];
        if(!pinput->v_st) {
            ret = FFWR_NO_VSTREAMS_ERR;
            spllog(4, "--");
            break;
        }
#if 1        
        pinput->v_codec = avcodec_find_decoder(
            pinput->v_st->codecpar->codec_id);
        if(!pinput->v_codec) {
            ret = FFWR_NO_VCODEC_ERR;
            spllog(4, "--");
            break;
        }        
        pinput->v_cctx  = avcodec_alloc_context3(pinput->v_codec);
        if(!pinput->v_cctx) {
            ret = FFWR_NO_VCONTEXT_ERR;
            spllog(4, "--");
            break;
        }    
        result = avcodec_parameters_to_context(pinput->v_cctx, pinput->v_st->codecpar);
        if(result < 0) {
            ret = FFWR_PARAMETERS_TO_CONTEXT_ERR;
            spllog(4, "--");
            break;
        }
		result = avcodec_open2(pinput->v_cctx, pinput->v_codec, 0);
		if (result < 0) {
			ret = FFWR_OPEN_VCODEC_ERR;
            spllog(4, "avcodec_open2, result: %d", result);
			break;
		}        
#endif            
        pinput->vframe = av_frame_alloc(); 
        if(!pinput->vframe) {
            ret = FFWR_VFRAME_ALLOC_ERR;
            spllog(4, "--");
            break;
        }
            
        /*------------------------------*/

        if(pinput->fmt_ctx->nb_streams > 1) {
            pinput->a_st = pinput->fmt_ctx->streams[1];
        } else {
			break;
		}
        if(!pinput->a_st) {
            ret = FFWR_NO_VSTREAM_ERR;
            spllog(1, "---");
            break;
        }
#if 1        
        pinput->a_codec = avcodec_find_decoder(
            pinput->a_st->codecpar->codec_id);

        if(!pinput->a_codec) {
            ret = FFWR_NO_ACONTEXT_ERR;
            spllog(4, "--");
            break;
        }      
        pinput->a_cctx  = avcodec_alloc_context3(pinput->a_codec);
        if(!pinput->a_cctx) {
            ret = FFWR_ALLOC_ACONTEX_ERR;
            spllog(4, "--a_cctx");
            break;
        }   
        result = avcodec_parameters_to_context(pinput->a_cctx, pinput->a_st->codecpar);
        if(result < 0) {
            ret = FFWR_PARAMETERS_TO_ACONTEXT_ERR;
            spllog(4, "--");
            break;
        }   
		result = avcodec_open2(pinput->a_cctx, pinput->a_codec, 0);
		if (result < 0) {
			ret = FFWR_OPEN_ACODEC_ERR;
            spllog(4, "--");
			break;
		} 
        pinput->a_frame = av_frame_alloc(); 
        if(!pinput->a_frame) {
            ret = FFWR_AFRAME_ALLOC_ERR;
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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
