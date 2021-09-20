// PerformanceView.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "PerformanceView.h"

// CPagePerformance

IMPLEMENT_DYNCREATE(CPagePerformance, CFormView)

CPagePerformance::CPagePerformance()
	: CFormView(CPagePerformance::IDD)
{

}

CPagePerformance::~CPagePerformance()
{
}

void CPagePerformance::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_OPENRESMON, mBtnOpenResMon);
}

BEGIN_MESSAGE_MAP(CPagePerformance, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()//	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BTN_OPENRESMON, &CPagePerformance::OnBnClickedBtnOpenResourceMonitor)
	
END_MESSAGE_MAP()


// CPagePerformance diagnostics

#ifdef _DEBUG
void CPagePerformance::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPagePerformance::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPagePerformance message handlers

int CPagePerformance::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
 

	
	 CCreateContext * pContextBox= NULL; 


	CWnd * pWnd = NULL; 




	//-----------------------------------------------------------------------
 
	pViewClass = RUNTIME_CLASS(CPerformanceBox);

	  
	 pContextBox= new CCreateContext; 

	pContextBox->m_pCurrentDoc = NULL; 
	pContextBox->m_pCurrentFrame = NULL; 
	pContextBox->m_pLastView = NULL; 
	pContextBox->m_pNewDocTemplate =NULL; 
 

	pContextBox->m_pNewViewClass = pViewClass; 

	pWnd = NULL; 
	pWnd = DYNAMIC_DOWNCAST(CWnd, pViewClass->CreateObject());
	pWnd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0,0,0,0), this, IDD_PAGE_PERFORMANCE_BOX, pContextBox); 

	pViewBox= DYNAMIC_DOWNCAST(CPerformanceBox, pWnd); 
	pViewBox->SetScrollSizes(MM_TEXT, CSize(BOX_W_MIN, 400)); 
    pViewBox->ModifyStyleEx(WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE,0);



	//--------------------

	//pViewClass = RUNTIME_CLASS(CFormView);

	//pContextBox->m_pNewViewClass = pViewClass; 
	//
	//pWnd = DYNAMIC_DOWNCAST(CWnd, pViewClass->CreateObject());
	//pWnd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0,0,0,0), this, -1, pContextBox); 

	//pViewList= DYNAMIC_DOWNCAST(CFormView, pWnd); 
	//pViewList->SetScrollSizes(MM_TEXT, CSize(BOX_W_MIN, 400)); 
	//pViewList->ModifyStyleEx(WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE,0);

	  

 //
 


	 delete pContextBox;
	 pContextBox=NULL;
 
 

	return 0;
}

void CPagePerformance::Init(void)
{




	

	pViewBox->Init();

	// pSplitter= new CDBCSplitterCtrl; 

	
	

	
	CRect Rect;
	this->GetClientRect( Rect);

	mSplitter.Create(AfxRegisterWndClass(0,NULL,NULL,NULL),_T("pSplitter"),     WS_CHILD, CRect(220,Rect.top,228,Rect.bottom), this,0);
	mSplitter.SetParent(this);
	mSplitter.SetSplitWindow(&pViewBox->mPItemList,pViewBox);
	mSplitter.ShowWindow(SW_SHOW);

	
	pViewBox->ShowWindow(SW_SHOW);
	//pViewBox->mPItemList.ModifyStyle(0,WS_CLIPCHILDREN);
	pViewBox->mPItemList.ModifyStyle(0,WS_CLIPCHILDREN); //重要！！！
	pViewBox->ModifyStyle(0,WS_CLIPCHILDREN); 



	//-------------停用--------------------
	//SCROLLINFO info;
	//info.cbSize = sizeof(SCROLLINFO);  
	//info.fMask = SIF_PAGE;    
	//info.nPage = 1;     //如果滚动条的Max和Min已设定好，那么这个数字越大滑块就越长。
	//mScrollBar.SetScrollInfo(&info);

	//设置按钮图标

	::ExtractIconEx(L"perfmon.exe",0,NULL,&mBtnOpenResMon.hIcon,1);

		

 

}

void CPagePerformance::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
 
	// TODO: Add your message handler code here
	CRect rc;
	CRect rcSplitter;
	CRect rcBox;
	CRect rcList;
	CRect rcListItem;

	this->GetClientRect(rc);
	if(!theApp.FlagSummaryView) //SummaryView 模式下 充满窗口 底部不用上移 
	{
		rc.bottom-=50;
	}
	rcBox.CopyRect(rc);
	 

	int SplitterSize =16;
	SplitterSize = mSplitter.Size ;

	int LeftPos = 3;
	
	
	pViewBox->mPItemList.GetClientRect(rcList);  
	pViewBox->mPItemList.GetItemRect(0,rcListItem,LVIR_BOUNDS);	

	


	BOOL ScrShow = FALSE;
	if(rcListItem.Height()*pViewBox->mPItemList.GetItemCount()>rcList.Height())
	{
		ScrShow= TRUE;		
	}
 
	rcBox.left= rcList.Width()+LeftPos+SplitterSize;
	int DxFix=0; //修复由于滚动条显示产生的位置错误

	int ScrollBarWidth= GetSystemMetrics(SM_CXVSCROLL);
	if(ScrShow) 
	{
		rcBox.left+=ScrollBarWidth;
		DxFix =  ScrollBarWidth;
	}


	if(theApp.FlagSummaryView)//SummaryView状态充满窗口
	{
		rcBox.left =0;
	}

	pViewBox->MoveWindow(rcBox);

	mSplitter.MoveWindow(LeftPos+rcList.Width()+DxFix,12,SplitterSize,rc.bottom-15);
	
	
 
	
	CRect rcTemp;

	pViewBox->mPItemList.GetWindowRect(rcTemp);
	this->ScreenToClient(rcTemp);

	rcTemp.left=LeftPos;
	rcTemp.bottom=rc.bottom;
	rcTemp.top = 12;
	 

	pViewBox->mPItemList.SetRowHeight(60);
    pViewBox->mPItemList.MoveWindow(rcTemp);




     CWnd* 	pWnd=GetDlgItem(IDC_BTN_OPENRESMON);
	if(pWnd!=NULL)
	{
		//rcTemp.SetRect(140,rc.bottom+25-12,140+200,rc.bottom+25+12);   // 此时 rc.bottom 已经上移
		pWnd->MoveWindow(115,rc.bottom+25-11, 200, 25 );// 此时 rc.bottom 已经上移
 
	}








}

HBRUSH CPagePerformance::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
//	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	return theApp.BkgBrush;
}



void CPagePerformance::OnBnClickedBtnOpenResourceMonitor()
{
	// TODO: Add your control notification handler code here
	ShellExecute(CWnd::GetDesktopWindow()->m_hWnd, L"open", L"perfmon.exe",L"/res" ,_T(""),SW_SHOW);
}



 