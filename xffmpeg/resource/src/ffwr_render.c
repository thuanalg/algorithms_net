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

#ifndef UNIX_LINUX
	#include <windows.h>
#else
#endif

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define ffwr_frame_unref(__fr__) if(__fr__) {av_frame_unref(__fr__);}
#define ffwr_frame_free(__fr__) if(__fr__) {av_frame_free(__fr__);}
#define ffwr_packet_unref(__pkt__) if(__pkt__) {av_packet_unref(__pkt__);}
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
	
	FFWR_DEMUX_DATA *buffer;

} FFWR_INSTREAM;
#endif
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* Functions */
static int
ffwr_convert_vframe(AVFrame *src, AVFrame *dst);

#ifndef UNIX_LINUX
static DWORD WINAPI 
ffwr_demux_routine(LPVOID lpParam);
#else	
#endif

static int 
ffwr_get_rawsize_vframe(AVFrame *src);

static int 
ffwr_create_rawvframe(FFWR_VFrame **dst, AVFrame *src);

static int 
ffwr_update_vframe(FFWR_VFrame **dst, AVFrame *src);

static int 
convert_audio_frame( AVFrame *src, AVFrame **outfr);

static int 
ffwr_fill_vframe(FFWR_VFrame *dst, AVFrame *src);

static int 
ffwr_create_a_swrContext(AVFrame *src, AVFrame *dst);

#if 0
static int 
spl_mutex_lock(void *mtx);

static int 
spl_mutex_unlock(void *mtx);
#endif

static int 
ffwr_init_gen_buff(ffwr_gen_data_st *obj, int sz);

static void 
ffwr_open_audio_output_cb(void *user, Uint8 * stream, int len);

static int 
ffwr_clode_audio_output();

static void 
ffwr_clear_gb_var();

static int
ffwr_open_instream(FFWR_INSTREAM *instr) ;
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/* Variables */
static FFWR_INSTREAM gb_instream;

void *ffwr_st_VFRAME_MTX;
void *ffwr_st_AFRAME_MTX;

ffwr_gen_data_st *st_shared_vframe;
ffwr_gen_data_st *st_renderVFrame;

ffwr_araw_stream *st_SharedAudioBuffer;
ffwr_araw_stream *st_AudioBuffer;

struct SwrContext *gb_aConvertContext;


SDL_AudioSpec gb_want, gb_have;
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
#ifndef UNIX_LINUX
		if(!ffwr_st_VFRAME_MTX) {
			ffwr_st_VFRAME_MTX = CreateMutexA(0, 0, 0);
		}
		if(!ffwr_st_VFRAME_MTX) {
			ret = FFWR_WIN_CREATE_MUTEX_ERR;
			break;
		}
		if(!ffwr_st_AFRAME_MTX) {
			ffwr_st_AFRAME_MTX = CreateMutexA(0, 0, 0);
		}
		if(!ffwr_st_AFRAME_MTX) {
			ret = FFWR_WIN_CREATE_MUTEX_ERR;
			break;
		}		
