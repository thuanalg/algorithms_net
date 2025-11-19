
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

DWORD WINAPI
MyThreadStop(LPVOID lpParam);

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
	//if ()
	PostMessage((HWND)(p->render_objects.native_window), 
		WM_FFWR_MESSAGE, 0, (LPARAM)obj);
	return 0;
}


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
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CFFmpegdemoDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDSTOP, &CFFmpegdemoDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDSTART, &CFFmpegdemoDlg::OnBnClickedStart)
	ON_MESSAGE(WM_FFWR_MESSAGE, &CFFmpegdemoDlg::OnFFWRMessage)
	END_MESSAGE_MAP()

int
CFFmpegdemoDlg::OnCloseFFWRFrame()
{
	for (int i = 0; i < m_listFrame.size(); ++i) {
		if (m_listFrame[i]->running)
			return 1;
	}
	return 0;
}

void CFFmpegdemoDlg::OnClose()
{
	ShowWindow(SW_HIDE); 
	if (OnCloseFFWRFrame()) {
		this->OnBnClickedStop();
		if (this->m_hThread) {
			TerminateThread(this->m_hThread, 0);
			this->m_hThread = 0;
		}
		
		//m_stopped = 1;
		DWORD dwThreadId;
		HANDLE hd = CreateThread(NULL, // security attributes
		    0,
		    MyThreadStop, // thread function
		    this,
		    0, //
		    &dwThreadId //
		);
		return;
	}
	clearVideoFrames();
	CDialogEx::OnClose();
}
	// CFFmpegdemoDlg message handlers
FFWR_INPUT_ST mfcinfo;
BOOL CFFmpegdemoDlg::OnInitDialog()
{
	//m_stopped = 0;
	m_listFrame.clear();
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


	addVideoFrame(1);

	DWORD dwThreadId;

	m_hThread = CreateThread(NULL, // security attributes
	    0, 
	    MyThreadProc, // thread function
	    this,
	    0, // 
	    &dwThreadId // 
	);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT
CFFmpegdemoDlg::OnFFWRMessage(WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < m_listFrame.size(); ++i) {
		m_listFrame[i]->render(1);
	}
	return 0;
}

void
CFFmpegdemoDlg::clearVideoFrames()
{
	for (auto frame : m_listFrame) {
		if (frame) {
			if (frame->GetSafeHwnd()) {
				frame->DestroyWindow();
			}
			delete frame;
		}
	}

	// 3. Xóa list
	m_listFrame.clear();
}

void
CFFmpegdemoDlg::addVideoFrame(int n)
{
	const int frameWidth = 640;
	const int frameHeight = 480;
	const int framesPerRow = 4; // 4 màn hình mỗi hàng

	for (int i = 0; i < n; ++i) {
		FFWRVideoFrame *m_vframe = new FFWRVideoFrame();
		m_listFrame.push_back(m_vframe);

		// Tính vị trí cột và hàng
		int col = i % framesPerRow;
		int row = i / framesPerRow;

		CRect rect(frameWidth * col, frameHeight * row,
		    frameWidth * (col + 1), frameHeight * (row + 1));

		m_vframe->Create(_T("MFCCstatic"),
		    WS_CHILD | WS_VISIBLE | SS_BLACKFRAME | SS_NOTIFY, rect,
		    this,
		    1001 + 777 + i);

		FFWR_DEMUX_OBJS *obj =
		    (FFWR_DEMUX_OBJS *)m_vframe->get_demux_obj();
		obj->input.cb = demux_callback_gui;
	}
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
	CFFmpegdemoDlg *p = (CFFmpegdemoDlg *)lpParam;
	HWND tmp = 0;
	for (;;) {
		PostMessage(p->m_hWnd, WM_FFWR_MESSAGE, 0, 0);
		Sleep(33); // sleep for 0.5 second
	}

	return 0;
}

DWORD WINAPI
MyThreadStop(LPVOID lpParam)
{
	CFFmpegdemoDlg *p = (CFFmpegdemoDlg *)lpParam;
	Sleep(50); 
	PostMessage(p->m_hWnd, WM_CLOSE, 0, 0);
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
	FFWR_DEMUX_OBJS *obj = 0;
	for (int i = 0; i < this->m_listFrame.size(); ++i) {
		obj = (FFWR_DEMUX_OBJS *)m_listFrame[i]->get_demux_obj();
		obj->input.cb = demux_callback_gui;
		m_listFrame[i]->stopxyx();
	}
}
const char *ghgdhsg[] = {
	//"tcp://127.0.0.1:12345",
	"C:/Users/DEll/Desktop/A3-TS_00_d.ts",
	"C:/Users/DEll/Desktop/A2-TS_00_d.ts",
	"C:/Users/DEll/Desktop/A1-TS_00_d.ts",
	"C:/Users/DEll/Desktop/A1-TS_00_d.ts",
	"C:/Users/DEll/Desktop/A1-TS_00_d.ts",
	"C:/Users/DEll/Desktop/A1-TS_00_d.ts",
	"C:/Users/DEll/Desktop/A2-TS_00_d.ts",
	"C:/Users/DEll/Desktop/A1-TS_00_d.ts",
	"C:/Users/DEll/Desktop/A1-TS_00_d.ts",
	"C:/Users/DEll/Desktop/A2-TS_00_d.ts",
	"C:/Users/DEll/Desktop/A1-TS_00_d.ts",

};

void CFFmpegdemoDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here
	FFWR_DEMUX_OBJS *obj = 0;
	for (int i = 0; i < this->m_listFrame.size(); ++i) {
		obj = (FFWR_DEMUX_OBJS *)m_listFrame[i]->get_demux_obj();
		obj->input.cb = demux_callback_gui;
#if 1
		snprintf(
		    obj->input.name, sizeof(obj->input.name), "%s", 
			ghgdhsg[i]);
#else
		snprintf(obj_demux.input.name, sizeof(obj_demux.input.name),
		    "%s", "tcp://127.0.0.1:12345");
#endif
		m_listFrame[i]->xyz();
	}
	obj = (FFWR_DEMUX_OBJS *)m_listFrame[0]->get_demux_obj();
	obj->audio.pause_on_fn(obj->audio.devid, 0);
}
