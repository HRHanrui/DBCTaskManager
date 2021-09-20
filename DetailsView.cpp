// DetailsView.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "DetailsView.h"
#include "SelColumnsDlg.h"
#include <WinBase.h>
#include "SetAffinityDlg.h"

#include "ModelessPropertySheet.h"
#include "PropertiesPageImage.h"
#include "PropertiesPagePerformance.h"
#include "PropertiesPageModules.h"
#include "PropertiesPageHandles.h"
#include "PropertiesPageThreads.h"
 
#include <locale.h>
 
//***********************************************************************************************************************************



//***********************************************************************************************************************************





#include <Psapi.h>


#pragma comment( lib,  "Version.lib" ) // function



//***********************************************************************************************************************************



static UINT Thread_MonitorCreateAndExit(LPVOID pParam)
{
 

	CPageDetails  *Dlg=(CPageDetails *)pParam;
	if(Dlg == NULL) return -1; //

//
	PROCESSENTRY32 pe32;
	//在使用这个结构前，先设置它的大小
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap;

	while(1)
	{
		if(Map_PidToData.size()>2)	break;
		Sleep(500);
	}



	 
	while(1)
	{
		 map<DWORD,PVOID> TempMap_PidToData;
		
		TempMap_PidToData.insert(Map_PidToData.begin(),Map_PidToData.end());

		//给系统内所有的进程拍个快照
		 hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) //CreateToolhelp32Snapshot 调用失败
		{
			Sleep(1000);
			continue;
		}

		//遍历进程快照，轮流显示每个进程的信息
		BOOL bMore = ::Process32First(hProcessSnap,&pe32);		
		int n =0;

	   
		while (bMore)
		{			
			map<DWORD,PVOID>::iterator Iter= TempMap_PidToData.find(pe32.th32ProcessID); 
			if(Iter == TempMap_PidToData.end())// 不存在 
			{			
				Dlg->mDetailsList.SetRedraw(0);
				Dlg->AddNewProcessToList(pe32.th32ProcessID);
				Dlg->mDetailsList.SetRedraw(1);
			}
			else
			{
				TempMap_PidToData.erase(Iter);
			}

			bMore = ::Process32Next(hProcessSnap,&pe32);
			
		}


		CString StrPID;

		for (map<DWORD,PVOID>::iterator ProcToDel=TempMap_PidToData.begin(); ProcToDel!=TempMap_PidToData.end();  ProcToDel++ ) 
		{  
			 
			 Dlg->_RemoveProcessFromList(ProcToDel->first);

		}

		 TempMap_PidToData.empty();

		Sleep(1000);
	}



 

	//清除掉snapshot对象
	::CloseHandle(hProcessSnap);
	AfxEndThread(0,TRUE);
	return 0;

}


//-----------------------



//------------------------------------------------------------------------------------




static UINT Thread_SetAllListData(LPVOID pParam)
{


	CPageDetails  *Dlg=(CPageDetails *)pParam;
	if(Dlg  == NULL) return -1; //

	Dlg->SetAllListItemData(TRUE);
	Dlg->FlagEnableRefresh = TRUE;
	Dlg->ReSort(FALSE);//矫正排序结果
	Dlg->PostMessage(UM_ALLINFO_OK); //通知 第一页列表开始完善数据	 
	Dlg->mDetailsList.Invalidate();
	AfxEndThread(0,TRUE);
	return 0;

}
static UINT Thread_ReloadAllListData(LPVOID pParam)
{


	CPageDetails  *Dlg=(CPageDetails *)pParam;
	if(Dlg  == NULL) return -1; //

	Dlg->SetAllListItemData(FALSE);	
	Dlg->ReSort(FALSE);//矫正排序结果
	Dlg->pTaskList->PostMessage(UM_ALLINFO_OK); //通知 第一页列表开始完善数据	 
	Dlg->mDetailsList.Invalidate();
	AfxEndThread(0,TRUE);
	return 0;

}

static UINT Thread_EndProcessTree(LPVOID pParam)
{
	CPageDetails  *pView=(CPageDetails *)pParam;
	if(pView  == NULL) return -1; //
	pView->_EndProcessTree();

	AfxEndThread(0,TRUE);
	return 0;
}


//****************** 排序等操作 *****************************************************************************************************************



//排序   升降 续 由 类中 SortType  变量控制
int CALLBACK Sort_Details(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)  
{

	CDBCListCtrl* pList= &( ((CPageDetails*)theApp.pSelPage)->mDetailsList);




	int nCol =(int) lParamSort;
	int result = 0;     //返回值   

	PROCLISTDATA * pData1 = NULL;
	PROCLISTDATA * pData2 = NULL;

	pData1 = (PROCLISTDATA * )lParam1;
	pData2 = (PROCLISTDATA * )lParam2;

	if(pList==NULL || pData1 == NULL || pData2 == NULL)return result;




	//	int Data1,Data2;

	CString Str1,Str2;



	Str1= pList->GetItemText(pData1->SortID,nCol);
	Str2= pList->GetItemText(pData2->SortID,nCol);



	switch(nCol)
	{
	case DETAILLIST_NAME:
	case DETAILLIST_TYPE:
	case DETAILLIST_STATUS:
	case  DETAILLIST_USER:
	case  DETAILLIST_BP:
	case DETAILLIST_IMAGEPATH:
	case DETAILLIST_COMMANDLINE:
	case DETAILLIST_DESCRIPTION:
	case DETAILLIST_DEP:		

		result =lstrcmp(Str1,Str2);
		break;

	default:

		/*Str1.Remove(L',');Str2.Remove(L',');	
		Str1.Replace(L" KB",L"");Str2.Replace(L" KB",L"");
		Data1 = _wtoi(Str1); Data2 = _wtoi(Str2); */
		CString StrL1,StrL2;
		StrL1.Format(L"%16s",Str1);//转为同样长度字符串前面补空格
		StrL2.Format(L"%16s",Str2);//转为同样长度字符串前面补空格

		result =lstrcmp(StrL1,StrL2);


	}





	//颠倒排序结果
	if( pList->FlagSortUp == FALSE)
	{
		result = -result;

	}

	return result;  


}



//************ 显示进程属性对话框 ***********************************************************************************************************************


static UINT Thread_ShowProperties(LPVOID pParam)
{



	PROCLISTDATA  *pData=(PROCLISTDATA *)pParam;
	if(pData  == NULL) return -1; //


	CString StrCaption=L" ";

	StrCaption.Format(L"%s : %d  %s",pData->Name,pData->PID,STR_PROPERTIESDLG_CAPTION);


	
	CModelessPropertySheet PropertiesDlg(StrCaption,CWnd::GetDesktopWindow());

	PropertiesDlg.ModifyStyle(0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	 
 
	
	
	//---------------------------
	CPropertiesPageImage  PageImage;
	PropertiesDlg.AddPage(&PageImage);	//
	PageImage.pData= pData;
	//---------------------------
	CPropertiesPagePerformance  PagePerformance;
	PropertiesDlg.AddPage(&PagePerformance);	//
	PagePerformance.pData= pData;

	//---------------------------
	CPropertiesPageModules  PageModules;
	PropertiesDlg.AddPage(&PageModules);	//
	PageModules.PID= pData->PID;

	
	//---------------------------
	CPropertiesPageHandles  PageHandles;
	PropertiesDlg.AddPage(&PageHandles);	//
	PageHandles.PID= pData->PID;

		//---------------------------
	CPropertiesPageThreads  PageThreads;
	PropertiesDlg.AddPage(&PageThreads);	//
	PageThreads.PID= pData->PID;





	//-------------------Test ----------------------------


	//------------------------------------------------------------



	
	//HICON hIcon = theApp.mImagelist.ExtractIconW(pData->IconIndex);
	PropertiesDlg.iIcon = pData->IconIndex;
	
	//
	PropertiesDlg.DoModal();

	AfxEndThread(0,TRUE);
	return 0;

}












//***********************************************************************************************************************************
// Class :   CPageDetails
//***********************************************************************************************************************************







// CPageDetails

IMPLEMENT_DYNCREATE(CPageDetails, CFormView)

CPageDetails::CPageDetails()
: CFormView(CPageDetails::IDD)

, AllProcessCpuUsage(0) 
, pLoc (0)
, pSvc (0)
, FlagEnableRefresh(FALSE)
, CurrentRightClickCol(-1)
{

	

}

CPageDetails::~CPageDetails()
{
}

void CPageDetails::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DETAILSLIST, mDetailsList);
}

