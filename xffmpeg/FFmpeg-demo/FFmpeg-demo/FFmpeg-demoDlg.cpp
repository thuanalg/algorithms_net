
// FFmpeg-demoDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FFmpeg-demo.h"
#include "FFmpeg-demoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void xyz(HWND);

DWORD WINAPI
MyThreadProc(LPVOID lpParam);
    // CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFFmpegdemoDlg dialog

HWND gb_hwnd;

CFFmpegdemoDlg::CFFmpegdemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FFMPEGDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFFmpegdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFFmpegdemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CFFmpegdemoDlg message handlers

BOOL CFFmpegdemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	m_vframe.Create(_T("MFCCstatic"), // 
	    WS_CHILD | WS_VISIBLE | SS_BLACKFRAME, // style
	    CRect(0, 0, 640, 480), // 
	    this, //
	    1001 // ID control
	);
	gb_hwnd = m_vframe.m_hWnd;
	xyz(gb_hwnd);
	HANDLE hThread;
	DWORD dwThreadId;
	ffwr_InitD3D11(gb_hwnd);
	hThread = CreateThread(NULL, // security attributes
	    0, 
	    MyThreadProc, // thread function
	    "HelloThread",
	    0, // 
	    &dwThreadId // 
	);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFFmpegdemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFFmpegdemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFFmpegdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Thread function — must return DWORD and use WINAPI calling convention
DWORD WINAPI
MyThreadProc(LPVOID lpParam)
{
	printf("Thread started! Param = %s\n", (char *)lpParam);

	// Simulate work
	for (;;) {
		PostMessage(gb_hwnd, WM_PAINT, 0, 0);
		Sleep(30); // sleep for 0.5 second
	}

	printf("Thread finished.\n");
	return 0;
}

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
//#include <SDL3/SDL_render_direct3d11.h>
void
xyz(HWND hd)
{
#if 1
	SDL_Init(SDL_INIT_VIDEO );
	bool rs = false;
	SDL_Window *window = 0;
	
	// SDL_GetRendererProperties : ok
	//	SDL_CreateRenderer: ok
	//SDL_CreateTexture: ok
	//SDL_GetRendererName: ok
	//SDL_UpdateYUVTexture: ok
	//SDL_CreateWindow()
	//SDL_CreateWindowFrom: not OK
	//SDL_CreateWindowWithProperties
	//SDL_PropertiesID window_props = SDL_CreateProperties();
	//SDL_SetStringProperty(window_props, " SDL_PROP_WINDOW_WIN32_HWND",
	//    (const char *)native_window_handle);
	//SDL_SetStringProperty
	SDL_PropertiesID window_props = SDL_CreateProperties();
	rs = SDL_SetPointerProperty(
	    window_props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, hd);
	//SDL_SetNumberProperty(window_props, "width", 800);
	//SDL_SetNumberProperty(window_props, "height", 600);
	window = SDL_CreateWindowWithProperties(window_props);
	SDL_DestroyProperties(window_props);
	// 6. Check for errors
	if (!window) {
		// Handle error
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return;
	}
	SDL_Renderer *renderer = SDL_GetRenderer(window);
#endif
}

#if 0
#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>

// ...

// 1. Get your native window handle. This varies by platform and toolkit.
//    Example: On Windows, this could be an HWND.
//    void* native_window_handle = /* ... get your native window handle ... */;
void* native_window_handle = get_native_window_handle(); // Placeholder

// 2. Create a properties object
SDL_PropertiesID window_props = SDL_CreateProperties();

// 3. Set the native handle property. The property name depends on the backend.
//    Common names include SDL_PROP_WINDOW_NATIVE_HANDLE, SDL_PROP_WINDOW_WIN32_HWND, etc.
//    For example, on Windows:
SDL_SetStringProperty(window_props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, (const char*)native_window_handle);
//    For a generic native handle, check SDL's documentation for the correct property name.

// 4. Create the window using the properties object
SDL_Window* window = SDL_CreateWindowWithProperties(window_props);

// 5. Destroy the properties object after the window is created
SDL_DestroyProperties(window_props);

// 6. Check for errors
if (!window) {
    // Handle error
    SDL_Log("Failed to create window: %s", SDL_GetError());
    return 1;
}

// ... use the window ...

// Remember to destroy the window when done
SDL_DestroyWindow(window);

#endif