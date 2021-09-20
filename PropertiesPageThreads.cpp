// PropertiesPageThreads.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "PropertiesPageThreads.h"
#include <tlhelp32.h>


// CPropertiesPageThreads dialog

IMPLEMENT_DYNAMIC(CPropertiesPageThreads, CPropertyPage)

CPropertiesPageThreads::CPropertiesPageThreads()
	: CPropertyPage(CPropertiesPageThreads::IDD)
	, PID(0)
{

}

CPropertiesPageThreads::~CPropertiesPageThreads()
{
}

void CPropertiesPageThreads::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_THREADS, mThreadsList);
}


BEGIN_MESSAGE_MAP(CPropertiesPageThreads, CPropertyPage)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPropertiesPageThreads message handlers

BOOL CPropertiesPageThreads::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	mThreadsList.SetExtendedStyle( mThreadsList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER   );

	mThreadsList.InsertColumn(0,L"TID",0,60);
	mThreadsList.InsertColumn(1,L"CPU",0,60);

	LoadThreadList(TRUE);

	SetTimer(0,1000,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CPropertiesPageThreads::PlaceAllCtrl(void)
{
	CRect Rc;

	this->GetClientRect(Rc);

	Rc.DeflateRect(9,9);
	Rc.top+=3;

	Rc.bottom-=150;
	mThreadsList.MoveWindow(Rc);

	return 0;
}

void CPropertiesPageThreads::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	PlaceAllCtrl();
}

int CPropertiesPageThreads::LoadThreadList(BOOL Init)
{






    HANDLE        hThreadSnap = NULL; 
    int          nThreadsCount        = 0; 
    THREADENTRY32 te32        = {0}; 
 
    // Take a snapshot of all threads currently in the system. 

    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); 
    if (hThreadSnap == INVALID_HANDLE_VALUE)      return (0); 
 
    // Fill in the size of the structure before using it. 

    te32.dwSize = sizeof(THREADENTRY32); 
 
    // Walk the thread snapshot to find all threads of the process. 
    // If the thread belongs to the process, add its information 
    // to the display list.
 
    if (Thread32First(hThreadSnap, &te32)) 
    { 
		CString StrTemp;
        do 
        { 
            if (te32.th32OwnerProcessID ==  PID) 
            { 
               // printf( "/nTID/t/t%d/n", te32.th32ThreadID); 
               // printf( "Owner PID/t%d/n", te32.th32OwnerProcessID); 
                //printf( "Delta Priority/t%d/n", te32.tpDeltaPri); 
                //printf( "Base Priority/t%d/n", te32.tpBasePri); 
				
				

				StrTemp.Format(L"%d",te32.th32ThreadID);
				mThreadsList.InsertItem(nThreadsCount,StrTemp);


				if(Init)
				{


					ThreadListData[te32.th32ThreadID].CPU_Usage = 0;
					ThreadListData[te32.th32ThreadID].CpuUsage.lastCPU.QuadPart = 0;
					ThreadListData[te32.th32ThreadID].CpuUsage.lastSysCPU.QuadPart = 0;
					ThreadListData[te32.th32ThreadID].CpuUsage.lastUserCPU.QuadPart = 0;
					//ThreadListData[te32.th32ThreadID].PID = te32.th32ThreadID;
					ThreadListData[te32.th32ThreadID].hThread =  ::OpenThread(THREAD_QUERY_INFORMATION | THREAD_TERMINATE  ,FALSE,te32.th32ThreadID);

				
				}

				if(PID!=0) //暂时排除特殊进程 system Idle process
				{

					ThreadListData[te32.th32ThreadID].CPU_Usage = CProcess::GetThreadCpuUsage(&ThreadListData[te32.th32ThreadID]);
					StrTemp.Format(L"%0.2f%%",ThreadListData[te32.th32ThreadID].CPU_Usage);
					//StrTemp.Format(L"%I64d",ThreadListData[te32.th32ThreadID].CpuUsage.lastUserCPU);
					mThreadsList.SetItemText(nThreadsCount,1,StrTemp);
				}

				
		
 
				nThreadsCount++;
            } 
        } 
        while (Thread32Next(hThreadSnap, &te32)); 
         
    } 
    
    // Do not forget to clean up the snapshot object. 

    CloseHandle (hThreadSnap); 



 
	return nThreadsCount;
}

void CPropertiesPageThreads::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if(this->IsWindowVisible())
	{
		mThreadsList.SetRedraw(0);

		int iSel = mThreadsList.GetNextItem( -1, LVNI_SELECTED );
		mThreadsList.DeleteAllItems();
		LoadThreadList();
		mThreadsList.SetItemState(iSel,LVIS_FOCUSED | LVIS_SELECTED,LVIS_FOCUSED | LVIS_SELECTED);

		mThreadsList.SetRedraw(1);

	}

	CPropertyPage::OnTimer(nIDEvent);
}
