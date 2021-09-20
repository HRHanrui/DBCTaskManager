// MiniPerfermanceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "MiniPerfermanceDlg.h"

// CMiniPerfermanceDlg dialog

IMPLEMENT_DYNAMIC(CMiniPerfermanceDlg, CDialog)

CMiniPerfermanceDlg::CMiniPerfermanceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMiniPerfermanceDlg::IDD, pParent)
	, MouseTrackNow(FALSE)
	, IsHoverNow(FALSE)
	, ClickInThisWnd(FALSE)
	, StrLinkName(_T(""))
{

}

CMiniPerfermanceDlg::~CMiniPerfermanceDlg()
{
}

void CMiniPerfermanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_CPU, mCpuBar);
	DDX_Control(pDX, IDC_PROGRESS_MEMORY, mMemBar);
	DDX_Control(pDX, IDC_PROGRESS_DISK, mDiskBar);
	DDX_Control(pDX, IDC_PROGRESS_NETWORK, mNetworkBar);
}


BEGIN_MESSAGE_MAP(CMiniPerfermanceDlg, CDialog)
	ON_WM_ACTIVATE()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()

	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CMiniPerfermanceDlg message handlers

void CMiniPerfermanceDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if (WA_INACTIVE == nState) 
	{
		CDialog::OnOK();
	}

	// TODO: Add your message handler code here
}

BOOL CMiniPerfermanceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	this->BringWindowToTop();
	this->SetForegroundWindow();

	CRect rcTaskBar,rcMiniBox;

	int W,H;

	this->GetWindowRect(rcMiniBox);

	W= rcMiniBox.Width(); H =rcMiniBox.Height();
	
	CWnd* pTaskBar =  CWnd::FindWindowW(L"Shell_TrayWnd",NULL);
	
	RECT rcWorkspace;
	SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rcWorkspace,0);  

	pTaskBar->GetWindowRect(rcTaskBar);

	if(rcTaskBar.top == rcWorkspace.bottom)//任务栏在底部
	{
		rcMiniBox.left=rcWorkspace.right-W-9;  rcMiniBox.top=rcWorkspace.bottom-H-9;
		
	}
	//else  if(rcTaskBar.left==0 && ( rcTaskBar.Height() == (rcWorkspace.bottom-rcWorkspace.top) ))
	else  if(rcTaskBar.right == rcWorkspace.left)//任务栏在左侧
	{
		rcMiniBox.left=rcTaskBar.right+9;  rcMiniBox.top=rcWorkspace.bottom-H-9;
		
	}
	else  if(rcTaskBar.left == rcWorkspace.right)//任务栏在右侧
	{
		rcMiniBox.left=rcWorkspace.right-W-9;  rcMiniBox.top=rcWorkspace.bottom-H-9;
	
	}
	else  if(rcTaskBar.bottom == rcWorkspace.top)//任务栏在顶部
	{
		rcMiniBox.left=rcWorkspace.right-W-9;  rcMiniBox.top=rcWorkspace.top+9;
		
	}
	rcMiniBox.right=rcMiniBox.left+W;  rcMiniBox.bottom=rcMiniBox.top+H;


	this->MoveWindow(rcMiniBox);
	


	mCpuBar.SetRange(0,100);
	mMemBar.SetRange(0,100);
	mDiskBar.SetRange(0,100);
	mNetworkBar.SetRange(0,100);

	PerformanceMon.Init();

	SetBarStatus();

	//-------------------------------

	CWnd * pWndTemp = GetDlgItem(IDC_LINKBTNNAME);

	if(pWndTemp!=NULL)
	{
		pWndTemp->GetWindowTextW(StrLinkName);	
		pWndTemp->ShowWindow(SW_HIDE);

	}
	else
	{
		StrLinkName = L"Performance";
	}



	this->SetTimer(0,1500,NULL);




	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CMiniPerfermanceDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	
	SetBarStatus();

	//CDialog::OnTimer(nIDEvent);
}

 

HBRUSH CMiniPerfermanceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(theApp.WndBkgColor);
		pDC->SetTextColor(theApp.WndTextColor);
	}
  

	return theApp.BkgBrush;
}

void CMiniPerfermanceDlg::SetBarStatus(void)
{
	 
 
	mCpuBar.SetPos((int)theApp.PerformanceInfo.CpuUsage);
	mMemBar.SetPos((int)theApp.PerformanceInfo.MemoryUsage);
	mDiskBar.SetPos((int)theApp.PerformanceInfo.TotalDiskUsage);
	mNetworkBar.SetPos((int)theApp.PerformanceInfo.TotalNetUsage);


	CWnd *pWnd= NULL;
	CString StrInfo;

	//------------------------

	pWnd =  GetDlgItem(IDC_STATIC_CPU);
	StrInfo.Format(L"%.2f%%",theApp.PerformanceInfo.CpuUsage);
	if(pWnd!=NULL) pWnd->SetWindowTextW(StrInfo);

		//------------------------

	pWnd =  GetDlgItem(IDC_STATIC_MEM);
	StrInfo.Format(L"%.2f%%",theApp.PerformanceInfo.MemoryUsage);
	if(pWnd!=NULL) pWnd->SetWindowTextW(StrInfo);


		//------------------------

	pWnd =  GetDlgItem(IDC_STATIC_DISK);
	StrInfo.Format(L"%.2f%%",theApp.PerformanceInfo.TotalDiskUsage);
	if(pWnd!=NULL) pWnd->SetWindowTextW(StrInfo);


		//------------------------

	pWnd =  GetDlgItem(IDC_STATIC_NET);
	StrInfo.Format(L"%.2f%%",theApp.PerformanceInfo.TotalNetUsage);
	if(pWnd!=NULL) pWnd->SetWindowTextW(StrInfo);







	
}


