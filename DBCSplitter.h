#pragma once
#include "afxwin.h"


// CDBCSplitter

class CDBCSplitterCtrl : public CWnd
{
	DECLARE_DYNAMIC(CDBCSplitterCtrl)

public:
	CDBCSplitterCtrl();
	virtual ~CDBCSplitterCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	CWnd *pWnd_A;
	CWnd *pWnd_B;
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	void SetSplitWindow(CWnd * WndA , CWnd * WndB);
	int Size;
	
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	int ScrollBarSizeA;
};