BEGIN_MESSAGE_MAP(CPageDetails, CFormView)
	ON_WM_SIZE()

	ON_MESSAGE(UM_TIMER,OnUMTimer)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CPageDetails::OnHdnItemclickProcesslist)
	ON_WM_CTLCOLOR()

	//ON_NOTIFY ( NM_CUSTOMDRAW, IDC_DETAILSLIST, OnCustomdrawMyList )

	ON_WM_MEASUREITEM()


	ON_NOTIFY(NM_RCLICK, IDC_DETAILSLIST, &CPageDetails::OnNMRClickDetailslist)
	ON_COMMAND(ID_POP_DETAILSLIST_ENDPROCESS, &CPageDetails::OnPop_EndProcess)
	ON_COMMAND(ID_DETAILS_SELECTCOLUMNS, &CPageDetails::OnDetailsSelectColumns)
	ON_COMMAND(ID_POP_DETAILSLIST_OPENFILELOCATION, &CPageDetails::OnPop_Openfilelocation)
	ON_COMMAND(ID_DETAILSLIST_GOTOSERVICE, &CPageDetails::OnPop_GotoService)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DETAILSLIST, &CPageDetails::OnLvnItemchangedDetailslist)
	ON_BN_CLICKED(IDC_BTN_ENDPROCESS, &CPageDetails::OnBnClickedBtnEndProcess)
	ON_COMMAND(ID_SETPRIORITY_REALTIME, &CPageDetails::OnPop_SetPriorityRealtime)
	ON_COMMAND(ID_SETPRIORITY_LOW, &CPageDetails::OnPop_SetPriorityLow)
	ON_COMMAND(ID_SETPRIORITY_NORMAL, &CPageDetails::OnPop_SetPriorityNormal)
	ON_COMMAND(ID_SETPRIORITY_HIGH, &CPageDetails::OnPop_SetPriorityHigh)
	ON_COMMAND(ID_SETPRIORITY_ABOVENORMAL, &CPageDetails::OnPop_SetPriorityAboveNormal)
	ON_COMMAND(ID_SETPRIORITY_BELOWNORMAL, &CPageDetails::OnPop_SetPriorityBelowNormal)
	ON_COMMAND(ID_DETAILSLIST_SETAFFINITY, &CPageDetails::OnPop_SetAffinity)
	//	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_DETAILSLIST_PROPERTIES, &CPageDetails::OnPop_Properties)
	ON_COMMAND(ID_POP_DETAILSLIST_ENDPROCESSTREE, &CPageDetails::OnPop_EndProcessTree)
	ON_COMMAND(ID_DETAILS_HIDECOLUMN, &CPageDetails::OnPop_HideColumn)
	ON_COMMAND(ID_CREATEDUMPFILE_CREATEMINIDUMP, &CPageDetails::OnPop_CreateMiniDump)
	ON_COMMAND(ID_CREATEDUMPFILE_CREATEFULLDUMP, &CPageDetails::OnPop_CreateFullDump)
	ON_COMMAND(ID_DETAILSLIST_SEARCHONLINE, &CPageDetails::OnPop_Searchonline)

	ON_NOTIFY(NM_DBLCLK, IDC_DETAILSLIST, &CPageDetails::OnNMDblclkDetailslist)
	ON_NOTIFY(LVN_KEYDOWN, IDC_DETAILSLIST, &CPageDetails::OnLvnKeydownDetailslist)
	ON_COMMAND(ID_FILE_SAVELIST, &CPageDetails::OnFileSaveList)
END_MESSAGE_MAP()


// CPageDetails diagnostics

#ifdef _DEBUG
void CPageDetails::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPageDetails::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPageDetails message handlers

void CPageDetails::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	CRect rc;	
	CRect rcParent;
	GetClientRect(rc);

	rc.InflateRect(1,1);

	rc.bottom-=50;

	mDetailsList.MoveWindow(rc);
	InvalidateIfVisible(&mDetailsList);
	


	CWnd *pBtn= GetDlgItem(IDC_BTN_ENDPROCESS);

	if(pBtn!=NULL)
	{
		CRect rcBtn(rc.right-95-15,rc.bottom+25-12,rc.right-20,rc.bottom+25+12);
		pBtn->MoveWindow(rcBtn);
	}


	mDetailsList._GetRedrawColumn();////重要！！必须马上更新否则显示跟不上！！！



}

void CPageDetails::InitList(void)
{




	// 如果想自会这个效果 就不能再设置了 ！！！
	//SetWindowTheme(mDetailsList.GetSafeHwnd(),L"explorer", NULL);


	CString  StrSysDir;

	CString strBuf;


	HIMAGELIST FakeImagelist;
	FakeImagelist=::ImageList_Create(LINE_H1,LINE_H1,ILC_COLOR32,1,1);


	if (FakeImagelist )
	{
		::SendMessage(mDetailsList.m_hWnd, LVM_SETIMAGELIST,(WPARAM)LVSIL_SMALL, (LPARAM)FakeImagelist);
	}


	//  最简单的改变行高方式
	//  1 通过对话框编辑器 设置LVS_OWNERDRAWFIXED
	//   然后改回	  mDetailsList.ModifyStyle( LVS_OWNERDRAWFIXED, 0, 0 );
	//在通过 OnMeasureItem



	//-------------------
	//   |LVS_EX_TWOCLICKACTIVATE  |LVS_EX_TRACKSELECT 

	//必须在对话框编辑器 界面里 也设置成 自绘属性 刷新速度才能更快 ！！！

	//整体风格和 ~LVS_EX_INFOTIP 做 与运算 去掉 显示不全时的工具提示
	mDetailsList.SetExtendedStyle( (mDetailsList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES| LVS_OWNERDRAWFIXED|LVS_EX_DOUBLEBUFFER |LVS_EX_HEADERDRAGDROP)&(~LVS_EX_INFOTIP)   );  //| LVS_EX_GRIDLINES |LVS_EX_CHECKBOXES /

	//---------------------------------------

	mDetailsList.InitAllColumn(COL_SAT_DETAIL,STR_COLUMN_DETAILS,COL_COUNT_DETAIL);//总数33  COL_COUNT_DETAIL

	//-----------------------------------------------


	mDetailsList.CurrentSortColumn = DETAILLIST_NAME;
	mDetailsList.FlagSortUp =TRUE;
	



	PreLoadProcesses();
	
	_StartProcessMon();//马上开始监视  不用担心 错过进程

	//ReSort(FALSE); //不要在此时排序 否则可能有问题！！！！！

	AfxBeginThread(Thread_SetAllListData,this); 



}


void CPageDetails::AddNewProcessToList(DWORD PID)
{


	CString StrSystemDir,StrSvchost ; 
	GetSystemDirectory(StrSystemDir.GetBuffer(MAX_PATH),MAX_PATH);
	StrSystemDir.ReleaseBuffer();
	StrSvchost = StrSystemDir+L"\\svchost.exe";


	HANDLE hProcess = OpenProcess( PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE ,FALSE,PID);  //注意 PROCESS_ALL_ACCESS 会导致 驱动级别的无法获取信息  //PROCESS_TERMINATE  PROCESS_ALL_ACCESS| PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION   /PROCESS_VM_READ  PROCESS_QUERY_LIMITED_INFORMATION
	//Process name.

	PROCLISTDATA *pPLdata = new PROCLISTDATA;

	CString StrProcessFullPathName;

	if(hProcess >0) 
	{ 
		StrProcessFullPathName = mProcInfo.GetPathName(hProcess);
		pPLdata->hProcess = hProcess;
	}
	else
	{
		StrProcessFullPathName = GetProcessInfoUseWMI(PID,NULL);
	}

	
	CString StrProcessName=mProcInfo.GetFileName(StrProcessFullPathName);

	pPLdata->Type = BKGPROC; //初始认为是 后台进程

	pPLdata->PID = PID;
	pPLdata->Name = StrProcessName;

	pPLdata->IconIndex = _GetIconIndex(StrProcessFullPathName);


	mDetailsList.InsertItem(0,StrProcessName,pPLdata->IconIndex);
	mDetailsList.SetItemData(0,(DWORD_PTR)pPLdata);
	Map_PidToData[pPLdata->PID] = pPLdata; //添加到映射

		//----Parent PID-----
	mProcInfo.GetParentPID(pPLdata);



	CString StrTemp; StrTemp.Format(L"%d",pPLdata->PID);

	//写入初始数据 为了视觉效果 不停顿


	mDetailsList.SetItemText(0,DETAILLIST_PID,StrTemp);
	mDetailsList.SetItemText(0,DETAILLIST_CPU,L"00");
	mDetailsList.SetItemText(0,DETAILLIST_STATUS,L"Running");



	//预先填充SameNameID 防止 最初列表创建后 SameNameID 未完成 有新进程创建造成错误




	map<CString,int>::iterator Iter = InstanceNameID.find(pPLdata->Name); 

	if(Iter == InstanceNameID.end())//不存在于列表 则添加
	{
		InstanceNameID[StrProcessName] = 0;
		pPLdata->SameNameID = 0;
	}
	else
	{
		Iter->second = Iter->second++;
		pPLdata->SameNameID = Iter->second;

	}		


	//-----------------------------------------------

	DWORD SessionID;
	ProcessIdToSessionId(pPLdata->PID,&SessionID);
	if(pPLdata->PID ==0)SessionID =0;

	pPLdata->SessionID = SessionID;


	if(SessionID == 0) //判断是windows进程的 
	{
		pPLdata->Type = WINPROC;
	}
	else if(_IsAppProcess(pPLdata->PID))
	{
		pPLdata->Type = APP;

	}



	_FillAllDataOfItem(0,pPLdata,TRUE);


	_AddToTaskList(pPLdata);
	_AddToUserList(pPLdata);
	_AddToSimpleList(pPLdata);

	if(pServiceView!=NULL)pServiceView->PostMessage(UM_PROCSTART );


	//不能加 CloseHandle  ！！！！


	PERFORMANCE_INFORMATION MyPMInfo;
	ZeroMemory(&MyPMInfo, sizeof(PERFORMANCE_INFORMATION));
	MyPMInfo.cb = sizeof(PERFORMANCE_INFORMATION);
	GetPerformanceInfo(&MyPMInfo,sizeof(PERFORMANCE_INFORMATION));
	theApp.PerformanceInfo.ProcessCount =MyPMInfo.ProcessCount ;

	if(this->FlagEnableRefresh) //允许刷新后在添加的进程才自动排序 因为程序干启动时候正在获取信息如果这时排序就会产生位置错乱！
	{
		ReSort(FALSE);
	}


}


