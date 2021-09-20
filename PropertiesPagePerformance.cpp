// PropertiesPagePerformance.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "PropertiesPagePerformance.h"


// CPropertiesPagePerformance dialog

IMPLEMENT_DYNAMIC(CPropertiesPagePerformance, CPropertyPage)

CPropertiesPagePerformance::CPropertiesPagePerformance()
	: CPropertyPage(CPropertiesPagePerformance::IDD)
	, mStrPriority(_T(""))
	,pData(NULL)
	, mStrPrivateBytes(_T(""))
	, mStrPeakPrivateBytes(_T(""))
	, mStrVirtualSize(_T(""))
	, mStrPageFaults(_T(""))
	, mStrWorkingSet(_T(""))
	, mStrPeakWorkingSet(_T(""))
	, mStrWSPrivate(_T(""))
	, mStrWSShareable(_T(""))
	, mStrReads(_T(""))
	, mStrWrites(_T(""))
	, mStrOther(_T(""))
	, ReadsOld(0)
	, ReadBytesOld(0)
	, WritesOld(0)
	, WriteBytesOld(0)
	, mStrTotalTime(_T(""))
{

}

CPropertiesPagePerformance::~CPropertiesPagePerformance()
{
}

void CPropertiesPagePerformance::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PRIORITY, mStrPriority);
	DDX_Text(pDX, IDC_PRIVATE_BYTES, mStrPrivateBytes);
	DDX_Text(pDX, IDC_PEAKPRIVATE_BYTES2, mStrPeakPrivateBytes);
	DDX_Text(pDX, IDC_VIRTUALSIZE, mStrVirtualSize);
	DDX_Text(pDX, IDC_PF, mStrPageFaults);
	DDX_Text(pDX, IDC_WS, mStrWorkingSet);
	DDX_Text(pDX, IDC_PEAKWS, mStrPeakWorkingSet);
	DDX_Text(pDX, IDC_WS_PRIVATE, mStrWSPrivate);
	DDX_Text(pDX, IDC_WS_SHAREABLE, mStrWSShareable);
	DDX_Text(pDX, IDC_READS, mStrReads);
	DDX_Text(pDX, IDC_WRITES, mStrWrites);
	DDX_Text(pDX, IDC_OTHER, mStrOther);
	DDX_Text(pDX, IDC_TOTALTIME, mStrTotalTime);
}


BEGIN_MESSAGE_MAP(CPropertiesPagePerformance, CPropertyPage)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPropertiesPagePerformance message handlers

HBRUSH CPropertiesPagePerformance::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(nCtlColor==CTLCOLOR_STATIC)
	{
		//pDC->SetBkMode(TRANSPARENT); 
		//pDC->SetTextColor(RGB(255,255,0)); 
	//	HBRUSH B = CreateSolidBrush(RGB(125,125,255)); 
		return NULL; 
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL CPropertiesPagePerformance::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	UpdateAllPerformanceItems();

	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CPropertiesPagePerformance::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	//CWnd *pChild = GetWindow( GW_CHILD ); //取第一个子窗口
	//while( pChild != NULL )
	//{
	//	InvalidateIfVisible(pChild);
	//	//pChild->ShowWindow(SW_HIDE);
	//	pChild = pChild->GetNextWindow(); //取下一个子窗口
	//}

	InvalidateIfVisible(this->GetParent());
}

void CPropertiesPagePerformance::UpdateAllPerformanceItems(void)
{
	
	if(pData!=NULL)
	{ 
		CString StrTemp;
		DWORD Priority = mProcess.GetPriority(pData->hProcess,&StrTemp);

		mStrPriority.Format(L"%s",StrTemp);


		mProcess.GetMemoryPriority(pData->hProcess);
	
	//---------------------------------- 计算 此进程 CPU  使用率 ---------------------------

		double ProcCpuTime;
		mProcess.GetCpuUsage(pData,&ProcCpuTime);
		 		 

		int nHour,nMin,nSec;

		nHour=nMin=nSec=0;

		double Time1 = ProcCpuTime/10000000;
		LONG   LTime   = (LONG)Time1;

		if(LTime>=60)
		{
			nMin =  LTime/60;	nSec = LTime%60;
			if(nMin>=60){ nHour= nMin/60;nMin = nMin%60; }
		}
		else
		{
			nSec = (int)LTime;
		}


		CString StrT;
		StrTemp.Format(L"%0.3f",Time1-nSec);

		StrT = StrTemp.Right(4);
		StrTemp=StrT;
		mStrTotalTime.Format(L"%d:%.2d:%.2d%s",nHour,nMin,nSec,StrTemp);


		//--------Mem ---------------------------------

		if (pData->hProcess!= NULL)//hProcess
		{
			PROCESS_MEMORY_COUNTERS_EX pmc;
			pmc.cb=sizeof(PROCESS_MEMORY_COUNTERS_EX);
			BOOL Ret;


			Ret = GetProcessMemoryInfo(  pData->hProcess,(PROCESS_MEMORY_COUNTERS*)&pmc,sizeof(pmc));
 			

			if( Ret) 
			{
				mStrPrivateBytes.Format(L"%d K",pmc.PrivateUsage/1024);
				mStrPeakPrivateBytes.Format(L"%d K",pmc.PeakPagefileUsage/1024);
				//mStrVirtualSize.Format(L"%d K",pmc.QuotaPagedPoolUsage/1024);
				mStrPageFaults.Format(L"%d",pmc.PageFaultCount );
				mStrWorkingSet.Format(L"%d K",pmc.WorkingSetSize/1024);
				mStrPeakWorkingSet.Format(L"%d K",pmc.PeakWorkingSetSize/1024);

				double  WSPrivate = mProcess.GetWsPrivate_PDH(pData);
				mStrWSPrivate.Format(L"%.0f K",WSPrivate/1024);//
				mStrWSShareable.Format(L"%.0f K",(pmc.WorkingSetSize-WSPrivate)/1024);

				//----------------
				IO_COUNTERS  IOCounter;
				GetProcessIoCounters(pData->hProcess,&IOCounter);

				mStrReads.Format(L"%u",IOCounter.ReadOperationCount);//
				mStrWrites.Format(L"%u",IOCounter.WriteOperationCount);//
				mStrOther.Format(L"%u",IOCounter.OtherOperationCount);//

				
				 
			}
			

		}



		
	}


	this->UpdateData(FALSE);
}
