// PropertiesPageModules.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "PropertiesPageModules.h"
#include "ProcessInfo.h"

// CPropertiesPageModules dialog





//************************************************************************************



IMPLEMENT_DYNAMIC(CPropertiesPageModules, CPropertyPage)

CPropertiesPageModules::CPropertiesPageModules()
	: CPropertyPage(CPropertiesPageModules::IDD)
	, PID(0)
{
	
	HTHEME hTheme=OpenThemeData(this->GetSafeHwnd(), L"TAB");
	COLORREF BkgColor;
	GetThemeColor(hTheme,TABP_AEROWIZARDBODY,0,TMT_FILLCOLOR ,&BkgColor);

	Br.CreateSolidBrush(BkgColor);



}

CPropertiesPageModules::~CPropertiesPageModules()
{
}

void CPropertiesPageModules::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MOUDLE, mModulesList);
}


BEGIN_MESSAGE_MAP(CPropertiesPageModules, CPropertyPage)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPropertiesPageModules message handlers

BOOL CPropertiesPageModules::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here



	

	mModulesList.SetExtendedStyle( mModulesList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER   ); //mModulesList

	CString StrLoad,StrColumn;
	StrLoad.LoadString(STR_COLUMN_PROP_MODULES);
	int ColumnWidth[4] = {150,200,180,250} ;

	for(int i=0 ; i<4; i++)
	{
		AfxExtractSubString(StrColumn,StrLoad,i,L';');
		mModulesList.InsertColumn(i,StrColumn,0,ColumnWidth[i]);
	}


 	ListModules();
	

	SetTimer(0,1000,NULL);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CPropertiesPageModules::ListModules(BOOL Sort)
{
	int nModules = 0;
	
	HMODULE  hModuleArray[256];
	DWORD RealSize;
	DWORD hModuleSize = sizeof(HMODULE);

	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ    ,FALSE,PID);

	if(hProcess<=0)	{ return 0;	}





	BOOL Ret; 

#ifdef _M_X64
	Ret = EnumProcessModulesEx(hProcess,hModuleArray,hModuleSize*256,&RealSize,LIST_MODULES_ALL);
#else
	Ret = EnumProcessModules(hProcess,hModuleArray,hModuleSize*256,&RealSize);
#endif

	//MSB(::GetLastError())

 
	if(Ret)
	{
		nModules  = RealSize/hModuleSize;
		
		CString StrTemp;
		
		WCHAR StrName[MAX_PATH];

		for(int i=0;i<nModules;i++)
		{
			
		 
			 
			GetModuleBaseName(hProcess,hModuleArray[i],StrName,MAX_PATH);	

		 	
			mModulesList.InsertItem(0,StrName);

			CProcess mProcInfo;
			GetModuleFileNameEx(hProcess,hModuleArray[i],StrName,MAX_PATH);
     	    StrTemp=mProcInfo.GetVerInfoString(StrName,L"FileDescription");			
			mModulesList.SetItemText(0,1,StrTemp);

			StrTemp=mProcInfo.GetVerInfoString(StrName,L"CompanyName");
			mModulesList.SetItemText(0,2,StrTemp);

			mModulesList.SetItemText(0,3,StrName);



		
		}
		//MSB_S(StrTip);
	}

	
	if(Sort)
	{
		mModulesList.SortListItems(0);
		 
	}

	CloseHandle(hProcess);

	return nModules;
}

void CPropertiesPageModules::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	//----------------------------------

	CRect rc;
	this ->GetClientRect(rc);
	rc.DeflateRect(9,9);
	rc.top+=3;
	mModulesList.MoveWindow(rc);
}

HBRUSH CPropertiesPageModules::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	CRect rc;
	this->GetClientRect(rc);

	HTHEME hTheme=OpenThemeData(this->GetSafeHwnd(), L"TAB");

	DrawThemeBackground(hTheme,pDC->m_hDC,  TABP_AEROWIZARDBODY ,    0 ,rc ,NULL); 	

	pDC->FillSolidRect(rc,RGB(0,0,0));
	
	//pDC->SetBkMode(TRANSPARENT);
	//return NULL;
	return Br;
}


void CPropertiesPageModules::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if(this->IsWindowVisible())
	{
		CRect RcItem;
		mModulesList.GetItemRect(0,RcItem,LVIR_BOUNDS);

		SIZE ScrollPos;
		ScrollPos.cx = mModulesList.GetScrollPos( SB_HORZ );
		ScrollPos.cy = mModulesList.GetScrollPos( SB_VERT );

		int iSel = mModulesList.GetNextItem( -1, LVNI_SELECTED );
	 

		mModulesList.SetRedraw(0);
		mModulesList.DeleteAllItems();
		 
		ListModules( );
		mModulesList.SortListItems(0);
	
		mModulesList.Scroll(CSize(ScrollPos.cx, ScrollPos.cy*RcItem.Height() ));

		mModulesList.SetItemState(iSel,LVIS_FOCUSED | LVIS_SELECTED,LVIS_FOCUSED | LVIS_SELECTED);

		mModulesList.SetRedraw(1);
	}

	CPropertyPage::OnTimer(nIDEvent);
}