LRESULT CPageDetails::OnUMTimer(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default


	if(! FlagEnableRefresh) return 0; 


	int i=0;


	CRect rcItem,rc,rcTemp;

	rc = mDetailsList._GetRedrawColumn();


	if(COL_SAT_DETAIL[DETAILLIST_THREADS].Redraw)
	{
		GetThreadsConutAllItem();

	}


	mDetailsList.GetItemRect(i,rcItem,LVIR_BOUNDS);

	int nCount =mDetailsList.GetItemCount();
	int  ItemHight =rcItem.Height() ;


	//if((DWORD)nCount > theApp.PerformanceInfo.ProcessCount)//删除错误项
	//{
	//	i=nCount-1;
	//	while (i>=0)
	//	{		
	//		PROCLISTDATA * pData = (PROCLISTDATA * )mDetailsList.GetItemData(i);
	//		if(pData == NULL) continue;
	//		if(pData->Mem_PrivateWS == 0 &&pData->PID >4)	
	//		{
	//			//setwndtext(rand())
	//			map<DWORD,PVOID>::iterator Iter= Map_PidToData.find	(pData->PID); 
	//			if(Iter != Map_PidToData.end())// 存在 
	//			{		
	//				Map_PidToData.erase(Iter);
	//			}
	//			mDetailsList.DeleteItem(i);
	//			delete pData;
	//			pData = NULL;
	//		}

	//		i--;

	//	}

	//}

	nCount =mDetailsList.GetItemCount();
	//	mDetailsList.SetRedraw(0);
	while (i<nCount)
	{		
		BOOL RedrawItem =IntersectRect (rcTemp,rcItem,rc);
		if(RedrawItem)
		{
			UpdateProcessInfo(i);
		}
		i++;
		rcItem.OffsetRect(0,ItemHight);

	}
	//mDetailsList.SetRedraw(1);

	ReSort();




	return 0;


}

//-------------------------------------------------------------------------------------------

void CPageDetails::UpdateProcessInfo(int nListID )
{


	CRect  rcTemp,rcSubItem;



	PROCLISTDATA * pPLData=(PROCLISTDATA * )mDetailsList.GetItemData(nListID);


	if(pPLData==NULL)return;




	CString StrTemp=L"";
	CString StrTemp2=L"";

	double ProcCpuTime;

	//---------------------------------- 计算 此进程 CPU  使用率 ---------------------------

	if( COL_SAT_DETAIL[DETAILLIST_CPU].Redraw)
	{

		double percent=0;
		percent = mProcInfo.GetCpuUsage(pPLData,&ProcCpuTime);

		StrTemp=L"";
		StrTemp.Format(L"%02.0f", percent);	

		mDetailsList.SetItemText(nListID,DETAILLIST_CPU,StrTemp);  //CPU  

	
	
	
	}





	//----------------------------------------  内存-------------------------------


	double OldPF = pPLData->Mem_PageFaults;
	double OldWorkingSet = pPLData->Mem_WorkingSet;

	mProcInfo.GetMemUsageInfo(pPLData);

	if(COL_SAT_DETAIL[DETAILLIST_MEM_WS].Redraw)
	{		

		StrTemp =_FormatSizeString(pPLData->Mem_WorkingSet/1024);					
		mDetailsList.SetItemText(nListID,DETAILLIST_MEM_WS,StrTemp+L" KB");  //Working set
	}

	if( COL_SAT_DETAIL[DETAILLIST_MEM_PEAKWS].Redraw)
	{
		StrTemp =_FormatSizeString(pPLData->Mem_PeakWorkingSet/1024);
		mDetailsList.SetItemText(nListID,DETAILLIST_MEM_PEAKWS,StrTemp+L" KB");  //Peak Working Set
	}

	if( COL_SAT_DETAIL[DETAILLIST_MEM_PPOOL].Redraw)
	{
		StrTemp =_FormatSizeString(pPLData->Mem_PagePool/1024);
		mDetailsList.SetItemText(nListID,DETAILLIST_MEM_PPOOL,StrTemp+L" KB");  //Page pool 
	}

	if( COL_SAT_DETAIL[DETAILLIST_MEM_NPPOOL].Redraw)
	{
		StrTemp =_FormatSizeString(pPLData->Mem_NonPagePool/1024);
		mDetailsList.SetItemText(nListID,DETAILLIST_MEM_NPPOOL,StrTemp+L" KB");  //NonPage pool 
	}


	if(COL_SAT_DETAIL[DETAILLIST_MEM_PF].Redraw)
	{
		StrTemp =_FormatSizeString(pPLData->Mem_PageFaults);
		mDetailsList.SetItemText(nListID,DETAILLIST_MEM_PF,StrTemp );  //Page Faults
	}

	if( COL_SAT_DETAIL[DETAILLIST_MEM_COMMITSIZE].Redraw)
	{
		StrTemp =_FormatSizeString(pPLData->Mem_Commit/1024);
		mDetailsList.SetItemText(nListID,DETAILLIST_MEM_COMMITSIZE,StrTemp );  //COMMITSIZE
	}

	if( COL_SAT_DETAIL[DETAILLIST_MEM_PFDELTA].Redraw)
	{

		if(pPLData->Mem_PageFaults - OldPF  == 0)
		{
			StrTemp =L"";
		}
		else
		{
			StrTemp.Format(L"%.0f",pPLData->Mem_PageFaults - OldPF);

		}
		mDetailsList.SetItemText(nListID,DETAILLIST_MEM_PFDELTA,StrTemp );

	}

	if( COL_SAT_DETAIL[DETAILLIST_MEM_WSDELTA].Redraw)
	{
		if(pPLData->Mem_WorkingSet - OldWorkingSet  == 0)
		{
			StrTemp =L"";
		}
		else
		{
			StrTemp.Format(L"%.0f KB",(pPLData->Mem_WorkingSet - OldWorkingSet)/1024);

		}
		mDetailsList.SetItemText(nListID,DETAILLIST_MEM_WSDELTA,StrTemp );


	}

	//WsPrivate

	if(COL_SAT_DETAIL[DETAILLIST_MEM_PWS].Redraw &&  (pPLData->Mem_WorkingSet - OldWorkingSet != 0))
	{

		double  MemUsage;

		MemUsage = mProcInfo.GetWsPrivate_PDH(pPLData);		

		pPLData->Mem_PrivateWS = MemUsage;

		StrTemp =_FormatSizeString(MemUsage/1024);

		mDetailsList.SetItemText(nListID,DETAILLIST_MEM_PWS,StrTemp+L" KB"); //WSPrivate
	}

	//--------------------------------------Status -----------------------

	//单独设置


	//------------------------------------ Handles----------------------------------


	if( COL_SAT_DETAIL[DETAILLIST_HANDLES].Redraw)
	{

		StrTemp.Format(L"%d",mProcInfo.GetHandles(pPLData->hProcess));
		mDetailsList.SetItemText(nListID,DETAILLIST_HANDLES, StrTemp);  //Handles
	}

	if( COL_SAT_DETAIL[DETAILLIST_GDIOBJ].Redraw)
	{

		StrTemp.Format(L"%d",GetGuiResources(pPLData->hProcess,GR_GDIOBJECTS));
		mDetailsList.SetItemText(nListID,DETAILLIST_GDIOBJ, StrTemp);  //Handles
	}

	if( COL_SAT_DETAIL[DETAILLIST_USEROBJ].Redraw)
	{

		StrTemp.Format(L"%d",GetGuiResources(pPLData->hProcess,GR_USEROBJECTS));
		mDetailsList.SetItemText(nListID,DETAILLIST_USEROBJ, StrTemp);  //Handles
	}

	//-----------------------------------------------IO--------------------------------------------


	IO_COUNTERS  IOCounter;

	GetProcessIoCounters(pPLData->hProcess,&IOCounter);
	if( COL_SAT_DETAIL[DETAILLIST_IO_R].Redraw)
	{

		StrTemp.Format(L"%u",IOCounter.ReadOperationCount);
		if(pPLData->PID == 0) StrTemp =L"";
		mDetailsList.SetItemText(nListID,DETAILLIST_IO_R, StrTemp);  
	}

	if(COL_SAT_DETAIL[DETAILLIST_IO_W].Redraw)
	{

		StrTemp.Format(L"%u",IOCounter.WriteOperationCount);
		if(pPLData->PID == 0) StrTemp =L"";
		mDetailsList.SetItemText(nListID,DETAILLIST_IO_W, StrTemp);  
	}

	if( COL_SAT_DETAIL[DETAILLIST_IO_O].Redraw)
	{

		StrTemp.Format(L"%u",IOCounter.OtherOperationCount);
		if(pPLData->PID == 0) StrTemp =L"";
		mDetailsList.SetItemText(nListID,DETAILLIST_IO_O, StrTemp);   
	}

	//------------------

	if( COL_SAT_DETAIL[DETAILLIST_IO_RB].Redraw)
	{

		StrTemp.Format(L"%u",IOCounter.ReadTransferCount);
		if(pPLData->PID == 0) StrTemp =L"";
		mDetailsList.SetItemText(nListID,DETAILLIST_IO_RB, StrTemp);  //Handles
	}

	if( COL_SAT_DETAIL[DETAILLIST_IO_WB].Redraw)
	{

		StrTemp.Format(L"%u",IOCounter.WriteTransferCount);
		if(pPLData->PID == 0) StrTemp =L"";
		mDetailsList.SetItemText(nListID,DETAILLIST_IO_WB, StrTemp);  //Handles

	}

	if( COL_SAT_DETAIL[DETAILLIST_IO_OB].Redraw)
	{

		StrTemp.Format(L"%u",IOCounter.OtherTransferCount);
		if(pPLData->PID == 0) StrTemp =L"";
		mDetailsList.SetItemText(nListID,DETAILLIST_IO_OB, StrTemp);  //Handles
	}

	//--------------------------------Proc CpuTime -------------------------

	if( COL_SAT_DETAIL[DETAILLIST_CPUTIME].Redraw)
	{


		int nHour,nMin,nSec;

		nHour=nMin=nSec=0;

		double Time1 = ProcCpuTime/10000000;
		LONG   LTime   = (LONG)Time1;

		if(LTime>=60)
		{
			nMin =  LTime/60;
			nSec = LTime%60;
			if(nMin>=60)
			{
				nHour= nMin/60;
				nMin = nMin%60;

			}
		}
		else
		{
			nSec = (int)LTime;
		}



		CString StrT;
		StrTemp.Format(L"%0.3f",Time1-nSec);

		StrT = StrTemp.Right(4);
		StrTemp=StrT;
		StrTemp2.Format(L"%d:%.2d:%.2d%s",nHour,nMin,nSec,StrTemp);

		mDetailsList.SetItemText(nListID,DETAILLIST_CPUTIME, StrTemp2);  //Handles
	}


	//----------------------------Threads---------------------------------

	if( COL_SAT_DETAIL[DETAILLIST_THREADS].Redraw)
	{

		//int  ThreadsCount =mProcInfo.GetThreadCount(pPLData) ;
		StrTemp.Format(L"%d",pPLData->ThreadsCount);
		mDetailsList.SetItemText(nListID,DETAILLIST_THREADS, StrTemp);  //Handles

	}

	//-------------------------------------------------------------

	//优先级

	mProcInfo.GetPriority(pPLData->hProcess,&StrTemp);
	mDetailsList.SetItemText(nListID,DETAILLIST_BP, StrTemp);  //Handles


	//-------------------------------------------------------------

	//	PROCESS_MITIGATION_DEP_POLICY  StatusDEP;

	//GetProcessMitigationPolicy (












}

