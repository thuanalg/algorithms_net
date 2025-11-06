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
#define ffwr_frame_free(__fr__) \
if(__fr__) {spllog(1, "av_frame_free frame: 0x%p", *(__fr__)); \
av_frame_free(__fr__); }

#define ffwr_frame_alloc(__fr__) { (__fr__) =  av_frame_alloc(); \
spllog(1, "av_frame_alloc frame: 0x%p", (__fr__));}

#define ffwr_packet_unref(__pkt__) \
	if(__pkt__) {av_packet_unref(__pkt__);}


#define ffwr_avcodec_alloc_context3(__ret__, __codec__) \
{(__ret__) = avcodec_alloc_context3(__codec__); \
spllog(1, "avcodec_alloc_context: 0x%p", (__ret__));}

#define ffwr_avcodec_free_context(__obj__) \
	{if((__obj__) && *(__obj__)) \
	{ spllog(1, "avcodec_free_context: 0x%p", *(__obj__)); \
	avcodec_free_context(__obj__); }};
	
#define ffwr_sws_getContext(\
__ret__, __v0__, __v1__, __v2__, __v3__, \
__v4__, __v5__, __v6__, __v7__, __v8__, __v9__ ) \
{ (__ret__) = sws_getContext((__v0__), (__v1__), \
(__v2__), (__v3__), (__v4__), (__v5__), \
(__v6__), (__v7__), (__v8__), (__v9__)); \
spllog(1, "alloc vscaleContext: 0x%p", (__ret__));}	

#define ffwr_sws_freeContext(__o__) {if(__o__){ \
	spllog(1, "sws_freeContext, free vscaleContext: 0x%p", (__o__)); \
	sws_freeContext(__o__); (__o__) = 0;}\
}

#define ffwr_swr_alloc_set_opts2( __v0__, __v1__, \
__v2__, __v3__, __v4__, __v5__, __v6__, __v7__, __v8__) {\
swr_alloc_set_opts2( (__v0__), (__v1__), (__v2__), \
(__v3__), (__v4__), (__v5__), (__v6__), \
(__v7__), (__v8__)); spllog(1, "alloc ascaleContext: 0x%p", *(__v0__));\
}

#define ffwr_swr_free(__o__) {if((__o__) && (*(__o__))) {\
	spllog(1, "free ascaleContext: %p", *(__o__));\
	swr_free(__o__);}\
}	

#define ffwr_avformat_open_input(__ret__, \
	__v0__, __v1__, __v2__, __v3__) {\
	(__ret__) = avformat_open_input((__v0__), \
		(__v1__), (__v2__), (__v3__));\
	spllog(1, "open avformat: 0x%p", *(__v0__));\
}

#define ffwr_avformat_close_input(__fmt__) {\
	spllog(1, "close avformat: 0x%p", *(__fmt__));\
	avformat_close_input(__fmt__);\
}

#define ffwr_SDL_DestroyWindow(__win__) {\
	spllog(1, "Destroy Window-render:: 0x%p", (__win__));\
	SDL_DestroyWindow(__win__);(__win__) = 0;\
}

#define ffwr_SDL_DestroyTexture(__texture__) {\
	spllog(1, "Destroy Texture-render:: 0x%p", (__texture__));\
	SDL_DestroyTexture(__texture__);(__texture__) = 0;\
}

#define ffwr_SDL_DestroyRenderer(__ren__) {\
	spllog(1, "Destroy Renderer-render:: 0x%p", (__ren__));\
	SDL_DestroyRenderer(__ren__);(__ren__) = 0;\
}

#define ffwr_SDL_CreateWindowFrom(__sdl__, __native__) {\
	(__sdl__) = SDL_CreateWindowFrom(__native__);\
	spllog(1, "Create Window-render:: 0x%p", (__sdl__));\
}

#define ffwr_SDL_CreateRenderer(__ren__, __v0__,  __v1__,  __v2__) {\
	(__ren__) = SDL_CreateRenderer((__v0__),  (__v1__),  (__v2__));\
	spllog(1, "Create Renderer-render:: 0x%p", (__ren__));\
}

#define ffwr_SDL_CreateTexture( __texture__,__ren__, __v0__,  __v1__,  __v2__, __v3__) {\
	(__texture__) = SDL_CreateTexture( (__ren__), (__v0__),  (__v1__),  (__v2__), (__v3__));\
	spllog(1, "Create Texture-render: 0x%p", (__texture__));\
}
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
/* Functions */


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

