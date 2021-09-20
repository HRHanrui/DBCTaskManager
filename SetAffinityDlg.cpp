// SetAffinityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "SetAffinityDlg.h"


// CSetAffinityDlg dialog

IMPLEMENT_DYNAMIC(CSetAffinityDlg, CDialog)

CSetAffinityDlg::CSetAffinityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetAffinityDlg::IDD, pParent)
	, StrProcName(_T(""))
	,BlockItemChangeMessage(FALSE)

{

}

CSetAffinityDlg::~CSetAffinityDlg()
{
}

void CSetAffinityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AFFINITYCPULIST, mCpuList);
	DDX_Control(pDX, IDC_STATIC_TITLE, mTitle);
}


BEGIN_MESSAGE_MAP(CSetAffinityDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AFFINITYCPULIST, &CSetAffinityDlg::OnLvnItemchangedAffinitycpulist)
	ON_BN_CLICKED(IDOK, &CSetAffinityDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetAffinityDlg message handlers

BOOL CSetAffinityDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	SetWindowTheme(mCpuList.GetSafeHwnd(),L"explorer", NULL);

	mCpuList.SetExtendedStyle(mCpuList.GetExtendedStyle()|LVS_EX_FULLROWSELECT| LVS_EX_DOUBLEBUFFER |LVS_EX_CHECKBOXES );  //| LVS_EX_GRIDLINES |LVS_EX_CHECKBOXES
  

	CRect rc;
	mCpuList.GetClientRect(rc);

	mCpuList.InsertColumn(0,L"CPU",0,rc.Width()-5);

	int nLogicalProcessor = theApp.PerformanceInfo.nLogicalProcessor;

	mCpuList.InsertItem(0,L"<All Processors>");

#ifdef _M_X64
	DWORD64  MaskProc = 0x0000;
	DWORD64  MaskSys = 0x0000;
#else
	DWORD MaskProc = 0x0000;
	DWORD  MaskSys = 0x0000;
#endif



	GetProcessAffinityMask(hProcess,&MaskProc,&MaskSys);

	int nCheckCount = 0;

	CString  StrCPU;
	for(int i=0;i<nLogicalProcessor;i++)
	{
		int ID = mCpuList.GetItemCount();

		StrCPU.Format(L"CPU %d",i);
		mCpuList.InsertItem(ID,StrCPU);

		 BYTE CpuMask = MaskProc&0x01;
		 if(CpuMask==1)
		 {
			 mCpuList.SetCheck(ID);
			 nCheckCount++;
		 }	

		MaskProc =  MaskProc>>1;//右移一位为了下次判断

	}

	if(nCheckCount == nLogicalProcessor)
	{
		mCpuList.SetCheck(0); //<All Processors>
	}


	CString StrTitle;
	mTitle.GetWindowTextW(StrTitle);
	//mTitle.SetWindowTextW(StrProcName);
	StrTitle.Replace(L"$",StrProcName);
	mTitle.SetWindowTextW(StrTitle);
	

	




	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetAffinityDlg::OnLvnItemchangedAffinitycpulist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	int n = mCpuList.GetItemCount();

	if(!BlockItemChangeMessage)
	{		

		BlockItemChangeMessage = TRUE;
		if(pNMLV->iItem == 0  )
		{

			BOOL IsCheck = mCpuList.GetCheck(0);

			


			for(int i=0;i<n;i++)
			{
				mCpuList.SetCheck(i,IsCheck);
			}

		}
		else
		{
			BOOL IsCheck = TRUE;
			for(int i=1;i<n;i++)
			{
				IsCheck = IsCheck && mCpuList.GetCheck(i);
			}

			mCpuList.SetCheck(0,IsCheck);


		}

		BlockItemChangeMessage = FALSE;
	}


	CWnd *pWnd = GetDlgItem(IDOK);

	

	if(pWnd!=NULL)
	{
		int nCheck =  0;
		for(int i=1;i<n;i++)
		{
			if( mCpuList.GetCheck(i))
			{				
				nCheck++; break;
			}
		}

		if(nCheck>0)
			pWnd->EnableWindow(TRUE);
		else
			pWnd->EnableWindow(FALSE);

		
	}



	*pResult = 0;
}

void CSetAffinityDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	int n = mCpuList.GetItemCount();


	DWORD_PTR   MaskProc = 0x0000;

	for(int i=n-1;i>0;i--)
	{
		if( mCpuList.GetCheck(i))
		{
			MaskProc = MaskProc|0x01;
		}

		if(i!=1)MaskProc=MaskProc<<1;  //最后一次不要移位
	}


	HANDLE hMyProcess = OpenProcess( PROCESS_SET_INFORMATION,FALSE,PID); 

	if(hMyProcess!=NULL)
	{
	
		SetProcessAffinityMask(hMyProcess,MaskProc);

		::CloseHandle(hMyProcess);
	}
	



	OnOK();
}