//****************************************************************************************************
void CPageDetails::OnHdnItemclickProcesslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here


	PROCLISTDATA *pData = NULL;
	int nCount = mDetailsList.GetItemCount();

	for(int i= 0;i<nCount;i++)
	{
		pData = (PROCLISTDATA *) mDetailsList.GetItemData(i);
		pData->SortID = i;
	}

	if(mDetailsList.CurrentSortColumn == phdr->iItem)
	{
		mDetailsList.FlagSortUp = !mDetailsList.FlagSortUp;
	}
	else
	{
		mDetailsList.FlagSortUp = TRUE;
	}


	mDetailsList.CurrentSortColumn =  phdr->iItem;
	//设置回调函数的参数和入口地址   
	mDetailsList.SortItems(Sort_Details, phdr->iItem);

	*pResult = 0;




}


HBRUSH CPageDetails::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	return theApp.BkgBrush;
}





int CPageDetails::_GetIconIndex(LPCTSTR lpszPath)
{

	SHFILEINFO sfi;

	memset(&sfi, 0, sizeof(sfi));

	SHGetFileInfo (lpszPath,     FILE_ATTRIBUTE_NORMAL,     &sfi,      sizeof(sfi),SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES |SHGFI_ICON  );

	return   sfi.iIcon;


}


void CPageDetails::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	lpMeasureItemStruct->itemHeight = LINE_H1 ;


	CFormView::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CPageDetails::Refresh(void)
{
	//	 AfxBeginThread(Thread_RefreshList,this);
	// ListProcesses();

	COLUMNSTATUS   Temp[COL_COUNT_DETAIL];

	memcpy(Temp,COL_SAT_DETAIL,sizeof(COL_SAT_DETAIL));

	int ItemCount = mDetailsList.GetItemCount();

	
	for(int i=0;i<COL_COUNT_DETAIL;i++)
	{
		COL_SAT_DETAIL[i].Redraw = 1;
	}

	for(int i=0;i<ItemCount;i++)
	{
		UpdateProcessInfo(i);
	}


	memcpy(COL_SAT_DETAIL,Temp,sizeof(COL_SAT_DETAIL));


 
}




BOOL CPageDetails::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if(pMsg->message == UM_REFRESH)
	{	
		AfxBeginThread(Thread_ReloadAllListData,this);		
	}



	return CFormView::PreTranslateMessage(pMsg);
}

void CPageDetails::_StartProcessMon(void)
{
	AfxBeginThread(Thread_MonitorCreateAndExit ,this);

	return ;
}


void CPageDetails::_RemoveProcessFromList( DWORD PID)
{


	CString Str;
	 
	PROCLISTDATA *pListData = NULL;
	int n = mDetailsList.GetItemCount();
	for(int i = 0;i< n ;i++)
	{
		pListData = NULL;
		pListData = (PROCLISTDATA *) mDetailsList.GetItemData(i);
		if(pListData == NULL)  continue; 	
		 
		if(PID == pListData->PID)  
		{		
				

			map<CString,int>::iterator Iter = InstanceNameID.find(pListData->Name); 
			if(Iter != InstanceNameID.end())//存在,InstanceNameID记录减少
			{
				Iter->second = Iter->second--;	
				if(Iter->second == 0) 
				{
					InstanceNameID.erase(Iter);
				}
			}

			mDetailsList.DeleteItem(i);

			//delete pListData;
			//pListData = NULL;

			
			/*PdhCloseQuery(pListData->hQueryIO);
			PdhCloseQuery(pListData->hCounterMem);*/

			if(pListData!=NULL)
			{
				if(pListData->hProcess!= INVALID_HANDLE_VALUE)
				{
					CloseHandle(pListData->hProcess);
				}


				map<DWORD,PVOID>::iterator Iter= Map_PidToData.find	(pListData->PID); //xxxxxxxxx
				if(Iter != Map_PidToData.end())// 存在 
				{		
					Map_PidToData.erase(Iter);
				}
				//放在最后 否则 pListData有可能在 PostMessageW 之后 变为NULL
				theApp.m_pMainWnd->PostMessageW(UM_PROCEXIT,0,(LPARAM)pListData); //向主窗口发送删除消息	主窗口统一处理放在 Itemdata 呗提前删除		
 

			}


			break ;
		}
	}
















	//更新总 进程数量 !!!!!!!!!

	PERFORMANCE_INFORMATION MyPMInfo;


	ZeroMemory(&MyPMInfo, sizeof(PERFORMANCE_INFORMATION));	 
	//memset(&MyPMInfo,0,sizeof(PERFORMANCE_INFORMATION));
	//MyPMInfo.cb = sizeof(PERFORMANCE_INFORMATION);
	GetPerformanceInfo(&MyPMInfo,sizeof(PERFORMANCE_INFORMATION));
	theApp.PerformanceInfo.ProcessCount =MyPMInfo.ProcessCount ;

	//发送消息给服务列表
	if(pServiceView!=NULL)pServiceView->PostMessage(UM_PROCEXIT );


	//ReSort(FALSE);
}

void CPageDetails::_AddToTaskList( PROCLISTDATA *pListData)  //添加到第一页列表
{


	if(pTaskList==NULL) return;
	pTaskList->PostMessageW(UM_PROCSTART,0,(LPARAM)pListData);

}

void CPageDetails::_AddToUserList(PROCLISTDATA *pListData)
{
	if(pUsersView==NULL) return;
	pUsersView->PostMessage(UM_PROCSTART,0,(LPARAM)pListData);


}

void CPageDetails::OnNMRClickDetailslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here



	CMenu PopMenu;
	CMenu *pMenu;
	PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_BASE) );
	pMenu = PopMenu.GetSubMenu(3);  //3是 这个 对应的 菜单 和标签顺序对应

	CPoint CurPos ;
	GetCursorPos(&CurPos); 




	int nSel = mDetailsList.GetNextItem( -1, LVNI_SELECTED );

	PROCLISTDATA * pData = (PROCLISTDATA *)mDetailsList.GetItemData(nSel);
	UINT nID=-1;
	if(pData != NULL)
	{
		DWORD ProcPriority = GetPriorityClass(pData->hProcess);

		switch (ProcPriority)
		{
		case ABOVE_NORMAL_PRIORITY_CLASS :nID=ID_SETPRIORITY_ABOVENORMAL;break;
		case BELOW_NORMAL_PRIORITY_CLASS :nID=ID_SETPRIORITY_BELOWNORMAL;break;
		case HIGH_PRIORITY_CLASS :nID=ID_SETPRIORITY_HIGH;break;
		case IDLE_PRIORITY_CLASS :nID=ID_SETPRIORITY_LOW;break;
		case NORMAL_PRIORITY_CLASS :nID=ID_SETPRIORITY_NORMAL;break;
		case REALTIME_PRIORITY_CLASS :nID=ID_SETPRIORITY_REALTIME;break;
		default :
			nID = -1;

		}
		if( nID!=-1)
			pMenu->CheckMenuRadioItem(ID_SETPRIORITY_REALTIME,ID_SETPRIORITY_LOW  ,nID,MF_BYCOMMAND);		


	}


	pMenu->TrackPopupMenu(TPM_LEFTALIGN,CurPos.x,CurPos.y,this);

	*pResult = 0;
}

void CPageDetails::OnPop_EndProcess()
{
	  theApp.Global_EndProcessesInList(&mDetailsList,TRUE);


//	int iSel = mDetailsList.GetNextItem( -1, LVNI_SELECTED );


//	PROCLISTDATA *pData;
//	pData = (PROCLISTDATA *) mDetailsList.GetItemData(iSel);

	
//	TerminateProcessEx2(pData->PID);

//	if(pData)TerminateProcessEx2(MiniFxOpenProcess(GENERIC_WRITE,pData->PID));
	
	






}

