#include "pch.h"
#include "FFWRVideoFrame.h"

#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")

void updateFakeTexture();
void
createFakeTexture();

// Global pointers ()
ID3D11Device *gb_ffwr_d3dDevice = 0;
ID3D11DeviceContext *gb_ffwr_d3dContext = 0;
IDXGISwapChain *gb_ffwr_swapChain = 0;
ID3D11RenderTargetView *gb_ffwr_renderTargetView = 0;
ID3D11Texture2D *gb_ffwr_fakeTexture = 0;

BEGIN_MESSAGE_MAP(FFWRVideoFrame, CStatic)
ON_WM_PAINT()
END_MESSAGE_MAP()
#if 0
void FFWRVideoFrame::OnPaint()
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
#else

void
FFWRVideoFrame::OnPaint()
{
	CPaintDC dc(this);

	if (!gb_ffwr_d3dContext || !gb_ffwr_renderTargetView)
		return;
	if (!gb_ffwr_fakeTexture) {
		createFakeTexture();
		return;
	}
	// Clear màn hình
	float clearColor[4] = {0, 0, 0, 1};
	gb_ffwr_d3dContext->ClearRenderTargetView(
	    gb_ffwr_renderTargetView, clearColor);

	// Update fake texture
	updateFakeTexture();

	// Lấy back buffer của swapchain
	ID3D11Texture2D *backBuffer = nullptr;
	gb_ffwr_swapChain->GetBuffer(
	    0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBuffer);

	// Copy fakeTexture vào back buffer
	gb_ffwr_d3dContext->CopyResource(backBuffer, gb_ffwr_fakeTexture);

	backBuffer->Release();

	// Swap buffer hiển thị
	gb_ffwr_swapChain->Present(1, 0);
}

#endif



HWND gbffwr_hWnd; 

int
ffwr_InitD3D11(HWND hWnd)
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
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, // adapter: 
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
		return 1;
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

	return 0;
}

int
ffwr_CloseD3D11()
{
	// Ensure all pending commands are flushed
	if (gb_ffwr_d3dContext) {
		gb_ffwr_d3dContext->ClearState();
	}

	// Release render target
	if (gb_ffwr_renderTargetView) {
		gb_ffwr_renderTargetView->Release();
		gb_ffwr_renderTargetView = 0;
	}

	// Release swap chain
	if (gb_ffwr_swapChain) {
		gb_ffwr_swapChain->Release();
		gb_ffwr_swapChain = 0;
	}

	// Release device context
	if (gb_ffwr_d3dContext) {
		gb_ffwr_d3dContext->Release();
		gb_ffwr_d3dContext = 0;
	}

	// Release device
	if (gb_ffwr_d3dDevice) {
		gb_ffwr_d3dDevice->Release();
		gb_ffwr_d3dDevice = 0;
	}

	return 0;
}

const int WIDTH = 320;
const int HEIGHT = 240;
uint8_t fakeFrame[WIDTH * HEIGHT * 3];

void
gen_fake_frame()
{
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			int idx = (y * WIDTH + x) * 3;
			fakeFrame[idx + 0] = (uint8_t)x; // R
			fakeFrame[idx + 1] = (uint8_t)y; // G
			fakeFrame[idx + 2] = (uint8_t)((x + y) / 2); // B
		}
	}
}

void
createFakeTexture()
{
	if (!gb_ffwr_d3dDevice)
		return;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = WIDTH;
	desc.Height = HEIGHT;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA32
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.SampleDesc.Count = 1;

	gb_ffwr_d3dDevice->CreateTexture2D(&desc, nullptr, &gb_ffwr_fakeTexture);
}
void
updateFakeTexture()
{
	if (!gb_ffwr_fakeTexture || !gb_ffwr_d3dContext)
		return;

	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(gb_ffwr_d3dContext->Map(
		gb_ffwr_fakeTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
		uint8_t *dst = (uint8_t *)mapped.pData;
		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x < WIDTH; x++) {
				int srcIdx = (y * WIDTH + x) * 3;
				int dstIdx = y * mapped.RowPitch + x * 4;
				dst[dstIdx + 0] = fakeFrame[srcIdx + 0]; // R
				dst[dstIdx + 1] = fakeFrame[srcIdx + 1]; // G
				dst[dstIdx + 2] = fakeFrame[srcIdx + 2]; // B
				dst[dstIdx + 3] = 255; // A
			}
		}
		gb_ffwr_d3dContext->Unmap(gb_ffwr_fakeTexture, 0);
	}
}
