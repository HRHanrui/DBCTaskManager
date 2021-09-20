#pragma once
#include "afxcmn.h"
#include "performancemon.h"


// CMiniPerfermanceDlg dialog

class CMiniPerfermanceDlg : public CDialog
{
	DECLARE_DYNAMIC(CMiniPerfermanceDlg)

public:
	CMiniPerfermanceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMiniPerfermanceDlg();

// Dialog Data
	enum { IDD = IDD_MINI_PERFERMANCEBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual BOOL OnInitDialog();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CProgressCtrl mCpuBar;
	CProgressCtrl mMemBar;
	CProgressCtrl mDiskBar;
	CProgressCtrl mNetworkBar;
	void SetBarStatus(void);
	CPerformanceMon PerformanceMon;
//	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
//	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnPaint();
	BOOL MouseTrackNow;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	BOOL IsHoverNow;
	void _RedrawButton(bool IsHover);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	BOOL ClickInThisWnd;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	CString StrLinkName;
};
