// PerformanceView.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "PerformanceBox.h"
#include "MemCompBox.h"
#include "SMBIOS.h"


extern "C" {
#include <powrprof.h>
}
#pragma comment(lib , "PowrProf.lib") // 




#include <atlbase.h>

// #include <Iphlpapi.h>
#pragma comment(lib , "Iphlpapi.lib") //网络数据
// CPerformanceBox

static  CPerformanceBox* pThisBoxView;

static   BOOL FlagStartDiskMon = FALSE;


typedef struct _PROCESSOR_POWER_INFORMATION {
	ULONG  Number;
	ULONG  MaxMhz;
	ULONG  CurrentMhz;
	ULONG  MhzLimit;
	ULONG  MaxIdleState;
	ULONG  CurrentIdleState;
} PROCESSOR_POWER_INFORMATION , *PPROCESSOR_POWER_INFORMATION;


UINT Thread_GetDiskOtherStaticInfo(LPVOID lparam)
{
	CPerformanceBox * pWnd =  (CPerformanceBox * )lparam;

	if(pWnd !=NULL)
	{
		pWnd->_GetDiskOtherStaticInfo();
	}

		
	
	//MSB (GetDiskVolFailCount);
	if(GetDiskVolFailCount>0)
	{
		pWnd->_GetDiskLetterUseWmi();
	}



	FlagStartDiskMon = TRUE;



	::AfxEndThread(0);

	return 0;
}



UINT Thread_LoadDiskStaticInfo(LPVOID lparam)
{
	PerferListData * pData =  (PerferListData * )lparam;

	if(pData !=NULL)
	{
		pThisBoxView->_LoadDiskStaticInfo(pData);
	}


	

	::AfxEndThread(0);

	return 0;
}






//============================


IMPLEMENT_DYNCREATE(CPerformanceBox, CFormView)

CPerformanceBox::CPerformanceBox()
: CFormView(CPerformanceBox::IDD)
, StrCPUName(_T(""))
, pInfoBoxCpu(NULL)
, pInfoBoxMemory(NULL)
, nLogicalProcessor(0)
, pTotalCpuBox(NULL)
, pCpuBox(NULL)

, nStep(0)
, FlagFirstUpdate(TRUE)
, ShowThisPage(TRUE)
, MaxCPUSpeed(0)
, pViewClass(NULL)
, pHotBox(NULL)
, CurrentDiskCount(0)
{

	 ArraySize= sizeof(float)*60;
}

CPerformanceBox::~CPerformanceBox()
{
}

void CPerformanceBox::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_PERFORMANCEITEM, mPItemList);

}

BEGIN_MESSAGE_MAP(CPerformanceBox, CFormView)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_MESSAGE(UM_TIMER,OnUMTimer)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PERFORMANCEITEM, &CPerformanceBox::OnLvnItemchangedListPerformanceitem)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CHANGEGRAPHTO_OVERALLUTILIZATION, &CPerformanceBox::OnPop_ChangeGraphToOverallutilization)
	ON_COMMAND(ID_CHANGEGRAPHTO_LOGICALPROCESSORS, &CPerformanceBox::OnPop_ChangeGraphToLogicalprocessors)
	//	ON_WM_CREATE()
	ON_WM_INITMENUPOPUP()
	
	ON_COMMAND(ID_PERFORMANCE_SHOWKERNELTIMES, &CPerformanceBox::OnPerformanceShowkerneltimes)
	ON_UPDATE_COMMAND_UI(ID_PERFORMANCE_SHOWKERNELTIMES, &CPerformanceBox::OnUpdatePerformanceShowkerneltimes)
	
	
	ON_COMMAND(ID_VIEW_CPU, &CPerformanceBox::OnViewCpu)
	ON_COMMAND(ID_VIEW_MEMORY, &CPerformanceBox::OnViewMemory)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PERFORMANCEITEM, &CPerformanceBox::OnNMRClickListPerformanceitem)
	ON_COMMAND(ID_PERFORMANCETYPE_HIDEGRAPHS, &CPerformanceBox::OnPop_PerformanceListShowHideGraphs)
	ON_UPDATE_COMMAND_UI(ID_PERFORMANCETYPE_HIDEGRAPHS, &CPerformanceBox::OnUpdatePerformancetypeHidegraphs)
	ON_WM_CREATE()
	ON_COMMAND(ID_CHANGEGRAPHTO_NUMANODES, &CPerformanceBox::OnPop_ChangeGraphToNumaNodes)	
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, SetTipText)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_PERFORMANCE_GRAPHSUMMARYVIEW, &CPerformanceBox::OnPop_GraphSummaryView)
	
	ON_COMMAND(ID_PERFORMANCETYPE_SUMMARYVIEW, &CPerformanceBox::OnPop_PerformanceListSummaryView)

END_MESSAGE_MAP()


// CPerformanceBox diagnostics

#ifdef _DEBUG
void CPerformanceBox::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPerformanceBox::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPerformanceBox message handlers

HBRUSH CPerformanceBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here





	CString StrFontName;
	StrFontName.LoadStringW(IDS_FONTNAME);


	if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(theApp.WndBkgColor);
		int nID=pWnd->GetDlgCtrlID();

		CFont StaticFont;

		if(nID== IDC_TIP1|| nID== IDC_TIP2  )
		{
			pDC->SetTextColor(RGB(112,112,112));
		}
		if(nID== IDC_ITEMLABEL )
		{
			pDC->SetTextColor(theApp.WndTextColor);		

			StaticFont.CreateFont(30,   // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_NORMAL,             // nWeight     FW_NORMAL,     FW_BOLD
				FALSE,                     // bItalic
				FALSE,                     // bUnderline下划线标记，需要下划线把这里设置成TRUE
				0,                         // cStrikeOut
				DEFAULT_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				StrFontName);                 // lpszFacename

			pDC->SelectObject(StaticFont);

			//pDC->SetBkColor(RGB(251, 247, 200));
			//pDC->SetBkMode(TRANSPARENT);
			//  return (HBRUSH) m_brush.GetSafeHandle();
		}

		if(nID== IDC_ITEMNAME )
		{
			pDC->SetTextColor(theApp.WndTextColor);	


			StaticFont.CreateFont(17,   // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_NORMAL,             // nWeight     FW_NORMAL,     FW_BOLD
				FALSE,                     // bItalic
				FALSE,                     // bUnderline下划线标记，需要下划线把这里设置成TRUE
				0,                         // cStrikeOut
				DEFAULT_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				StrFontName);                 // lpszFacename

			pDC->SelectObject(StaticFont);

			//pDC->SetBkColor(RGB(251, 247, 200));
			//pDC->SetBkMode(TRANSPARENT);
			//  return (HBRUSH) m_brush.GetSafeHandle();
		}

		StaticFont.DeleteObject();

	}



	// TODO:  Return a different brush if the default is not desired
	return theApp.BkgBrush;
}

void CPerformanceBox::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	//-----------------------------------------
	PlaceAllCtrl();
	InvalidateIfVisible(this);	 

}

void CPerformanceBox::Init(void)
{





	GetCPUInfo();

	pThisBoxView = this;

	//-------- init  logigal Processor Usage ------------------


	unsigned long bytesreturned;

	spi_old = new SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION [nLogicalProcessor];
	spi = new SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION [nLogicalProcessor];

	memset(spi_old,0,sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)*nLogicalProcessor);
	memset(spi,0,sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)*nLogicalProcessor);

	MyNtQuerySystemInformation =(API_NtQuerySystemInformation) GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQuerySystemInformation");		

	MyNtQuerySystemInformation(SystemProcessorPerformanceInformation,spi_old, (sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)*nLogicalProcessor),&bytesreturned);


	

	//-------------------------- Left Item  lsit----------------------------

	//SetWindowTheme(mPItemList.GetSafeHwnd(),L"explorer", NULL);


	CRect rcCtrl;
	mPItemList.GetClientRect(rcCtrl);

	mPItemList.SetExtendedStyle(mPItemList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES | LVS_OWNERDRAWFIXED|LVS_EX_DOUBLEBUFFER);  //| LVS_EX_GRIDLINES |LVS_EX_CHECKBOXES



	mPItemList.SetParent(this->GetParent());

	mPItemList.InsertColumn(0,L"",0,0); //标题
	mPItemList.InsertColumn(1,L"",0,0);//第二行性能显示
	mPItemList.InsertColumn(2,L"",0,0);//设备名称
	mPItemList.InsertColumn(3,L"",0,0);//Tip1
	mPItemList.InsertColumn(4,L"",0,0);//Tip2


	//mPItemList.ShowScrollBar(SB_VERT,TRUE);





	//------------------------------ info box ------------------------------------------------------------------------

	//--------------  CPU---------

	pInfoBoxCpu = new CInfoBox;

	pInfoBoxCpu->Create(L"",WS_CHILD|WS_VISIBLE|SS_CENTER,CRect(0,0,1,1),this);
	pInfoBoxCpu->ShowWindow(SW_SHOW);

	pInfoBoxCpu->Type = PM_CPU ;



	pInfoBoxCpu->Info[0].StrTitle  = STR_CPUINFO_0 ;
	pInfoBoxCpu->Info[1].StrTitle  = STR_CPUINFO_1 ;
	pInfoBoxCpu->Info[2].StrTitle  = STR_CPUINFO_2 ;
	pInfoBoxCpu->Info[3].StrTitle  = STR_CPUINFO_3 ;
	pInfoBoxCpu->Info[4].StrTitle  = STR_CPUINFO_4 ;
	pInfoBoxCpu->Info[5].StrTitle  = STR_CPUINFO_5 ;
	pInfoBoxCpu->Info[6].StrTitle  = STR_CPUINFO_6 ;

	//修改默认位置
	pInfoBoxCpu->Info[4].rc = pInfoBoxCpu->Info[3].rc;  pInfoBoxCpu->Info[4].rc.OffsetRect(pInfoBoxCpu->Info[3].rc.Width(),0);
	pInfoBoxCpu->Info[5].rc.left = pInfoBoxCpu->Info[0].rc.left;   	pInfoBoxCpu->Info[6].rc.left  = pInfoBoxCpu->Info[6].rc.left+pInfoBoxCpu->Info[5].rc.left+22;



	_UpdateCpuInfoBox(TRUE);

	// --------------Memory---------

	pInfoBoxMemory = NULL;
	pInfoBoxMemory = new CInfoBox;
	pInfoBoxMemory->Create(L"",WS_CHILD|WS_VISIBLE|SS_CENTER|SS_NOTIFY,CRect(0,0,1,1),this); //SS_NOTIFY必须带有不然无法改字体 颜色 等
	
	pInfoBoxMemory->Type = PM_MEMORY;

	pInfoBoxMemory->Info[0].StrTitle  = STR_MEMINFO_0;
	pInfoBoxMemory->Info[1].StrTitle  = STR_MEMINFO_1 ;
	pInfoBoxMemory->Info[2].StrTitle  = STR_MEMINFO_2 ;
	pInfoBoxMemory->Info[3].StrTitle  = STR_MEMINFO_3 ;
	pInfoBoxMemory->Info[4].StrTitle  = STR_MEMINFO_4 ;
	pInfoBoxMemory->Info[5].StrTitle  = STR_MEMINFO_5 ;
	pInfoBoxMemory->Info[6].StrTitle  = STR_MEMINFO_6 ;

	//修改默认位置

	pInfoBoxMemory->Info[1].rc.OffsetRect(30,0); 
	pInfoBoxMemory->Info[3].rc.OffsetRect(30,0); 
	pInfoBoxMemory->Info[5].rc.OffsetRect(30,0);


	_UpdateMemoryInfoBox(TRUE);



	//------------------------------------------------------------------------------------------------------------------------------

	//------------------------------------------添加项 及  波形图 框----------------------------------------------------------------------

	//-------------------------------------------- 总 CPU --------------------------------------


	pTotalCpuBox  = new CWaveBox;
	pTotalCpuBox->Create(NULL,WS_CHILD,CRect(0,0,1,1),this);   

	pTotalCpuBox->SetLineColumn(1,1);
	pTotalCpuBox->SetColor(theApp.AppSettings.CpuColor);
	pTotalCpuBox->DrawSecondWave =  theApp.AppSettings.ShowKernelTime ;
	mPItemList.NumArray = pTotalCpuBox->Num[0];



	//-------------------------------------------- 逻辑 CPU  --------------------------------------


	CString StrItemName;
	StrItemName.LoadStringW(IDS_PITEM_CPU);





	mPItemList.InsertItem(0,StrItemName);	
	CString StrTemp=L"";
	double CurrentSpeed = _GetSurrentCpuSpeed();
	StrTemp.Format(L"0.00%%  %0.2f GHz",CurrentSpeed);

	mPItemList.SetItemText(0,1,StrTemp);
	mPItemList.SetItemText(0,2,StrCPUName);
	StrTemp=L"";

	pCpuBox = new CWaveBox;
	PerferListData * pPData =  new PerferListData;	

	pCpuBox->Create(NULL,WS_CHILD,CRect(0,0,1,1),this);  
	//--------------决定几行几列显示-------------------
	int nRow,nColum;

	_DetermineRowCol(theApp.PerformanceInfo.nLogicalProcessor,&nRow,&nColum );


	pCpuBox->SetLineColumn(nRow,nColum);
	pCpuBox->SetColor(theApp.AppSettings.CpuColor);
	pCpuBox->DrawSecondWave =  theApp.AppSettings.ShowKernelTime ;

	//--------CPU hot box -------------------
	pHotBox->nRow=nRow; pHotBox->nCol = nColum;
	pHotBox->ArrayPerLogicalCpuUsage = new float[theApp.PerformanceInfo.nLogicalProcessor];




	if(theApp.AppSettings.ProcessorDisplayMode == 1 )
	{
		pPData->pWaveBox = pCpuBox;
		pHotBox->ShowWindow(SW_HIDE);

		CWnd *pWnd=GetDlgItem(IDC_TIP1);
		CString StrTime;
		if((double)theApp.AppSettings.TimerStep == 0.5)
		{
			StrTime.LoadStringW(IDS_STRING_WAVETIME_FAST);
		}
		else if ((double)theApp.AppSettings.TimerStep == 1)
		{
			StrTime.LoadStringW(IDS_STRING_WAVETIME_NORMAL);
		}
		else if ((double)theApp.AppSettings.TimerStep == 4)
		{
			StrTime.LoadStringW(IDS_STRING_WAVETIME_SLOW);
		}
		if(pWnd!=NULL)	
		{ 
			StrTemp.Format(STR_TIP1_CPU_LOGICAL,StrTime);

			mPItemList.SetItemText(0,3,StrTemp);
			pWnd->SetWindowTextW(StrTemp);
		}



	}
	else if(theApp.AppSettings.ProcessorDisplayMode == 0 )
	{
		pPData->pWaveBox = pTotalCpuBox;
		CWnd * pWnd=GetDlgItem(IDC_TIP1);
		if(pWnd!=NULL)	
		{ 
			CString StrTemp = STR_TIP1_CPU_TOTAL;
			mPItemList.SetItemText(0,3,StrTemp);
			pWnd->SetWindowTextW(StrTemp);
		}
	 

	}
	else if(theApp.AppSettings.ProcessorDisplayMode == 2)
	{
		pPData->pWaveBox = pTotalCpuBox; //也要
		pCpuBox->ShowWindow(SW_HIDE);
		pTotalCpuBox->ShowWindow(SW_HIDE);

		CWnd *pWnd=GetDlgItem(IDC_TIP1);
		if(pWnd!=NULL)	
		{ 
			CString StrTemp = STR_TIP1_CPU_NUMA;
			mPItemList.SetItemText(0,3,StrTemp);
			pWnd->SetWindowTextW(StrTemp);
		}
	 



	}




	pPData->pInfoBox = pInfoBoxCpu;
	pPData->Type = PM_CPU;
	pPData->ID = -1; //表示此项无效
	pPData->pOtherWnd = NULL;



	mPItemList.SetItemData(0,(DWORD_PTR)pPData);
	mPItemList.SetItemText(0,4,L"100%");







	//----------------------------------------------- Memroy --------------------------------------------

	
	StrItemName.LoadStringW(IDS_PITEM_MEM);

	 
	mPItemList.InsertItem(1,StrItemName);
	StrTemp.Format(L"0.0/%0.1f GB (0.00%%)", (double)theApp.PerformanceInfo.TotalPhysMem/1024/1024/1024);
	mPItemList.SetItemText(1,1,StrTemp);	




	StrTemp.Format(L"%.0f GB %s" ,(double) (theApp.PerformanceInfo.InstalledMemKB)/1024/1024,theApp.PerformanceInfo.StrMemoryType); 
	mPItemList.SetItemText(1,2,StrTemp);

	CWaveBox *    pMemoryBox= new CWaveBox;
	CMemCompBox * pMemoryCompBox= new CMemCompBox;
	pPData =  new PerferListData;

	pPData->pWaveBox = pMemoryBox;
	pPData->pOtherWnd = pMemoryCompBox;

	pPData->pInfoBox = pInfoBoxMemory;
	pPData->Type = PM_MEMORY;
	pPData->ID = -1;


	pMemoryBox->Create(NULL,WS_CHILD,CRect(0,0,1,1),this);
	pMemoryCompBox->Create(NULL,WS_CHILD|SS_NOTIFY,CRect(0,0,1,1),this);
	mPItemList.SetItemData (1,(DWORD_PTR)pPData);

	pMemoryBox->SetLineColumn(1,1);
	pMemoryBox->SetColor(theApp.AppSettings.MemoryColor );

	pMemoryCompBox->SetColor( );

	pMemoryBox->DrawSecondWave = FALSE;

	mPItemList.SetItemText(1,3,STR_TIP3_MEMORY);
	StrTemp.Format(L"%0.1f GB", (double)theApp.PerformanceInfo.TotalPhysMem/1024/1024/1024);
	mPItemList.SetItemText(1,4,StrTemp);




	//-------------------------------------------------------------------------------------
	
 


	//枚举磁盘
	AddDiskToList();


	//------------------------------------------------------------------------------

	//枚举网卡
	AddEthernetAdapterToList();


	theApp.StartPerformancePageTimer = TRUE;



	//-----------------------------------------------------------------

	// AfxBeginThread(Thread_SetDiskList,this);  //完善磁盘监视数据  因为这个比较慢所有放独立线程


	//选中第一项  即 CPU


	mPItemList.SetItemState(0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);	 
	mPItemList.MoveWindow(0,0,187,300);


	//--------------------------Labels ---------------------


	GetDlgItem(IDC_ITEMLABEL)->SetWindowTextW(L"CPU");
	GetDlgItem(IDC_ITEMNAME)->SetWindowTextW( StrCPUName );

	PlaceAllCtrl();



	EnableToolTips(TRUE);
	mToolTip.Create(this);
	mToolTip.Activate(TRUE);
	mToolTip.AddTool(pMemoryCompBox,LPSTR_TEXTCALLBACK);
	mToolTip.SetMaxTipWidth(200);
	 


	pInfoBoxCpu->Info[1].StrInfo.Format(L"%0.2f GHz",CurrentSpeed);

 





}