static int
ffwr_open_instream(FFWR_DEMUX_OBJS *obj) ;

static int
ffwr_close_instream(FFWR_DEMUX_OBJS *obj) ;

static int
ffwr_open_render_sdl_pipe(FFWR_DEMUX_OBJS *obj) ;

static int 
ffwr_create_sync_buff(FFWR_DEMUX_OBJS *obj) ;



static int 
ffwr_create_genbuff(ffwr_gen_data_st **obj, int sz) ;

static int
ffwr_create_demux_ext(void *obj);

static int 
ffwr_create_a_swrContext_ext(FFWR_INSTREAM *p, AVFrame *src, AVFrame *dst);

static int 
convert_audio_frame_ext(FFWR_INSTREAM *p, AVFrame *src, AVFrame **outfr);

static int
ffwr_convert_vframe_ext(FFWR_INSTREAM *p, AVFrame *src, AVFrame *dst);
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/* Variables */


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
//		if(!ffwr_st_VFRAME_MTX) {
//			ffwr_st_VFRAME_MTX = CreateMutexA(0, 0, 0);
//		}
//		if(!ffwr_st_VFRAME_MTX) {
//			ret = FFWR_WIN_CREATE_MUTEX_ERR;
//			break;
//		}
//		if(!ffwr_st_AFRAME_MTX) {
//			ffwr_st_AFRAME_MTX = CreateMutexA(0, 0, 0);
//		}
//		if(!ffwr_st_AFRAME_MTX) {
//			ret = FFWR_WIN_CREATE_MUTEX_ERR;
//			break;
//		}		
#else	
#endif
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
		if(result < 0) {
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
#ifndef UNIX_LINUX
DWORD WINAPI ffwr_demux_routine(LPVOID lpParam)
{
	int ret = 0, result = 0;
	FFWR_DEMUX_OBJS *obj = 0;
	obj = (FFWR_DEMUX_OBJS *) lpParam;
    AVFrame *tmp = 0;
    FFWR_VFrame *ffwr_vframe = 0;
    int stopping = 0;
	FFWR_INPUT_ST *info = 0;
	FFWR_INSTREAM *pgb_instream = 0;
	ffwr_gen_data_st *pst_renderVFrame = 0, *pst_shared_vframe = 0;	
	ffwr_gen_data_st *abuf = 0, *shared_abuf = 0;	
	void *vmutex = 0;
	void *amutex = 0;
	do {
		if(!obj) {
			ret = FFWR_DEMUX_OBJS_NULL_ERR;
			spllog(4, "FFWR_DEMUX_OBJS_NULL_ERR");
			break;
		}		
		ret = ffwr_open_instream(obj);
		if(ret) {
			spllog(4, "ffwr_open_instream");
			break;			
		}
		pgb_instream = (FFWR_INSTREAM *) obj->inner_demux;
		info = &(obj->input);
		ffwr_frame_alloc(tmp);
		tmp->width = 640;
		tmp->height = 480;
		tmp->format = 4;
		tmp->pts = 0;
	
		av_frame_get_buffer(tmp, 32);
		
		//SDL_CreateWindowFrom
	
		pgb_instream->vframe->width = 640;
		pgb_instream->vframe->height = 480;
		pgb_instream->vframe->format = 4;
		pgb_instream->vframe->pts = 0;
		av_frame_get_buffer(pgb_instream->vframe, 32);       
	
		/*-----------------*/

		/*-----------------*/
		pst_renderVFrame = obj->buffer.vbuf;
		pst_shared_vframe = obj->buffer.shared_vbuf;
		vmutex = obj->buffer.mtx_vbuf;
		amutex = obj->buffer.mtx_abuf;
		abuf = obj->buffer.abuf;
		shared_abuf = obj->buffer.shared_abuf;
		/*-----------------*/
		//ffwr_open_audio_output( 2000000);
		/*-----------------*/
		while(1) 
		{
			stopping = ffwr_get_stopping(obj);
			if(stopping) {
				break;
			}
			av_packet_unref(&(pgb_instream->pkt));
			result = av_read_frame(pgb_instream->fmt_ctx, &(pgb_instream->pkt)); 
			if(result) {
				continue;
			}
			if(pgb_instream->pkt.stream_index == 0) {
				result = avcodec_send_packet(pgb_instream->v_cctx, &(pgb_instream->pkt));
				if(result < 0) {
					spllog(1, "v_cctx: 0x%p", pgb_instream->v_cctx);
					break;
				}
				result = avcodec_receive_frame(pgb_instream->v_cctx, tmp);
				if (result < 0) {
					spllog(4, "avcodec_receive_frame");
					break;
				} 
	
				ffwr_convert_vframe_ext(pgb_instream, tmp, pgb_instream->vframe);
	
	
				spl_vframe(pgb_instream->vframe);
				if(!ffwr_vframe) {
					ffwr_create_rawvframe(&ffwr_vframe, pgb_instream->vframe);
					if(!ffwr_vframe) {
						spllog(4, "ffwr_vframe");
						break;
					}                  
				}
				else {
					ffwr_update_vframe(&ffwr_vframe, pgb_instream->vframe);
				} 
				if(ffwr_vframe->tt_sz.total < 1) {
					spllog(4, "ffwr_vframe->tt_sz.total");
					break;
				}  
				if(!pst_shared_vframe) {
					spllog(4, "pst_shared_vframe");
					break;
				}
				spl_mutex_lock(vmutex);
				do {
					if(pst_shared_vframe->range > 
						pst_shared_vframe->pl + ffwr_vframe->tt_sz.total) {                      
						memcpy(pst_shared_vframe->data + pst_shared_vframe->pl, 
							ffwr_vframe, 
							ffwr_vframe->tt_sz.total);
						pst_shared_vframe->pl += ffwr_vframe->tt_sz.total;
						spllog(1, "pst_shared_vframe->pl: %d", 
							pst_shared_vframe->pl);
					} else {
						pst_shared_vframe->pl = 0;
						pst_shared_vframe->pc = 0;
					}
	
				} while(0);
				spl_mutex_unlock(vmutex);
				av_frame_unref(tmp);
				av_frame_unref(pgb_instream->vframe);
			}
			if(pgb_instream->pkt.stream_index == 1) 
			{
				result = avcodec_send_packet(
					pgb_instream->a_cctx, &(pgb_instream->pkt));
				if(result < 0) {
					spllog(4, "avcodec_send_packet");
					break;
				}
				result = avcodec_receive_frame(
					pgb_instream->a_cctx, pgb_instream->a_frame);
				if (result < 0) {
					spllog(4, "avcodec_receive_frame");
					break;
				}  
#if 1			
            convert_audio_frame_ext(pgb_instream, pgb_instream->a_frame, 
                &(pgb_instream->a_dstframe));
            spl_mutex_lock(amutex);
            do {
                if(shared_abuf->range > 
                    shared_abuf->pl + 
                    pgb_instream->a_dstframe->linesize[0]) 
                {
                    memcpy(shared_abuf->data + 
                            shared_abuf->pl, 
                        pgb_instream->a_dstframe->data[0], 
                        pgb_instream->a_dstframe->linesize[0]
                    );
                    shared_abuf->pl += 
                        pgb_instream->a_dstframe->linesize[0];
                } else {
                    shared_abuf->pl = 0;
                    shared_abuf->pc = 0;
                    spllog(1, "over audio range");
                }
                spllog(1, "(pl, pc, range)=(%d, %d, %d)", 
                    shared_abuf->pl, 
                    shared_abuf->pc, 
                    shared_abuf->range);
            } while(0);
            spl_mutex_unlock(amutex);
#endif							
				spl_vframe(pgb_instream->a_dstframe);
				av_frame_unref(pgb_instream->a_dstframe); 
				av_frame_unref(pgb_instream->a_frame);   				
			}
		}
		/*-----------------*/
	} while(0);
	
	ffwr_frame_free(&tmp); 
	ffwr_free(ffwr_vframe);
	
	ffwr_close_instream(obj);
	
	if(obj->input.cb) {
		obj->input.sz_type.type = FFWR_DEMUX_THREAD_EXIT;
		obj->input.cb(obj);
	}
	
	return ret;
}

#else
#endif
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_convert_vframe_ext(FFWR_INSTREAM *p, AVFrame *src, AVFrame *dst)
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
   


    if(! (p->vscale)) { 
        //p->vscale = sws_getContext(
	    //	src->width, 
	    //	src->height, 
	    //	src->format, 
	    //	dst->width, 
	    //	dst->height, 
	    //	dst->format,
        //    SWS_BILINEAR, NULL, NULL, NULL
        //);
        ffwr_sws_getContext(
			p->vscale,
	    	src->width, 
	    	src->height, 
	    	src->format, 
	    	dst->width, 
	    	dst->height, 
	    	dst->format,
            SWS_BILINEAR, NULL, NULL, NULL
        );
    }
    if (!p->vscale) {
        spllog(4, "Error: cannot create sws context\n");
        return -1;
    }

    ret = sws_scale(
        p->vscale,
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
int ffwr_get_stopping(FFWR_DEMUX_OBJS *obj) {
    int ret = 0;
    spl_mutex_lock(obj->buffer.mtx_vbuf);
        ret = obj->isstop;
    spl_mutex_unlock(obj->buffer.mtx_vbuf);
    return ret;;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ffwr_set_stopping(FFWR_DEMUX_OBJS *obj, int v) {
    spl_mutex_lock(obj->buffer.mtx_vbuf);
        obj->isstop = v;
    spl_mutex_unlock(obj->buffer.mtx_vbuf);
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
int convert_audio_frame_ext(FFWR_INSTREAM *p, AVFrame *src, AVFrame **outfr)
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
			ffwr_frame_alloc(dst);
            if(!dst) {
                break;
            }
          
        }

        av_channel_layout_default(&dst->ch_layout, 2);
        dst->format = AV_SAMPLE_FMT_FLT;
        dst->sample_rate = FFWR_OUTPUT_ARATE;  

        if(!p->a_scale) {
            ret = ffwr_create_a_swrContext_ext(p, src, dst);
            if(ret) {
                break;
            }
        }
        if(!p->a_cctx) {
            ret = 1;
            break;            
        }
        src_layout = &(p->a_cctx->ch_layout);
        if(!src_layout) {
            ret = 1;
            break;                    
        }

        dst->nb_samples = av_rescale_rnd(
            swr_get_delay(p->a_scale, 
                src->sample_rate) + src->nb_samples, 
                FFWR_OUTPUT_ARATE, src->sample_rate, AV_ROUND_UP);

        n = av_frame_get_buffer(dst, 0);
	    if (n < 0) {
	    	spllog(4, "Error: cannot allocate dst buffer (%d)\n", n);
            ret = 1;
	    	break;
	    }
        av_channel_layout_copy(&(dst->ch_layout), src_layout);
	    n = swr_convert(p->a_scale, dst->data, dst->nb_samples,
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
int ffwr_create_a_swrContext_ext(FFWR_INSTREAM *p, AVFrame *src, AVFrame *dst) {
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
		
        ffwr_swr_alloc_set_opts2(
            &swr,                        	// NULL → Create new one
            &(dst->ch_layout),         		// output channel
            dst->format,          			// output format
            dst->sample_rate,               // sample rate output
            &(src->ch_layout),           	// input channel
            src->format,           			// layout of outpu
            src->sample_rate,               // sample rate of input
            0, 
			0                      			// log offset, log context
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

        p->a_scale = swr;

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

int ffwr_open_audio_output(FFWR_DEMUX_OBJS *obj, int sz)
{
    int ret = 0;
    int insz = 3 * sz;
    do {

        gb_want.freq = FFWR_OUTPUT_ARATE;
        gb_want.format = AUDIO_F32SYS;
        gb_want.channels = 2;
        gb_want.samples = 4096;        // kích thước buffer SDL
        gb_want.callback = ffwr_open_audio_output_cb;
        gb_want.userdata = obj; // buffer 
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
    ffwr_gen_data_st *obj = 0;
	FFWR_DEMUX_OBJS *demux = 0;
    int real_len = 0;
    //static int step = 0;
    int stopping = 0;
	void *amutex = 0;
	FFWR_DEMUX_DATA *bufffer = 0;
	
	demux = (FFWR_DEMUX_OBJS *)user;
	stopping = ffwr_get_stopping(demux);
	
	if(stopping) {
		return;
	}
	
	bufffer = &(demux->buffer);
	obj = bufffer->abuf;	
	
    if(!obj) {
        return;
    }

    if( obj->pl <= obj->pc) 
    {
        obj->pl = obj->pc = 0;
        spl_mutex_lock(amutex);
        do {
            if(bufffer->shared_abuf->pl < 1) {
                break;
            }
            //if(step == 0) {
            //    if(bufffer->shared_abuf->pl > 0) {
            //        step++;
            //        bufffer->shared_abuf->pc = 0;
            //        bufffer->shared_abuf->pl = 0;
            //        break;
            //    }
            //}
            //if(bufffer->shared_abuf->pl < 1) {
            //    break;
            //}

            memcpy(obj->data + obj->pl, 
                bufffer->shared_abuf->data, 
                bufffer->shared_abuf->pl);

            obj->pl += bufffer->shared_abuf->pl;

            bufffer->shared_abuf->pc = 0;
            bufffer->shared_abuf->pl = 0;
        } while(0);
        spl_mutex_unlock(amutex);
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
            spl_mutex_lock(amutex);
            do {
                if(obj->range >= obj->pl + bufffer->shared_abuf->pl) {
                    memcpy(obj->data + obj->pl, 
                        bufffer->shared_abuf->data, 
                        bufffer->shared_abuf->pl);

                    obj->pl += bufffer->shared_abuf->pl;
                }

                bufffer->shared_abuf->pc = 0;
                bufffer->shared_abuf->pl = 0;
            } while(0);
            spl_mutex_unlock(amutex);
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
		/*------------*/
		if(p->sdl_texture) {
			ffwr_SDL_DestroyTexture(p->sdl_texture);
		}
		if(p->sdl_render) {
			ffwr_SDL_DestroyRenderer(p->sdl_render);
		}
		if(p->sdl_window) {
			ffwr_SDL_DestroyWindow(p->sdl_window);
		}		
		p->sdl_texture = 0;;
		p->sdl_render = 0;;
		p->sdl_window = 0;;
	} while(0);
	return ret;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int 
ffwr_create_demux_objects(FFWR_DEMUX_OBJS *obj) 
{
	int ret = 0;
	do {
		ret = ffwr_create_sync_buff(obj);
		if(ret) {
			spllog(4, "ffwr_create_sync_buff");
			break;
		}		
		ret = ffwr_create_demux_ext(obj);
		if(ret) {
			spllog(4, "ffwr_create_demux_ext");
			break;
		}
		ret = ffwr_open_render_sdl_pipe(obj);
		if(ret) {
			spllog(4, "ffwr_open_render_sdl_pipe");
			break;
		}		
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
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
*/
int 
ffwr_destroy_demux_objects(FFWR_DEMUX_OBJS *obj) 
{
	int ret = 0;
	FFWR_INSTREAM *inner_demux = 0;
	do {
		if(!obj) {
			ret = FFWR_DEMUX_OBJS_NULL_ERR;
			spllog(4, "FFWR_DEMUX_OBJS_NULL_ERR");
			break;
		}	
		/*Clear: 
			typedef struct __FFWR_DEMUX_OBJS__ {
				int isstop;
				3. FFWR_RENDER_OBJECTS render_objects;
				1. void *inner_demux = 0; 
				//Did clear in demux thread.
				2. FFWR_DEMUX_DATA buffer;
				FFWR_INPUT_ST input;
			} FFWR_DEMUX_OBJS;
		
		*/

		/*-------*/
		/*Clear: FFWR_DEMUX_DATA
			typedef struct __FFWR_DEMUX_DATA__
			{
				int vbuf_size;
				ffwr_gen_data_st * vbuf; 
				ffwr_gen_data_st * shared_vbuf; 
				void *mtx_vbuf; 
				
				int abuf_size;
				ffwr_gen_data_st * abuf;
				ffwr_gen_data_st * shared_abuf; 
				void *; 
				
			} FFWR_DEMUX_DATA;
		*/
		ffwr_free(obj->buffer.vbuf);
		ffwr_free(obj->buffer.shared_vbuf);
		ffwr_free(obj->buffer.abuf);
		ffwr_free(obj->buffer.shared_abuf);
		ffwr_destroy_mutex(obj->buffer.mtx_vbuf);
		obj->buffer.mtx_vbuf = 0;
		ffwr_destroy_mutex(obj->buffer.mtx_abuf);
		obj->buffer.mtx_abuf = 0;
		/*-------*/
		ffwr_destroy_render_objects(&(obj->render_objects));
		/*-------*/
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_open_instream(FFWR_DEMUX_OBJS *obj)
{
	int result = 0;
	int ret = 0;
	FFWR_INSTREAM *pinput = 0;
	int sz = sizeof(FFWR_INSTREAM);
    AVInputFormat *iformat = 0; 
	char *name = 0;
    AVDictionary *options = 0;	
	
	do {
		if(!obj) {
			ret = FFWR_DEMUX_OBJS_NULL_ERR;
			spllog(4, "FFWR_DEMUX_OBJS_NULL_ERR");
			break;
		}	
		
		ffwr_malloc(sz, pinput, FFWR_INSTREAM);
		if(!pinput) {
			ret = FFWR_MALLOC_ERR;
			spllog(4, "FFWR_MALLOC_ERR");
			break;
		}
		obj->inner_demux = pinput;
		if (!av_dict_get(options, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
			av_dict_set(&options, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		}        
		name = obj->input.name;
		spllog(1, "name: %s", name);
		

        ffwr_avformat_open_input(result, 
			&(pinput->fmt_ctx), 
			name,  iformat, &options);

        if(result < 0) {
            ret = FFWR_AV_OPEN_INPUT_ERR;
            spllog(4, "FFWR_AV_OPEN_INPUT_ERR, name: %s", name);
            break;
        }	
        result = avformat_find_stream_info(pinput->fmt_ctx, 0);
        if(result < 0) {
            ret = FFWR_AV_FIND_STREAM_INFO_ERR;
            spllog(4, "FFWR_AV_FIND_STREAM_INFO_ERR");
            break;
        }
        if(pinput->fmt_ctx->nb_streams < 1) {
            ret = FFWR_AV_NB_STREAMS_ERR;
            spllog(4, "FFWR_AV_NB_STREAMS_ERR");
            break;
        }
        pinput->v_st = pinput->fmt_ctx->streams[0];
        if(!pinput->v_st) {
            ret = FFWR_NO_VSTREAMS_ERR;
            spllog(4, "FFWR_NO_VSTREAMS_ERR");
            break;
        }		
        pinput->v_codec = avcodec_find_decoder(
            pinput->v_st->codecpar->codec_id);
        if(!pinput->v_codec) {
            ret = FFWR_NO_VCODEC_ERR;
            spllog(4, "FFWR_NO_VCODEC_ERR");
            break;
        }        
        ffwr_avcodec_alloc_context3(
			pinput->v_cctx, pinput->v_codec);
        if(!pinput->v_cctx) {
            ret = FFWR_NO_VCONTEXT_ERR;
            spllog(4, "FFWR_NO_VCONTEXT_ERR");
            break;
        }    
        result = avcodec_parameters_to_context(pinput->v_cctx, pinput->v_st->codecpar);
        if(result < 0) {
            ret = FFWR_PARAMETERS_TO_CONTEXT_ERR;
            spllog(4, "FFWR_PARAMETERS_TO_CONTEXT_ERR");
            break;
        }
		result = avcodec_open2(pinput->v_cctx, pinput->v_codec, 0);
		if (result < 0) {
			ret = FFWR_OPEN_VCODEC_ERR;
            spllog(4, "avcodec_open2, result: %d", result);
			break;
		}        
		ffwr_frame_alloc(pinput->vframe);
        if(!pinput->vframe) {
            ret = FFWR_VFRAME_ALLOC_ERR;
            spllog(4, "FFWR_VFRAME_ALLOC_ERR");
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
            spllog(1, "FFWR_NO_VSTREAM_ERR");
            break;
        }
		
        pinput->a_codec = avcodec_find_decoder(
            pinput->a_st->codecpar->codec_id);

        if(!pinput->a_codec) {
            ret = FFWR_NO_ACONTEXT_ERR;
            spllog(4, "FFWR_NO_ACONTEXT_ERR");
            break;
        }      
        ffwr_avcodec_alloc_context3(
			pinput->a_cctx, pinput->a_codec);
        if(!pinput->a_cctx) {
            ret = FFWR_ALLOC_ACONTEX_ERR;
            spllog(4, "FFWR_ALLOC_ACONTEX_ERR");
            break;
        }   
        result = avcodec_parameters_to_context(pinput->a_cctx, pinput->a_st->codecpar);
        if(result < 0) {
            ret = FFWR_PARAMETERS_TO_ACONTEXT_ERR;
            spllog(4, "FFWR_PARAMETERS_TO_ACONTEXT_ERR");
            break;
        }   
		result = avcodec_open2(pinput->a_cctx, pinput->a_codec, 0);
		if (result < 0) {
			ret = FFWR_OPEN_ACODEC_ERR;
            spllog(4, "FFWR_OPEN_ACODEC_ERR");
			break;
		} 
        ffwr_frame_alloc(pinput->a_frame); 
        if(!pinput->a_frame) {
            ret = FFWR_AFRAME_ALLOC_ERR;
            spllog(4, "FFWR_AFRAME_ALLOC_ERR");
            break;
        }   
		ffwr_frame_alloc(pinput->a_dstframe);
        if(!pinput->a_dstframe) {
            ret = FFWR_AFRAME_ALLOC_ERR;
            spllog(4, "FFWR_AFRAME_ALLOC_ERR");
            break;
        }   
        spllog(1, "openInout OK, ret: %d", ret);		
	} while(0);
	return ret;
}


/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_close_instream(FFWR_DEMUX_OBJS *obj)
{
	int ret = 0;
	FFWR_INSTREAM *inner_demux = 0;
	
	do {
		if(!obj) {
			ret = FFWR_DEMUX_OBJS_NULL_ERR;
			spllog(4, "FFWR_DEMUX_OBJS_NULL_ERR");
			break;
		}
		inner_demux = (FFWR_INSTREAM *) obj->inner_demux;
		if(!inner_demux) {
			break;
		}
		if(inner_demux->vframe) {
			ffwr_frame_free(&(inner_demux->vframe));
			inner_demux->vframe = 0;
		}
		if(inner_demux->a_dstframe) {
			ffwr_frame_free(&(inner_demux->a_dstframe)); 
			inner_demux->a_dstframe = 0;
		}
		if(inner_demux->a_frame) {
			ffwr_frame_free(&(inner_demux->a_frame));   
			inner_demux->a_frame = 0;
		}	
		/*-------*/
		ffwr_packet_unref(&(inner_demux->pkt));
	
		if(inner_demux->vscale) {
			ffwr_sws_freeContext(inner_demux->vscale);
			inner_demux->vscale = 0;
		}
		if(inner_demux->a_scale) {

			ffwr_swr_free(&(inner_demux->a_scale));
			inner_demux->a_scale = 0;
		}
		if(inner_demux->v_cctx) {
			ffwr_avcodec_free_context(
				&(inner_demux->v_cctx));
		}
		if(inner_demux->a_cctx) {
			ffwr_avcodec_free_context(
				&(inner_demux->a_cctx));
		}
		if(inner_demux->fmt_ctx) {
			ffwr_avformat_close_input(
				&(inner_demux->fmt_ctx));
		}	
		ffwr_free(obj->inner_demux);
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int
ffwr_open_render_sdl_pipe(FFWR_DEMUX_OBJS *obj)
{
	int ret = 0;
	FFWR_RENDER_OBJECTS *p = 0;
	SDL_Window *win = 0;
	SDL_Renderer *ren = 0;
	SDL_Texture *texture = 0;
	
	do 
	{
		if(!obj) {
			ret = FFWR_DEMUX_OBJS_NULL_ERR;
			spllog(4, "FFWR_DEMUX_OBJS_NULL_ERR");
			break;
		}	
		p = &(obj->render_objects);
		if(!p->native_window) {
			ret = 0;
			spllog(1, "For test none of GUI!");
			break;			
		}

		ffwr_SDL_CreateWindowFrom(p->sdl_window, p->native_window);
		if(!p->sdl_window) {
			ret = FFWR_CANNOT_CREATE_WIN_ERR;
			spllog(4, "SDL_CreateWindowFrom: %s\n", SDL_GetError());
			break;
		}
		win = (SDL_Window *) p->sdl_window;
		//ren = SDL_CreateRenderer(win, -1, p->ren_flags);
	
		ffwr_SDL_CreateRenderer(ren, win, -1, SDL_RENDERER_ACCELERATED);
		if(!ren) {
			ret = FFWR_CREATERENDERER_ERR;
			spllog(4, "SDL_CreateRenderer: %s\n", SDL_GetError());
			break;
		}
		p->sdl_render = ren;

		
        ffwr_SDL_CreateTexture(texture, ren,
            p->format, p->access, p->w, p->h);		
		if(!texture) {
			spllog(4, "SDL_CreateTexture: %s\n", SDL_GetError());
			break;
		}			
		p->sdl_texture =  texture;
	} 
	while(0);
	
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int 
ffwr_create_sync_buff(FFWR_DEMUX_OBJS *obj) 
{
	int ret = 0;
	FFWR_DEMUX_DATA *p = 0;
	void *mtx = 0;
	ffwr_gen_data_st *buf = 0;
	do {
		if(!obj) {
			ret = FFWR_DEMUX_OBJS_NULL_ERR;
			spllog(4, "FFWR_DEMUX_OBJS_NULL_ERR");
			break;
		}
		p = &(obj->buffer);
		ffwr_create_mutex(&mtx, 0);
		if(!mtx) {
			ret = FFWR_CREATE_MUTEX_NULL_ERR;
			break;
		}
		p->mtx_vbuf = mtx;
		
		ffwr_create_mutex(&mtx, 0);
		if(!mtx) {
			ret = FFWR_CREATE_MUTEX_NULL_ERR;
			break;
		}		
		p->mtx_abuf = mtx;
		/*-----*/
		buf = 0;
		ret = ffwr_create_genbuff(&buf, p->vbuf_size);
		if(ret) {
			break;
		}
		p->vbuf = buf;

		buf = 0;
		ret = ffwr_create_genbuff(&buf, p->vbuf_size);
		if(ret) {
			break;
		}
		p->shared_vbuf = buf;

		buf = 0;
		ret = ffwr_create_genbuff(&buf, p->abuf_size);
		if(ret) {
			break;
		}
		p->abuf = buf;		
		
		buf = 0;
		ret = ffwr_create_genbuff(&buf, p->abuf_size);
		if(ret) {
			break;
		}
		p->shared_abuf = buf;
		
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_create_mutex(void **outmutex, char *name)
{
	void *obj = 0;
	int ret = 0;
	do {
		if(!outmutex) {
			ret = FFWR_WIN_OUTMUTEX_NULL_ERR;
			break;
		}
#ifndef UNIX_LINUX
		obj = CreateMutexA(0, 0, name);
		if(!obj) {
			spllog(4, "GetLastError(): %d", 
			GetLastError());
			ret = FFWR_WIN_CREATE_MUTEX_ERR;
			break;
		}
		*outmutex = obj;
#else
#endif		
	} while(0);
	
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ffwr_create_genbuff(ffwr_gen_data_st **obj, int sz) 
{
    int ret = 0;
    ffwr_gen_data_st *tmp = 0;
    do {
		if(!obj) {
			ret = FFWR_GEN_DATA_NULL_ERR;
			spllog(4, "FFWR_GEN_DATA_NULL_ERR");
			break;
		}
		ffwr_malloc(sz, tmp, ffwr_gen_data_st);
		if(!tmp) {
			ret = FFWR_MALLOC_ERR;
			break;
		}
        tmp->total = sz;
        tmp->range = sz - sizeof(ffwr_gen_data_st);
        tmp->pl = tmp->pc = 0;
		*obj = tmp;
    } while(0);
    return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_create_demux_ext(void *obj)
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
int 
ffwr_destroy_mutex(void *obj)
{
	int ret = 0;
	do {
#ifndef UNIX_LINUX	
		if(obj) {
			CloseHandle(obj);
		}
#else
#endif			
	} while(0);

	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_create_semaphore(void **outsem, char *name)
{
	void *obj = 0;
	int ret = 0;
	do {
		if(!outsem) {
			ret = FFWR_OUTSEM_NULL_ERR;
			break;
		}
#ifndef UNIX_LINUX	
/*
HANDLE CreateSemaphoreA(
  [in, optional] LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
  [in]           LONG                  lInitialCount,
  [in]           LONG                  lMaximumCount,
  [in, optional] LPCSTR                lpName
);
*/
		obj = CreateSemaphoreA(0, 0, 1, name);
		if(!obj) {
			spllog(4, "GetLastError: %d", 
			(int)GetLastError());
			ret = FFWR_WIN_CREATE_SEM_ERR;
			break;
		}
		*outsem = obj;
#else
#endif			
	} while(0);	
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_destroy_semaphore(void *obj)
{
	int ret = 0;
	do {
		if(!obj) {
			ret = FFWR_SEM_NULL_ERR;
			break;
		}
#ifndef UNIX_LINUX	
		if(obj) {
			int done = 0;
			done = CloseHandle(obj);
			if(!done) {
				ret = FFWR_WIN_CLOSE_SEM_ERR;
				spllog(4, "GetLastError: %d", 
					(int)GetLastError());				
			}
		}
#else
#endif			
	} while(0);
	
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_semaphore_post(void *obj)
{
	int ret = 0;
	do {
		if(!obj) {
			ret = FFWR_SEM_NULL_ERR;
			break;
		}
	} while(0);
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int
ffwr_semaphore_wait(void *obj)
{
	int ret = 0;
	return ret;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
