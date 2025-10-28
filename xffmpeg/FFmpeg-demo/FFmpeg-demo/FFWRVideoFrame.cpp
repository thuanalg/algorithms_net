#include "pch.h"
#include "FFWRVideoFrame.h"

#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")

// Global pointers ()
ID3D11Device *gb_ffwr_d3dDevice = nullptr;
ID3D11DeviceContext *gb_ffwr_d3dContext = nullptr;
IDXGISwapChain *gb_ffwr_swapChain = nullptr;
ID3D11RenderTargetView *gb_ffwr_renderTargetView = nullptr;

BEGIN_MESSAGE_MAP(FFWRVideoFrame, CStatic)
ON_WM_PAINT()
END_MESSAGE_MAP()

void
FFWRVideoFrame::OnPaint()
{
	spllog(1, "---");
	CPaintDC dc(this); 
	CRect rect;
	GetClientRect(&rect);

	dc.FillSolidRect(&rect, RGB(255, 255, 255));

	dc.SetTextColor(RGB(0, 0, 255));
	dc.SetBkMode(TRANSPARENT);
	dc.DrawText(_T("Hello MFCCstatic!"), -1, &rect,
	    DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")



HWND gbffwr_hWnd; 

bool
InitD3D11(HWND hWnd)
{
	gbffwr_hWnd = hWnd;

	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
#if defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr =
	    D3D11CreateDeviceAndSwapChain(nullptr, // adapter: 
		D3D_DRIVER_TYPE_HARDWARE, // driver: hardware
		0, // hModule: 
		createDeviceFlags, // flags
		0, 0, // feature levels (auto)
		D3D11_SDK_VERSION, // SDK version
		&scd, // swap chain description
		&gb_ffwr_swapChain, // output swap chain
		&gb_ffwr_d3dDevice, // output device
		&featureLevel, // output feature level
		&gb_ffwr_d3dContext // output context
	    );

	if (FAILED(hr)) {
		MessageBox(hWnd, L"Failed to create D3D11 device!", L"Error",
		    MB_ICONERROR);
		return false;
	}

	// Tạo render target view
	ID3D11Texture2D *pBackBuffer = nullptr;
	gb_ffwr_swapChain->GetBuffer(
	    0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
	gb_ffwr_d3dDevice->CreateRenderTargetView(
	    pBackBuffer, nullptr, &gb_ffwr_renderTargetView);
	pBackBuffer->Release();

	gb_ffwr_d3dContext->OMSetRenderTargets(
	    1, &gb_ffwr_renderTargetView, nullptr);

	return true;
}
