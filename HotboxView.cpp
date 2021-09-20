// HotboxView.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "HotboxView.h"


// CHotboxView

IMPLEMENT_DYNCREATE(CHotboxView, CFormView)

CHotboxView::CHotboxView()
	: CFormView(CHotboxView::IDD)
	, nRow(1)
	, nCol(1)
{
	float A = (float)theApp.AppSettings.CpuColor.GridColorAlpha/255;

		

//	GridColor = ;

	GridColor = AlphaBlendColor(theApp.AppSettings.CpuColor.BackgroundColor,theApp.AppSettings.CpuColor.GridColorRGB,A);

}

CHotboxView::~CHotboxView()
{
}

void CHotboxView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHotboxView, CFormView)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CHotboxView diagnostics

#ifdef _DEBUG
void CHotboxView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CHotboxView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CHotboxView message handlers

HBRUSH CHotboxView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
		
	return theApp.BkgBrush;
}

void CHotboxView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CFormView::OnPaint() for painting messages

	int SizeX = 32;

	CRect rc,rcClient;
	this->GetWindowRect(rc);
	this->GetClientRect(rcClient);
	rc.SetRect(0,0,rc.Width(),rc.Height());



	int W= rcClient.Width()/nCol;
	int H =rcClient.Height()/nRow;
	int X,Y;

	X=Y=0;

	CRect rcGrid(X,Y,W,H);

	int nProcessor = 0;


	
	CString StrUsage;
	
	dc.SelectObject(this->GetParent()->GetFont());

	

	for(int y=0;y<nRow;y++)
	{

		for(int x=0;x<nCol;x++)
		{

			
			StrUsage.Format(L"%.0f %%",ArrayPerLogicalCpuUsage[nProcessor]*100);
			

			rcGrid.SetRect(X,Y,X+W,Y+H);	

			if(x == nCol-1)  {rcGrid.right=rc.right;}
			dc.FillSolidRect(rcGrid, AlphaBlendColor(theApp.AppSettings.CpuColor.BackgroundColor, theApp.AppSettings.CpuColor.FillAColor.ToCOLORREF(), ArrayPerLogicalCpuUsage[nProcessor]) );	

			dc.DrawText(StrUsage,rcGrid,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_CENTER);
			
			X+=W;
			nProcessor++;
		}
		X=0;Y+=H;

	}

	
	Y=H;
	for(int y=0;y<nRow-1;y++)
	{
		dc.FillSolidRect(0,Y,rcClient.right,1,GridColor);	
		Y+=H;		
	}

	X=W;
	for(int x=0;x<nCol-1;x++)	
	{
		dc.FillSolidRect(X,0,1,rcClient.bottom,GridColor);	
		X+=W;
		
	}

	dc.Draw3dRect(rc,theApp.AppSettings.CpuColor.BorderColor,theApp.AppSettings.CpuColor.BorderColor);

	 

}