void CPerformanceBox::GetCPUInfo(void)
{ 

	CString strPath=_T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");//注册表子键路径  
	CRegKey regkey;//定义注册表类对象  
	LONG lResult;//LONG型变量－反应结果  
	lResult=regkey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_QUERY_VALUE ); //打开注册表键   注意 要用 读 的权限 KEY_ALL_ACCESS 在某些用户会导致错误
	if (lResult==ERROR_SUCCESS)  
	{  
		WCHAR chCPUName[50] = {0};  
		DWORD dwSize=50;   

		//获取ProcessorNameString字段值  
		if (ERROR_SUCCESS == regkey.QueryStringValue(_T("ProcessorNameString"),chCPUName,&dwSize))  
		{  
			StrCPUName = chCPUName;  
		}  

	}  


	nLogicalProcessor  =1;


	//查询CPU主频  
	DWORD dwValue;  
	if (ERROR_SUCCESS == regkey.QueryDWORDValue(_T("~MHz"),dwValue))  
	{  
		MaxCPUSpeed = dwValue;  

	}  
	regkey.Close();//关闭注册表  

	//UpdateData(FALSE);  

	//获取CPU核心数目  
	/*   SYSTEM_INFO si;  
	memset(&si,0,sizeof(SYSTEM_INFO));  
	GetSystemInfo(&si);  
	nLogicalProcessor = si.dwNumberOfProcessors;  
	*/

	PM.GetCpuInfo();

	nLogicalProcessor =  theApp.PerformanceInfo.nLogicalProcessor;

	if(theApp.PerformanceInfo.nLogicalProcessor == 0)
	{
		// 获取CPU核心数目  
		SYSTEM_INFO si;  
		memset(&si,0,sizeof(SYSTEM_INFO));  
		GetSystemInfo(&si);  
		nLogicalProcessor = si.dwNumberOfProcessors;  
		theApp.PerformanceInfo.nLogicalProcessor = nLogicalProcessor;


	}




}


LRESULT CPerformanceBox::OnUMTimer(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default

	//::AfxBeginThread(Thread_UpDatePM,this);



	//mPItemList.SetRedraw(0);


	ShowThisPage =( theApp.pSelPage == (CFormView *)GetParent());

	UpdateAllPMInfo();

	// mPItemList.SetRedraw(1);

	nStep++;
	if(nStep==60)nStep =0;


	FlagFirstUpdate = FALSE; 


	return 0;

}

void CPerformanceBox::OnLvnItemchangedListPerformanceitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	INT nSel = 0;
	nSel = mPItemList.GetNextItem( -1, LVNI_SELECTED );




	if(nSel== -1) return;


	int n = mPItemList.GetItemCount();



	//	PerferListData  *pPData;

	CWnd *pWnd= NULL;
	CString StrTip;


	for(int i=0;i<n;i++)
	{


		PerferListData * pPData = (PerferListData*)mPItemList.GetItemData(i);	

		if(pPData == NULL) { continue ;  } 
		if(pPData->pWaveBox==NULL) {continue ;  }
		if(pPData->pInfoBox==NULL) {continue ;  }

		if(i==nSel)
		{ 


			pPData->pWaveBox->ShowWindow(SW_SHOW);
			pPData->pInfoBox->ShowWindow(SW_SHOW);
			if(pPData->pOtherWnd != NULL)
			{
				//MSG(nSel)
				pPData->pOtherWnd->ShowWindow(SW_SHOW);
			}



			if(pPData->Type == PM_DISK)
			{

				pWnd=GetDlgItem(IDC_TIP5);
				if(pWnd!=NULL)
				{
					pWnd->SetWindowTextW(STR_TIP5_DISK); 
					pWnd->ShowWindow(SW_SHOW);
				}
				pWnd=GetDlgItem(IDC_TIP6);
				if(pWnd!=NULL)
				{
					pWnd->SetWindowTextW(pPData->StrOther1); 
					pWnd->ShowWindow(SW_SHOW);
				}
				pWnd=GetDlgItem(IDC_TIP7);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }
				pWnd=GetDlgItem(IDC_TIP8);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }
			}
			else if(pPData->Type == PM_MEMORY)
			{
				pWnd=GetDlgItem(IDC_TIP5);
				if(pWnd!=NULL)
				{
					pWnd->SetWindowTextW( STR_TIP5_MEMORY); 
					pWnd->ShowWindow(SW_SHOW);
				}
				pWnd=GetDlgItem(IDC_TIP6);
				if(pWnd!=NULL)
				{
					pWnd->SetWindowTextW(L""); 
					pWnd->ShowWindow(SW_SHOW);
				}
				pWnd=GetDlgItem(IDC_TIP7);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
				pWnd=GetDlgItem(IDC_TIP8);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
			}
			else
			{



				pWnd=GetDlgItem(IDC_TIP5);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
				pWnd=GetDlgItem(IDC_TIP6);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
				pWnd=GetDlgItem(IDC_TIP7);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
				pWnd=GetDlgItem(IDC_TIP8);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }

			}


		}
		else  
		{ 


			pPData->pWaveBox->ShowWindow(SW_HIDE);
			pPData->pInfoBox->ShowWindow(SW_HIDE);
			if(pPData->pOtherWnd != NULL)
			{
				pPData->pOtherWnd->ShowWindow(SW_HIDE);
			}
		} 


	}





	if(nSel==0) //CPU
	{


		if(theApp.AppSettings.ProcessorDisplayMode  ==0 ) // 显示总CPU 模式
		{
			pWnd=GetDlgItem(IDC_TIP3);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }
			pWnd=GetDlgItem(IDC_TIP4);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }
			pWnd=GetDlgItem(IDC_TIP2);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }
			pHotBox->ShowWindow(SW_HIDE);	
		}
		else if (theApp.AppSettings.ProcessorDisplayMode  ==1   )
		{
			pWnd=GetDlgItem(IDC_TIP3);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
			pWnd=GetDlgItem(IDC_TIP4);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
			pWnd=GetDlgItem(IDC_TIP2);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }
			pHotBox->ShowWindow(SW_HIDE);	
		}
		else if (theApp.AppSettings.ProcessorDisplayMode  == 2 )
		{
			pWnd=GetDlgItem(IDC_TIP3);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
			pWnd=GetDlgItem(IDC_TIP4);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
			pWnd=GetDlgItem(IDC_TIP2);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
			pCpuBox->ShowWindow(SW_HIDE);
			pTotalCpuBox->ShowWindow(SW_HIDE);	
			pHotBox->ShowWindow(SW_SHOW);	

		}

	}
	else //其其他项目时 标签2 3 4 都显示！
	{
		pWnd=GetDlgItem(IDC_TIP3);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }
		pWnd=GetDlgItem(IDC_TIP4);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }
		pWnd=GetDlgItem(IDC_TIP2);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }
		pHotBox->ShowWindow(SW_HIDE);	
	}









	pWnd=GetDlgItem(IDC_TIP1);
	if(pWnd!=NULL)
	{					
		StrTip = mPItemList.GetItemText(nSel,3);
		pWnd->SetWindowTextW(StrTip); //后两个是宽和高不是位置
	}


	pWnd=GetDlgItem(IDC_TIP2);
	if(pWnd!=NULL)
	{					
		StrTip = mPItemList.GetItemText(nSel,4);
		pWnd->SetWindowTextW(StrTip); //后两个是宽和高不是位置
	}




	PerferListData * pPData = (PerferListData*)mPItemList.GetItemData(nSel);

	if(pPData->Type == PM_ETHERNET)
	{
		SetScrollSizes(MM_TEXT, CSize(BOX_W_MIN, 300)); 
	}
	else
	{
		SetScrollSizes(MM_TEXT, CSize(BOX_W_MIN, 400)); 

	}












	this->PlaceAllCtrl(); 

	GetDlgItem(IDC_ITEMLABEL)->SetWindowTextW(mPItemList.GetItemText(nSel,0));
	GetDlgItem(IDC_ITEMNAME)->SetWindowTextW(mPItemList.GetItemText(nSel,2));

	UpdateInfoBox();

	mPItemList.Invalidate();
	this->Invalidate();






	*pResult = 0;
}

