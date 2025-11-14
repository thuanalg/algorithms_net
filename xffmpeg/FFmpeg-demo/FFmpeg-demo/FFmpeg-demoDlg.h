
// FFmpeg-demoDlg.h : header file
//

#pragma once
#include "FFWRVideoFrame.h"
#include <algorithm>
#include <iostream>
#include <list>
#include <vector>
using namespace std;

// CFFmpegdemoDlg dialog
class CFFmpegdemoDlg : public CDialogEx
{
// Construction
public:
	CFFmpegdemoDlg(CWnd* pParent = nullptr);	// standard constructor
	
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FFMPEGDEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	
	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnFFWRMessage(WPARAM wParam, LPARAM lParam);

private:
	//FFWRVideoFrame *m_vframe;
	HANDLE m_hThread;
	//int m_stopped;	

public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedStart();
	std::vector<FFWRVideoFrame *> m_listFrame;
	void addVideoFrame(int n);
	int OnCloseFFWRFrame();
};
