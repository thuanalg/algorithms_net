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

/*---------------------------------*/

	static int sdl_init;
	static unsigned int sdl_flag;
	//static void *sdl_texture;
	//static void *sdl_render;
	static void sdl_quit();
	static void sdl_Init();

/*---------------------------------*/
	void create_sdlwin();


protected:
	afx_msg void OnPaint();
	afx_msg LRESULT OnFFWRMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	//void *sdl_window;
	//void *sdl_render;
	//void *sdl_texture;
	//void clearSDLWinRenderContext();
	FFWR_RENDER_OBJECTS sdl_winrentext;
};