int CPerformanceBox::AddEthernetAdapterToList(void)
{

 




	//---------------------- new ------------------------------------------





	WSADATA WsaData;   
	WSAStartup(MAKEWORD(1,1), &WsaData);


	DWORD dwSize = 0;
	DWORD dwRetVal = 0;

	unsigned int i = 0;

	// Set the flags to pass to GetAdaptersAddresses
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
	// default to unspecified address family (both)

	ULONG family = AF_UNSPEC;
	LPVOID lpMsgBuf = NULL;

	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	ULONG OutBufLen = 0;
	ULONG Iterations = 0;

	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
	PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
	PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
	IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
	IP_ADAPTER_PREFIX *pPrefix = NULL;

	PIP_ADAPTER_UNICAST_ADDRESS pCurrentUnicastAddr = NULL;

	// Allocate a 15 KB buffer to start with.

	OutBufLen = 15000;

	do 
	{
		pAddresses = (IP_ADAPTER_ADDRESSES *) HeapAlloc(GetProcessHeap(), 0, (OutBufLen));
		if (pAddresses == NULL) break;		
		dwRetVal =GetAdaptersAddresses(family, flags, NULL, pAddresses, &OutBufLen);
		if (dwRetVal == ERROR_BUFFER_OVERFLOW)
		{
			HeapFree(GetProcessHeap(), 0, (pAddresses));   pAddresses = NULL;
		} 
		else 
		{
			break;
		}

		Iterations++;

	} while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < 3)); //3是最大重试次数

	//成功，实际获取信息

	if (dwRetVal == NO_ERROR)
	{

		// If successful, output some information from the data we received
		pCurrAddresses = pAddresses;

		CString StrIPv4,StrIPv6;
		CString StrInfo,StrTemp;
		while (pCurrAddresses)
		{
			if(pCurrAddresses->IfType != IF_TYPE_ETHERNET_CSMACD && pCurrAddresses->IfType != IF_TYPE_IEEE80211 ) 
			{				
				pCurrAddresses = pCurrAddresses->Next; continue;//
			}		


			CString StrTypeTitle = L" "; 
			StrTypeTitle.LoadStringW(IDS_PITEM_NET);
			if(pCurrAddresses->IfType==IF_TYPE_IEEE80211)
			{
				StrTypeTitle.LoadStringW(IDS_PITEM_WIRELESS);
			}

			CString StrName  ;
			StrName= pCurrAddresses->Description;//网卡描述    AdapterName是数字组成的名称
			PerferListData *pPData = _InsertNetAdapterItem(StrTypeTitle,StrName,pCurrAddresses->IfIndex); 		

			StrInfo = L"";
			StrTemp.Format(L"%wS\n", pCurrAddresses->FriendlyName);
			StrInfo=StrInfo+StrTemp;

			pUnicast = pCurrAddresses->FirstUnicastAddress;
			if (pUnicast != NULL)
			{
				WCHAR buff [1024];
				DWORD bufflen = 1024;
				StrIPv4=L""; StrIPv6=L"";
				for (i = 0; pUnicast != NULL; i++)
				{
					// IPV4  
					if (pUnicast->Address.lpSockaddr->sa_family == AF_INET)
					{
						sockaddr_in *sa_in = (sockaddr_in *)pUnicast->Address.lpSockaddr;
						StrTemp = inet_ntoa (sa_in->sin_addr);				
						StrIPv4=StrIPv4+StrTemp+L"  ";
					}
					//-------------------
					else if (pUnicast->Address.lpSockaddr->sa_family == AF_INET6)
					{
						ZeroMemory(buff,dwSize); 
						WSAAddressToString (pUnicast->Address.lpSockaddr, pUnicast->Address.iSockaddrLength,NULL, buff, &bufflen);
						//MSG(WSAGetLastError() )
						StrTemp = buff;				
						StrIPv6 =StrIPv6+StrTemp+L"  ";
					}

					pUnicast = pUnicast->Next;

				}

				ZeroMemory(buff,dwSize); 
				//StrTemp.Format(L"Number of Unicast Addresses: %d\n", i);
				//MessageBox(Str); 
			} 

			StrInfo = StrInfo+StrTypeTitle+L"\n"+StrIPv4+L"\n"+StrIPv6;
			pPData->pInfoBox->Info[6].StrInfo = StrInfo;

			pPData->pInfoBox->SetColor();




			//if (pCurrAddresses->PhysicalAddressLength != 0)
			//{
			//	Str=Str+L"Physical address: ";
			//	CString StrMAC;

			//	for (i = 0; i < (int) pCurrAddresses->PhysicalAddressLength;i++)
			//	{
			//		if (i == (pCurrAddresses->PhysicalAddressLength - 1))
			//		{
			//			StrTemp.Format(L"%.2X\n",(int) pCurrAddresses->PhysicalAddress[i]);
			//			StrMAC+=StrTemp;
			//		}
			//		else
			//		{
			//			StrTemp.Format(L"%.2X-",(int) pCurrAddresses->PhysicalAddress[i]);
			//			StrMAC+=StrTemp;
			//		}
			//	}
			//	Str+=StrMAC;
			//}


			pCurrAddresses = pCurrAddresses->Next;
		}
	} 
	//else 
	//{		
	//	if (dwRetVal != ERROR_NO_DATA)		
	//	{
	//		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) & lpMsgBuf, 0, NULL)) 
	//		{
	//			//printf("\tError: %s", lpMsgBuf);
	//			LocalFree(lpMsgBuf);
	//			if (pAddresses)HeapFree(GetProcessHeap(), 0, (pAddresses));
	//			return 0;
	//		}
	//	}
	//}
	if (pAddresses)
	{
		HeapFree(GetProcessHeap(), 0, (pAddresses));
	}


	WSACleanup();
	

	return 0;


	//====================以下为注册表读取方式 暂时保留！！！！=================================


	/*


	CInfoBox *pInfoBox ;

	HKEY hKey, hSubKey, hNdiIntKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,L"System\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	return  0;

	int dwIndex = 0;
	DWORD dwBufSize = 256;
	DWORD dwDataType;
	WCHAR szSubKey[256];
	WCHAR szData[256];

	CString StrInfo; 
	CString StrAdapterName =L"" ; 



	CString  StrPdh;

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//  	System\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}枚举后 读取NetCfgInstanceId 可以获得一个ID

	//	     HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\Tcpip\Parameters\Interfaces\加这个ID   可以读取 IP地址等信息   

	//	  HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Network\{4D36E972-E325-11CE-BFC1-08002BE10318}  完了链接名称等


	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//


	while(RegEnumKeyEx(hKey, dwIndex++, szSubKey, &dwBufSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
	{

	if(RegOpenKeyEx(hKey, szSubKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
	{	
	if(RegOpenKeyEx(hSubKey, L"Ndi\\Interfaces", 0, KEY_READ, &hNdiIntKey) == ERROR_SUCCESS)
	{
	dwBufSize = 256;
	memset(szData, 0, sizeof(szData));
	if(RegQueryValueEx(hNdiIntKey, L"LowerRange", 0, &dwDataType, (BYTE*)szData, &dwBufSize) == ERROR_SUCCESS)
	{
	int NetType = 0;

	if(lstrcmp( szData, L"ethernet") == 0 ) NetType = 1;//普通网卡
	if(lstrcmp( szData, L"wlan,ethernet,vwifi") == 0 ) NetType = 2;//无线网卡

	if( NetType!= 0 )	 //	判断是不是以太网卡
	{

	dwBufSize = 256;
	if(RegQueryValueEx(hSubKey, L"*PhysicalMediaType", 0, &dwDataType, (BYTE*)szData, &dwBufSize) != ERROR_SUCCESS)
	{
	goto	LOOP_01  ;  //不要用 continue 否则 有一个满足 就 会直接跳出循环
	}

	dwBufSize = 256;	dwDataType = REG_SZ ;memset(szData, 0, sizeof(szData));
	if(RegQueryValueEx(hSubKey, L"NoDisplayClass", 0, &dwDataType, (BYTE*)szData, &dwBufSize) == ERROR_SUCCESS)
	{
	if(lstrcmp( szData, L"1") == 0)	
	{
	goto	LOOP_01  ;//不要用 continue 否则 有一个满足 就 会直接跳出循环
	}


	}
	------------------------------------------------------------------

	dwBufSize = 256;
	if(RegQueryValueEx(hSubKey, L"DriverDesc", 0, &dwDataType, (BYTE*)szData, &dwBufSize) == ERROR_SUCCESS)
	{
	PerferListData *pPData =  new PerferListData;
	CString StrName = szData;

	// szData 中便是适配器详细描述
	int n =mPItemList.GetItemCount();
	CString StrTypeTitle = L"Ethernet"; 
	if(NetType == 2)
	{
	StrTypeTitle =  L"Wireless"; 
	}

	mPItemList.InsertItem(n,StrTypeTitle);

	mPItemList.SetItemText(n,1,L"S: 0 Kbps R: 0 Kbps");
	mPItemList.SetItemText(n,2,StrName);  // 注册表 DriverDesc 读取到 szData 是网卡显示名称
	mPItemList.SetItemData(n,(DWORD_PTR)pPData);



	CWaveBox * pEthernetBox= new CWaveBox;
	pInfoBox = new CInfoBox;	

	pInfoBox->Create(L"",WS_CHILD|WS_VISIBLE|SS_CENTER,CRect(0,0,1,1),this);
	pEthernetBox->Create(NULL,WS_CHILD,CRect(0,0,1,1),this);

	pPData->Type = PM_ETHERNET;
	pInfoBox->Type  = PM_ETHERNET;
	pPData->ID = dwIndex-1;	

	pPData->pWaveBox = pEthernetBox; //注意顺序必须在SetItemData前 否则有问题不知为何：(
	pPData->pInfoBox = pInfoBox;
	pPData->pWaveBox->DrawSecondWave = TRUE;
	pPData->pOtherWnd = NULL;

	pInfoBox->Info[0].StrTitle  = L"Send" ;
	pInfoBox->Info[2].StrTitle  = L"Receive" ;

	pInfoBox->Info[0].StrInfo=L"0 Kbps";
	pInfoBox->Info[2].StrInfo=L"0 Kbps";

	pInfoBox->Info[0].Type=2;  pInfoBox->Info[2].Type=1; //图例类型
	pInfoBox->Info[6].StrTitle  = L"Adapter name:\nConnection type:\nIPv4 address:\nIPv6 address:";
	pInfoBox->Info[6].rc.left-= 100;


	mPItemList.SetItemText(n,3,L"Throughput");
	mPItemList.SetItemText(n,4,L"100 Kbps");



	pEthernetBox->SetLineColumn(1,1);
	pEthernetBox->SetColor(RGB(167,79,1),RGB(238,222,207));

	pPData->MaxVar = 100*1024/8; //默认最大值100Kbps


	//	---------------  get Connect Name ---------
	CString StrAdapterName;

	dwBufSize = 256;
	if(RegQueryValueEx(hSubKey, L"NetCfgInstanceID", 0, &dwDataType, (BYTE*)szData, &dwBufSize) == ERROR_SUCCESS)
	{

	CString StrPath =  szData;
	StrPath = L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\"+StrPath+L"\\Connection";

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, StrPath, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
	{
	dwBufSize = 256;
	if(RegQueryValueEx(hSubKey, L"Name", 0, &dwDataType, (BYTE*)szData, &dwBufSize) == ERROR_SUCCESS)
	{

	StrAdapterName =  szData;
	}
	}

	}

	pInfoBox->Info[6].StrInfo.Format(L"%s\nEthernet\n",StrAdapterName );
	_LoadNetworkStaticInfo (pPData); //注意：这个函数位置要放在最后 否则 pPData数据不全会有问题


	//	---- Init Pdh  of ethernet Adapter----

	StrName.Replace(L"(",L"[");
	StrName.Replace(L")",L"]");
	StrName.Replace(L"/",L"_");

	if ( PdhOpenQuery(NULL, NULL, &pPData->Query)== ERROR_SUCCESS )  //总
	{

	StrPdh.Format(L"\\Network Interface(%s)\\Bytes Total/sec", StrName );  
	PdhAddCounter(pPData->Query, StrPdh, 0, &pPData->Counter);  
	}

	if ( PdhOpenQuery(NULL, NULL, &pPData->QueryA)== ERROR_SUCCESS )  //Send
	{

	StrPdh.Format(L"\\Network Interface(%s)\\Bytes Received/sec", StrName );  
	PdhAddCounter(pPData->QueryA, StrPdh, 0, &pPData->CounterA);  
	}

	if ( PdhOpenQuery(NULL, NULL, &pPData->QueryB)== ERROR_SUCCESS )  //Receive
	{

	StrPdh.Format(L"\\Network Interface(%s)\\Bytes Sent/sec", StrName );  
	PdhAddCounter(pPData->QueryB, StrPdh, 0, &pPData->CounterB);  
	}


	}
	}
	}
	RegCloseKey(hNdiIntKey);
	}
	RegCloseKey(hSubKey);


	}

	LOOP_01:	dwBufSize = 256;
	}	//end of while 

	RegCloseKey(hKey);


	*/



}

void CPerformanceBox::_UpdateCpuInfoBox(BOOL UpdateAll)
{
	if(!ShowThisPage) return ;


	PERFORMANCE_INFORMATION MyPMInfo;
	ZeroMemory(&MyPMInfo, sizeof(PERFORMANCE_INFORMATION));
	MyPMInfo.cb = sizeof(PERFORMANCE_INFORMATION);
	GetPerformanceInfo(&MyPMInfo,sizeof(PERFORMANCE_INFORMATION));

	theApp.PerformanceInfo.ProcessCount =MyPMInfo.ProcessCount ;

	//MSG(0)

	CString StrUpTime;




	StrUpTime.Format(L"%d:%02d:%02d:%02d",theApp.UpTimeDay,theApp.UpTimeHour,theApp.UpTimeMin,(int)theApp.UpTimeSec);

	double  CurrentSpeed = 0;
	double  nThreads;
	double  nHandles;


	nThreads = MyPMInfo.ThreadCount;//PM.PdhGetInfo(L"\\System\\Threads");
	nHandles = MyPMInfo.HandleCount;//PM.PdhGetInfo(L"\\Process(_Total)\\Handle Count");


	//CurrentSpeed = PM.PdhGetInfo(L"\\Processor Information(0,0)\\Processor Frequency");


	
	PROCESSOR_POWER_INFORMATION  *PPInfo = new PROCESSOR_POWER_INFORMATION [theApp.PerformanceInfo.ProcessCount] ;
	NTSTATUS Ret = CallNtPowerInformation( ProcessorInformation,NULL, 0,PPInfo,sizeof(PROCESSOR_POWER_INFORMATION)*theApp.PerformanceInfo.ProcessCount );

	 

	CurrentSpeed =  PPInfo[0].CurrentMhz;	
	CurrentSpeed=CurrentSpeed/1000;//注意这个cpu频率是 除以1000不是1024;






   /// CPU 当前速度该在函数  UpdateAllPMInfo 中获取 否则作出列表 不同步
 
	//启动时提前获取一次否则无显示

	pInfoBoxCpu->Info[0].StrInfo.Format(L"%0.0f%%", theApp.PerformanceInfo.CpuUsage) ;
	//pInfoBoxCpu->Info[1].StrInfo.Format(L"%0.2f GHz",CurrentSpeed);
	pInfoBoxCpu->Info[2].StrInfo.Format(L"%d",theApp.PerformanceInfo.ProcessCount);  //Processes
	pInfoBoxCpu->Info[3].StrInfo.Format(L"%d",(int)nThreads);  //Threads
	pInfoBoxCpu->Info[4].StrInfo.Format(L"%d",(int)nHandles);//Handles
	pInfoBoxCpu->Info[5].StrInfo  = StrUpTime ;//Up time
  
 

	 

	if(UpdateAll)
	{
		CString StrCpuInfo;

		CString StrMaxSpeed;

		if(MaxCPUSpeed>1000) //注意这个不是1024进制
		{
			MaxCPUSpeed= MaxCPUSpeed/1000;
			StrMaxSpeed.Format(L"%.2f GHz",MaxCPUSpeed);

		}
		else
		{
			StrMaxSpeed.Format(L"%.2f MHz",MaxCPUSpeed);
		}

		StrCpuInfo.Format(L"%s\n%d\n%d\n%d\n%s\n%s\n%s\n%s",StrMaxSpeed, theApp.PerformanceInfo.nPhysicalProcessorPackages,theApp.PerformanceInfo.nProcessorCores,theApp.PerformanceInfo.nLogicalProcessor,L" ",theApp.PerformanceInfo.StrL1Cache,theApp.PerformanceInfo.StrL2Cache,theApp.PerformanceInfo.StrL3Cache);

		pInfoBoxCpu->Info[6].StrInfo  = StrCpuInfo ;

	} 

	delete []  PPInfo;
}