BOOL CPageDetails::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	if ((((LPNMHDR)lParam)->code == NM_RCLICK))  
	{  
		CPoint pt, pt2;  
		GetCursorPos(&pt);  
		pt2 = pt;  
		mDetailsList.ScreenToClient(&pt);  
		CWnd* pWnd = mDetailsList.ChildWindowFromPoint(pt);  
		CHeaderCtrl* pHeader = mDetailsList.GetHeaderCtrl();  
		if(pWnd && (pWnd->GetSafeHwnd() == pHeader->GetSafeHwnd()))  
		{  
			//-------------可检测 在哪列按右键--------------
			HDHITTESTINFO info = {0};  
			info.pt = pt;  
			pHeader->SendMessage(HDM_HITTEST, 0, (LPARAM)&info); 

			CurrentRightClickCol = info.iItem;
			//MSB(info.iItem);
			//-------------------------------------------------------------------
			CMenu PopMenu;
			CMenu *pMenu;
			PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_COLUMN) );
			pMenu = PopMenu.GetSubMenu(2);  //3是 这个 对应的 菜单 和标签顺序对应

			pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt2.x,pt2.y,this);

		}  
	}  

	return CFormView::OnNotify(wParam, lParam, pResult);
}

void CPageDetails::OnDetailsSelectColumns()
{
	CSelColumnsDlg Dlg;

	Dlg.pDetailList = &mDetailsList ;

	Dlg.DoModal();
}

CString CPageDetails::_FormatSizeString(double SizeNum)
{

	CString StrTemp;
	WCHAR  StrOut[MAX_PATH]; 

	StrTemp.Format(L"%.0f",  SizeNum);

	if(SizeNum>0)
	{
		GetNumberFormat(LOCALE_USER_DEFAULT,LOCALE_NOUSEROVERRIDE,StrTemp,NULL, StrOut,MAX_PATH);
		StrTemp = StrOut;
		StrTemp=StrTemp.Left(StrTemp.GetLength()-3);
	}





	return StrTemp ;
}

void CPageDetails::OnPop_Openfilelocation()
{
	// TODO: Add your command handler code here
	PROCLISTDATA * pData = NULL;
	pData = _GetSelectionItemData();

	if(pData == NULL) return ;

	CString StrTargetPath = mProcInfo.GetPathName(pData->hProcess);
	OpenFileLocation(StrTargetPath);


}

CString  CPageDetails::GetProcessInfoUseWMI(DWORD PID,PROCLISTDATA * pData)
{
	IEnumWbemClassObject* pEnumerator = NULL;

	CString StrWmi;
	StrWmi.Format(L"Win32_Process WHERE ProcessId = %d",PID);
	pEnumerator = GetWmiObject(StrWmi);

	if(pEnumerator == NULL) return 0;

	IWbemClassObject *pclsObj;
	ULONG uReturn = 0;   

	CString StrTemp;  

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,    &pclsObj, &uReturn);
		if(0 == uReturn)  { break; }
		VARIANT vtProp;

		// Get the value of the Name property

		hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
		StrTemp= vtProp.bstrVal;

		SysFreeString(vtProp.bstrVal);

		VariantClear(&vtProp);
		pclsObj->Release();

		//break;
	}




	pEnumerator->Release();
	pEnumerator = NULL;
	return StrTemp;
}

void CPageDetails::OnPop_GotoService()
{
	// TODO: Add your command handler code here
	theApp.pMainTab->SetCurSel(4);
	//--------------触发实际动作---------- 
	NMHDR nmhdr; 
	nmhdr.code = TCN_SELCHANGE;  
	nmhdr.hwndFrom = theApp.pMainTab->GetSafeHwnd();  
	nmhdr.idFrom= theApp.pMainTab->GetDlgCtrlID();  
	::SendMessage(theApp.pMainTab->GetSafeHwnd(), WM_NOTIFY,MAKELONG(TCN_SELCHANGE,4), (LPARAM)(&nmhdr));

}

void CPageDetails::PreLoadProcesses(void)
{

	

	PROCESSENTRY32 ProcEntry;	
	ProcEntry.dwSize = sizeof(ProcEntry);	//在使用这个结构前 先设置大小

	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	
	if (hProcessSnap == INVALID_HANDLE_VALUE)return ;
	
	BOOL bMore = ::Process32First(hProcessSnap,&ProcEntry);	


	DWORD ProcessesCount;






	CString StrTemp;

	mDetailsList.SetRedraw(0);


	

	while (bMore)
	{
		DWORD PID = ProcEntry.th32ProcessID;

		PROCLISTDATA *pPLdata = new PROCLISTDATA;

		pPLdata->hProcess = NULL;
		HANDLE hProcess = OpenProcess( PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE  ,FALSE,PID);  //注意 PROCESS_ALL_ACCESS 会导致 驱动级别的无法获取信息  //PROCESS_TERMINATE  PROCESS_ALL_ACCESS| PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION   /PROCESS_VM_READ  PROCESS_QUERY_LIMITED_INFORMATION

		if(hProcess > 0) 
		{
			pPLdata->hProcess = hProcess;
		}


	
	

		CString StrProcessName= ProcEntry.szExeFile;


		//！！！！！！！这是一个特殊情况 开始 PID == 4 的进程名是空白 要人工填上 否则影响最初的排序!!!!!!!!!!!
		if(PID == 4){StrProcessName = L"System" ;}


		pPLdata->Type = BKGPROC; //初始认为是 后台进程
		pPLdata->PID = PID;
		pPLdata->Name = StrProcessName;


		DWORD SessionID;
		ProcessIdToSessionId(pPLdata->PID,&SessionID);		
		if(pPLdata->PID == 0)SessionID =0;
		pPLdata->SessionID = SessionID;


		pPLdata->IconIndex = 0;


		Map_PidToData[pPLdata->PID] = pPLdata; //添加到映射
		mDetailsList.InsertItem(0,StrProcessName,pPLdata->IconIndex); //实际插入
		mDetailsList.SetItemData(0,(DWORD_PTR)pPLdata);
	

		StrTemp.Format(L"%d",pPLdata->PID);

		//写入初始数据 为了视觉效果 不停顿
		mDetailsList.SetItemText(0,DETAILLIST_PID,StrTemp);
		mDetailsList.SetItemText(0,DETAILLIST_CPU,L"00");
		mDetailsList.SetItemText(0,DETAILLIST_STATUS,L"Running");


		//预先填充SameNameID 防止 最初列表创建后 SameNameID 未完成 有新进程创建造成错误



		map<CString,int>::iterator Iter = InstanceNameID.find(pPLdata->Name); 

		if(Iter == InstanceNameID.end())//不存在于列表 则添加
		{
			InstanceNameID[StrProcessName] = 0;
			pPLdata->SameNameID = 0;
		}
		else
		{
			Iter->second = Iter->second++;
			pPLdata->SameNameID = Iter->second;

		}		



		//-----------------------------------------------


		CString StrProcessFullPathName;
		if(hProcess  > 0) 
		{ 
			StrProcessFullPathName = mProcInfo.GetPathName(hProcess);	
		}
		else
		{
			StrProcessFullPathName = GetProcessInfoUseWMI(PID,NULL);
		}		
		::PathRemoveFileSpec((LPTSTR)(LPCTSTR)StrProcessFullPathName);
		
		BOOL IsSystemFolder = FALSE;
		
		if(StrProcessFullPathName.Find(L":"))
		{
			StrProcessFullPathName.Delete(0,3);

			IsSystemFolder = (StrProcessFullPathName.CompareNoCase(L"windows\\system32") == 0);

		}
		else//无路径的认为是windows进程
		{
			IsSystemFolder = TRUE;
		}

 	
		if(_IsAppProcess(pPLdata->PID))  //先判断是不是有窗口
		{
			pPLdata->Type = APP;
		}
		else  if( IsSystemFolder ) //判断是windows进程的 
		{
			pPLdata->Type = WINPROC;
		}
		
	

		



		//下一个进程

		bMore = ::Process32Next(hProcessSnap,&ProcEntry);


	}

	ProcessesCount =mDetailsList.GetItemCount();

	theApp.PerformanceInfo.ProcessCount = ProcessesCount;

	//------------------------------

	//------------------------------
	ReSort(FALSE);

	mDetailsList.SetRedraw(1);


	//最开始_处调用 StartProcessMon();马上开始监视 否则可能错过进程


	InvalidateIfVisible(&mDetailsList);

	::CloseHandle(hProcessSnap);



	pTaskList->PostMessageW(UM_BASELISTOK);




}

void CPageDetails::SetAllListItemData(BOOL NewAdd)
{ 


	//	mDetailsList.SetRedraw(0);

	for(DWORD i=0;    i<theApp.PerformanceInfo.ProcessCount;    i++)
	{

		_FillAllDataOfItem(i,NULL,NewAdd);

	}

 






}

