#pragma once
#include <afxwin.h>
#include <simplelog.h>
#include <ffwr_render.h>

#define WM_FFWR_MESSAGE (WM_USER + 10)

class FFWRVideoFrame : public CStatic
{
public:
	FFWRVideoFrame();
	virtual ~FFWRVideoFrame();
	void render(int increase);

/*---------------------------------*/

	static int sdl_init;
	static unsigned int sdl_flag;
	//static void *sdl_texture;
	//static void *sdl_render;
	static void sdl_Quit();
	static void sdl_Init();

/*---------------------------------*/
	void xyz();
	void *get_demux_obj();
	void stopxyx();
	

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg LRESULT OnFFWRMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

    BOOL m_bDragging; //
	CPoint m_ptStart; //
    int running;

  private:
	FFWR_DEMUX_OBJS obj_demux;
	
};