int CPerformanceBox::AddDiskToList(int LastID)
{

	

	CInfoBox *pInfoBox= NULL;

	HKEY hKeyList,hKeyDisk;
	DWORD dwDataType;
	DWORD dwBufSize = MAX_PATH;

	WCHAR StrRegData[MAX_PATH] ;
	WCHAR  RegData;

	if(  ERROR_SUCCESS  != RegOpenKeyEx(HKEY_LOCAL_MACHINE,L"SYSTEM\\CurrentControlSet\\services\\Disk\\Enum",0,KEY_READ,&hKeyList) )
	{
		return 0;
	}

	RegQueryValueEx(hKeyList, L"Count", 0, &dwDataType, (BYTE*)&RegData, &dwBufSize );

	int i,n;
	n = (int) RegData;

	CString  StrValue;
	CString  StrKey ;

	CString StrID;


	int nItem = 2+CurrentDiskCount;


	CString StrItemName;

	StrItemName.LoadStringW(IDS_PITEM_DISK);

	
	 

	for(i=0;i<n;i++)
	{
		if(i<CurrentDiskCount) continue ;

	

		dwBufSize = MAX_PATH;
		dwDataType = REG_SZ;
		StrValue.Format(L"%d",i);
		RegQueryValueEx(hKeyList, StrValue, 0, &dwDataType, (BYTE*)StrRegData, &dwBufSize );
		StrKey=L"SYSTEM\\CurrentControlSet\\Enum\\";
		StrKey=StrKey+StrRegData;

		

		RegOpenKeyEx(HKEY_LOCAL_MACHINE,StrKey,0,KEY_READ,&hKeyDisk);
		dwBufSize = MAX_PATH;
		RegQueryValueEx(hKeyDisk, L"FriendlyName", 0, &dwDataType, (BYTE*)StrRegData, &dwBufSize );


	



			
		StrID.Format(L"%s %d",StrItemName,i);
		

		mPItemList.InsertItem(nItem,StrID);

		
	 

		mPItemList.SetItemText(nItem,1,L"0.00%");
		mPItemList.SetItemText(nItem,2,StrRegData); //硬盘名称 比如 ADATA 等等


		

		CWaveBox * pDiskBox= new CWaveBox;
		CWaveBox * pDiskBox2= new CWaveBox; //第二个 波形图

		PerferListData *pPData =  new PerferListData;
		pInfoBox = new CInfoBox;


		

		pPData->Type = PM_DISK;


		//测试ID 是否有效  当ID 不是最后一个 的硬盘被弹出后  iD变为不连续；

		pPData->ID = _TestDiskID(LastID);//此函数会将正确 硬盘传给 pPData->ID ;
		LastID = pPData->ID+1;

		//MSB(LastID)


	

		pPData->pWaveBox = pDiskBox; 
		pPData->pInfoBox = pInfoBox;
		pPData->pOtherWnd = pDiskBox2;

		pPData->MaxVar = 100*1024; //默认最大值100KB/s
		pPData->StrOther1 = L"100 KB/s";

		//pPData->StrOther0 = L" ";//预先存入26个空格用于接收对应位置盘符信息！！！！！！！
 



		pDiskBox->Create(NULL,WS_CHILD,CRect(0,0,1,1),this);
		pDiskBox2->Create(NULL,WS_CHILD,CRect(0,0,1,1),this);


		pDiskBox2->DrawSecondWave = TRUE;

		pInfoBox->Create(L"",WS_CHILD|WS_VISIBLE|SS_CENTER,CRect(0,0,1,1),this);

		pInfoBox->Info[0].StrTitle  = STR_DISKINFO_0 ;
		pInfoBox->Info[1].StrTitle  = STR_DISKINFO_1 ;
		pInfoBox->Info[2].StrTitle  = STR_DISKINFO_2 ;
		pInfoBox->Info[3].StrTitle  = STR_DISKINFO_3 ;		 
		pInfoBox->Info[6].StrTitle  = STR_DISKINFO_6 ;

		pInfoBox->Info[2].Type=1;  pInfoBox->Info[3].Type=2; //图例类型

		//写入初始数据 为了视觉效果 不停顿
		pInfoBox->Info[0].StrInfo= L"0%";  pInfoBox->Info[1].StrInfo= L"0.0 ms";
		pInfoBox->Info[2].StrInfo= L"0.0 KB/s";  pInfoBox->Info[3].StrInfo= L"0.0 KB/s"; //图例类型

		pInfoBox->Type = PM_DISK;

		pInfoBox->Info[3].rc.OffsetRect(30,0);
		pInfoBox->Info[6].rc.OffsetRect(22,0);

		pPData->DataA=pPData->DataB=pPData->DataC =0;

		pInfoBox->SetColor();

		//---------------------------------固定信息--------------------------------------


		

	   ::AfxBeginThread(Thread_LoadDiskStaticInfo,pPData );


		//--------------------



		mPItemList.SetItemText(nItem,3,STR_DISKINFO_0);
		mPItemList.SetItemText(nItem,4,L"100%");

		 

		mPItemList.SetItemData (nItem,(DWORD_PTR)pPData);

		pDiskBox->SetLineColumn(1,1);
		pDiskBox->SetColor(theApp.AppSettings.DiskColor);

		pDiskBox2->SetLineColumn(1,1);
		pDiskBox2->SetColor(theApp.AppSettings.DiskColor);



		nItem++;


	}

	
		
		
	RegCloseKey(hKeyList);
	RegCloseKey(hKeyDisk);

 
  ::AfxBeginThread(Thread_GetDiskOtherStaticInfo,this);

	 

	return n;
}

void CPerformanceBox::PlaceAllCtrl(void)
{


	CRect rc;
	this->GetClientRect(rc);
	CRect rcWaveList;

	PerferListData * pData = (PerferListData*) mPItemList.GetItemData(0);
	BOOL Ret = FALSE;

	if(pData!=NULL)
	{
		if(pData->pWaveBox != NULL)
		{
			Ret= TRUE;
		}

	}


	if( Ret)     // && rc.Height()>400 
	{
		int WaveBoxBottom = rc.Height()-200;

		if(WaveBoxBottom-72<80)  WaveBoxBottom = 72+80;

		if(rc.Width()<BOX_W_MIN)
		{
			rcWaveList.SetRect(22,72,BOX_W_MIN-20,WaveBoxBottom);	
		}
		else
		{

			rcWaveList.SetRect(22,72,rc.right-20,WaveBoxBottom);	

		}



		int n = mPItemList.GetItemCount();

		INT nSel = 0;
		nSel = mPItemList.GetNextItem( -1, LVNI_SELECTED );




		for(int i=0;i<n;i++)
		{
			PerferListData * pPData = (PerferListData *)mPItemList.GetItemData(i);
			if(pPData==NULL)continue ;
			if(pPData->pInfoBox == NULL)continue ;
			
			if(pPData->pInfoBox->IsWindowVisible()|| i==nSel)  //实际上只有一个被移动  不要用当前选定项判断 移动哪个 因为可能改变没有选中项！！！
			{
				//PerferListData * pPData = (PerferListData *)mPItemList.GetItemData(i);
				//if(pPData==NULL)continue ;
				CWnd *pWnd = NULL ;
				CWnd *pBoxWnd = pPData->pWaveBox;
				if(pBoxWnd == NULL)continue ;


				//------------------------------------------------------------------
				pWnd=GetDlgItem(IDC_TIP2);
				if(pWnd!=NULL)
				{					 
					pWnd->MoveWindow(rcWaveList.right-80,rcWaveList.top-15,80,15);  //后两个是宽和高不是位置	
					InvalidateIfVisible(pWnd);
					
				}

				pWnd=GetDlgItem(IDC_TIP1);
				if(pWnd!=NULL)
				{					 
					pWnd->MoveWindow(rcWaveList.left,rcWaveList.top-15,rcWaveList.Width()-80,15);  //后两个是宽和高不是位置				
					InvalidateIfVisible(pWnd);
				}




				//--------





				//------------------------------------------------------------------



				//波形图底部提示框高度15

				if(pPData->Type == PM_MEMORY || pPData->Type == PM_DISK)
				{

					rcWaveList.bottom-=80;
					if(pPData->Type == PM_DISK) rcWaveList.bottom+=30;
					if(rcWaveList.bottom-72<80)  rcWaveList.bottom  = 72+80;

				}
				if(pPData->Type == PM_ETHERNET  )
				{
					CRect rc1;
					this->GetClientRect(rc1);
					rcWaveList.bottom =rc1.bottom-150;				
					if(rcWaveList.bottom<155)  rcWaveList.bottom  = 155;

				}


				
				//----------------------------------------------

				if(theApp.FlagSummaryView)//SummaryView状态波形图 充满窗口
				{
					CRect rcTemp;
					this->GetClientRect(rcTemp);

					if(pPData->Type == PM_MEMORY || pPData->Type == PM_DISK)
					{
						rcWaveList.bottom = rcTemp.bottom-125;
					}
					else
					{
						rcWaveList.bottom = rcTemp.bottom-40;
					}

					

				}

				pHotBox->MoveWindow(rcWaveList);
				InvalidateIfVisible(pHotBox);
				


				//----------------------------------------------

				pBoxWnd->MoveWindow(rcWaveList);
				InvalidateIfVisible(pBoxWnd);

			

				//------------------------------------------------------------------

				pWnd=GetDlgItem(IDC_TIP3);
				if(pWnd!=NULL)
				{					 
					pWnd->MoveWindow(rcWaveList.left+2,rcWaveList.bottom,rcWaveList.Width()-80,15);  //后两个是宽和高不是位置
					InvalidateIfVisible(pWnd);
				}
				pWnd=GetDlgItem(IDC_TIP4);
				if(pWnd!=NULL)
				{					 
					pWnd->MoveWindow(rcWaveList.right-50,rcWaveList.bottom,50,15);
					InvalidateIfVisible(pWnd);
				}
				//-----------

			


				pWnd=GetDlgItem(IDC_TIP5);
				if(pWnd!=NULL)
				{					 
					pWnd->MoveWindow(rcWaveList.left+2,rcWaveList.bottom+19,rcWaveList.Width()-80,15);  //后两个是宽和高不是位置
					InvalidateIfVisible(pWnd);
				}
				pWnd=GetDlgItem(IDC_TIP6);
				if(pWnd!=NULL)
				{					 
					pWnd->MoveWindow(rcWaveList.right-80,rcWaveList.bottom+19,80,15);  //后两个是宽和高不是位置				
					InvalidateIfVisible(pWnd);
				}

				

				//------------------------------移动第二波形图------------------------------------

				pBoxWnd = pPData->pOtherWnd;
				if(pBoxWnd != NULL)
				{

					rcWaveList.OffsetRect(0,rcWaveList.Height()+34);
					if(pPData->Type == PM_MEMORY)
					{
						rcWaveList.bottom = rcWaveList.top+50;
						rcWaveList.DeflateRect(2,0);
						rcWaveList.top+=2;

					}
					else
					{
						rcWaveList.bottom = rcWaveList.top+66;
					}
					pBoxWnd->MoveWindow(rcWaveList);
					InvalidateIfVisible(pBoxWnd);
					
				}


				pWnd = pPData->pInfoBox;
				if(pWnd == NULL)continue ;

				if(theApp.FlagSummaryView) //SummaryView状态移动到可以隐藏的位置！
				{
					pWnd->MoveWindow(rcWaveList.left,rcWaveList.bottom+10000,1,1);	
				}
				else
				{
				pWnd->MoveWindow(rcWaveList.left,rcWaveList.bottom+27,rcWaveList.Width(),160);	
				}

				InvalidateIfVisible(pWnd);

				//---------------------

				pWnd=GetDlgItem(IDC_TIP7);
				if(pWnd!=NULL)
				{					 
					pWnd->MoveWindow(rcWaveList.left+2,rcWaveList.bottom,rcWaveList.Width()-80,15);  //后两个是宽和高不是位置
					InvalidateIfVisible(pWnd);
				}
				pWnd=GetDlgItem(IDC_TIP8);
				if(pWnd!=NULL)
				{					 
					pWnd->MoveWindow(rcWaveList.right-80,rcWaveList.bottom,80,15);  //后两个是宽和高不是位置				
					InvalidateIfVisible(pWnd);
				}



			} //if 


		}  //for	 



	}








	//--------------

	CWnd *pWnd= NULL;
	CRect rcTemp;
	CRect rcTemp2;



	//	rcTemp.SetRect(5,12,205,rc.bottom);
	//mPItemList.MoveWindow(rcTemp);






	pWnd=GetDlgItem(IDC_ITEMLABEL);
	if(pWnd!=NULL)
	{

		pWnd->GetWindowRect(rcTemp2);
		this->ScreenToClient(rcTemp2);

	}




	/*pWnd=GetDlgItem(IDC_TIP2);
	if(pWnd!=NULL)
	{

	pWnd->GetWindowRect(rcTemp);
	this->ScreenToClient(rcTemp);
	int W=rcTemp.Width();
	rcTemp.right = rc.right-25;
	rcTemp.left=rcTemp.right-W;
	pWnd->MoveWindow(rcTemp);
	}*/





	pWnd=GetDlgItem(IDC_ITEMNAME);
	if(pWnd!=NULL)
	{

		pWnd->GetWindowRect(rcTemp);
		this->ScreenToClient(rcTemp);
		rcTemp.right = rcWaveList.right;
		rcTemp.left=rcTemp2.right+5;
		pWnd->MoveWindow(rcTemp);
		InvalidateIfVisible(pWnd);
	}





}





//-----------------------------

BOOL CPerformanceBox::_NumberIsPN(int Number) //素数
{
	BOOL Ret;

	int i ;
	for( i=2;i<=Number-1;i++)
		if(Number%i==0)
		{  
			Ret = FALSE;
			break;
		}
		if(i>Number/2) Ret =TRUE;

		return Ret;
}

void CPerformanceBox::_GetDiskOtherStaticInfo()
{ 


	// 以放入线程中


	CString StrPagingfilePath;


	CString strPath=_T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Memory Management");//注册表子键路径  
	CRegKey regkey;//定义注册表类对象  
	LONG lResult;//LONG型变量－反应结果  
	lResult=regkey.Open(HKEY_LOCAL_MACHINE,LPCTSTR(strPath),KEY_QUERY_VALUE ); //打开注册表键   注意 要用 读 的权限 KEY_ALL_ACCESS 在某些用户会导致错误
	if (lResult==ERROR_SUCCESS)  
	{  
		WCHAR StrData[90] = {0};  
		DWORD dwSize=90;   
		//获取ProcessorNameString字段值  
		if (ERROR_SUCCESS == regkey.QueryMultiStringValue(_T("ExistingPageFiles"),StrData,&dwSize))  
		{  
			StrPagingfilePath = StrData;  
		}  

	}  


	regkey.Close();//关闭注册表  

	//MSB_S(StrPagingfilePath);

	WCHAR CharPagingfile = StrPagingfilePath.GetAt(StrPagingfilePath.Find(L':')-1);

	PerferListData * pData = NULL ;


	CString StrPdh;

	int nCount = mPItemList.GetItemCount();
	for(int i=CurrentDiskCount+2;i<nCount;i++)
	{	


		pData = (PerferListData*)mPItemList.GetItemData(i);

		if(pData==NULL) continue ;

		if(pData->Type != PM_DISK)
		{
			break ;

		}


		//-------------------- 系统盘相关信息 -------------------

		CString StrSysPath;
		::GetSystemDirectory(StrSysPath.GetBuffer(MAX_PATH),MAX_PATH);
		StrSysPath.ReleaseBuffer();

		CString StrIsSysDisk=L"No";
		CString StrIsPagingFile = L"No";
		if(pData->StrOther0.Find(StrSysPath.GetAt(0))!=-1) StrIsSysDisk=L"Yes";
		if(pData->StrOther0.Find(CharPagingfile)!=-1) StrIsPagingFile=L"Yes";

		//theApp.m_pMainWnd->SetWindowTextW(pData->StrOther0);
		pData->pInfoBox->Info[6].StrInfo = pData->pInfoBox->Info[6].StrInfo+L"\n"+StrIsSysDisk+L"\n"+StrIsPagingFile;
		pData->pInfoBox->Invalidate();

	}




}



void CPerformanceBox::UpdateInfoBox()
{


	if(!ShowThisPage) return ;


	INT nSel = 0;
	//pNMLV->iItem
	nSel = mPItemList.GetNextItem( -1, LVNI_SELECTED );

	if(nSel<0 ) return;

	PerferListData * pPData = (PerferListData *)mPItemList.GetItemData(nSel);


	if(pPData->Type == PM_CPU)
	{
		this->_UpdateCpuInfoBox();
	}
	if(pPData->Type == PM_MEMORY)
	{
		pPData->pOtherWnd->Invalidate(0);
		this->_UpdateMemoryInfoBox();
	}

	//if(pPData->Type == PM_ETHERNET) 特殊 不在这里执行_UpdateNetworkInfoBox(nSel,pPData );

	//if(pPData->Type == PM_DISK)特殊 不在这里执行  this->_UpdateDiskInfoBox( );



	pPData->pInfoBox->Invalidate();


}