void CPageDetails::_FillAllDataOfItem(int Index,PROCLISTDATA *pInputdata,BOOL NewAdd)
{

	

	PROCLISTDATA *pPLdata = NULL;

	if(pInputdata != NULL)
	{
		pPLdata = pInputdata;

	}
	else
	{
		pPLdata=(PROCLISTDATA *) mDetailsList.GetItemData( Index ) ;

	}

	if( pPLdata == NULL ) return;

	if( pPLdata->PID <0 ) return;






	HANDLE	hProcess = pPLdata->hProcess;
	INT PID = pPLdata->PID;

	CString StrTemp;


	CString StrProcessFullPathName;

	if(hProcess  > 0) 
	{ 
		StrProcessFullPathName = mProcInfo.GetPathName(hProcess);	
	}
	else
	{
		StrProcessFullPathName = GetProcessInfoUseWMI(PID,NULL);
	}



	
	pPLdata->IconIndex = _GetIconIndex(StrProcessFullPathName);

	mDetailsList.SetItem(Index,0, LVIF_IMAGE, pPLdata->Name,pPLdata->IconIndex,0,0,0); 






	//----Parent PID-----
	mProcInfo.GetParentPID(pPLdata);

	pPLdata->CPU_Usage = 0;

	CString StrDescription=mProcInfo.GetVerInfoString(StrProcessFullPathName,L"FileDescription");//_GetFileDescription(StrProcessFullPathName);

	pPLdata->Description =StrDescription;

	



	ULONGLONG  NewOtherIO;
	pPLdata->DiskIO =   mProcInfo.GetDiskIO(hProcess,&NewOtherIO);	

	//	pPLdata->CurrentID = 0;


	//----------------------PID---------------------


	StrTemp.Format(L"%d",PID);
	mDetailsList.SetItemText(Index,DETAILLIST_PID,StrTemp);  //PID

	//------------------------------------


	if(PID == 0)
	{

		mDetailsList.SetItemText(Index,0,L"System Idle Process" );  //OK
		pPLdata->Name=L"System Idle Process" ;

		StrTemp = L"Idle";
		pPLdata->Description =L"Percentage of time the processor is idle";

	}

	if(PID == 4)
	{
		mDetailsList.SetItemText(Index,0,L"System" );  //OK
		pPLdata->Name=L"System";

		StrTemp = L"System";
		pPLdata->Description = L"NT Kernel && System";  // 字符 & 必须写成 && 才能输出 & 
	}




	mDetailsList.SetItemText(Index,DETAILLIST_DESCRIPTION, pPLdata->Description);



	// --------不需要显示-------------


	CString StrPDHProcessName;
	StrPDHProcessName = pPLdata->Name;

	if(StrPDHProcessName.Right(4).CompareNoCase(L".exe") == 0)  //exe去掉 .dll则不去掉 Pdh 要求这样格式
	{	
		StrPDHProcessName=StrPDHProcessName.Left(StrPDHProcessName.GetLength()-4);
	}



	if(pPLdata->SameNameID == 0)
	{
		StrTemp = StrPDHProcessName;
	}
	else
	{
		StrTemp.Format(L"%s#%d",StrPDHProcessName,pPLdata->SameNameID)  ;
	}

	StrPDHProcessName = StrTemp;


	//特殊的 


	if(PID == 0){	StrPDHProcessName = L"Idle";	}
	if(PID == 4){	StrPDHProcessName = L"System";	}






	//---------------------------------------------------



	if(NewAdd)
	{


		//----------------------------------添加获取  Working Set - Private 的计数器 ----------------

		// 打开一个查询对象
		PdhOpenQuery (0, 0,   &pPLdata->hQueryMem );
		pPLdata->hCounterMem= NULL;

		CString Str=L"";
		Str.Format(L"\\Process(%s)\\Working Set - Private", StrPDHProcessName );
		PdhAddCounter(pPLdata->hQueryMem,Str, 0, & pPLdata->hCounterMem);



		//----------------------------------添加获取  IO  的计数器 ----------------

		//// 打开一个查询对象
		//PdhOpenQuery (0, 0,   &pPLdata->hQueryIO );
		//pPLdata->hCounterIO= NULL;
		//Str.Format(L"\\Process(%s)\\IO Data Bytes/sec", StrPDHProcessName );
		//PdhAddCounter(pPLdata->hQueryIO,Str, 0, & pPLdata->hCounterIO);

		//----------------------------------添加获取   Thread   的计数器 ----------------

		// 打开一个查询对象
		/*PdhOpenQuery (0, 0,   &pPLdata->hQueryThread );
		pPLdata->hCounterThread= NULL;
		Str.Format(L"\\Process(%s)\\Thread Count", StrPDHProcessName );
		PdhAddCounter(pPLdata->hQueryThread,Str, 0, & pPLdata->hCounterThread);*/

		//----------------------------------添加获取  Working Set  的计数器 ----------------

		// 打开一个查询对象
		PdhOpenQuery (0, 0,   &pPLdata->hQueryWs );
		pPLdata->hCounterWs= NULL;
		Str.Format(L"\\Process(%s)\\Working Set", StrPDHProcessName );
		PdhAddCounter(pPLdata->hQueryWs,Str, 0, & pPLdata->hCounterWs);


		pPLdata->Mem_Commit = pPLdata->Mem_NonPagePool = pPLdata->Mem_PageFaults= pPLdata->Mem_PagePool = pPLdata->Mem_PeakWorkingSet = pPLdata->Mem_PrivateWS = pPLdata->Mem_WorkingSet = 0;



	}




	//----------------------User  Name-------------------


	pPLdata->User  = mProcInfo.GetUser(hProcess,pPLdata->PID);
	mDetailsList.SetItemText(Index,DETAILLIST_USER,pPLdata->User);

	//----------------------SessionID-------------------


	DWORD SessionID;
	ProcessIdToSessionId(PID,&SessionID);
	if(PID == 0)  SessionID= 0;
	StrTemp.Format(L"%d",SessionID);
	mDetailsList.SetItemText(Index,DETAILLIST_SESSIONID, StrTemp);



	//-----------------------x86  or  x64 ------------------

	BOOL IsWow64 = FALSE;

	if(theApp.FlagSysIs32Bit) 
	{
		BOOL IsWow64 = TRUE;
	}
	else
	{		
		IsWow64Process(hProcess,&IsWow64);
	}

	if(IsWow64)
	{
		StrTemp = L"32 bit";
	}
	else
	{
		StrTemp = L"64 bit";
	}



	mDetailsList.SetItemText(Index,DETAILLIST_TYPE,StrTemp);

	mDetailsList.SetItemText(Index,DETAILLIST_CPU,L"00");

	mDetailsList.SetItemText(Index,DETAILLIST_STATUS,L"Running");


	//------------- WSPrivate 先完整获取一次 因为以后不变的就不再刷新了 ---------------

	double  MemUsage;
	MemUsage = mProcInfo.GetWsPrivate_PDH(pPLdata) ;
	pPLdata->Mem_PrivateWS = MemUsage;
	StrTemp =_FormatSizeString(MemUsage/1024);
	mDetailsList.SetItemText(Index,DETAILLIST_MEM_PWS,StrTemp+L" KB"); //WSPrivate


	//-----------------------------------------------------------------------



	mDetailsList.SetItemText(Index,DETAILLIST_COMMANDLINE, mProcInfo.GetProcCommandLine(PID) ); 

	mDetailsList.SetItemText(Index,DETAILLIST_IMAGEPATH,StrProcessFullPathName); //

	mDetailsList.SetItemText(Index,DETAILLIST_DESCRIPTION,pPLdata->Description); //


	///-----------------------------------------------------------------------------

	DWORD DEPFlags;
	BOOL DEPPermanent;
	theApp.GetProcessDEPPolicy(hProcess,&DEPFlags,&DEPPermanent);

	StrTemp = L"Disabled";

	if(DEPFlags == PROCESS_DEP_ENABLE)StrTemp = L"Enabled";

	mDetailsList.SetItemText(Index,DETAILLIST_DEP,StrTemp ); //


	///------------------------------------------------------------------------------

	//优先级
	mProcInfo.GetPriority(pPLdata->hProcess,&StrTemp);
	mDetailsList.SetItemText(Index,DETAILLIST_BP,StrTemp); //



}

BOOL CPageDetails::_IsAppProcess(int PID)
{

	//本进程直接返回 TRUE
	if(theApp.CurrentPID == PID)
	{
		return TRUE;
	}


	//-----------------------------------------------判断是否为 前台进程----------------------------




	BOOL Ret = FALSE;

	CWnd *pDeskTop=CWnd::GetDesktopWindow();
	CWnd  *pAppWnd = pDeskTop->GetWindow(GW_CHILD);

	//CString  StrWndCaption;
	CString Strtemp;
	DWORD WindowPID ;

	while( pAppWnd!=NULL)
	{
		DWORD  WndExStyle =  pAppWnd->GetExStyle(); 
		if(pAppWnd->IsWindowVisible()&& (!(WndExStyle&WS_EX_TOOLWINDOW))   )  //      WS_CAPTION   && (pAppWnd->GetExStyle()&(WS_EX_APPWINDOW)  )  0x200000 是win 10的 Windows Shell Experience Host
		{
			GetWindowThreadProcessId(pAppWnd->m_hWnd,&WindowPID);
			if( (WindowPID == PID)    )
			{
				if(! (WndExStyle&0x200000)  )
				Ret = TRUE;
				break;
			}


		}

		pAppWnd=pAppWnd->GetWindow(GW_HWNDNEXT);

	}

	return Ret;
}

void CPageDetails::_AddToSimpleList(PROCLISTDATA *pListData)
{
	// 添加到simple list  注意 不能用 UM_PROCSTART 因为 这个消息向多个列表发送 
	// 都会被主对话框反复接收造成重复添加

	if(pListData->Type == APP) 
	{
		theApp.m_pMainWnd->PostMessageW(UM_ADD_PROC_SIMPEL_LIST,0,(LPARAM)pListData);
	}
}


void CPageDetails::SetProcessStatusInfo(DWORD PID,int Status)
{
	PROCLISTDATA *pData = NULL;
	CString  StrStatus ;

	switch(Status)
	{
	case PS_RUNNING:StrStatus = L"Running";break;
	case PS_NOTRESPONDING:StrStatus = L"Not Responding";break;
		//case 2:StrStatus = L"Running";break;
	}



	for(int i=0;i<mDetailsList.GetItemCount();i++)
	{
		pData = (PROCLISTDATA *)mDetailsList.GetItemData(i);
		if(pData ==NULL) continue ;
		if(pData->PID == PID)
		{
			mDetailsList.SetItemText(i,DETAILLIST_STATUS,StrStatus);
			break;
		}
	}



}

