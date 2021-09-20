// DBCSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "DBCSplitter.h"


// CDBCSplitter

IMPLEMENT_DYNAMIC(CDBCSplitterCtrl, CWnd)

CDBCSplitterCtrl::CDBCSplitterCtrl()
: pWnd_A(NULL)
, pWnd_B(NULL)
, Size(16)
, ScrollBarSizeA(0)
{

}

CDBCSplitterCtrl::~CDBCSplitterCtrl()
{
}


BEGIN_MESSAGE_MAP(CDBCSplitterCtrl, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_MOVING()
//	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CDBCSplitter message handlers



void CDBCSplitterCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
//MessageBox(L"ssssssssssssssssqsqsqsqsqss");
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, 0)); //ÍÏ¶¯

	//CWnd::OnLButtonDown(nFlags, point);
}

void CDBCSplitterCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	CRect rc;
	this->GetClientRect(rc);
	dc.FillSolidRect(rc,theApp.WndBkgColor);	
	Graphics  Grap(dc.m_hDC);
	RectF rcF(3.0,0,1.0,(float)rc.bottom);


	Gdiplus::LinearGradientBrush LBr(rcF,Color(0,160,160,160),Color(255,160,160,160),LinearGradientModeVertical);


	Gdiplus::REAL Pos[] = {0.0f,0.2f,0.8f,1.0f};
	Gdiplus::Color PosClr[] = {Gdiplus::Color(0,190,190,190),Gdiplus::Color(255,190,190,190),Gdiplus::Color(255,190,190,190),Gdiplus::Color(0,190,190,190)};

	LBr.SetInterpolationColors(&PosClr[0],&Pos[0],4);
	
	Grap.FillRectangle(&LBr,rcF);
 


}

void CDBCSplitterCtrl::OnMoving(UINT fwSide, LPRECT pRect)
{
	CWnd::OnMoving(fwSide, pRect);

	CWnd *pParent =	this->GetParent();

	CRect rcA,rcB,rc;
	// TODO: Add your message handler code here
	pWnd_A->GetWindowRect(rcA);
	pWnd_B->GetWindowRect(rcB);
	
	GetWindowRect(rc);
	

	pParent->ScreenToClient(rcA);
	pParent->ScreenToClient(rcB);
	pParent->ScreenToClient(rc);


	rcA.right=rc.left;
	rcB.left=rc.right;
	pWnd_A->MoveWindow(rcA);
	pWnd_B->MoveWindow(rcB);


	rc.SetRect(0,rcA.top,Size,rcA.bottom);
	pParent->ClientToScreen(rc);
	pRect->top=rc.top;
	pRect->bottom=rc.bottom-5;

	InvalidateIfVisible(this);




}

void CDBCSplitterCtrl::SetSplitWindow(CWnd * WndA , CWnd * WndB)
{
	pWnd_A = WndA;
	pWnd_B = WndB;

}

 
void CDBCSplitterCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);		
	InvalidateIfVisible(this);
	 

}

BOOL CDBCSplitterCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
 
	SetCursor(LoadCursor(NULL,IDC_SIZEWE));	 
	return FALSE;
	//return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
