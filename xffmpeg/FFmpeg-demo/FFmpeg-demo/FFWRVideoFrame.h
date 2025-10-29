#pragma once
#include <afxwin.h>
#include <simplelog.h>

//#include <SDL3/SDL_syswm.h>
class FFWRVideoFrame : public CStatic
{
public:
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
};
int
ffwr_InitD3D11(HWND hWnd);

int
ffwr_CloseD3D11();

