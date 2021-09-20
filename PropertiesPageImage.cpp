// PropertiesPageImage.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "PropertiesPageImage.h"
#include "PropertiesPageImage.h"

#include "ProcessInfo.h"


// CPropertiesPageImage dialog

IMPLEMENT_DYNAMIC(CPropertiesPageImage, CPropertyPage)

CPropertiesPageImage::CPropertiesPageImage()
	: CPropertyPage(CPropertiesPageImage::IDD)
	, mPath(_T(""))
	, mCmdLine(_T(""))
	, mDescription(_T(""))
	, mPublisher(_T(""))
	, mVersion(_T(""))
	, mParent(_T(""))
	, pData(NULL)
	, mUser(_T(""))
	, hIcon(NULL)
	
{

}

CPropertiesPageImage::~CPropertiesPageImage()
{
}

void CPropertiesPageImage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, mPath);
	DDX_Text(pDX, IDC_EDIT_CMDLINE, mCmdLine);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, mDescription);
	DDX_Text(pDX, IDC_EDIT_PUBLISHER, mPublisher);
	DDX_Control(pDX, IDC_STATIC_ICON, mIcon);
	DDX_Text(pDX, IDC_EDIT_VER, mVersion);
	DDX_Text(pDX, IDC_EDIT_PARENT, mParent);
	DDX_Text(pDX, IDC_EDIT_USER, mUser);
}


BEGIN_MESSAGE_MAP(CPropertiesPageImage, CPropertyPage)

	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_SIZE()

//	ON_WM_ERASEBKGND()
ON_WM_PAINT()
END_MESSAGE_MAP()


// CPropertiesPageImage message handlers

BOOL CPropertiesPageImage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CProcess  Proc;
	// TODO:  Add extra initialization here

	//ZeroMemory(&sfi, sizeof(SHFILEINFO));
	
	mPath = Proc.GetPathName(((PROCLISTDATA *)pData) ->hProcess);
	mCmdLine = Proc.GetProcCommandLine(((PROCLISTDATA *)pData) ->PID);
	mDescription = Proc.GetVerInfoString(mPath,L"FileDescription");
	mPublisher = Proc.GetVerInfoString(mPath,L"CompanyName");
	mVersion   = Proc.GetVerInfoString(mPath,L"FileVersion");
	
	DWORD  ParentPID = ((PROCLISTDATA *)pData)->ParentPID;

	PROCLISTDATA  * pParentData =  NULL;

	map<DWORD,PVOID>::iterator Iter= Map_PidToData.find	(ParentPID); 

	if(Iter != Map_PidToData.end())// ´æÔÚ 
	{
		pParentData =(PROCLISTDATA  *) Iter->second;
		if(pParentData!=NULL)
		{
			mParent.Format(L"%s (PID:%d)",pParentData->Name,pParentData->PID);
		}

	}

	WCHAR StrDomain[MAX_PATH];



	Proc.GetUser(((PROCLISTDATA *)pData)->hProcess,((PROCLISTDATA *)pData)->PID,StrDomain);

	mUser.Format(L"%s\\%s"  ,StrDomain,((PROCLISTDATA *)pData)->User );

 
	
	sfi.dwAttributes =FILE_ATTRIBUTE_NORMAL;
	SHGetFileInfo(mPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES|SHGFI_ICON | SHGFI_LARGEICON  );  //SHGFI_SMALLICON   SHGFI_ICON
	
	this->GetParent()->SetIcon(sfi.hIcon, TRUE);  

	

	SHDefExtractIcon(mPath,0,0,&hIcon,NULL,48);	
	if(hIcon<=0)
	{
		 hIcon = sfi.hIcon;
	}
	
	mIcon.SetIcon(hIcon);
	mIcon.MoveWindow(10,18,48,48);

 
	//this->GetParent()->GetDlgItem(0x3020)->ModifyStyle(0,WS_CLIPCHILDREN);
	//this->SetParent(this->GetParent()->GetDlgItem(0x3020));
	//this->GetParent()->GetDlgItem(0x3020)->GetParent()->ModifyStyle(0,WS_CLIPCHILDREN);



	SetFocus();

	
	this->UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}




HBRUSH CPropertiesPageImage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	if((nCtlColor== CTLCOLOR_STATIC)|| (nCtlColor==CTLCOLOR_EDIT))
	{
		pDC->SetBkMode(TRANSPARENT);
		return NULL;	
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CPropertiesPageImage::OnDestroy()
{
	CPropertyPage::OnDestroy();

	// TODO: Add your message handler code here
	::DestroyIcon(sfi.hIcon);
	::DestroyIcon(hIcon);
	
	 
}

void CPropertiesPageImage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);



	CWnd * pWnd = NULL;
	pWnd = this->GetWindow(GW_CHILD);

	CRect rcEdit;
	CRect rc;
	this ->GetClientRect(rc);
 
	while(pWnd!=NULL)
	{
		WCHAR WcClassName[MAX_PATH]={L'\0'};
		 GetClassName(pWnd->GetSafeHwnd(),WcClassName,MAX_PATH);
		 if(lstrcmp(WcClassName,L"Edit")==0)
		 {
			 pWnd->GetWindowRect(rcEdit);
			 this->ScreenToClient(rcEdit);
			 rcEdit.right=rc.right-10;
			 pWnd->MoveWindow(rcEdit,0);
			 InvalidateIfVisible(pWnd);
			
			
		 }
		 pWnd  = pWnd->GetWindow(GW_HWNDNEXT);
		 


	}

	




	

}



void CPropertiesPageImage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CPropertyPage::OnPaint() for painting messages


	

	CRect RcAll;
	CRect rcEdit; 

	CWnd * pWnd = NULL;


	GetClientRect(RcAll);
	dc.FillSolidRect(RcAll,RGB(255,255,255));

	UINT CtrlID[2]={IDC_EDIT_PATH,IDC_EDIT_CMDLINE};

	for( int i = 0 ;i<2; i++ )
	{

		pWnd=this->GetDlgItem(CtrlID[i]);
		if(pWnd)
		{
			pWnd->GetWindowRect(rcEdit);
			this->ScreenToClient(rcEdit);
			rcEdit.InflateRect(1,1); rcEdit.top-=3;
			dc.DrawEdge(rcEdit,BDR_SUNKENINNER,BF_RECT);
		}
	}


	

			
	
}
