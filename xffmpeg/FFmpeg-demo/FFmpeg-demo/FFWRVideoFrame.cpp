#include "pch.h"
#include "FFWRVideoFrame.h"


#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")

//void *gb_texture = 0;
//void *render = 0;

BEGIN_MESSAGE_MAP(FFWRVideoFrame, CStatic)
ON_WM_PAINT()
END_MESSAGE_MAP()

void *ref_ffwr_mtx = 0;

void FFWRVideoFrame::OnPaint()
{
	FFWR_VFrame *p = 0;
	FFWR_SIZE_TYPE *it = 0;
	ffwr_gen_data_st *gb_frame = 0;
	ffwr_gen_data_st *gb_tsplanVFrame = 0;

	gb_frame = ffwr_gb_frame();
	if (!gb_frame) {
		spllog(3, "gb_frame null");
		return;
	}

	gb_tsplanVFrame = ffwr_gb_shared_vframe();
	if (!gb_tsplanVFrame) {
		spllog(3, "gb_tsplanVFrame null");
		return;
	}

	if (!ref_ffwr_mtx) {
		ref_ffwr_mtx = ffwr_mutex_data();
		if (!ref_ffwr_mtx) {
			spllog(4, "ref_ffwr_mtx null");
			return;
		}
	}
	if (!this->sdl_window) {
		spllog(4, "sdl_window null");
		return;
	}
	if (!this->sdl_window) {
		spllog(4, "sdl_window null");
		return;
	}
	if (!FFWRVideoFrame::sdl_render) {
		spllog(4, "FFWRVideoFrame::sdl_render null");
		return;
	}
	if (!FFWRVideoFrame::sdl_texture) {
		spllog(4, "FFWRVideoFrame::sdl_texture null");
		return;
	}
	spllog(1, "window, sdl_render, sdl_texture");
	spl_mutex_lock(ref_ffwr_mtx);
	do {
	} while (0);
	spl_mutex_unlock(ref_ffwr_mtx);
}

FFWRVideoFrame::FFWRVideoFrame()
{
	int ret = 0;
	sdl_window = 0;
	if (!sdl_init) {
		ret = ffwr_init(FFWRVideoFrame::sdl_flag);
		if (ret) {
			spllog(4, "ffwr_init");
		}
		sdl_init = 1;
	}
}

FFWRVideoFrame::~FFWRVideoFrame()
{
}

void
FFWRVideoFrame::sdl_quit()
{
	if (FFWRVideoFrame::sdl_init) {

		FFWRVideoFrame::sdl_init = 0;
		ffwr_Quit();
	}
}
void
FFWRVideoFrame::create_sdlwin()
{
	int ret = 0;
	void *win = 0;
	ret = ffwr_CreateWindowFrom(this->m_hWnd, &win);
	if (ret) {
		spllog(4, "Error");
		return;
	}
	this->sdl_window = win;
	if (!FFWRVideoFrame::sdl_texture) {
		void *render = 0;
		ret = ffwr_CreateRenderer(&render, win, -1, FFWR_RENDERER_ACCELERATED);
		if (ret) {
			spllog(4, "Error");
			return;
		}
		FFWRVideoFrame::sdl_render = render;
	}
	if (!FFWRVideoFrame::sdl_texture) {
		void *texture = 0;
		ret = ffwr_CreateTexture(&texture, FFWRVideoFrame::sdl_render,
		    FFWR_PIXELFORMAT_IYUV, FFWR_TEXTUREACCESS_STREAMING, 640,
		    480);
		if (ret) {
			spllog(4, "Error");
			return;		
		}
		FFWRVideoFrame::sdl_texture = texture;
	}
}
int FFWRVideoFrame::sdl_init = 0;
unsigned int FFWRVideoFrame::sdl_flag = (FFWR_INIT_AUDIO | FFWR_INIT_VIDEO);
void *FFWRVideoFrame::sdl_texture = 0;
void *FFWRVideoFrame::sdl_render = 0;