void CPerformanceBox::_UpdateMemoryInfoBox(BOOL UpdateAll )
{


	if(!ShowThisPage) return ;

	double  Num,Num2;

	CString StrInUse;

	Num = (double)(theApp.PerformanceInfo.InUsePhysMem/1024/1024);
	StrInUse.Format(L"%0.1f MB",Num );

	if(Num>=1024)
	{
		Num= Num/1024;
		StrInUse.Format(L"%0.1f GB",Num );
	}
	//=-------------

	CString StrAvialable;

	//Num =(double)(MyPMInfo.PhysicalAvailable*MyPMInfo.PageSize/1024/1024);
	Num = (double)(( MyPMInfo.PhysicalAvailable*MyPMInfo.PageSize)/1024/1024);
	StrAvialable.Format(L"%0.1f MB",Num );
	if(Num>=1024)
	{
		Num= Num/1024;
		StrAvialable.Format(L"%0.1f GB",Num );
	}

	//---------------Committed--------------

	CString StrCommitCurrent,StrCommit;

	double CommitCurrent = (double)MyPMInfo.CommitPeak*MyPMInfo.PageSize;
	double CommitTotal = (double)MyPMInfo.CommitLimit*MyPMInfo.PageSize;




	Num = (double)(CommitCurrent/1024/1024);
	StrCommitCurrent.Format(L"%0.1f",Num );
	Num2 = (double)(CommitTotal/1024/1024);
	StrCommit.Format(L"%0.1f MB",Num2 );
	if(Num2>=1024)
	{
		Num= Num/1024;
		StrCommitCurrent.Format(L"%0.1f",Num );
		Num2= Num2/1024;
		StrCommit.Format(L"%0.1f GB",Num2 );
	}

	//------------------ Paged Pool ------------------

	CString StrPagedPool;
	//Num = PM.PdhGetInfo(L"\\Memory\\Pool Paged Bytes");
	Num =(double)(MyPMInfo.KernelPaged*MyPMInfo.PageSize);
	Num= Num/1024/1024;
	StrPagedPool.Format(L"%0.1f MB",Num );
	if(Num>=1024)
	{
		Num= Num/1024;
		StrPagedPool.Format(L"%0.1f GB",Num );
	}
	//------------------ Non-Paged Pool ------------------


	CString StrNonPagedPool;
	//Num = PM.PdhGetInfo(L"\\Memory\\Pool Nonpaged Bytes");
	Num =(double)(MyPMInfo.KernelNonpaged*MyPMInfo.PageSize);

	Num= Num/1024/1024;
	StrNonPagedPool.Format(L"%0.1f MB",Num );
	if(Num>=1024)
	{
		Num= Num/1024;
		StrNonPagedPool.Format(L"%0.1f GB",Num );
	}





	//-----------StrSystemCache ------------



	CString StrSystemCache;
	//Num =(double)(MyPMInfo.SystemCache*MyPMInfo.PageSize);
	Num = theApp.PerformanceInfo.Mem_Modified+theApp.PerformanceInfo.Mem_Standby;
	Num= Num/1024/1024;
	StrSystemCache.Format(L"%0.1f MB",Num );
	if(Num>=1024)
	{
		Num= Num/1024;
		StrSystemCache.Format(L"%0.1f GB",Num );
	}











	/*CString TTT;
	TTT.Format(L"%d",MemStatus.ullTotalPhys/1024/1024/1024);
	theApp.GetMainWnd()->SetWindowTextW(TTT);*/



	pInfoBoxMemory->Info[0].StrInfo  = StrInUse ;
	pInfoBoxMemory->Info[1].StrInfo  = StrAvialable ;
	pInfoBoxMemory->Info[2].StrInfo  = StrCommitCurrent+L"/"+StrCommit;  
	pInfoBoxMemory->Info[3].StrInfo  = StrSystemCache ;  
	pInfoBoxMemory->Info[4].StrInfo  = StrPagedPool ; 
	pInfoBoxMemory->Info[5].StrInfo  = StrNonPagedPool ; 


	if(UpdateAll)
	{

		CSMBIOS  MySMBios;

		UINT  Speed;
		int  nSlotUsed;
		int  nSlot;
		WCHAR   StrFormFactor[100];


		theApp.PerformanceInfo.StrMemoryType = MySMBios.GetMemoryInfo(&nSlotUsed,&nSlot,&Speed,StrFormFactor);



		if(theApp.PerformanceInfo.TotalPhysMem ==0)
		{
			MEMORYSTATUSEX  MemStatus;
			MemStatus.dwLength = sizeof(MemStatus);
			BOOL Ret = GlobalMemoryStatusEx(&MemStatus);
			if(Ret)
			{
				theApp.PerformanceInfo.TotalPhysMem = MemStatus.ullTotalPhys;
			}
		}

		//StringCchCat(StrFormFactor,100,L"\n");

		double MemHWReserved = ((double)theApp.PerformanceInfo.InstalledMemKB*1024-theApp.PerformanceInfo.TotalPhysMem)/1024/1024; //MB



		pInfoBoxMemory->Info[6].StrInfo.Format(L"%d MHz \n%d of %d \n%s \n%0.0f MB",Speed,nSlotUsed,nSlot,StrFormFactor,MemHWReserved);  


       



	}



}

void CPerformanceBox::OnContextMenu(CWnd* pWnd, CPoint point)
{
	
	


	// TODO: Add your message handler code here
	CMenu PopMenu;
	CMenu *pMenu;

	PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_BASE ) );
	pMenu = PopMenu.GetSubMenu(1);  //1是 这个 对应的 菜单 和标签顺序对应


 

	 



	//注意:不要通过选中项来确定当前显示那个设备性能 因为可能根本没有选中项！！！

	int n= mPItemList.GetItemCount();


	PerferListData *pData = NULL ;
	for(int i=0;i<n;i++)
	{
		pData =(PerferListData *) mPItemList.GetItemData(i);
		if(pData==NULL) continue;
		//决定获取那个类型pData 这将影响菜单显示
		if(pData->pInfoBox->IsWindowVisible())
		{
			break;
		}


	}
	

	if(pData==NULL) return;


	if(pData->Type != PM_CPU) //去掉不该在此显示的项
	{
		 
		pMenu->DeleteMenu(0,MF_BYPOSITION);
		pMenu->DeleteMenu(0,MF_BYPOSITION);
		pMenu->DeleteMenu(0,MF_BYPOSITION);

	}

	if(pData->Type != PM_ETHERNET)
	{
		 
		pMenu->DeleteMenu(ID_PERFORMANCE_VIEWNETWORKDETAILS,MF_BYCOMMAND);
	}

	CRect rcBox;

	pData->pWaveBox->GetWindowRect(rcBox);

	

	CPoint CurPos ;
	GetCursorPos(&CurPos);

	
	int L1SubMenuID = 1; //View 子菜单位置

	if(pData->Type == PM_CPU)
	{
		L1SubMenuID = 4; //显示cpu时候位置不同于其他
		if( ! rcBox.PtInRect(CurPos) )
		{
			L1SubMenuID = 3; //显示cpu时候位置不同于其他
			pMenu->DeleteMenu(ID_PERFORMANCE_SHOWKERNELTIMES,MF_BYCOMMAND);
		}
	}





	//--------------------------------
 

	
	int iDiskMenu =0;
	UINT BaseID = ID_DISK_NONE;
	pData = NULL ;

 
	 
	CMenu *pSubMenu1 = pMenu->GetSubMenu(L1SubMenuID);
	CMenu *pSubMenu2 =NULL;
	if(pSubMenu1 )
	{
		pSubMenu2=pSubMenu1->GetSubMenu(2);
	}
 



	if(pSubMenu2)
	{
		for(int i=0;i<n;i++)
		{
			pData =(PerferListData *) mPItemList.GetItemData(i);
			if(pData==NULL) continue;	
			if(pData->Type == PM_DISK)
			{
				CString StrID;
				StrID.Format(L"Disk %d %s",pData->ID,L"");
				pSubMenu2->AppendMenu(MF_STRING|MF_BYCOMMAND,++BaseID ,StrID);
			}
		}

		pSubMenu2->DeleteMenu(0,MF_BYPOSITION);

	}


	//--------------------------------



	
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,CurPos.x,CurPos.y,this);
}

