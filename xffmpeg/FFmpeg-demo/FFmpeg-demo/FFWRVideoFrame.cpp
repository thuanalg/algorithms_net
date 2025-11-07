#include "pch.h"
#include "FFWRVideoFrame.h"


#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")




BEGIN_MESSAGE_MAP(FFWRVideoFrame, CStatic)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_MESSAGE(WM_FFWR_MESSAGE, &FFWRVideoFrame::OnFFWRMessage)
END_MESSAGE_MAP()

void *ref_ffwr_mtx = 0;
void
FFWRVideoFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDragging = TRUE;
	SetCapture();
	ClientToScreen(&point);
	m_ptStart = point;
	CStatic::OnLButtonDown(nFlags, point);
}

void
FFWRVideoFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		ClientToScreen(&point);
		CPoint delta = point - m_ptStart;

		// get current rect
		CRect rc;
		GetWindowRect(&rc);

		// new rect = old rect + delta
		rc.OffsetRect(delta);

		// convert to parent client coordinates
		GetParent()->ScreenToClient(&rc);

		MoveWindow(&rc);
		m_ptStart = point;
	}
	CStatic::OnMouseMove(nFlags, point);
}

void
FFWRVideoFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		m_bDragging = FALSE;
		ReleaseCapture();
	}
	CStatic::OnLButtonUp(nFlags, point);
}
void FFWRVideoFrame::OnPaint()
{
	FFWR_VFrame *p = 0;
	FFWR_SIZE_TYPE *it = 0;
	ffwr_gen_data_st *gb_frame = 0;
	ffwr_gen_data_st *gb_tsplanVFrame = 0;
#if 0
	CPaintDC dc(this); 

	CBrush brush(RGB(0, 0, 0)); // màu xanh nhạt

	CRect rect;
	GetClientRect(&rect);

	dc.FillRect(&rect, &brush);
	return;
#endif
	//gb_frame = ffwr_gb_renderVFrame();

	gb_frame = obj_demux.buffer.vbuf;
	if (!gb_frame) {
		spllog(1, "gb_frame null");
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
	//spllog(1, "window, sdl_render, sdl_texture");
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
		int isstop = 0;
		isstop = ffwr_get_stopping(&obj_demux);
		if (isstop) {
			gb_tsplanVFrame->pl = gb_tsplanVFrame->pc = 0;
		} else {
			gb_frame->pc += it->total;
		}
	}
}

FFWRVideoFrame::FFWRVideoFrame()
{
	int ret = 0;
	m_bDragging = 0;
	memset(&obj_demux, 0, sizeof(obj_demux));
	if (!sdl_init) {
		ret = ffwr_init(FFWRVideoFrame::sdl_flag);
		if (ret) {
			spllog(4, "ffwr_init");
		}
		sdl_init = 1;
	}

	obj_demux.render_objects.w = 640;
	obj_demux.render_objects.h = 480;
	obj_demux.render_objects.format = FFWR_PIXELFORMAT_IYUV;
	obj_demux.render_objects.access = FFWR_TEXTUREACCESS_STREAMING;
	obj_demux.render_objects.ren_flags = (FFWR_INIT_AUDIO | FFWR_INIT_VIDEO);
	obj_demux.buffer.vbuf_size = 10000000;
	obj_demux.buffer.abuf_size = 6000000;
	obj_demux.render_objects.native_window = this->m_hWnd;

	ffwr_init_demux_objects(&obj_demux);
}

FFWRVideoFrame::~FFWRVideoFrame()
{
}

void
FFWRVideoFrame::xyz()
{
	int ret = 0;
	ModifyStyle(0, SS_NOTIFY);

	if (obj_demux.buffer.vbuf) {
		ffwr_gen_data_st *gb_frame = 0;
		gb_frame = obj_demux.buffer.vbuf;
		gb_frame->pc = 0;
		gb_frame->pl = 0;
	}
	obj_demux.isstop = 0;
	obj_demux.render_objects.native_window = this->m_hWnd;
	//this->obj_demux.render_objects.
	snprintf(obj_demux.input.name, sizeof(obj_demux.input.name), 
		"%s", "tcp://127.0.0.1:12345");
	//snprintf(obj_demux.input.name, sizeof(obj_demux.input.name), "%s",
	//    "C:/Users/DEll/Desktop/A1-TS_00_d.ts");
	ret = ffwr_create_demux_objects(&obj_demux);
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
		//ffwr_destroy_demux_objects(p);
		
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
