// MyTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "MyTabCtrl.h"


// CMyTabCtrl

IMPLEMENT_DYNAMIC(CMyTabCtrl, CTabCtrl)

CMyTabCtrl::CMyTabCtrl()
{

}

CMyTabCtrl::~CMyTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

END_MESSAGE_MAP()



// CMyTabCtrl message handlers



BOOL CMyTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	 
	if(theApp.FlagThemeActive)
		return TRUE ;
	else
		return CTabCtrl::OnEraseBkgnd(pDC);
}

HBRUSH CMyTabCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CTabCtrl::OnCtlColor(pDC, pWnd, nCtlColor);

 HBRUSH hbr  = ::CreateSolidBrush(RGB(255,255,255));
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

 