int CPerformanceBox::UpdateAllPMInfo(void)
{


	int n = mPItemList.GetItemCount();
	
	int iSel = mPItemList.GetNextItem( -1, LVNI_SELECTED );

	double *PerformanceData  = new double[n]; 
	double *PerformanceDataA  = new double[n]; 
	double *PerformanceDataB  = new double[n]; 

	//ULONG64 TempData;


	CString StrTemp;


	if(ShowThisPage)
	{
		mPItemList.SetRedraw(0);
	}



	// 波形图 内部绘制采用 0～1  浮点数数控制显示点的Y坐标（比如50% =  0.5）  而不是百分数  所以得到百分数的要转化 这样避免重复乘除 100 



	//------------------------------------CPU   -----------------------------



	//------------------------------------Memory -------------------------


	double MemUsagePercent;

	ZeroMemory(&MyPMInfo, sizeof(PERFORMANCE_INFORMATION));
	MyPMInfo.cb = sizeof(MyPMInfo);
	GetPerformanceInfo(&MyPMInfo,sizeof(PERFORMANCE_INFORMATION));



	
	//为了绘制 memory composition
	theApp.PerformanceInfo.Mem_Modified =  PM.PdhGetInfo(L"\\Memory\\Modified Page List Bytes");	
	//此方法获取 Mem_Standby更可靠 
	if(iSel == 1)
	theApp.PerformanceInfo.Mem_Standby = MyPMInfo.PhysicalAvailable*MyPMInfo.PageSize - PM.PdhGetInfo(L"\\Memory\\Free & Zero Page List Bytes");


	//------------------------------------------------------------------------------------------------------------------------------------    

	//MEMORYSTATUSEX  MemStatus;
	//MemStatus.dwLength = sizeof(MemStatus);
	//BOOL Ret;
	//BOOL Ret = GlobalMemoryStatusEx(&MemStatus);
	//theApp.PerformanceInfo.PagedVirtual = MemStatus.ullTotalVirtual;
	//theApp.PerformanceInfo.CommitLimit =   MemStatus.ullTotalPageFile;
	//theApp.PerformanceInfo.CommitCurrent =MemStatus.ullTotalPageFile-MemStatus.ullAvailPageFile;

	//------------------------------------------------------------------------------------------------------------------------------------

	// 预先定义了  PERFORMANCE_INFORMATION MyPMInfo;


 
	theApp.PerformanceInfo.InUsePhysMem = (DWORDLONG)( (MyPMInfo.PhysicalTotal-MyPMInfo.PhysicalAvailable)*MyPMInfo.PageSize-theApp.PerformanceInfo.Mem_Modified-(theApp.PerformanceInfo.InstalledMemKB*1024-theApp.PerformanceInfo.TotalPhysMem));
 


	theApp.PerformanceInfo.MemoryUsage =  ((double)(theApp.PerformanceInfo.InUsePhysMem))/((double) (theApp.PerformanceInfo.TotalPhysMem)) ;

	if(theApp.PerformanceInfo.MemoryUsage>1.0  ) theApp.PerformanceInfo.MemoryUsage = 1.0;
	
	MemUsagePercent = theApp.PerformanceInfo.MemoryUsage *100;//// 转为百分比

	if(ShowThisPage)
	{
		StrTemp.Format(L"%.1f/%.1f GB (%.2f%%)",((double)(theApp.PerformanceInfo.InUsePhysMem ))/1024/1024/1024,(double) (theApp.PerformanceInfo.TotalPhysMem)/1024/1024/1024,MemUsagePercent  );
		mPItemList.SetItemText(1,1,StrTemp);
		MySetItem(1,StrTemp);
	}


	PerformanceData[1]=theApp.PerformanceInfo.MemoryUsage ;
	PerformanceDataA[1] = PerformanceDataB[1] =PerformanceData[1];
	theApp.PerformanceInfo.MemoryUsage = MemUsagePercent;// 转为百分比



	//------------------------------------Disk    -------------------------



	double SumDiskUsage=0; int nDiskCount=0;


	PerferListData *pPData = NULL;


	ULONG64 RWTime;
	ULONG64 TotalTime;


	if(! FlagStartDiskMon )  goto SKIPDISK;
	for(int nItem=2;nItem<n;nItem++)
	{
		
		pPData = (PerferListData *)mPItemList.GetItemData(nItem);
		if(pPData==NULL) continue ;
		if(pPData->Type == PM_ETHERNET   ) break ; 
 
		DISK_PERFORMANCE DiskPerformance;
		DiskPerformance=PM.GetDiskPerformance(pPData->ID);


		nDiskCount++;

		//Read	

		if(pPData->DataA<=0)
		{
			pPData->DataA = DiskPerformance.BytesRead.QuadPart;
		}		

		PerformanceDataA[nItem] = (double)(DiskPerformance.BytesRead.QuadPart- pPData->DataA)/theApp.AppSettings.TimerStep ;

		if(PerformanceDataA[nItem]<0.001)PerformanceDataA[nItem]=0.0;

		pPData->DataA = DiskPerformance.BytesRead.QuadPart;

		//Write		

		if(pPData->DataB<=0)
		{
			pPData->DataB = DiskPerformance.BytesWritten.QuadPart;
		}		
		PerformanceDataB[nItem] = (double)(DiskPerformance.BytesWritten.QuadPart- pPData->DataB)/theApp.AppSettings.TimerStep ;
		if(PerformanceDataB[nItem]<0.001)PerformanceDataB[nItem]=0.0;
		pPData->DataB = DiskPerformance.BytesWritten.QuadPart;




		//------------- 另行获取 特别需要的数据---------- 因为disk 要显示两个独立波形图


		RWTime = DiskPerformance.ReadTime.QuadPart+DiskPerformance.WriteTime.QuadPart;  //RWTime此次读写时间
	    TotalTime = DiskPerformance.IdleTime.QuadPart+RWTime;
		
		//此时  pPData->DataC保存了上次读写时间 pPData->DataD保存了上次总时间
		PerformanceData[nItem] =  (double)(RWTime-pPData->DataC)/( TotalTime - pPData->DataD); //

		pPData->DataD = TotalTime;// 
		//--------------------------------------------------------------------------


		//平均响应时间
		double AvgResponseTime = 0;

		if(ShowThisPage||theApp.UpTimeSec<10)
		{
			 

			if(pPData->DataC <= 0)
			{
				pPData->DataC = RWTime;
			}			

			AvgResponseTime = 	(double)( RWTime-pPData->DataC)/2/10000/theApp.AppSettings.TimerStep; //注意必须除以 theApp.AppSettings.TimerStep 才是正确结果！！！			

			

		}
		
		pPData->DataC =RWTime;//无论此页是否显示都有更新 否则影响 磁盘使用率获取


		if(pPData->pInfoBox->IsWindowVisible())//在数据被改变之前调用
		{
			_UpdateDiskInfoBox(pPData,PerformanceDataA[nItem],PerformanceDataB[nItem],PerformanceData[nItem]*100,AvgResponseTime);
		}


		//---------------------------------------------------------------------------

		if( ((int)theApp.UpTimeSec) % 6 == 0 )
		{
			_TryToChangeDiskMaxVar(PerformanceDataA[nItem],PerformanceDataB[nItem],pPData);
		}


		PerformanceDataA[nItem] = PerformanceDataA[nItem]/(pPData->MaxVar);
		PerformanceDataB[nItem] = PerformanceDataB[nItem]/(pPData->MaxVar);

		if(PerformanceDataA[nItem]<0.001 || PerformanceDataA[nItem]>1)PerformanceDataA[nItem]=0;
		if(PerformanceDataB[nItem]<0.001 || PerformanceDataB[nItem]>1)PerformanceDataB[nItem]=0;

		 



		if(ShowThisPage)		{
			
			StrTemp.Format(L"%0.2f%%",PerformanceData[nItem]*100); //此时为百分之几
			//mPItemList.SetItemText(nItem,1,StrTemp);
			MySetItem(nItem,StrTemp);
		}	

		SumDiskUsage = SumDiskUsage+PerformanceData[nItem];
		

	}

	
	 
	 theApp.PerformanceInfo.TotalDiskUsage = SumDiskUsage/nDiskCount*100;
 
	 

SKIPDISK:
 



	//---------------------------------		Network    ---------------------------------


	double SumNetUsage=0; int nNetCount=0;



	MIB_IF_TABLE2   *pIfTable = NULL;
	ULONG          dwSize   = 0;
	DWORD          dwRet;
	dwRet = GetIfTable2( &pIfTable );

	if ( dwRet == ERROR_NOT_ENOUGH_MEMORY )
	{
		 ;
	}
	else if(dwRet == STATUS_SUCCESS )
	{
		CString Str,s1,s2,s3,StrTemp;
		for ( ULONG  i=0; i<pIfTable->NumEntries; i++ )
		{

			if((pIfTable->Table[i].Type == IF_TYPE_IEEE80211 )|| (pIfTable->Table[i].Type ==IF_TYPE_ETHERNET_CSMACD) )
			{


				ULONG Index =pIfTable->Table[i].InterfaceIndex;

				map<int, int>::iterator Iter= NetAdapterList.find((int)Index); // <网卡ID,列表项目ID>
				int nItemID;

				
				if(Iter != NetAdapterList.end())//存在
				{
					nNetCount++;

					nItemID = Iter->second;
 
					 
					pPData = (PerferListData*)mPItemList.GetItemData(nItemID);	

					if(pPData!=NULL) 
					{							
						 

						// InOctets  OutOctets单位为字节
						//------------接收
						if(pPData->DataA==0)    
						{
							PerformanceDataA[nItemID]=0;
						}
						else
						{
							PerformanceDataA[nItemID]= (double)(pIfTable->Table[i].InOctets - pPData->DataA)/(double)theApp.AppSettings.TimerStep;
						}

						//------------发送
						if(pPData->DataB==0) 	
						{
							PerformanceDataB[nItemID] =0;
						}
						else
						{
							PerformanceDataB[nItemID]=(double)(pIfTable->Table[i].OutOctets - pPData->DataB)/(double)theApp.AppSettings.TimerStep;
						}


						double ThisUsaeg = (PerformanceDataA[nItemID]+PerformanceDataB[nItemID])/(pIfTable->Table[i].TransmitLinkSpeed/8);

						//防止出现负值
						if(ThisUsaeg>0.001)
						{
							SumNetUsage = SumNetUsage+ThisUsaeg;
						}



						//-----------------------------------------

						if(ShowThisPage)
						{

							StrTemp.Format(L"S: %.0f Kbps R: %.0f Kbps", (PerformanceDataB[nItemID]/1024*8), (PerformanceDataA[nItemID]/1024*8));
							//mPItemList.SetItemText(nItemID,1,StrTemp);
							MySetItem(nItemID,StrTemp);
						}

						if(pPData->pInfoBox->IsWindowVisible())
						{
							_UpdateNetworkInfoBox(pPData,PerformanceDataA[nItemID],PerformanceDataB[nItemID]);
						}

						if( ((int)theApp.UpTimeSec) % 6 == 0 )	
						{
							_TryToChangeNetworkMaxVar(PerformanceDataA[nItemID],PerformanceDataB[nItemID],pPData);
						}

						PerformanceDataA[nItemID] =PerformanceDataA[nItemID]/(pPData->MaxVar);
						PerformanceDataB[nItemID] =PerformanceDataB[nItemID]/(pPData->MaxVar);



						pPData->DataA =pIfTable->Table[i].InOctets;
						pPData->DataB =pIfTable->Table[i].OutOctets;


					} //pPData!=NULL

				}//Iter != NetAdapterList.end()


			}	//Type			

		}//for


		FreeMibTable (pIfTable);
	}




	if(nNetCount>0)
	{
		theApp.PerformanceInfo.TotalNetUsage = SumNetUsage/nNetCount*100; //百分比
	}
	else
	{
		theApp.PerformanceInfo.TotalNetUsage = 0;
	}





	//----------------------------------更新波形图-----------------------------







	CWaveBox *pBox = NULL;
	CWaveBox *pBox2 = NULL;




	//---------------多cpu显示状态 获取数据及更新 视图---------------------

	double CpuUsage,KernelUsage;



	_GetLogicalProcessorUsage(pCpuBox, &CpuUsage,&KernelUsage); //具体执行函数

	pPData = (PerferListData *)mPItemList.GetItemData(0);
	if(pPData!=NULL) 
	{
		pBox = pPData->pWaveBox;

		if(pBox->IsWindowVisible()) pBox->Invalidate();

	}




	//--------------------


	if(theApp.AppSettings.ProcessorDisplayMode == 2)
	{
		for(int i =0;i<theApp.PerformanceInfo.nLogicalProcessor;i++)
		{
			pHotBox->ArrayPerLogicalCpuUsage[i] = pCpuBox->Num[i][60];
		} 
		pHotBox->Invalidate(0);

	}


	//----------------------整体cpu----------------------

	memmove(&pTotalCpuBox->Num[0][0],&pTotalCpuBox->Num[0][1],ArraySize);
	memmove(&pTotalCpuBox->Num2[0][0],&pTotalCpuBox->Num2[0][1],ArraySize);

	/*for(int i=0;i<61-1;i++)
	{
	pTotalCpuBox->Num[0][i] = pTotalCpuBox->Num[0][i+1];
	pTotalCpuBox->Num2[0][i] = pTotalCpuBox->Num2[0][i+1];
	}*/


	theApp.PerformanceInfo.CpuUsage = CpuUsage*100;

	PerformanceDataA[0] = PerformanceData[0]= CpuUsage;
	PerformanceDataB[0] =KernelUsage;

	pTotalCpuBox->Num[0][60] = (float)PerformanceDataA[0]; //
	pTotalCpuBox->Num2[0][60] =(float)PerformanceDataB[0];

	
	//CPU当前速度 
	//这个执行在更新右侧各种信息框之前  所以 _UpdateCpuInfoBox中不用再吃获取 CurrentSpeed

	if(ShowThisPage)
	{
	
		pInfoBoxCpu->Info[1].StrInfo.Format(L"%0.2f GHz",_GetSurrentCpuSpeed());


		StrTemp.Format(L"%.2f%%  %s",theApp.PerformanceInfo.CpuUsage,pInfoBoxCpu->Info[1].StrInfo);
		MySetItem(0,StrTemp);		
	//	mPItemList.SetItemText(0,1,StrTemp);

	}







	//---------------  其他 的 显示更新---------------------




	for(int nItem=1;nItem<n;nItem++)
	{
		pPData = (PerferListData *)mPItemList.GetItemData(nItem);
		if(pPData==NULL) continue ;
		pBox = pPData->pWaveBox;
		pBox2= (CWaveBox *)pPData->pOtherWnd;	
		if(pBox==NULL)  continue ;

		//----------------数据前移-------------------



		memmove(&pBox->Num[0][0],&pBox->Num[0][1],ArraySize);
		memmove(&pBox->Num2[0][0],&pBox->Num2[0][1],ArraySize);
		/*
		for(int i=0;i<61-1;i++)
		{
		pBox->Num[0][i] = pBox->Num[0][i+1];
		pBox->Num2[0][i] = pBox->Num2[0][i+1];

		}*/

		if(pPData->Type == PM_DISK)   //包含 第二个box
		{
			if(pBox2!=NULL)
			{/*
			 for(int i=0;i<61-1;i++)
				{
				pBox2->Num[0][i] = pBox2->Num[0][i+1];
				pBox2->Num2[0][i] = pBox2->Num2[0][i+1];
				}*/

				memmove(&pBox2->Num[0][0], &pBox2->Num[0][1],ArraySize);
				memmove(&pBox2->Num2[0][0],&pBox2->Num2[0][1],ArraySize);


				pBox2->Num[0][60] =(float) PerformanceDataA[nItem];			
				pBox2->Num2[0][60] = (float)PerformanceDataB[nItem];

				if(pBox2->IsWindowVisible())pBox2->Invalidate(); //必须判断 IsWindowVisible 否则波形画乱

			}

			pBox->Num[0][60] = (float)PerformanceData[nItem];

		}
		else  if(pPData->Type == PM_MEMORY)
		{
			pBox->Num[0][60] = (float)PerformanceData[nItem];			
		}
		else
		{
			pBox->Num[0][60] = (float)PerformanceDataA[nItem];
			pBox->Num2[0][60] = (float)PerformanceDataB[nItem];
		}




		if(pBox->IsWindowVisible()) pBox->Invalidate();




	}



	if(ShowThisPage)
	{
		UpdateInfoBox();
		mPItemList.SetRedraw(1);
		mPItemList.InvalidateRect(NULL,0);
	}





	delete [] PerformanceData ;PerformanceData=NULL;
	delete [] PerformanceDataA ;PerformanceDataA=NULL;
	delete [] PerformanceDataB ;PerformanceDataB=NULL;





	return 0;
}

void CPerformanceBox::OnPop_ChangeGraphToOverallutilization()
{
	// TODO: Add your command handler code here

	if(theApp.AppSettings.ProcessorDisplayMode == 0) return;


	theApp.AppSettings.ProcessorDisplayMode = 0;

	PerferListData *pData =(PerferListData *) mPItemList.GetItemData(0);
	if(pData==NULL) return;
	if(pData->pWaveBox==NULL)return;





	pData->pWaveBox = pTotalCpuBox;



	pTotalCpuBox->ShowWindow(SW_SHOW);
	pCpuBox->ShowWindow(SW_HIDE);
	pHotBox->ShowWindow(SW_HIDE);


	pData->pWaveBox->Invalidate();


	CWnd *pWnd=GetDlgItem(IDC_TIP3);
	if(pWnd!=NULL)	{  pWnd->ShowWindow(SW_SHOW); }
	pWnd=GetDlgItem(IDC_TIP4);
	if(pWnd!=NULL)	{  pWnd->ShowWindow(SW_SHOW); }


	pWnd=GetDlgItem(IDC_TIP1);
	if(pWnd!=NULL)	
	{ 
		CString StrTemp = STR_TIP1_CPU_TOTAL;
		mPItemList.SetItemText(0,3,StrTemp);
		pWnd->SetWindowTextW(StrTemp);
	}

	pWnd=GetDlgItem(IDC_TIP2);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }



	this->PlaceAllCtrl();









}

void CPerformanceBox::OnPop_ChangeGraphToLogicalprocessors()
{
	if(theApp.AppSettings.ProcessorDisplayMode == 1) return;



	theApp.AppSettings.ProcessorDisplayMode = 1;

	PerferListData *pData =(PerferListData *) mPItemList.GetItemData(0);
	if(pData==NULL) return;
	if(pData->pWaveBox==NULL)return;

	pData->pWaveBox = pCpuBox;


	pCpuBox->ShowWindow(SW_SHOW);
	pTotalCpuBox->ShowWindow(SW_HIDE);
	pHotBox->ShowWindow(SW_HIDE);


	pData->pWaveBox->Invalidate();



	CString StrTime;

	CWnd *pWnd=GetDlgItem(IDC_TIP3);
	if(pWnd!=NULL)	{  pWnd->ShowWindow(SW_HIDE); pWnd->GetWindowTextW(StrTime); }
	pWnd=GetDlgItem(IDC_TIP4);
	if(pWnd!=NULL)	{  pWnd->ShowWindow(SW_HIDE); }


	pWnd=GetDlgItem(IDC_TIP1);
	if(pWnd!=NULL)	
	{ 
		CString StrTemp;    

		StrTemp.Format(STR_TIP1_CPU_LOGICAL,StrTime);
		
		mPItemList.SetItemText(0,3,StrTemp);
		pWnd->SetWindowTextW(StrTemp);
	}


	pWnd=GetDlgItem(IDC_TIP2);if(pWnd!=NULL){ pWnd->ShowWindow(SW_SHOW); }



	this->PlaceAllCtrl();



}

void CPerformanceBox::_UpdateNetworkInfoBox(PerferListData* pData,double Received,double Sent,BOOL UpdateAll)
{

	if(!ShowThisPage) return ;


	double SentSpeed = Sent/1024*8;
	if(SentSpeed>1024)
	{
		pData->pInfoBox->Info[0].StrInfo.Format(L"%.2f Mbps",(SentSpeed/1024));
	}
	else
	{
		pData->pInfoBox->Info[0].StrInfo.Format(L"%.2f Kbps",(SentSpeed));
	}


	double ReceivedSpeed = Received/1024*8;

	if(ReceivedSpeed>1024)
	{
		pData->pInfoBox->Info[2].StrInfo.Format(L"%.2f Mbps",(ReceivedSpeed/1024));
	}
	else
	{
		pData->pInfoBox->Info[2].StrInfo.Format(L"%.2f Kbps",(ReceivedSpeed));
	}


	






	//---------get IP v4  -------
	/*CString StrKey = pData->StrOther0;
	HKEY hKey  ;
	DWORD dwIndex = 0;
	DWORD dwBufSize = 256;
	DWORD dwDataType;
	WCHAR szData[256];

	CString StrType ;
	if(pData->Type == PM_ETHERNET){StrType = L"Ethernet" ;}


	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,StrKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
	dwBufSize = 256;
	if(RegQueryValueEx(hKey, L"IPAddress", 0, &dwDataType, (BYTE*)szData, &dwBufSize) == ERROR_SUCCESS)
	{

	pData->pInfoBox->Info[6].StrInfo.Format(L"%s\n \n%s",StrType,szData);
	}
	}*/

	//--------- IPV6--------

	//PIP_ADAPTER_ADDRESSES pAddresses = NULL;

	//GetAdaptersAddresses(AF_UNSPEC,GAA_FLAG_INCLUDE_PREFIX,NULL ,)





}

void CPerformanceBox::_UpdateDiskInfoBox(PerferListData* pData,double Read,double Write,double ActiveTime,double Other,BOOL UpdateAll)
{


	if(!ShowThisPage) return ;


	pData->pInfoBox->Info[0].StrInfo.Format(L"%.0f%%",ActiveTime );


	double Num = Read;

	Num = Num/1024;

	pData->pInfoBox->Info[2].StrInfo.Format(L"%0.1f KB/s",Num);

	if(Num>1024)
	{
		Num = Num/1024;
		pData->pInfoBox->Info[2].StrInfo.Format(L"%0.1f MB/s",Num);
	}

	//--------------------------


	Num = Write;

	Num = Num/1024;

	pData->pInfoBox->Info[3].StrInfo.Format(L"%0.1f KB/s",Num);

	if(Num>1024)
	{
		Num = Num/1024;
		pData->pInfoBox->Info[3].StrInfo.Format(L"%0.1f MB/s",Num);
	}




	//-------------------------

	//CString StrPdh;

	//StrPdh.Format(L"\\PhysicalDisk(%d%s)\\Avg. Disk Queue Length",pData->ID,pData->StrOther0);

	//theApp.m_pMainWnd->SetWindowTextW(StrPdh);

	//double AvgRespondseTime = PM.PdhGetInfo(StrPdh);

	pData->pInfoBox->Info[1].StrInfo.Format(L"%.1f ms", Other);







}




