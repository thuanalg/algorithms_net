
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
int
demux_callback_gui(void *obj)
{
	FFWR_DEMUX_OBJS *p = (FFWR_DEMUX_OBJS *)obj;
	if (!p) {
		return 1;
	}
	PostMessage((HWND)(p->render_objects.native_window), 
		WM_FFWR_MESSAGE, 0, (LPARAM)obj);
	return 0;
}

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
	ON_BN_CLICKED(IDCANCEL, &CFFmpegdemoDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDSTOP, &CFFmpegdemoDlg::OnBnClickedStop)
	END_MESSAGE_MAP()


// CFFmpegdemoDlg message handlers
FFWR_INPUT_ST mfcinfo;
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
	//m_vframe.create_sdlwin();

	m_vframe.xyz();

	HANDLE hThread;
	DWORD dwThreadId;

	hThread = CreateThread(NULL, // security attributes
	    0, 
	    MyThreadProc, // thread function
	    "HelloThread",
	    0, // 
	    &dwThreadId // 
	);

	//snprintf(mfcinfo.name, sizeof(mfcinfo.name), "%s", 
	//	"tcp://127.0.0.1:12345");
	//mfcinfo.native_drawing = m_vframe.m_hWnd;
	//mfcinfo.cb = demux_callback_gui;
	//ffwr_create_demux(&mfcinfo);
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
	spllog(1, "Thread started! Param = %s\n", (char *)lpParam);

	// Simulate work
	for (;;) {
		PostMessage(gb_hwnd, WM_PAINT, 0, 0);
		Sleep(31); // sleep for 0.5 second
	}

	return 0;
}

void
CFFmpegdemoDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
	//ffwr_set_running(0);
}

void
CFFmpegdemoDlg::OnBnClickedStop()
{
	// TODO: Add your control notification handler code here
	//ffwr_set_stopping(m_vframe., 1);
	FFWR_DEMUX_OBJS *obj = 0;
	obj = (FFWR_DEMUX_OBJS *)m_vframe.get_demux_obj();
	obj->input.cb = demux_callback_gui;
	ffwr_set_stopping(obj, 1);
}