BOOL CMiniPerfermanceDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	 

	return CDialog::PreTranslateMessage(pMsg);
}

void CMiniPerfermanceDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	POINT Wndsize ;
	Wndsize.x = 259;
	Wndsize.y = 311;
	lpMMI->ptMaxSize = Wndsize;
	lpMMI->ptMaxTrackSize = Wndsize;
	lpMMI->ptMinTrackSize = Wndsize;

	 
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CMiniPerfermanceDlg::OnPaint()
{
	CPaintDC dc(this); 

	//DefWindowProc(WM_PAINT,(WPARAM)dc.m_hDC,(LPARAM)0);

	HTHEME hTheme = OpenThemeData(this->GetSafeHwnd(), L"Explorer::Flyout"); 

	CRect rc;
	this->GetClientRect(rc);

	rc.top = rc.bottom-50;
	DrawThemeBackground(hTheme,dc.m_hDC,  FLYOUT_LINKAREA ,    0 ,rc ,NULL); 	

	 

	if(!MouseTrackNow)
	{

		DrawThemeText(hTheme,dc.m_hDC,FLYOUT_LINK,FLYOUTLINK_NORMAL,StrLinkName,StrLinkName.GetLength(),DT_CENTER | DT_VCENTER | DT_SINGLELINE,NULL,&rc);
	}
	else
	{
		DrawThemeText(hTheme,dc.m_hDC,FLYOUT_LINK,FLYOUTLINK_HOVER,StrLinkName,StrLinkName.GetLength(),DT_CENTER | DT_VCENTER | DT_SINGLELINE,NULL,&rc);
	}
	::CloseThemeData(hTheme);

	 

	
}

void CMiniPerfermanceDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CRect rc;
	this->GetClientRect(rc);
	rc.top = rc.bottom-50;
	rc.DeflateRect(80,15);



	if (!MouseTrackNow)     //  允许开始追踪  
	{
		

		if(rc.PtInRect(point))
		{
			TRACKMOUSEEVENT csTME;
			csTME.cbSize = sizeof (csTME);
			csTME.dwFlags = TME_LEAVE|TME_HOVER;
			csTME.hwndTrack = m_hWnd ;// 指定要 追踪 的窗口 
			csTME.dwHoverTime = 10;  // 鼠标在按钮上停留超过 10ms ，才认为状态为 HOVER
			::_TrackMouseEvent (&csTME); // 开启 Windows 的 WM_MOUSELEAVE ， WM_MOUSEHOVER 事件支持 
			MouseTrackNow=TRUE ;   
		}		
	}
	else
	{
		if(!rc.PtInRect(point))
		{
			this->PostMessageW(WM_MOUSELEAVE);
		}
		

	}

	CDialog::OnMouseMove(nFlags, point);
}

void CMiniPerfermanceDlg::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
 
		_RedrawButton(TRUE);
	 
		
	CDialog::OnMouseHover(nFlags, point);
}

void CMiniPerfermanceDlg::OnMouseLeave()
{
	_RedrawButton(FALSE);
	MouseTrackNow=FALSE;
	ClickInThisWnd = FALSE;

	CDialog::OnMouseLeave();
}

void CMiniPerfermanceDlg::_RedrawButton(bool IsHover)
{
	IsHoverNow = IsHover;
	CRect rc;
	this->GetClientRect(rc);
	rc.top = rc.bottom-50;
	 
	this->InvalidateRect(rc,0);
}

BOOL CMiniPerfermanceDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	if(IsHoverNow)
		SetCursor(LoadCursor(NULL,IDC_HAND));
	else
		SetCursor(LoadCursor(NULL,IDC_ARROW));


	return FALSE;

	//return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CMiniPerfermanceDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if(IsHoverNow)
		ClickInThisWnd = TRUE;

	CDialog::OnLButtonDown(nFlags, point);
}

void CMiniPerfermanceDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if(ClickInThisWnd&&IsHoverNow)
	{
		theApp.m_pMainWnd->ShowWindow(SW_RESTORE);
		theApp.m_pMainWnd->SetForegroundWindow();
		//theApp.m_pMainWnd->PostMessageW(WM_COMMAND,ID_NOTIFY_RESTORE);
		CRect rc;
		theApp.m_pMainWnd->GetWindowRect(rc);
		rc.bottom-=1;
		theApp.m_pMainWnd->MoveWindow(rc);
		rc.bottom+=1;
		theApp.m_pMainWnd->MoveWindow(rc);
		 
	}


	CDialog::OnLButtonUp(nFlags, point);
}