#else	
#endif
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
		
		pinput = &gb_instream;
		
		if(!pinput) {
			ret = FFWR_MEMORY_ERR;
			break;
		}
		info->ffinfo = pinput;
		if (!av_dict_get(options, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
			av_dict_set(&options, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		}        
		
		spllog(1, "name: %s", name);

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
            ret = FFWR_AFRAME_ALLOC_ERR;
            spllog(4, "--");
            break;
        }   
        spllog(1, "openInout OK, ret: %d", ret);
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


int
ffwr_create_demux(void *obj)
{
	int ret = 0;
#ifndef UNIX_LINUX
	HANDLE hThread = 0;
	DWORD dwThreadId = 0;
	hThread = CreateThread(NULL, // security attributes
	    0, 
	    ffwr_demux_routine, // thread function
	    obj,
	    0, // 
	    &dwThreadId // 
	);
#else
#endif	
	return ret;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef UNIX_LINUX
DWORD WINAPI ffwr_demux_routine(LPVOID lpParam)
{
    int ret = 0;
    int result = 0;
    AVFrame *tmp = 0;
    FFWR_VFrame *ffwr_vframe = 0;
    int running = 0;
	FFWR_INPUT_ST *info = 0;
    
	info = (FFWR_INPUT_ST *)lpParam;
    ret = ffwr_open_input(info);
    if(ret) {
        return 0;
    }
    tmp = av_frame_alloc();
	spllog(1, "av_frame_alloc");
    tmp->width = 640;
    tmp->height = 480;
    tmp->format = 4;
    tmp->pts = 0;

    av_frame_get_buffer(tmp, 32);
    
    //SDL_CreateWindowFrom

    gb_instream.vframe->width = 640;
    gb_instream.vframe->height = 480;
    gb_instream.vframe->format = 4;
    gb_instream.vframe->pts = 0;
    av_frame_get_buffer(gb_instream.vframe, 32);       
	
	/*-----------------*/
	ffwr_malloc(FFWR_BUFF_SIZE, st_shared_vframe, ffwr_gen_data_st);
    if(!st_shared_vframe) {
        exit(1);
    }
    //memset(st_shared_vframe, 0, FFWR_BUFF_SIZE);
    st_shared_vframe->total = FFWR_BUFF_SIZE;
    st_shared_vframe->range = st_shared_vframe->total -sizeof(ffwr_gen_data_st);

    //st_renderVFrame = malloc(FFWR_BUFF_SIZE);
    ffwr_malloc(FFWR_BUFF_SIZE, st_renderVFrame, ffwr_gen_data_st);
    if(!st_renderVFrame) {
        exit(1);
    }
    //memset(st_renderVFrame, 0, FFWR_BUFF_SIZE);

    st_renderVFrame->total = FFWR_BUFF_SIZE;
    st_renderVFrame->range = st_renderVFrame->total -sizeof(ffwr_gen_data_st);    
	
	/*-----------------*/
	ffwr_open_audio_output( 2000000);
	/*-----------------*/
	
	
    while(1) {
        running = ffwr_get_running();
        if(!running) {
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
				spllog(4, "avcodec_receive_frame");
		    	break;
		    } 

            ffwr_convert_vframe(tmp, gb_instream.vframe);


            spl_vframe(gb_instream.vframe);
#if 1			
            if(!ffwr_vframe) {
                ffwr_create_rawvframe(&ffwr_vframe, gb_instream.vframe);
                if(!ffwr_vframe) {
					spllog(4, "ffwr_vframe");
                    break;
                }                  
            }
            else {
                ffwr_update_vframe(&ffwr_vframe, gb_instream.vframe);
            } 
            if(ffwr_vframe->tt_sz.total < 1) {
				spllog(4, "ffwr_vframe->tt_sz.total");
                break;
            }  
            if(!st_shared_vframe) {
				spllog(4, "st_shared_vframe");
                break;
            }
            spl_mutex_lock(ffwr_st_VFRAME_MTX);
            do {
                if(st_shared_vframe->range > 
                    st_shared_vframe->pl + ffwr_vframe->tt_sz.total) {                      
                    memcpy(st_shared_vframe->data + st_shared_vframe->pl, 
                        ffwr_vframe, 
                        ffwr_vframe->tt_sz.total);
                    st_shared_vframe->pl += ffwr_vframe->tt_sz.total;
                    spllog(1, "st_shared_vframe->pl: %d", 
                        st_shared_vframe->pl);
                } else {
                    st_shared_vframe->pl = 0;
                    st_shared_vframe->pc = 0;
                }

            } while(0);
            spl_mutex_unlock(ffwr_st_VFRAME_MTX);
#endif			
            av_frame_unref(tmp);
            av_frame_unref(gb_instream.vframe);
        }   
        else if (gb_instream.pkt.stream_index == 1) {
            result = avcodec_send_packet(
                gb_instream.a_cctx, &(gb_instream.pkt));
            if(result < 0) {
                spllog(4, "avcodec_send_packet");
                break;
            }
		    result = avcodec_receive_frame(
                gb_instream.a_cctx, gb_instream.a_frame);
		    if (result < 0) {
				spllog(4, "avcodec_receive_frame");
		    	break;
		    }  
#if 1			
            convert_audio_frame(gb_instream.a_frame, 
                &(gb_instream.a_dstframe));
            spl_mutex_lock(ffwr_st_VFRAME_MTX);
            do {
                if(st_SharedAudioBuffer->range > 
                    st_SharedAudioBuffer->pl + 
                    gb_instream.a_dstframe->linesize[0]) 
                {
                    memcpy(st_SharedAudioBuffer->data + 
                            st_SharedAudioBuffer->pl, 
                        gb_instream.a_dstframe->data[0], 
                        gb_instream.a_dstframe->linesize[0]
                    );
                    st_SharedAudioBuffer->pl += 
                        gb_instream.a_dstframe->linesize[0];
                } else {
                    st_SharedAudioBuffer->pl = 0;
                    st_SharedAudioBuffer->pc = 0;
                    spllog(1, "over audio range");
                }
                spllog(1, "(pl, pc, range)=(%d, %d, %d)", 
                    st_SharedAudioBuffer->pl, 
                    st_SharedAudioBuffer->pc, 
                    st_SharedAudioBuffer->range);
            } while(0);
            spl_mutex_unlock(ffwr_st_VFRAME_MTX);
#endif			
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
	ffwr_clode_audio_output();
	
	//ffwr_clear_gb_var();
	
	if(info->cb) {
		info->sz_type.type = FFWR_DEMUX_THREAD_EXIT;
		info->cb(info);
	}
	
	return 0;
}
#endif
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ffwr_gb_running = 1;
int ffwr_get_running() {
    int ret = 0;
    spl_mutex_lock(ffwr_st_VFRAME_MTX);
        ret = ffwr_gb_running;
    spl_mutex_unlock(ffwr_st_VFRAME_MTX);
    return ret;;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ffwr_set_running(int v) {
    spl_mutex_lock(ffwr_st_VFRAME_MTX);
        ffwr_gb_running = v;
    spl_mutex_unlock(ffwr_st_VFRAME_MTX);
    return 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
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
            &swr,                        	// NULL → tạo mới
            &(dst->ch_layout),         		// kênh đầu ra
            dst->format,          			// định dạng sample đầu ra
            dst->sample_rate,               // sample rate đầu ra
            &(src->ch_layout),           	// kênh đầu vào
            src->format,           			// định dạng sample đầu vào
            src->sample_rate,               // sample rate đầu vào
            0, NULL                      	// log offset, log context
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
	if(ret) {
		spllog(4, "ret");
	}
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#if 0
int spl_mutex_lock(void *mtx) {
	int ret = 0;
#ifndef UNIX_LINUX
	DWORD err = 0;
#else
	int err = 0;
#endif		
	do {
		if(!mtx) {
			ret = spl_mutex_NULL;
			spllog(4, "spl_mutex_NULL");
		}
#ifndef UNIX_LINUX		
		err = WaitForSingleObject(mtx, INFINITE);
		if (err != WAIT_OBJECT_0) {
			ret = FFWR_WIN32_MUTEX_RELEASE;
			spllog(4, "WaitForSingleObject");
			break;
		}		
#else
#endif	
	} while(0);
	return ret;
}

int spl_mutex_unlock(void *mtx) {
	int ret = 0;
#ifndef UNIX_LINUX
	DWORD done = 0;
#else
	int err = 0;
#endif
	do {
		if(!mtx) {
			ret = spl_mutex_NULL;
			spllog(4, "spl_mutex_NULL");
		}		
#ifndef UNIX_LINUX
		done = ReleaseMutex(mtx);
		if (!done) {
			ret = FFWR_WIN32_MUTEX_RELEASE;
			spllog(4, "ReleaseMutex, GetLastError() %d", 
				(int)GetLastError());
			break;
		}		
#else
#endif	
	} while(0);
	return ret;
}
#endif
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void*
ffwr_mutex_data() 
{
	return ffwr_st_VFRAME_MTX;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
ffwr_gen_data_st*
ffwr_gb_renderVFrame()
{
	return st_renderVFrame;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
ffwr_gen_data_st*
ffwr_gb_shared_vframe()
{
	return st_shared_vframe;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ffwr_open_audio_output(int sz)
{
    int ret = 0;
    int insz = 3 * sz;
    do {
        ffwr_malloc(sz, st_SharedAudioBuffer, ffwr_araw_stream);
        if(!st_SharedAudioBuffer) {
            ret = 1;
            break;
        }
        ffwr_init_gen_buff(st_SharedAudioBuffer, sz);

        ffwr_malloc(insz, st_AudioBuffer, ffwr_araw_stream);
        if(!st_AudioBuffer) {
            ret = 1;
            break;
        }        
        ffwr_init_gen_buff(st_AudioBuffer, insz);

        gb_want.freq = FFWR_OUTPUT_ARATE;
        gb_want.format = AUDIO_F32SYS;
        gb_want.channels = 2;
        gb_want.samples = 4096;        // kích thước buffer SDL
        gb_want.callback = ffwr_open_audio_output_cb;
        gb_want.userdata = st_AudioBuffer; // buffer 
#if 0
        ret = SDL_OpenAudio(&gb_want, 0);
        spllog(1, "ret-SDL_OpenAudio: %d", ret);
        SDL_PauseAudio(0);           // start audio playback
#else
        SDL_AudioDeviceID dev = SDL_OpenAudioDevice(0, 0, &gb_want, &gb_have, 
            0);
        if(!dev) {
            printf("SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        }
        SDL_PauseAudioDevice(dev, 0); // bật playback
#endif
    } while(0);
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ffwr_init_gen_buff(ffwr_gen_data_st *obj, int sz) 
{
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
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void ffwr_open_audio_output_cb(void *user, Uint8 * stream, int len)
{
    ffwr_gen_data_st *obj = (ffwr_gen_data_st*) user;
    int real_len = 0;
    static int step = 0;
    int running = 0;

	running = ffwr_get_running();
	
	if(!running) {
		return;
	}

    if(!obj) {
        return;
    }

    if( obj->pl <= obj->pc) 
    {
        obj->pl = obj->pc = 0;
        spl_mutex_lock(ffwr_st_AFRAME_MTX);
        do {
            if(st_SharedAudioBuffer->pl < 1) {
                break;
            }
            if(step == 0) {
                if(st_SharedAudioBuffer->pl > 0) {
                    step++;
                    st_SharedAudioBuffer->pc = 0;
                    st_SharedAudioBuffer->pl = 0;
                    break;
                }
            }
            if(st_SharedAudioBuffer->pl < 1) {
                break;
            }

            memcpy(obj->data + obj->pl, 
                st_SharedAudioBuffer->data, 
                st_SharedAudioBuffer->pl);

            obj->pl += st_SharedAudioBuffer->pl;

            st_SharedAudioBuffer->pc = 0;
            st_SharedAudioBuffer->pl = 0;
        } while(0);
        spl_mutex_unlock(ffwr_st_AFRAME_MTX);
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
            spl_mutex_lock(ffwr_st_AFRAME_MTX);
            do {
                if(obj->range >= obj->pl + st_SharedAudioBuffer->pl) {
                    memcpy(obj->data + obj->pl, 
                        st_SharedAudioBuffer->data, 
                        st_SharedAudioBuffer->pl);

                    obj->pl += st_SharedAudioBuffer->pl;
                }

                st_SharedAudioBuffer->pc = 0;
                st_SharedAudioBuffer->pl = 0;
            } while(0);
            spl_mutex_unlock(ffwr_st_AFRAME_MTX);
        }
    }
    if(obj->pl > 800000 + obj->pc) {
        obj->pl = obj->pc = 0;
    }
}           
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ffwr_clode_audio_output() {
    int ret = 0;
    do {
        SDL_PauseAudio(1); // stop audio
        SDL_CloseAudio();  // close, free resources
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int 
ffwr_destroy_render_objects(FFWR_RENDER_OBJECTS *p) 
{
	int ret = 0;
	do {
		if(!p) {
			ret = FFWR_WIN32_RENDER_OBJECTS_NULL_ERR;
			break;
		}
		if(p->sdl_texture) {
			spllog(1, "sdl_texture: %p", p->sdl_texture);
			SDL_DestroyTexture(p->sdl_texture);
		}
		if(p->sdl_render) {
			spllog(1, "sdl_render: %p", p->sdl_render);
			SDL_DestroyRenderer(p->sdl_render);
		}
		if(p->sdl_window) {
			spllog(1, "sdl_window: %p", p->sdl_window);
			SDL_DestroyWindow(p->sdl_window);
		}		
		p->sdl_texture = 0;;
		p->sdl_render = 0;;
		p->sdl_window = 0;;
		ffwr_clear_gb_var();
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void ffwr_clear_gb_var() {
    ffwr_free(st_shared_vframe);
    ffwr_free(st_renderVFrame);
    ffwr_free(st_SharedAudioBuffer);
    ffwr_free(st_AudioBuffer);
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int 
ffwr_create_demux_objects(FFWR_DEMUX_OBJS *obj) 
{
	int ret = 0;
	FFWR_INSTREAM *inner_demux = 0;
	int sz = sizeof(FFWR_INSTREAM);
	do {
		if(!obj) {
			ret = FFWR_DEMUX_OBJS_NULL_ERR;
			spllog(4, "FFWR_DEMUX_OBJS_NULL_ERR");
			break;
		}	
		ffwr_malloc(sz, inner_demux, FFWR_INSTREAM);
		if(!inner_demux) {
			ret = FFWR_MALLOC_ERR;
			spllog(4, "FFWR_MALLOC_ERR");
			break;
		}
		obj->inner_demux = inner_demux;
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int 
ffwr_destroy_demux_objects(FFWR_DEMUX_OBJS *obj) {return 0;}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int 
ffwr_get_demux_data(FFWR_DEMUX_OBJS *obj, FFWR_DEMUX_DATA **out) 
{
	int ret  = 0;
	FFWR_INSTREAM *demux = 0;
	do {
		if(!obj) {
			ret = FFWR_DEMUX_OBJS_NULL_ERR;
			spllog(4, "FFWR_DEMUX_OBJS_NULL_ERR");
			break;
		}		
		if(!out) {
			ret = FFWR_DEMUX_DATA_OUT_NULL_ERR;
			spllog(4, "FFWR_DEMUX_DATA_OUT_NULL_ERR");
			break;
		}
		*out = 0;
		demux = (FFWR_INSTREAM *) obj->inner_demux;
		if(!demux) {
			ret = FFWR_INSTREAM_NULL_ERR;
			spllog(4, "FFWR_INSTREAM_NULL_ERR");
			break;
		}
		*out = demux->buffer;
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_open_instream(FFWR_INSTREAM *instr)
{
	return 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