BOOL CPerformanceBox::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	mToolTip.RelayEvent(pMsg);



	int n =mPItemList.GetItemCount();
	int DiskCount = 0;
	PerferListData *pPData =NULL;	
	for(int i=2;i<n;i++)
	{		
		if(pPData == NULL) continue;
		pPData=(PerferListData*)mPItemList.GetItemData(i);
		if(pPData->Type==PM_DISK) DiskCount++;
	}




	if(pMsg->message == WM_COMMAND)
	{
		UINT CmdMenuID = (UINT) pMsg->wParam;
		if( (CmdMenuID > ID_DISK_NONE) &&  (CmdMenuID <= CmdMenuID+DiskCount) )
		{

			 
			_FakeSelPItem(CmdMenuID-ID_DISK_NONE+1);
		}

	}

	return CFormView::PreTranslateMessage(pMsg);
}

void CPerformanceBox::_GetLogicalProcessorUsage(CWaveBox *pBox,double * pTotalUsage,double * pTotalKernelUsage )
{



	memset(spi,0,sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)*nLogicalProcessor);

	unsigned long bytesreturned;
	MyNtQuerySystemInformation(SystemProcessorPerformanceInformation,spi, (sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)*nLogicalProcessor),&bytesreturned);


	double TotalCpuUsage = 0;
	double TotalKernelCpuUsage = 0;


	CString Str0,Str1,StrTemp;

	for(int cpuloopcount = 0; cpuloopcount < nLogicalProcessor; cpuloopcount++) 
	{
		//-----------数据前移---------------
		/*for(int i=0;i<61-1;i++)
		{
		pBox->Num[cpuloopcount][i] = pBox->Num[cpuloopcount][i+1];
		pBox->Num2[cpuloopcount][i] = pBox->Num2[cpuloopcount][i+1];
		}*/

		memmove(&pBox->Num[cpuloopcount][0],&pBox->Num[cpuloopcount][1],ArraySize);
		memmove(&pBox->Num2[cpuloopcount][0],&pBox->Num2[cpuloopcount][1],ArraySize);




		LONGLONG TimeUserAndKernel=  (spi[cpuloopcount].KernelTime.QuadPart + spi[cpuloopcount].UserTime.QuadPart) - (spi_old[cpuloopcount].KernelTime.QuadPart + spi_old[cpuloopcount].UserTime.QuadPart);

		LONGLONG TimeIdle = spi[cpuloopcount].IdleTime.QuadPart - spi_old[cpuloopcount].IdleTime.QuadPart ;

		//------------------------
		double CpuUsage =0;
		if(TimeUserAndKernel>0)
		{
			CpuUsage = 100 - (double)( TimeIdle * 100  / TimeUserAndKernel);
		}


		LONGLONG TimeUser = spi[cpuloopcount].UserTime.QuadPart-spi_old[cpuloopcount].UserTime.QuadPart;

		LONGLONG TimeKernel =  spi[cpuloopcount].KernelTime.QuadPart  - spi_old[cpuloopcount].KernelTime.QuadPart ;


		//  double CpuUsag_Kernel = (double)( TimeKernel*100/TimeUserAndKernel);	  

		double CpuUsag_Kernel = 0;
		if(TimeUserAndKernel>0)
		{
			CpuUsag_Kernel = 100  - (double)(  (TimeIdle+TimeUser)* 100  /TimeUserAndKernel);
		}



		pBox->Num[cpuloopcount][60] = ( float)  (CpuUsage/100 ); //
		pBox->Num2[cpuloopcount][60] =( float)  (CpuUsag_Kernel/100  );

		spi_old[cpuloopcount] = spi[cpuloopcount];

		TotalCpuUsage = TotalCpuUsage + pBox->Num[cpuloopcount][60];
		TotalKernelCpuUsage = TotalKernelCpuUsage + pBox->Num2[cpuloopcount][60];
	}

	TotalCpuUsage = TotalCpuUsage/nLogicalProcessor;
	TotalKernelCpuUsage = TotalKernelCpuUsage/nLogicalProcessor;

	* pTotalUsage = TotalCpuUsage;
	*pTotalKernelUsage = TotalKernelCpuUsage;



}

void CPerformanceBox::_DetermineRowCol(int nLogicalProcessor , int * pNRow, int * pNCol)//确定多cpu波形图分为几行几列显示
{
	int nRow =  (int)sqrt( (float) nLogicalProcessor );



	while(1)
	{
		if(	nLogicalProcessor%nRow == 0) //可以整除则用这个作为行数 否则减一 直到能整除
		{
			break;
		}
		else
		{
			nRow--;
		}

	}

	* pNRow = nRow;
	* pNCol = nLogicalProcessor/nRow;




}

void CPerformanceBox::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFormView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);


	switch(theApp.AppSettings.ProcessorDisplayMode)
	{
	case 0:
		pPopupMenu->CheckMenuRadioItem(ID_CHANGEGRAPHTO_OVERALLUTILIZATION,ID_CHANGEGRAPHTO_NUMANODES,ID_CHANGEGRAPHTO_OVERALLUTILIZATION,MF_BYCOMMAND);
		break;
	case 1:
		pPopupMenu->CheckMenuRadioItem(ID_CHANGEGRAPHTO_OVERALLUTILIZATION,ID_CHANGEGRAPHTO_NUMANODES,ID_CHANGEGRAPHTO_LOGICALPROCESSORS,MF_BYCOMMAND);
		break;
	case 2: 
		pPopupMenu->CheckMenuRadioItem(ID_CHANGEGRAPHTO_OVERALLUTILIZATION,ID_CHANGEGRAPHTO_NUMANODES,ID_CHANGEGRAPHTO_NUMANODES,MF_BYCOMMAND);
		break;
	}



	UINT Flag = (theApp.FlagSummaryView) ? MF_CHECKED:MF_UNCHECKED;
	pPopupMenu->CheckMenuItem(ID_PERFORMANCE_GRAPHSUMMARYVIEW,MF_BYCOMMAND|Flag);



	//-------------------------
	int iSel = mPItemList.GetNextItem( -1, LVNI_SELECTED );

	switch(iSel)
	{
	case 0:
		pPopupMenu->CheckMenuRadioItem(ID_VIEW_CPU,ID_VIEW_MEMORY,ID_VIEW_CPU,MF_BYCOMMAND);
		break;
	case 1:
		pPopupMenu->CheckMenuRadioItem(ID_VIEW_CPU,ID_VIEW_MEMORY,ID_VIEW_MEMORY,MF_BYCOMMAND);
		break;
	
	}
	



	if(pPopupMenu->GetMenuItemID(0) ==ID_VIEW_CPU)
	{
		

		


		int ItemCount =mPItemList.GetItemCount();
		PerferListData *pPData = NULL ;		
		int DiskCount = 0;
		int SelectedDiskID = -1;

	    int FirstDiskItemID = 2; //第一个磁盘项目在左侧项目列表中的ID

			 
		for(int i = FirstDiskItemID;  i<ItemCount ;i++ ) 
		{		
			pPData=(PerferListData*)mPItemList.GetItemData(i);
			if(pPData == NULL)continue;		
			if(pPData->Type == PM_DISK)
			{
				DiskCount++;
				if(i == iSel){ SelectedDiskID = iSel-FirstDiskItemID ;}
				
			}
		}

		Flag = (DiskCount>0)?MF_ENABLED:MF_GRAYED;  //根据是否有磁盘决定
		pPopupMenu->EnableMenuItem(2,MF_BYPOSITION|Flag  );  
		if(SelectedDiskID>=0)
		{
			CMenu *pSubMenu = pPopupMenu->GetSubMenu(2);
			if(pSubMenu)pSubMenu->CheckMenuRadioItem(0, DiskCount-1, SelectedDiskID, MF_BYPOSITION);
		}

		
	}




	//ASSERT(pPopupMenu != NULL);

	//CCmdUI state;
	//state.m_pMenu = pPopupMenu;
	//ASSERT(state.m_pOther == NULL);
	//ASSERT(state.m_pParentMenu == NULL);

	//HMENU hParentMenu;
	//if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
	//	state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	//else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	//{
	//	CWnd* pParent = this;

	//	if (pParent != NULL &&	(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
	//	{
	//		int nIndexMax = ::GetMenuItemCount(hParentMenu);
	//		for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
	//		{
	//			if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
	//			{

	//				state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
	//				break;
	//			}
	//		}
	//	}
	//}

	//state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	//for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;state.m_nIndex++)
	//{
	//	state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
	//	if (state.m_nID == 0)
	//		continue; // Menu separator or invalid cmd - ignore it.

	//	ASSERT(state.m_pOther == NULL);
	//	ASSERT(state.m_pMenu != NULL);
	//	if (state.m_nID == (UINT)-1)
	//	{
	//		// Possibly a popup menu, route to first item of that popup.
	//		state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
	//		if (state.m_pSubMenu == NULL ||	(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||state.m_nID == (UINT)-1)
	//		{
	//			continue;       // First item of popup can't be routed to.
	//		}
	//		state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
	//	}
	//	else
	//	{

	//		state.m_pSubMenu = NULL;
	//		state.DoUpdate(this, FALSE);
	//	}

	//	// Adjust for menu deletions and additions.
	//	UINT nCount = pPopupMenu->GetMenuItemCount();
	//	if (nCount < state.m_nIndexMax)
	//	{
	//		state.m_nIndex -= (state.m_nIndexMax - nCount);
	//		while (state.m_nIndex < nCount &&	pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
	//		{
	//			state.m_nIndex++;
	//		}
	//	}
	//	state.m_nIndexMax = nCount;
	//}



}




void CPerformanceBox::OnPerformanceShowkerneltimes()
{

	theApp.AppSettings.ShowKernelTime = !theApp.AppSettings.ShowKernelTime;

	pTotalCpuBox->DrawSecondWave = theApp.AppSettings.ShowKernelTime ;
	pCpuBox->DrawSecondWave = theApp.AppSettings.ShowKernelTime ;

	pTotalCpuBox->Invalidate();
	pCpuBox->Invalidate();


}

void CPerformanceBox::OnUpdatePerformanceShowkerneltimes(CCmdUI *pCmdUI)
{

	if(theApp.AppSettings.ShowKernelTime)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}





void CPerformanceBox::OnViewCpu()
{
	mPItemList.SetItemState(0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);	 
}

void CPerformanceBox::OnViewMemory()
{
	mPItemList.SetItemState(1,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);	 
}

double CPerformanceBox::_TryToChangeDiskMaxVar( double DataA,double DataB,PerferListData *pPData )
{


	double RetMaxVar;
	double Data = (DataA>DataB)?DataA:DataB;

	int nK,nM;


	CString Str ;

	nK = ((int)Data)/1024;  nM = ((int)Data)/1024/1024;

	RetMaxVar = 100*1024;

	if(nM == 0) //KB 级别
	{
		if(nK<200 )
		{
			RetMaxVar = 100*1024;
			Str =L"100 KB/s";
		}
		else if(nK<500 )
		{
			RetMaxVar = 500*1024;
			Str =L"500 KB/s";
		}
		else if(nK<1024)
		{
			RetMaxVar = 1024*1024;
			Str =L"1 MB/s";
		}

	}
	else
	{

		if(nM<5)
		{
			RetMaxVar = 5*1024*1024;//5MB
			Str =L"5 MB/s";
		}
		else if(nM<10) 
		{

			RetMaxVar =10*1024*1024 ; //10MB
			Str =L"10 MB/s";
		}
		else if(nM<50) 
		{

			RetMaxVar =50*1024*1024 ; //50
			Str =L"50 MB/s";
		}
		else if(nM<100) 
		{

			RetMaxVar =100*1024*1024 ; //100
			Str =L"100 MB/s";
		}
		else if(nM<250) 
		{

			RetMaxVar =250*1024*1024 ; //500
			Str =L"250 MB/s";
		}
		else if(nM<500) 
		{

			RetMaxVar =500*1024*1024 ; //500
			Str =L"500 MB/s";
		}
		else //if(nM<1000) 
		{

			RetMaxVar =1000*1024*1024 ; //1000
			Str =L"1000 MB/s";
		}

	}

	if(RetMaxVar!=pPData->MaxVar) //需要变更
	{
		double Per = RetMaxVar/pPData->MaxVar ;//变换比率
		CWaveBox *pBox2= (CWaveBox *)pPData->pOtherWnd;	

		for(int i=0;i<61;i++) //第61 个 用算
		{
			pBox2->Num[0][i] =(float)(pBox2->Num[0][i]/Per) ;//变换为新比例
			pBox2->Num2[0][i] =(float)(pBox2->Num2[0][i]/Per) ;//变换为新比例
		}

		pPData->StrOther1 = Str;
		pPData->MaxVar =	RetMaxVar;


		CWnd *pWnd=GetDlgItem(IDC_TIP6);
		if(pWnd!=NULL)
		{
			pWnd->SetWindowTextW(Str); 
			pWnd->Invalidate();
		}
		//pBox2->LineVar = (DataA+DataB)/2/RetMaxVar;


	}



	return RetMaxVar;





}

double CPerformanceBox::_TryToChangeNetworkMaxVar(double DataA,double DataB,PerferListData *pPData )
{
	double RetMaxVar;//   = pPData->MaxVar;     //   Byte/sec
	double Data = (DataA>DataB)?DataA:DataB;    // Byte/sec

	Data=Data*8; //bps

	int nK,nM;

	CString Str ;

	nK = ((int)Data)/1024;  nM = ((int)Data)/1024/1024;

	RetMaxVar = 100*1024/8;


	if(nM == 0) //KB 级别
	{
		if(nK<200 )
		{
			RetMaxVar = 100*1024/8;
			Str =L"100 Kbps";
		}
		else if(nK<500 )
		{
			RetMaxVar = 500*1024/8;
			Str =L"500 Kbps";
		}
		else if(nK<1024)
		{
			RetMaxVar = 1024*1024/8;
			Str =L"1 Mbps";
		}


	}
	else
	{

		if(nM<5)
		{
			RetMaxVar = 5*1024*1024/8;//5MB
			Str =L"5 Mbps";
		}
		else if(nM<10) 
		{

			RetMaxVar =10*1024*1024/8 ; //10MB
			Str =L"10 Mbps";
		}
		else if(nM<50) 
		{

			RetMaxVar =50*1024*1024/8 ; //50
			Str =L"50 Mbps";
		}
		else if(nM<100) 
		{

			RetMaxVar =100*1024*1024/8 ; //100
			Str =L"100 Mbps";
		}
		else if(nM<250) 
		{

			RetMaxVar =250*1024*1024/8 ; //500
			Str =L"250 Mbps";
		}
		else if(nM<500) 
		{

			RetMaxVar =500*1024*1024/8 ; //500
			Str =L"500 Mbps";
		}
		else //if(nM<1000) 
		{

			RetMaxVar =1000*1024*1024/8 ; //1000
			Str =L"1000 Mbps";
		}




	}





	if( RetMaxVar !=pPData->MaxVar) //需要变更
	{

		double Per = RetMaxVar/pPData->MaxVar ;//变换比率
		CWaveBox *pBox2= (CWaveBox *)pPData->pWaveBox;	

		for(int i=0;i<61;i++) //第61 个 用算
		{
			pBox2->Num[0][i] =(float)(pBox2->Num[0][i]/Per) ;//变换为新比例
			pBox2->Num2[0][i] =(float)(pBox2->Num2[0][i]/Per) ;//变换为新比例
		}

		pPData->StrOther1 = Str;
		pPData->MaxVar = RetMaxVar;


		CWnd *pWnd=GetDlgItem(IDC_TIP2);
		if(pWnd!=NULL)
		{
			pWnd->SetWindowTextW(Str); 
			pWnd->Invalidate();
		}
		//pBox2->LineVar = (DataA+DataB)/2/RetMaxVar;


	}



	return RetMaxVar;

}

