// ModelessPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "ModelessPropertySheet.h"


// CModelessPropertySheet




 






IMPLEMENT_DYNAMIC(CModelessPropertySheet, CPropertySheet)

CModelessPropertySheet::CModelessPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CModelessPropertySheet::CModelessPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

CModelessPropertySheet::~CModelessPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CModelessPropertySheet, CPropertySheet)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_TIMER()

END_MESSAGE_MAP()


// CModelessPropertySheet message handlers

BOOL CModelessPropertySheet::OnInitDialog()
{

	BOOL bResult = CPropertySheet::OnInitDialog();


	CMenu* pSysMenu = GetSystemMenu(FALSE);	
	if (pSysMenu != NULL)
	{	
		pSysMenu->AppendMenu(MF_STRING, SC_MINIMIZE, L"Minimize");
		pSysMenu->AppendMenu(MF_STRING, SC_MAXIMIZE, L"Maximize");
		
		
	}




 


	this->SetParent(CWnd::GetDesktopWindow());

	this->ModifyStyle(0,WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN);
 
	//// TODO:  Add your specialized code here
	//m_bModeless = FALSE;   
	//m_nFlags |= WF_CONTINUEMODAL;

	//BOOL bResult = CPropertySheet::OnInitDialog();

	//m_bModeless = TRUE;
	//m_nFlags &= ~WF_CONTINUEMODAL;

	hIcon = theApp.mImagelist.ExtractIconW(iIcon);

    SetIcon(hIcon,FALSE);
	 
	MoveWindow(0,0,550,580);

	this->CenterWindow();
 
	
	
    
    // -----------Hide Apply and Help buttons--------------

    CWnd *pWnd = GetDlgItem (ID_APPLY_NOW);
    pWnd->ShowWindow (FALSE);
    pWnd = GetDlgItem (IDHELP);
    pWnd->ShowWindow (FALSE);
    
   

	PlaceAllCtrl();


	 return bResult;
}


BOOL CModelessPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	if(wParam == IDOK  || wParam == IDCANCEL  )
	{
		DeleteMyObjects();
				 
	}
	return CPropertySheet::OnCommand(wParam, lParam);
}

void CModelessPropertySheet::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	DeleteMyObjects();

	
	CPropertySheet::OnClose();
}

void CModelessPropertySheet::DeleteMyObjects(void)
{
	
	::DestroyIcon(hIcon);
}

LRESULT CModelessPropertySheet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CPropertySheet::WindowProc(message, wParam, lParam);
}

void CModelessPropertySheet::OnSize(UINT nType, int cx, int cy)
{
	CPropertySheet::OnSize(nType, cx, cy);
	//--------------------------------------------
	PlaceAllCtrl();

}

void CModelessPropertySheet::PlaceAllCtrl(void)
{
	
	
	CRect rc;
	this->GetClientRect(rc);
	rc.DeflateRect(12,12);

	rc.bottom-=30;
  
	 

	CTabCtrl * pWndTab = NULL;
	pWndTab = GetTabControl();
	if(pWndTab!=NULL)
	{		 
		pWndTab->MoveWindow(rc,0);	
		InvalidateIfVisible(pWndTab);
	}

	//int nSel = pWndTab->GetCurSel();

	//-----------------
	int ids [] = {IDOK, IDCANCEL};//, ID_APPLY_NOW, IDHELP };
	 CRect rectBtn;
    int nSpacing = 6;        // space between two buttons...
	int X = 0;
	
	for( int i =0; i < sizeof(ids)/sizeof(int); i++)
	{
		CWnd* pWnd = GetDlgItem (ids [i]);

		if( pWnd !=NULL)
		{
			GetDlgItem (ids [i])->GetWindowRect (rectBtn);
			
			int W = rectBtn.Width();
			int H = rectBtn.Height();
			pWnd ->MoveWindow(rc.right-W-X,rc.bottom+10,W,H);
			X=X+W+nSpacing;
			InvalidateIfVisible(pWnd);
		}

	}

	int nPage = this->GetPageCount();
	
	for( int i =0; i < nPage; i++)
	{
		//this->GetPage(i)->PostMessageW(WM_SIZE);
		CRect rcPage;
		this->GetPage(i)->GetWindowRect(rcPage);
		int BorderSize = rcPage.left-rc.left;
		this->ScreenToClient(rcPage);
		//this->GetPage(i)->MoveWindow(rcPage.left,rcPage.top,rc.Width()-BorderSize*2,rc.bottom-BorderSize-rcPage.top);
		//this->GetPage(i)->ShowWindow(SW_HIDE);
		 

	}
    
	CWnd* pWnd  =NULL;

	pWnd=this->GetActivePage();
	if(pWnd!=NULL)
	{
		SetActivePage((CPropertyPage*)pWnd);
		if(!pWnd->IsWindowVisible())pWnd->ShowWindow(SW_SHOW);
		InvalidateIfVisible(pWnd);
		
	}

	
	//this->SetWindowTextW(L"sdfsdfsdf");

	InvalidateIfVisible(this);
	//setwndtext(rand());
	this->RedrawWindow();
	


}


int CALLBACK CModelessPropertySheet::XmnPropSheetCallback(HWND hWnd, UINT message, LPARAM lParam)
{
   extern int CALLBACK AfxPropSheetCallback(HWND, UINT message, LPARAM lParam);
   // XMN: Call MFC's callback.
   int nRes = AfxPropSheetCallback(hWnd, message, lParam);

   switch (message)
   {
   case PSCB_PRECREATE:
      // Set your own window styles.
     ((LPDLGTEMPLATE)lParam)->style |= (DS_3DLOOK | DS_SETFONT  | WS_THICKFRAME | WS_SYSMENU | WS_POPUP | WS_VISIBLE | WS_CAPTION |WS_CLIPCHILDREN );
      break;
   }
   return nRes;
}

INT_PTR CModelessPropertySheet::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class

	m_psh.dwFlags |= PSH_USECALLBACK;
    m_psh.pfnCallback = XmnPropSheetCallback;

	return CPropertySheet::DoModal();
}

void CModelessPropertySheet::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default
	
	CPropertySheet::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = 550;
	lpMMI->ptMinTrackSize.y = 580;

}

void CModelessPropertySheet::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default


	CPropertySheet::OnTimer(nIDEvent);
}