void CPageDetails::ReSort(BOOL SkipStaticColumn)
{

	//注意增减进程时候要马上调用一次本函数 SkipStaticColumn 要设为 FALSE否则静态列不会自动排序！！！

	if(SkipStaticColumn)
	{
		switch(mDetailsList.CurrentSortColumn)
		{//以下列 内容 相对于进程 固定 不需要每次刷新重新排序
		case DETAILLIST_NAME:
		case DETAILLIST_TYPE:
		case DETAILLIST_PID:
		case DETAILLIST_USER:
		case DETAILLIST_SESSIONID:
		case DETAILLIST_IMAGEPATH:
		case DETAILLIST_COMMANDLINE:
		case DETAILLIST_UAC:
		case DETAILLIST_DESCRIPTION:
		case DETAILLIST_DEP:
			return;

		}
	}
 

	PROCLISTDATA *pData = NULL;
	int nCount = mDetailsList.GetItemCount();

	for(int i= 0;i<nCount;i++)
	{
		pData = NULL;
		pData = (PROCLISTDATA *) mDetailsList.GetItemData(i);

		if(pData != NULL)
		{
			pData->SortID = i;
		}
	}


	mDetailsList.SortItems(Sort_Details, mDetailsList.CurrentSortColumn);
}

void CPageDetails::OnLvnItemchangedDetailslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	//	int nSel = mDetailsList.GetNextItem( -1, LVNI_SELECTED );

	CWnd * pButton=	this->GetDlgItem(IDC_BTN_ENDPROCESS);
	if(mDetailsList.GetSelectedCount()>0)
	{		
		if(pButton!=NULL)
		{
			pButton->EnableWindow(TRUE);
		}

	}
	else
	{
		if(pButton!=NULL)
		{
			pButton->EnableWindow(FALSE);
		}
	}







	*pResult = 0;
}

void CPageDetails::OnBnClickedBtnEndProcess()
{
	OnPop_EndProcess();
}

//---------------

void CPageDetails::_SetSelItemPriority(DWORD Priority)
{

	//ABOVE_NORMAL_PRIORITY_CLASS(0x00008000) 高于标准
	//BELOW_NORMAL_PRIORITY_CLASS(0x00004000) 低于标准
	//HIGH_PRIORITY_CLASS(0x00000080) 高
	//IDLE_PRIORITY_CLASS(0x00000040) 低
	//NORMAL_PRIORITY_CLASS(0x00000020) 标准
	//REALTIME_PRIORITY_CLASS(0x00000100) 实时



	PROCLISTDATA * pData = NULL;
	pData = _GetSelectionItemData();

	if(pData==NULL)return;
	if(pData->hProcess==NULL)return;	


	if(theApp.Global_ShowOperateTip(pData->Name,STR_SETPRIORITY_MAINTIP,STR_SETPRIORITY_CONTENT,STR_SETPRIORITY_BTN)==IDOK)
	{
		HANDLE hProcess = OpenProcess( PROCESS_SET_INFORMATION ,FALSE, pData->PID );  //注意 PPROCESS_SET_INFORMATION 要条优先级 要这个权限
		BOOL Ret=SetPriorityClass( hProcess, Priority );
		CloseHandle(hProcess);
		if(!Ret)
		{
			CString StrLoad,StrCaption,StrContent;
			StrLoad.LoadString(STR_SETPRIORITY_ERRORTIP);
			AfxExtractSubString(StrCaption,StrLoad,0,L';');
			AfxExtractSubString(StrContent,StrLoad,1,L';');
			MessageBox(StrContent,StrCaption,MB_OK|MB_ICONSTOP);
		}


	}
}

//---------------

void CPageDetails::OnPop_SetPriorityRealtime()
{

	_SetSelItemPriority(REALTIME_PRIORITY_CLASS);
}

void CPageDetails::OnPop_SetPriorityLow()
{
	_SetSelItemPriority(IDLE_PRIORITY_CLASS);
}


void CPageDetails::OnPop_SetPriorityNormal()
{
	_SetSelItemPriority(NORMAL_PRIORITY_CLASS);
}

void CPageDetails::OnPop_SetPriorityHigh()
{
	_SetSelItemPriority(HIGH_PRIORITY_CLASS);
}

void CPageDetails::OnPop_SetPriorityAboveNormal()
{
	_SetSelItemPriority(ABOVE_NORMAL_PRIORITY_CLASS);
}

void CPageDetails::OnPop_SetPriorityBelowNormal()
{
	_SetSelItemPriority(BELOW_NORMAL_PRIORITY_CLASS);
}

void CPageDetails::OnPop_SetAffinity()
{
	int nSel = mDetailsList.GetNextItem( -1, LVNI_SELECTED );

	CString StrProc = mDetailsList.GetItemText(nSel,DETAILLIST_NAME);

	PROCLISTDATA *pData   = (PROCLISTDATA *)mDetailsList.GetItemData(nSel);

	if(pData == NULL) return ;

	CSetAffinityDlg  Dlg;
	Dlg.StrProcName = StrProc;
	Dlg.hProcess =  pData->hProcess;
	Dlg.PID = pData->PID;


	Dlg.DoModal();
}



void CPageDetails::OnPop_Properties()
{
	PROCLISTDATA * pData = NULL;
	pData = _GetSelectionItemData();

	if(pData == NULL) return ;

	CString StrTargetPath = mProcInfo.GetPathName(pData->hProcess);

	OpenPropertiesDlg(StrTargetPath);


	//----------------------------------------------------------------------------------
	

}

void CPageDetails::GetThreadsConutAllItem(void)
{

 
	PROCESSENTRY32 ProcessEntry;
	//在使用这个结构前，先设置它的大小
	ProcessEntry.dwSize = sizeof(ProcessEntry);
	//给系统内所有的进程拍个快照
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{	
		return ;
	}
	//遍历进程快照，轮流显示每个进程的信息
	BOOL bMore = ::Process32First(hProcessSnap,&ProcessEntry);
	CString Str,StrT;

	map<DWORD,PVOID>::iterator Iter; 

	PROCLISTDATA * pData = NULL;
	while (bMore)
	{		

		Iter= Map_PidToData.find(ProcessEntry.th32ProcessID);			 
		if(Iter != Map_PidToData.end())// 存在 
		{	
			pData = (PROCLISTDATA *)(Iter->second);
			pData->ThreadsCount =  ProcessEntry.cntThreads;

		}

		bMore = ::Process32Next(hProcessSnap,&ProcessEntry);

	}
	//不要忘记清除掉snapshot对象
 
	::CloseHandle(hProcessSnap);



}

void CPageDetails::OnPop_EndProcessTree()
{
	::AfxBeginThread(Thread_EndProcessTree,this);
}

void CPageDetails::_EndProcessTree(void)
{
	int nSel = mDetailsList.GetNextItem( -1, LVNI_SELECTED );

	PROCLISTDATA * pData = NULL ;
	pData = (PROCLISTDATA *)mDetailsList.GetItemData(nSel);

	//----------------

	if(theApp.Global_ShowOperateTip(pData->Name,STR_ENDPROCTREE_MAINTIP,STR_ENDPROCTREE_CONTENT,STR_ENDPROCTREE_BTN)==IDOK)
	{
		DWORD * PidToTerminate = new DWORD [theApp.PerformanceInfo.ProcessCount-1];

		PROCLISTDATA *PSubData = NULL;

		mDetailsList.SetRedraw(0);
		int n=0;	


		for ( map<DWORD,PVOID>::iterator i = Map_PidToData.begin(); i!=Map_PidToData.end();  i++ ) 
		{  
			PSubData = (PROCLISTDATA *) i->second;

			DWORD PPID = PSubData->ParentPID;

			while(1)
			{
				if(PPID==pData->PID||PPID==-1) break;				
				PPID = mProcInfo.GetParentPID(PPID);				 
			}
			 
			if(PPID==pData->PID)
			{
				PidToTerminate[n++] = PSubData->PID;
				 

			}
			/*if(WaitForSingleObject(hProcess,20000)==WAIT_OBJECT_0)
			{
			MSB(0)
			}*/

		}

		for(int i=0;i<n;i++)
		{
			HANDLE hProcess = OpenProcess(  PROCESS_TERMINATE,FALSE,PidToTerminate[i]);  	
			if(hProcess!=NULL)
			{
				TerminateProcess(hProcess, 4);
				//MSB(PidToTerminate[i])
			}
		}


		TerminateProcess(pData->hProcess, 4);

		delete PidToTerminate;
		PidToTerminate = NULL;


		mDetailsList.SetRedraw(1);
	}



}

void CPageDetails::OnPop_HideColumn()
{

	
	mDetailsList.SetRedraw(0);
	LVCOLUMNW  Col;
	Col.mask = LVCF_ORDER |LVCF_SUBITEM ;
	theApp.AppSettings.ColIDS_DetailList[CurrentRightClickCol].ColWidth = 0 ; 
	theApp.AppSettings.ColIDS_DetailList[CurrentRightClickCol].Redraw = 0;
	Col.iOrder=0;
	mDetailsList.SetColumn(CurrentRightClickCol,&Col);
	mDetailsList.SetColumnWidth(CurrentRightClickCol,0 );
	mDetailsList.SetRedraw(1);

	CurrentRightClickCol= -1;
}

void CPageDetails::OnPop_CreateMiniDump()
{

	_CreateDumpForSelectionItem(MiniDumpNormal);	
	

}

PROCLISTDATA * CPageDetails::_GetSelectionItemData(void)
{

	int nSel = mDetailsList.GetNextItem( -1, LVNI_SELECTED );
	PROCLISTDATA * pData = NULL;
	pData = (PROCLISTDATA *)mDetailsList.GetItemData(nSel);

	return pData;
}

