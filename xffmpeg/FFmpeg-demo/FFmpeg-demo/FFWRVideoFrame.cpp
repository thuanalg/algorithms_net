#include "pch.h"
#include "FFWRVideoFrame.h"


#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")




BEGIN_MESSAGE_MAP(FFWRVideoFrame, CStatic)
ON_WM_PAINT()
ON_MESSAGE(WM_FFWR_MESSAGE, &FFWRVideoFrame::OnFFWRMessage)
END_MESSAGE_MAP()

void *ref_ffwr_mtx = 0;

void FFWRVideoFrame::OnPaint()
{
	FFWR_VFrame *p = 0;
	FFWR_SIZE_TYPE *it = 0;
	ffwr_gen_data_st *gb_frame = 0;
	ffwr_gen_data_st *gb_tsplanVFrame = 0;

	//gb_frame = ffwr_gb_renderVFrame();
	gb_frame = obj_demux.buffer.vbuf;
	if (!gb_frame) {
		spllog(3, "gb_frame null");
		return;
	}

	//gb_tsplanVFrame = ffwr_gb_shared_vframe();	
	gb_tsplanVFrame = obj_demux.buffer.shared_vbuf;
	if (!gb_tsplanVFrame) {
		gb_tsplanVFrame = obj_demux.buffer.shared_vbuf;
		spllog(3, "gb_tsplanVFrame null");
		return;
	}

	if (!ref_ffwr_mtx) {
		//ref_ffwr_mtx = ffwr_mutex_data();
		ref_ffwr_mtx = obj_demux.buffer.mtx_vbuf;
		if (!ref_ffwr_mtx) {
			spllog(4, "ref_ffwr_mtx null");
			return;
		}
	}
	//if (!sdl_winrentext.sdl_window) {
	if (!obj_demux.render_objects.sdl_window) {
		spllog(4, "sdl_window null");
		return;
	}

	//if (!sdl_winrentext.sdl_render) {
	if (!obj_demux.render_objects.sdl_render) {
		spllog(4, "FFWRVideoFrame::sdl_render null");
		return;
	}
	//if (!sdl_winrentext.sdl_texture) {
	if (!obj_demux.render_objects.sdl_texture) {
		spllog(4, "FFWRVideoFrame::sdl_texture null");
		return;
	}
	spllog(1, "window, sdl_render, sdl_texture");
	if (gb_frame->pl < 1) {
		spl_mutex_lock(ref_ffwr_mtx);
		do {
			memcpy(gb_frame->data + gb_frame->pl,
			    gb_tsplanVFrame->data + gb_tsplanVFrame->pc,
			    gb_tsplanVFrame->pl - gb_tsplanVFrame->pc);

			gb_frame->pl +=
			    gb_tsplanVFrame->pl - gb_tsplanVFrame->pc;
			gb_tsplanVFrame->pl = gb_tsplanVFrame->pc = 0;
		} while (0);
		spl_mutex_unlock(ref_ffwr_mtx);
	}
	if (gb_frame->pl <= gb_frame->pc) {
		gb_frame->pl = gb_frame->pc = 0;
		return;
	}
	it = (FFWR_SIZE_TYPE *)(gb_frame->data + gb_frame->pc);
	p = (FFWR_VFrame *)(gb_frame->data + gb_frame->pc);
	ffwr_UpdateYUVTexture(obj_demux.render_objects.sdl_texture, 0,
	    p->data + p->pos[0],  p->linesize[0], 
		p->data + p->pos[1], p->linesize[1],
	    p->data + p->pos[2], p->linesize[2]

	);
	spllog(1, "pc render: %d, pts: %d", gb_frame->pc, p->pts);
	ffwr_RenderClear(obj_demux.render_objects.sdl_render);
	ffwr_RenderCopy(obj_demux.render_objects.sdl_render, 
		obj_demux.render_objects.sdl_texture,
	    0, 0);
	ffwr_RenderPresent(obj_demux.render_objects.sdl_render);
	if (it) {
		gb_frame->pc += it->total;
	}
}

