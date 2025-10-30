#pragma once
#include <afxwin.h>
#include <simplelog.h>
#include <ffwr_render.h>

//#include <SDL3/SDL_syswm.h>
class FFWRVideoFrame : public CStatic
{
public:
	FFWRVideoFrame();
	virtual ~FFWRVideoFrame();

/*---------------------------------*/

	static int sdl_init;
	static unsigned int sdl_flag;
	static void *sdl_texture;
	static void *sdl_render;
	static void sdl_quit();

/*---------------------------------*/
	void create_sdlwin();


protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
	void *sdl_window;
};