void CPageDetails::_CreateDumpForSelectionItem(MINIDUMP_TYPE TYPE)
{
	PROCLISTDATA * pData = NULL;
	pData = _GetSelectionItemData();

	if(pData == NULL) return ;

	CTime CurrentTime = CTime::GetCurrentTime();
	CString StrTime;

	StrTime=CurrentTime.Format(L"(%d-%B-%Y %H%M%S)");

	CString StrName = pData->Name.Left(pData->Name.GetLength()-4) ;
	StrName = StrName+StrTime; 

	CFileDialog FileDlg	(FALSE, L".DMP", StrName, OFN_CREATEPROMPT | OFN_PATHMUSTEXIST , L"Dump File (*.DMP)|*.DMP||");

	if(FileDlg.DoModal() ==IDOK)
	{
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ ,FALSE,pData->PID);
		if(hProcess!=NULL)
		{
			HANDLE  hDumpFile = CreateFile(FileDlg.GetPathName(), GENERIC_WRITE|GENERIC_READ,FILE_SHARE_WRITE|FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			 
			BOOL Ret = MiniDumpWriteDump(hProcess, pData->PID,hDumpFile, TYPE, NULL, NULL, NULL);
			
			::CloseHandle(hDumpFile);
			::CloseHandle(hProcess);
		}
		else
		{
			MessageBox(L"The operation could not be completed!\n Access is denied",L"Dump Process",MB_OK|MB_ICONSTOP);
			
		}

	}

}

void CPageDetails::OnPop_CreateFullDump()
{
	_CreateDumpForSelectionItem((MINIDUMP_TYPE)(MiniDumpWithFullMemory|MiniDumpWithHandleData));	
}

void CPageDetails::OnPop_Searchonline()
{
	PROCLISTDATA * pData = NULL;
	pData = _GetSelectionItemData();

	if(pData == NULL) return ;
	SearchOnline(pData->Name);
}


void CPageDetails::OnNMDblclkDetailslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);


 

	PROCLISTDATA * pData = NULL;
	pData = (PROCLISTDATA*)mDetailsList.GetItemData(pNMItemActivate->iItem);

	if(pData!=NULL)
	{		
	    ::AfxBeginThread(Thread_ShowProperties,pData);
	}


	


	*pResult = 0;
}

void CPageDetails::OnLvnKeydownDetailslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(pLVKeyDow->wVKey ==VK_DELETE)		
	{
		OnPop_EndProcess();
	}
	*pResult = 0;
}

void CPageDetails::OnFileSaveList()
{

	CString StrDefaultFileName;

	StrDefaultFileName = L"DBCTaskman_";

	CString str;//获取系统时间
	
	CTime tm;
	tm=CTime::GetCurrentTime();
	str=tm.Format("(%Y%m%d-%X)");
	str.Remove(L':');
	StrDefaultFileName += str;




	CFileDialog FileDlg	(FALSE, L".Html", StrDefaultFileName, OFN_CREATEPROMPT | OFN_PATHMUSTEXIST , L"HTML File (*.Html)|*.html|Text File (*.TXT)|*.TXT||");
	

	if(FileDlg.DoModal() ==IDOK)
	{
		Refresh();

		 
		if(FileDlg.GetFileExt().CompareNoCase(L"html")==0)
		{
			SaveHtmlFile(FileDlg.GetPathName());
		}
		else
		{
			SaveTxtFile(FileDlg.GetPathName());
		}
	}



}

void CPageDetails::SaveHtmlFile(CString StrFileName)
{
	CString StrTime;//获取系统时间
	CTime tm;
	tm=CTime::GetCurrentTime();
	StrTime=tm.Format("(%Y-%m-%d %X)");
	
//-----------------------------

	CStdioFile ListFile;


	ListFile.Open(StrFileName,CFile::modeCreate|CFile::modeWrite); 
	CString StrTemp,StrItem,StrLine,StrOut;


	//   gb18030     /    utf-8
		
	CString StrTemplate,StrCharCode;

	StrCharCode.LoadString(STR_CHARCODE);


	StrTemplate=L"<!doctype html><html><head><meta charset=\"%s\">\
			 <title>DBC Task Manager</title>\
			 <style type=\"text/css\">\
			 body,td,th {font-size: 13px;}\
			 .table-c table{border-right:1px solid #555;border-bottom:1px solid #555}\
			 .table-c table td{border-left:1px solid #555;border-top:1px solid #555}\
			 </style>\
			 </head>\n\
			 <body>\
			 <p align=\"center\" style=\"font-size: 36px\">DBC Task Manager</p>\n\
			 <p align=\"center\" style=\"font-size: 15px\">"+StrTime+L"</p>\n\
			 <div class=\"table-c\">\
			 <table  border=0  align=\"center\" cellpadding=\"5\" cellspacing=\"0\">";
  

	StrTemp.Format(StrTemplate,StrCharCode);






	int ItemCount = mDetailsList.GetItemCount();


	CHeaderCtrl * pHeader = mDetailsList.GetHeaderCtrl();
	int nCol=0;

	if(pHeader==NULL)  
	{
		ListFile.Close(); 
		return ;
	}

	
	nCol = pHeader->GetItemCount();
	StrLine =StrTemp+L"<tr style=\"background-color: #666;color: #fff;\">\n";
	for(int i=0;i<nCol;i++)
	{
		if(mDetailsList.pColStatusArray[i].ColWidth>0) 
		{
			LVCOLUMN   lvColumn;
			TCHAR strChar[128];
			lvColumn.pszText=strChar; 
			lvColumn.cchTextMax=128; 
			lvColumn.mask   = LVCF_TEXT;
			mDetailsList.GetColumn(i,&lvColumn);
			StrItem=lvColumn.pszText;


			
			StrLine =StrLine+L"<td>"+StrItem+L"</td>";

		}
	}
	StrLine+=L"</tr>";


	StrOut=StrLine;

	for(int i=0;i<ItemCount;i++)
	{
		
		StrLine=L"<tr>\n";
		for(int iCol=0;iCol<nCol;iCol++)
		{
			if(mDetailsList.pColStatusArray[iCol].ColWidth>0) 
			{
				
				StrItem=mDetailsList.GetItemText(i,iCol);					

				StrLine =StrLine+L"<td>"+StrItem+L"</td>";

			}

		}

		StrOut = StrOut+StrLine+L"\n</tr>\n";

	}



	StrOut=StrOut+L"</table></div>\n<p>&nbsp;</p><p>&nbsp;</p></body>\n</html>";



	char* old_locale;
	if(theApp.IsChineseEdition)//中文
	{
		old_locale=_strdup(setlocale(LC_CTYPE,NULL));
		setlocale( LC_CTYPE, ("chs"));
	}
//----------------
	

	StrOut.Format(L"%s\r\n",StrOut);
	ListFile.Seek(0,CFile::end);
	ListFile.WriteString( StrOut ); 

	//关闭文件 
	ListFile.Close(); 
//----------------

	if(theApp.IsChineseEdition)//中文
	{
		setlocale(LC_CTYPE,old_locale);
		free(old_locale);
	}


}

void CPageDetails::SaveTxtFile(CString StrFileName)
{

	CStdioFile ListFile;
	ListFile.Open(StrFileName,CFile::modeCreate|CFile::modeWrite); 

	CString StrTemp,StrItem,StrLine,StrOut;

	CHeaderCtrl * pHeader = mDetailsList.GetHeaderCtrl();
	int nCol=0;
	if(pHeader==NULL)  
	{
		ListFile.Close(); 
		return ;
	}

	
	nCol = pHeader->GetItemCount();

	for(int i=0;i<nCol;i++)
	{
		if(mDetailsList.pColStatusArray[i].ColWidth>0) 
		{
			LVCOLUMN   lvColumn;
			TCHAR strChar[128];
			lvColumn.pszText=strChar;	lvColumn.cchTextMax=128; 
			lvColumn.mask   = LVCF_TEXT;
			mDetailsList.GetColumn(i,&lvColumn);
			StrItem=lvColumn.pszText;		
			StrTemp.Format(L"%-60s",StrItem);//转为同样长度字符串前面补空格
			StrLine =StrLine+StrTemp;

		}
	}




	StrOut=StrLine+L"\n\n";
	int ItemCount = mDetailsList.GetItemCount();

	for(int i=0;i<ItemCount;i++)
	{
		_FillAllDataOfItem(i);
		StrLine=L"";
		for(int x=0;x<nCol;x++)
		{
			if(mDetailsList.pColStatusArray[x].ColWidth>0) 
			{
				StrItem=mDetailsList.GetItemText(i,x);
				if(StrItem==L"")StrItem =L"----------";
				StrTemp.Format(L"%-60s",StrItem);//转为同样长度字符串前面补空格			
				StrLine =StrLine+StrTemp;
			}

		}

		StrOut=StrOut+StrLine+L"\n";

	}




	char* old_locale;
	if(theApp.IsChineseEdition)//中文
	{
		old_locale=_strdup(setlocale(LC_CTYPE,NULL));
		setlocale( LC_CTYPE, ("chs"));
	}


 

	StrOut.Format(L"%s\r\n",StrOut);



	ListFile.Seek(0,CFile::end);
	ListFile.WriteString( StrOut ); 

		
	//关闭文件 
	ListFile.Close(); 


	if(theApp.IsChineseEdition)//中文
	{
		setlocale(LC_CTYPE,old_locale);
		free(old_locale);
	}


}

void CPageDetails::ClearSelecet(void)
{
	//由于允许多选 所以要先清除之前选中的 

	if(mDetailsList.GetSelectedCount()>0)
	{
		POSITION SelPos = mDetailsList.GetFirstSelectedItemPosition();		
		while (SelPos)
		{
			int iItem = mDetailsList.GetNextSelectedItem(SelPos);
			mDetailsList.SetItemState(iItem,NULL,LVIS_SELECTED|LVIS_FOCUSED);//清除选中 
		}
	}
}