FFWRVideoFrame::FFWRVideoFrame()
{
	int ret = 0;
	memset(&obj_demux, 0, sizeof(obj_demux));
	sdl_winrentext.sdl_window = 0;
	if (!sdl_init) {
		ret = ffwr_init(FFWRVideoFrame::sdl_flag);
		if (ret) {
			spllog(4, "ffwr_init");
		}
		sdl_init = 1;
	}

	//sdl_winrentext.sdl_render = 0;
	//sdl_winrentext.sdl_texture = 0;

	obj_demux.render_objects.w = 640;
	obj_demux.render_objects.h = 480;
	obj_demux.render_objects.format = FFWR_PIXELFORMAT_IYUV;
	obj_demux.render_objects.access = FFWR_TEXTUREACCESS_STREAMING;
	obj_demux.render_objects.ren_flags = (FFWR_INIT_AUDIO | FFWR_INIT_VIDEO);
	obj_demux.buffer.vbuf_size = 12000000;
	obj_demux.buffer.abuf_size = 12000000;
	obj_demux.render_objects.native_window = this->m_hWnd;
}

FFWRVideoFrame::~FFWRVideoFrame()
{
}

void
FFWRVideoFrame::xyz()
{
	obj_demux.render_objects.native_window = this->m_hWnd;
	snprintf(obj_demux.input.name, sizeof(obj_demux.input.name), 
		"%s", "tcp://127.0.0.1:12345");
	ffwr_create_demux_objects(&obj_demux);
}

void
FFWRVideoFrame::sdl_Quit()
{
	if (FFWRVideoFrame::sdl_init) {

		FFWRVideoFrame::sdl_init = 0;
		ffwr_Quit();
	}
}

void
FFWRVideoFrame::sdl_Init()
{
	ffwr_init(FFWRVideoFrame::sdl_flag);
}

void
FFWRVideoFrame::create_sdlwin()
{
	//int ret = 0;
	//void *win = 0;
	//ret = ffwr_CreateWindowFrom(this->m_hWnd, &win);
	//if (ret) {
	//	spllog(4, "Error");
	//	return;
	//}
	//sdl_winrentext.sdl_window = win;
	//if (!sdl_winrentext.sdl_texture) {
	//	void *render = 0;
	//	ret = ffwr_CreateRenderer(&render, win, -1, FFWR_RENDERER_ACCELERATED);
	//	if (ret) {
	//		spllog(4, "Error");
	//		return;
	//	}
	//	sdl_winrentext.sdl_render = render;
	//}
	//if (!sdl_winrentext.sdl_texture) {
	//	void *texture = 0;
	//	ret = ffwr_CreateTexture(&texture, sdl_winrentext.sdl_render,
	//	    FFWR_PIXELFORMAT_IYUV, FFWR_TEXTUREACCESS_STREAMING, 640,
	//	    480);
	//	if (ret) {
	//		spllog(4, "Error");
	//		return;		
	//	}
	//	sdl_winrentext.sdl_texture = texture;
	//}
}

void *
FFWRVideoFrame::get_demux_obj()
{
	return &(this->obj_demux);
}

LRESULT
FFWRVideoFrame::OnFFWRMessage(WPARAM wParam, LPARAM lParam)
{
	FFWR_DEMUX_OBJS *p = (FFWR_DEMUX_OBJS *)lParam;
	if (p && p->input.sz_type.type == FFWR_DEMUX_THREAD_EXIT) {
		ffwr_destroy_demux_objects(p);
		
	}
	return 0;
}

//void
//FFWRVideoFrame::clearSDLWinRenderContext()
//{
//
//}

int FFWRVideoFrame::sdl_init = 0;
unsigned int FFWRVideoFrame::sdl_flag = (FFWR_INIT_AUDIO | FFWR_INIT_VIDEO);