void CPerformanceBox::_LoadDiskStaticInfo(PerferListData * pData)
{

	pData->StrOther0 = GetDrivelettersFormDiskID(pData->ID);

 
	// -------------------- 容量 -----------------------

	CString StrDev,Str ;
	StrDev.Format(L"\\\\.\\PhysicalDrive%d",pData->ID);

	HANDLE hDevice=CreateFile(StrDev,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);

	if(hDevice ==INVALID_HANDLE_VALUE) return;


	

	DISK_GEOMETRY_EX geoStruct;
	memset(&geoStruct,0,sizeof(geoStruct));
	DWORD bytesReturned;
	if(DeviceIoControl(hDevice,IOCTL_DISK_GET_DRIVE_GEOMETRY_EX ,NULL,0,&geoStruct,sizeof(geoStruct),&bytesReturned,NULL)!=0)
	{
		//MSB(88)
		double DiskSpace =  (double)(geoStruct.DiskSize.QuadPart/1024/1024); //MB;
		if(DiskSpace>1024)
		{
			DiskSpace = DiskSpace/1024;//GB

			Str.Format(L"%0.1f GB",(double)DiskSpace);
		}
		else
		{
			Str.Format(L"%0.1f MB",(double)DiskSpace);
		}

		pData->pInfoBox->Info[6].StrInfo = Str+L"\n";		

	}
	else
	{
		pData->pInfoBox->Info[6].StrInfo =  L"-\n";
	}
	CloseHandle(hDevice);


	// -------------------- 格式化容量 -----------------------

	LONGLONG DiskFmtSize=0;

	WCHAR strRootPath[]={L"c:\\"};//带根目录标记的磁盘符号
	DWORD dwSectorsPerCluster=0;//每簇中扇区数
	DWORD dwBytesPerSector=0;//每扇区中字节数
	DWORD dwFreeClusters=0;//剩余簇数
	DWORD dwTotalClusters=0;//总簇数
	int n =	0;	


	while(1)
	{

		n =	pData->StrOther0.Find(L":",n+1);
		if(n == -1) break;

		strRootPath[0] = pData->StrOther0.GetAt(n-1);

		//MSB_S(strRootPath)


		if (GetDiskFreeSpace(strRootPath,&dwSectorsPerCluster,&dwBytesPerSector,&dwFreeClusters,&dwTotalClusters))
		{
			//m_dwVolSize=dwTotalClusters*dwSectorsPerCluster*dwBytesPerSector;//不能这样，否则越界
			double VolSize=dwSectorsPerCluster*dwBytesPerSector/(1024.*1024.);	 
			//m_dVolSize=dwTotalClusters*dd;//该磁盘总大小
			DiskFmtSize = (LONGLONG) (DiskFmtSize+VolSize*dwTotalClusters);


		}

	}


	if(DiskFmtSize>1024)
	{

		Str.Format(L"%0.1f GB\n",(double)DiskFmtSize/(double)1024);
	}
	else
	{
		Str.Format(L"%0.1f MB\n",(double)DiskFmtSize );
	}

	if(DiskFmtSize<=0)
	{
		Str=L"-\n";
	}


	pData->pInfoBox->Info[6].StrInfo = pData->pInfoBox->Info[6].StrInfo+Str;


	


}



void CPerformanceBox::OnNMRClickListPerformanceitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	CMenu PopMenu;
	CMenu *pMenu= NULL;
	PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_BASE) );
	pMenu = PopMenu.GetSubMenu(5);  //5是 这个 对应的 菜单 和标签顺序对应

	CPoint CurPos ;
	GetCursorPos(&CurPos); 

	if(theApp.FlagSummaryView)
	{
		pMenu->CheckMenuItem(ID_PERFORMANCETYPE_SUMMARYVIEW, MF_BYCOMMAND|MF_CHECKED);
	}

	pMenu->TrackPopupMenu(TPM_LEFTALIGN,CurPos.x,CurPos.y,this);

	//----------

	*pResult = 0;
}

void CPerformanceBox::OnPop_PerformanceListShowHideGraphs()
{
	//theApp.AppSettings.PerformanceListShowGraph =   (theApp.AppSettings.PerformanceListShowGraph==0)?1:0 ;

	theApp.AppSettings.PerformanceListShowGraph =   (theApp.AppSettings.PerformanceListShowGraph==0)?1:0 ;
	mPItemList.SetRowHeight(20);
	//this->GetParent()->PostMessageW(WM_COMMAND,ID_PERFORMANCETYPE_HIDEGRAPHS);//UM_PERFORMANCELIST_SYTLECHANGED
	

}





void CPerformanceBox::OnUpdatePerformancetypeHidegraphs(CCmdUI *pCmdUI)
{
	CString Str;

	if(theApp.AppSettings.PerformanceListShowGraph!=0)
	{

		Str.LoadStringW(IDS_STRING_HIDEGRAPH);
	}
	else
	{
		Str.LoadStringW(IDS_STRING_SHOWGRAPH);
	}

	pCmdUI->SetText(Str);



}

PerferListData * CPerformanceBox::_InsertNetAdapterItem(CString StrType, CString StrDescription,int IfIndex)
{

	int n =mPItemList.GetItemCount();
	PerferListData *pPData =  new PerferListData;

	mPItemList.InsertItem(n,StrType);

	mPItemList.SetItemText(n,1,L"S: 0 Kbps R: 0 Kbps");
	mPItemList.SetItemText(n,2,StrDescription);  // 注册表 DriverDesc 读取到 szData 是网卡显示名称
	mPItemList.SetItemData(n,(DWORD_PTR)pPData);

	CInfoBox *pInfoBox = NULL;
	CString StrInfo; 
	CWaveBox * pEthernetBox= new CWaveBox;
	pInfoBox = new CInfoBox;	

	pInfoBox->Create(L"",WS_CHILD|WS_VISIBLE|SS_CENTER,CRect(0,0,1,1),this);
	pEthernetBox->Create(NULL,WS_CHILD,CRect(0,0,1,1),this);



	pPData->Type = PM_ETHERNET;
	pInfoBox->Type  = PM_ETHERNET;
	pPData->ID = IfIndex;	

	pPData->pWaveBox = pEthernetBox; 
	pPData->pInfoBox = pInfoBox;
	pPData->pWaveBox->DrawSecondWave = TRUE;
	pPData->pOtherWnd = NULL;
	pPData->DataA = pPData->DataB = 0;

	pInfoBox->Info[0].StrTitle  = STR_NETWORKINFO_0  ;
	pInfoBox->Info[2].StrTitle  = STR_NETWORKINFO_2  ;

	pInfoBox->Info[0].StrInfo=L"0 Kbps";
	pInfoBox->Info[2].StrInfo=L"0 Kbps";

	pInfoBox->Info[0].Type=2;  pInfoBox->Info[2].Type=1; //图例类型
	pInfoBox->Info[6].StrTitle  = STR_NETWORKINFO_6;
	pInfoBox->Info[6].rc.left-= 100;


	mPItemList.SetItemText(n,3,STR_TIP3_NETWORK);
	mPItemList.SetItemText(n,4,L"100 Kbps");

	pEthernetBox->SetLineColumn(1,1);
	pEthernetBox->SetColor(theApp.AppSettings.NetworkColor);

	pPData->MaxVar = 100*1024/8; //默认最大值100Kbps	
	NetAdapterList[IfIndex] = n;
 
	return pPData;
}

int CPerformanceBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CCreateContext * pContextBox; 


	CWnd * pWnd = NULL; 

	//-----------------------------------------------------------------------

	pViewClass = RUNTIME_CLASS(CHotboxView);

	pContextBox= new CCreateContext; 
	pContextBox->m_pCurrentDoc = NULL; 
	pContextBox->m_pCurrentFrame = NULL; 
	pContextBox->m_pLastView = NULL; 
	pContextBox->m_pNewDocTemplate =NULL; 

	pContextBox->m_pNewViewClass = pViewClass; 

	pWnd = NULL; 
	pWnd = DYNAMIC_DOWNCAST(CWnd, pViewClass->CreateObject());
	pWnd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0,0,0,0), this, IDD_HOTBOXVIEW, pContextBox); 

	pHotBox= DYNAMIC_DOWNCAST(CHotboxView, pWnd); 
	pHotBox->SetScrollSizes(MM_TEXT, CSize(1, 1)); //BOX_W_MIN,180
	pHotBox->ModifyStyleEx(WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE,0);

	delete pContextBox;
	pContextBox=NULL;


	return 0;
}

void CPerformanceBox::OnPop_ChangeGraphToNumaNodes()
{

	if(theApp.AppSettings.ProcessorDisplayMode == 2) return;

	theApp.AppSettings.ProcessorDisplayMode = 2;



	pTotalCpuBox->ShowWindow(SW_HIDE);
	pCpuBox->ShowWindow(SW_HIDE);

	pHotBox->ShowWindow(SW_SHOW);



	CWnd *pWnd= NULL;
	pWnd=GetDlgItem(IDC_TIP3);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
	pWnd=GetDlgItem(IDC_TIP4);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }


	pWnd=GetDlgItem(IDC_TIP1);
	if(pWnd!=NULL)	
	{ 
		CString StrTemp = STR_TIP1_CPU_NUMA;
		mPItemList.SetItemText(0,3,StrTemp);
		pWnd->SetWindowTextW(StrTemp);
	}

	pWnd=GetDlgItem(IDC_TIP2);if(pWnd!=NULL){ pWnd->ShowWindow(SW_HIDE); }
	 


	this->PlaceAllCtrl();
}



int CPerformanceBox::_TestDiskID(int TestID)
{

	int DiskID = TestID;
	CString StrDev ;
	while(1)
	{
		StrDev.Format(L"\\\\.\\PhysicalDrive%d",DiskID);
		HANDLE hDevice=CreateFile(StrDev,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
		if(( hDevice!=INVALID_HANDLE_VALUE)||(DiskID>1000))
		{	

			 CloseHandle(hDevice);
			return DiskID;
		}

		DiskID++;
	}
	return DiskID;

}

void CPerformanceBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//google 

	theApp.m_pMainWnd->PostMessage(UM_SUMMARYVIEW,(WPARAM)this);

	

	CFormView::OnLButtonDblClk(nFlags, point);
}

void CPerformanceBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CFormView::OnLButtonDown(nFlags, point);

	if(theApp.FlagSummaryView)
	{
		theApp.m_pMainWnd->PostMessage(WM_NCLBUTTONDOWN,    HTCAPTION,    MAKELPARAM(point.x, point.y)); 
	}
}


void CPerformanceBox::OnPop_GraphSummaryView()
{
	theApp.m_pMainWnd->PostMessage(UM_SUMMARYVIEW,(WPARAM)this);
	

}



void CPerformanceBox::OnPop_PerformanceListSummaryView()
{
	theApp.m_pMainWnd->PostMessage(UM_SUMMARYVIEW,(WPARAM)(&mPItemList));

}

BOOL CPerformanceBox::SetTipText( UINT ID, NMHDR * pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTooltipText = (TOOLTIPTEXT *)pTTTStruct;
	
	HWND hWnd = (HWND)(pTTTStruct->idFrom); //得到相应窗口ID，有可能是HWND 

 
	if (pTooltipText->uFlags & TTF_IDISHWND) //表明nID是否为HWND 
	{    
		PerferListData *pPData = (PerferListData *) mPItemList.GetItemData(1);
		if(pPData!=NULL)
		{
			CMemCompBox *pMemCompBox =(CMemCompBox *)( pPData->pOtherWnd);
			if(pMemCompBox->m_hWnd == hWnd)//从HWND得到ID值，当然你也可以通过HWND值来判断
			{		

				//pTooltipText->lpszText = L"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
			
				pTooltipText->lpszText=(LPWSTR)(LPCTSTR) pMemCompBox->GetMemCompType() ;
			//	StringCchCopy(pTooltipText->lpszText,MAX_PATH,pMemCompBox->GetMemCompType());

				//theApp.m_pMainWnd->SetWindowTextW(pMemCompBox->GetMemCompType());
				//pTooltipText->lpszText =(LPWSTR) pMemCompBox->GetMemCompType() ;
			}		
		}
	}
     
	return TRUE; 
}


void CPerformanceBox::_GetDiskLetterUseWmi(void)
{
	
    IEnumWbemClassObject* pEnumerator = NULL;
	pEnumerator = GetWmiObject(L"Win32_LogicalDiskToPartition");

	if(pEnumerator == NULL) return;

 
    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;   

	CString StrDisk,StrVolume,StrPartitionID ;

	int DiskID;	
	//int PartitionID ;
	PerferListData * pData ;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,    &pclsObj, &uReturn);
		if(0 == uReturn)  {break;}
		VARIANT vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(L"Dependent", 0, &vtProp, 0, 0);
		StrVolume = vtProp.bstrVal;
		hr = pclsObj->Get(L"Antecedent", 0, &vtProp, 0, 0);
		StrDisk = vtProp.bstrVal;
		StrPartitionID = StrDisk;


		StrVolume= StrVolume.Right(4);
		StrVolume.Remove(L'\"');

		int n=StrDisk.Find(L'#');
		StrDisk.Delete(0,n+1);
		n=StrDisk.Find(L',');  //注意不要直接取第一个作为物理硬盘ID  因为可能超过9个 （不止一位数字）
		StrDisk =StrDisk.Left(n);  

		StrPartitionID = StrPartitionID.Right(2); //最后两位可能是分区号   类似如下形式 rtition.DeviceID="Disk #4, Partition #0"
		//if(StrPartitionID.Left(1)==L"#")//只有一位数
		//{
		//	//StrPartitionID = StrPartitionID.Right(1);
		//}

		//PartitionID = _wtoi(StrPartitionID);

		DiskID = _wtoi(StrDisk);

		int i=2;
		 
		while(1)
		{

			pData = (PerferListData*) mPItemList.GetItemData(i);
			if(pData==NULL)break;
			if(pData->Type==PM_ETHERNET)break;
			if(pData->ID == DiskID && (pData->StrOther0.Find(StrVolume.GetAt(0))<0))
			{
				// 此处将盘符写入分区对应的位置 比如第0分区盘符是A则 设字符串第0个字符为A

				if(pData->StrOther0  != L" ")StrVolume =L" "+StrVolume;
				pData->StrOther0 = pData->StrOther0+StrVolume; 
			}
			i++;
		}

		

		VariantClear(&vtProp);

		pclsObj->Release();

		//break;
	}

	pEnumerator->Release();

}


void CPerformanceBox::MySetItem(int iItem, CString Strtext)
{
	LVITEM Item;	 
	Item.iItem = iItem;
	Item.iSubItem = 1;
	Item.mask = LVIF_TEXT;
	Item.cchTextMax=MAX_PATH;
	Item.pszText= (LPTSTR)(LPCTSTR)(Strtext);
	mPItemList.SetItem(&Item);

}


 
void CPerformanceBox::_FakeSelPItem(int ID)
{
	 

	   mPItemList.SetItemState(ID,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
}

double CPerformanceBox::_GetSurrentCpuSpeed(void)  //单位是 GHz
{
	double  CurrentSpeed = 0.0;
	PROCESSOR_POWER_INFORMATION  PPInfo;
	CallNtPowerInformation( ProcessorInformation,NULL, 0,&PPInfo, sizeof(PPInfo)*theApp.PerformanceInfo.nLogicalProcessor );

	CurrentSpeed = PPInfo.CurrentMhz;	
	CurrentSpeed=CurrentSpeed/1000;//注意这个cpu频率是 除以1000不是1024;

	return CurrentSpeed;
}
