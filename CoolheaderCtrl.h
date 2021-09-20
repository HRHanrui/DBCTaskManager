#pragma once
#include "afxwin.h"
#include "gdipluscolor.h"


// CCoolheaderCtrl

//struct HDRITEMDATA
//{
//	BOOL Cool;
//	CString StrItem;
//};

class CCoolheaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CCoolheaderCtrl)

public:
	CCoolheaderCtrl();
	virtual ~CCoolheaderCtrl();
	COLUMNSTATUS_EX  * ColStatusArray;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT  lpDrawItemStruct );	

	afx_msg  LRESULT OnLayout(WPARAM wParam, LPARAM lParam) ;
	afx_msg void OnPaint();
	

	int Height;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	 

	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);


	int PosX;
	int Width;
//protected:
//	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);


	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
	afx_msg void OnMouseLeave();
	BOOL  MouseTrackNow;
	BOOL FlagLeftBtnDown;
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	int StartDragCurPos;
	int WillSwapToID;
	BOOL *pFlagSortUp;
	int *pCurrentSortCol;
	HTHEME hTheme;
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	Gdiplus::Color GColor1;
	Gdiplus::Color GColor2;
};


