
// FFmpeg-demoDlg.h : header file
//

#pragma once
#include "FFWRVideoFrame.h"

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
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	FFWRVideoFrame *m_vframe;

public:
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedStop();
  afx_msg void OnBnClickedStart();
};
