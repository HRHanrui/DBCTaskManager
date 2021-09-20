// ProcessesView.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "ProcessesView.h"
#include "DetailsView.h"







//  ==========================================    最初生成列表 ===================================

static UINT Thread_FillAllItemsData(LPVOID pParam);
static UINT Thread_GenProcesses(LPVOID pParam)
{

	CPageProcesses  *Dlg=(CPageProcesses *)pParam;
	if(Dlg == NULL) return -1; //

	Dlg->PreListItems();	 	 

	AfxEndThread(0,TRUE);
	return 0;

}




///---------------- 完善其他数据   -------------

UINT Thread_FillAllItemsData(LPVOID pParam)
{

	CPageProcesses  *Dlg=(CPageProcesses *)pParam;
	if(Dlg == NULL) return -1; //

	Dlg->FillAllItemData();	 	 
	Dlg->FlagEnableRefresh = TRUE;
	AfxEndThread(0,TRUE);
	return 0;

}



static UINT Thread_GetItemName(LPVOID pParam)
{

	PROCLISTDATA  *Data=(PROCLISTDATA *)pParam;
	if(Data== NULL) return -1; //

	//Dlg->PreListItems();	 	 
	//Dlg->mTaskList.Invalidate();
	CProcess mProcInfo;
	CString StrDescription=mProcInfo.GetVerInfoString(mProcInfo.GetPathName(Data->hProcess),L"FileDescription");
	Data->Description=StrDescription;

	if(StrDescription == L"")
		Data->Description =Data->Name;

	
	AfxEndThread(0,TRUE);

	return 0;

}














//-------------------------------------------------------------------------

typedef struct tagWNDINFO
{
	DWORD dwProcessId;
	HWND hWnd;
	HWND Dll_hwnd;
} WNDINFO, *LPWNDINFO;




//  ==========================================     排序  ===================================







//排序   升降 续 由 类中 SortType  变量控制
int CALLBACK Sort_Processes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)  
{
	CCoolListCtrl* pList= &( ((CPageProcesses*)theApp.pSelPage)->mTaskList);

	int iCol =(int) lParamSort;
	int result = 0;     //返回值   


	APPLISTDATA * pData1 = NULL;
	APPLISTDATA * pData2 = NULL;

	pData1 = (APPLISTDATA * )lParam1;
	pData2 = (APPLISTDATA * )lParam2;

	//	int Data1,Data2;
	if((pData1==NULL) || (pData2 == NULL) )return result;

	CString Str1,Str2;

	Str1= pList->GetItemText(pData1->SortID,iCol);
	Str2= pList->GetItemText(pData2->SortID,iCol);









	switch(iCol)
	{
	case PROCLIST_TYPE:
	case PROCLIST_STATUS:
	case PROCLIST_PUB:
	case PROCLIST_PNAME:
	case PROCLIST_CMDLINE:

		//所有列都要先按 pid排列 否则但子项展开后排序子项与父项有可能错开！！！！


		result =lstrcmp(Str1,Str2);
		if(result==0)
		{
				int PID1,PID2;
				if(pData1->pPData == NULL ) PID1 =  0 ; else PID1 = ((PROCLISTDATA *) pData1->pPData)->PID;
				if(pData2->pPData == NULL ) PID2 =  0 ; else PID2 = ((PROCLISTDATA *) pData2->pPData)->PID;
				result=PID1-PID2;
		}		
		break;
	case PROCLIST_NAME: //第一列特殊处理

		if(theApp.AppSettings.GroupByType) //分组显示
			result = pData1->ItemType - pData2->ItemType;
		else//不分组显示时忽略
			result = 0; 

		//------------------------------

		if(result != 0)
		{
			goto SORTOK;
		}
		else //相同时 按描述排序 //包含主窗口的 排序 窗口项 PID都相同所以先按PID 分组然后 在按 SUBType 由于展开的 子项subtype 是1 而父项是0属于会排列正确！！！
		{
			CString  StrDescription1=L"";
			CString  StrDescription2=L"";


			if(pData1->pPData != NULL )
			{


				StrDescription1 = ((PROCLISTDATA *) pData1->pPData)->Description;//Xxxxxxxxx
				if(StrDescription1==L"") StrDescription1=  ((PROCLISTDATA *) pData1->pPData)->Name;

			}

			if(pData2->pPData != NULL )
			{	

				 
				StringCchCopy((LPTSTR)(LPCTSTR)StrDescription2,((PROCLISTDATA *) pData2->pPData)->Description.GetLength(),((PROCLISTDATA *) pData2->pPData)->Description);
				//StrDescription2 = ((PROCLISTDATA *) pData2->pPData)->Description;
				if(StrDescription2==L"") StrDescription2=  ((PROCLISTDATA *) pData2->pPData)->Name;

			}



			result=lstrcmp(StrDescription1,StrDescription2);
			if(result != 0)
			{
				goto SORTOK;
			}
			else //如果 进程名相同则先按PID区分
			{
				int PID1,PID2;
				if(pData1->pPData == NULL ) PID1 =  0 ; else PID1 = ((PROCLISTDATA *) pData1->pPData)->PID;
				if(pData2->pPData == NULL ) PID2 =  0 ; else PID2 = ((PROCLISTDATA *) pData2->pPData)->PID;
				result=PID1-PID2; 				
				goto SORTOK;
			}	 
		}


		break;

	default:


		



		CString StrL1,StrL2;
		StrL1.Format(L"%16s",Str1);//转为同样长度字符串前面补空格
		StrL2.Format(L"%16s",Str2);//转为同样长度字符串前面补空格

		result =lstrcmp(StrL1,StrL2);
		if(result==0)//所有列都要按 pid排列 否则但子项展开后排序子项与父项有可能错开！！！！
		{
			int PID1,PID2;
			if(pData1->pPData == NULL ) PID1 =  0 ; else PID1 = ((PROCLISTDATA *) pData1->pPData)->PID;
			if(pData2->pPData == NULL ) PID2 =  0 ; else PID2 = ((PROCLISTDATA *) pData2->pPData)->PID;
			result=PID1-PID2;
		}	


	}



SORTOK:
	if(result == 0)
	{							
		result = (pData1->SubType  - pData2->SubType);  //子项SubType 值最大！

		if(result != 0)
		{
			return result;   //不要再次反转了
		}
		else  //最后 按窗口标题区分 父项窗口标题为空
		{
			result =  lstrcmp(pData1->StrWnd,pData2->StrWnd);
			//颠倒排序结果
			if( pList->FlagSortUp == FALSE)
			{
				result = -result;
			}

			return result; 
		}
	}




	//颠倒排序结果
	if( pList->FlagSortUp == FALSE)
	{
		result = -result;

	}



	return result;  


}




//----------------------------------------------------------------------------------------------------














//======================================================================================================================
















//======================================================================================================================



// CPageProcesses

IMPLEMENT_DYNCREATE(CPageProcesses, CFormView)

CPageProcesses::CPageProcesses()
: CFormView(CPageProcesses::IDD)
,nApp(0)
,nBkg(0)
,nWin(0)
, StrWndList(_T(""))
, FlagEnableRefresh(FALSE)
, pPageDetails(NULL)
, pPerformanceMon(NULL)
, CoolheaderCtrl(NULL)
, SvchostIconIndex(0)
{

	nApp = nBkg = nWin =0;
}

CPageProcesses::~CPageProcesses()
{
}

void CPageProcesses::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCESSLIST, mTaskList);
}

BEGIN_MESSAGE_MAP(CPageProcesses, CFormView)

	ON_WM_SIZE()
	ON_MESSAGE(UM_TIMER,OnUMTimer)
	ON_WM_CTLCOLOR()
	ON_WM_INITMENUPOPUP()
	ON_NOTIFY(NM_RCLICK, IDC_PROCESSLIST, &CPageProcesses::OnNMRClickProcesslist)
	ON_COMMAND(ID_PROCESSESLIST_EXPAND, &CPageProcesses::OnPop_ProcesseslistExpand)
	ON_COMMAND(ID_PROCESSESLIST_ENDTASK, &CPageProcesses::OnPop_ProcesseslistEndtask)
	ON_COMMAND(ID_PROCESSESLIST_GOTODETAILS, &CPageProcesses::OnPop_ProcesseslistGotoDetails)
	ON_BN_CLICKED(IDC_BTN_ENDTASK, &CPageProcesses::OnBnClickedBtnEndTask)

	ON_NOTIFY(LVN_KEYDOWN, IDC_PROCESSLIST, &CPageProcesses::OnLvnKeydownProcesslist)

	ON_COMMAND(ID_PROCESSESLIST_OPENFILELOCATION, &CPageProcesses::OnPop_OpenFileLocation)
	ON_COMMAND(ID_PROCESSESLIST_PROPERTIES, &CPageProcesses::OnPop_Properties)
	ON_COMMAND(ID_PROCLISTTASKITEM_SWITCHTO, &CPageProcesses::OnPop_SwitchTo)
	ON_COMMAND(ID_PROCLISTTASKITEM_BRINGTOFRONT, &CPageProcesses::OnPop_Bringtofront)
	ON_COMMAND(ID_PROCLISTTASKITEM_MAXIMIZE, &CPageProcesses::OnPop_Maximize)
	ON_COMMAND(ID_PROCLISTTASKITEM_MINIMIZE, &CPageProcesses::OnPop_Minimize)
	ON_COMMAND(ID_PROCLISTTASKITEM_ENDTASK, &CPageProcesses::OnPop_Endtask)
	ON_COMMAND(ID_MEMORY_PERCENTS_PLIST, &CPageProcesses::OnPop_Memory_ShowAsPercents)
	ON_COMMAND(ID_MEMORY_VALUES_PLIST, &CPageProcesses::OnPop_Memory_ShowAsValues)
	ON_COMMAND(ID_PROCESSESLIST_SEARCHONLINE, &CPageProcesses::OnPop_Searchonline)
END_MESSAGE_MAP()


// CPageProcesses diagnostics

#ifdef _DEBUG
void CPageProcesses::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPageProcesses::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPageProcesses message handlers

void CPageProcesses::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here


	PlaceAllCtrl();

	mTaskList._GetRedrawColumn();////重要！！必须马上更新否则显示跟不上



}

void CPageProcesses::InitList(void)
{ 
	// 如果想自会这个效果 就不能再设置了 ！！！
	//SetWindowTheme(mTaskList.GetSafeHwnd(),L"explorer", NULL);


	mTaskList.SetExtendedStyle(mTaskList.GetExtendedStyle()|LVS_EX_FULLROWSELECT| LVS_OWNERDRAWFIXED|LVS_EX_DOUBLEBUFFER |LVS_EX_HEADERDRAGDROP );  //| LVS_EX_GRIDLINES |LVS_EX_CHECKBOXES

	mTaskList.ModifyStyle (0, LVS_SHAREIMAGELISTS, 0);
	mTaskList.FullColumnCount= 11; //这个是全部的列 数

	if(!theApp.FlagThemeActive)
	{
		mTaskList.ModifyStyleEx(WS_EX_CLIENTEDGE,0);
		mTaskList.ModifyStyle(0,WS_BORDER);
	}

	//---------------------------------------------------------------------

	mTaskList.pColStatusArray = COL_SAT_PROC;//记录列隐现的数组


	mTaskList.InitAllColumn(COL_SAT_PROC,STR_COLUMN_PROCESS,COL_COUNT_PROC);//11 是全部的列数



	//-------------------------------------------------------------------

	SvchostIconIndex = theApp.mImagelist.GetImageCount()-1;





	mTaskList.SetImageList(theApp.mImagelist.m_hImageList);  //决定了可以自绘表头 必须放在InsertColumn之后!!!!!!!!!


	//--------------------------------------------------------


	//ListProcesses(); //此函数转入独立线程中 以提高启动速度！！！





}

void CPageProcesses::OnLvnGetdispinfoProcesslist(NMHDR *pNMHDR, LRESULT *pResult)
{
}
 
LRESULT CPageProcesses::OnUMTimer( WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default



	if(!FlagEnableRefresh) return 0; 

	//mTaskList.XXX=0;



	int nCount = mTaskList.GetItemCount();
	APPLISTDATA * pData = (APPLISTDATA *)mTaskList.GetItemData(nCount-1);
	if(pData->SubType == -1) //最后一项如果是分组标题则说明是异常状态
	{
		


		mTaskList.CurrentSortColumn = PROCLIST_NAME ;
		Sort(0,FALSE);
	}


	BOOL  UpdateWndList = CheckWndChange();

	CRect rcItem,rc,rcTemp;

	rc = mTaskList._GetRedrawColumn();


	CString StrItem;
	CProcess mProcInfo;


	int i= 0;


	int NewAppCount,NewBkgPrcCount,NewWinPrcCount;

	NewAppCount = NewBkgPrcCount = NewWinPrcCount = 0;





	while(1)
	{
		if(mTaskList.GetItemCount() == i) break ;

		mTaskList.GetItemRect(i,rcItem,LVIR_BOUNDS);
		BOOL RedrawItem =  IntersectRect (rcTemp,rcItem,rc);


		APPLISTDATA *pListData = (APPLISTDATA *)mTaskList.GetItemData(i);

	 

		if( (pListData == NULL) || (pListData->pPData == NULL ))  //注意排除分组标题
		{ 		

			i++; continue ;  
		} 


		if(  pListData->SubType == SUB_ITEM)    //注意排除子项  子项 只需更新窗口标题 //和状态
		{


			if( RedrawItem && COL_SAT_PROC[PROCLIST_NAME].Redraw   )
			{

				APPLISTDATA *pAboveData = (APPLISTDATA *)mTaskList.GetItemData(i-1);

				if(pAboveData!=NULL)
				{
					pListData->CoolUsageArray[PROCLIST_CPU] = pAboveData->CoolUsageArray[PROCLIST_CPU];
					pListData->CoolUsageArray[PROCLIST_MEMORY] = pAboveData->CoolUsageArray[PROCLIST_MEMORY];
					pListData->CoolUsageArray[PROCLIST_DISK] = pAboveData->CoolUsageArray[PROCLIST_DISK];
					pListData->CoolUsageArray[PROCLIST_NETWORK] = pAboveData->CoolUsageArray[PROCLIST_NETWORK];
				}


				::GetWindowText(pListData->hMainWnd,pListData->StrWnd.GetBuffer(MAX_PATH),MAX_PATH);  
				pListData->StrWnd.ReleaseBuffer();	

				//CString StrTemp;
		
				//StrTemp=mTaskList.GetItemText(i,PROCLIST_NAME);
				//if(strcmp(StrTemp,pListData->StrWnd
				mTaskList.MySetItemText(i,PROCLIST_NAME,pListData->StrWnd);
			}

			i++;
			continue ; 

		}



		//----------------以上排除标题和子项-------------



		if( pListData->ItemType == APP  )
		{
			NewAppCount ++ ;
		}
		else if( pListData->ItemType == BKGPROC  )
		{
			NewBkgPrcCount++;
		}
		else if( pListData->ItemType == WINPROC  )
		{
			NewWinPrcCount++;
		}






		if( (pListData->ItemType != WINPROC)  &&  UpdateWndList) //UpdateWndList标志着 窗口列表有变化
		{

			ListItemWindows(i,pListData);	
		} 

		//以上为无论是否绘制都必须做的

		if( !RedrawItem ) {i++;continue ; } 



		//---------------------CPU---------------------------------


		if(COL_SAT_PROC[PROCLIST_CPU].Redraw)
		{			



			mProcInfo.GetCpuUsage( (PROCLISTDATA*)pListData->pPData );
			double Usage = ( (PROCLISTDATA*)pListData->pPData )->CPU_Usage;

			if(Usage>=0 && Usage<=100) //防止显示错乱的 数据
			{
				StrItem.Format(L"%0.2f%%",Usage );
				pListData->CoolUsageArray[PROCLIST_CPU]= Usage/100; //用于变色显示！！！
				mTaskList.MySetItemText(i,PROCLIST_CPU,StrItem);
			}


		}


		//---------------------Memory---------------------------------

		if(COL_SAT_PROC[PROCLIST_MEMORY].Redraw)
		{			 

			double OldWorkingSet = ((PROCLISTDATA*)pListData->pPData )->Mem_WorkingSet;
			mProcInfo.GetMemUsageInfo((PROCLISTDATA*)pListData->pPData);
			double WSDelta = ((PROCLISTDATA*)pListData->pPData )->Mem_WorkingSet - OldWorkingSet;

			if(WSDelta!=0)
			{
				_GetMemDataAndSetItemText(i,(APPLISTDATA *)pListData  );
			}

		}

		//---------------------disk---------------------------------



		double OtherBytePerSec;
		double DiskUsageBytePerSec;
		double DiskUsageMBPerSec;
		ULONGLONG NewOtherIO;
		ULONGLONG NewIO = mProcInfo.GetDiskIO(((PROCLISTDATA*)pListData->pPData)->hProcess,&NewOtherIO);
		DiskUsageBytePerSec =  ((double) (NewIO - ((PROCLISTDATA*)pListData->pPData)->DiskIO ))/theApp.AppSettings.TimerStep;
		OtherBytePerSec  =   ((double) (NewOtherIO - ((PROCLISTDATA*)pListData->pPData)->OtherIO ))/theApp.AppSettings.TimerStep;
		((PROCLISTDATA*)pListData->pPData)->OtherIO = NewOtherIO;


		if(COL_SAT_PROC[PROCLIST_DISK].Redraw)
		{
			DiskUsageMBPerSec = ((double)(NewIO-((PROCLISTDATA*)pListData->pPData)->DiskIO))/1048576/theApp.AppSettings.TimerStep ;

			if(DiskUsageMBPerSec>=0 )//防止显示错乱的 数据
			{

				StrItem.Format(L"%.2f MB/s",DiskUsageMBPerSec);    //  除以1024/1024/0.5
				mTaskList.MySetItemText(i,PROCLIST_DISK,StrItem);
				((PROCLISTDATA*)pListData->pPData)->DiskIO = NewIO;
				
			}			

		}



		//-------------- Network-----------------

		if(COL_SAT_PROC[PROCLIST_NETWORK].Redraw)
		{
			double NetUsage;
			
			//double IOUsage = mProcInfo.GetIOUsage(NULL,((PROCLISTDATA*)pListData->pPData)->hQueryIO,((PROCLISTDATA*)pListData->pPData)->hCounterIO);
			//NetUsage = IOUsage-DiskUsageBytePerSec-OtherBytePerSec;


			IO_COUNTERS  IOCounter;
			GetProcessIoCounters(((PROCLISTDATA*)pListData->pPData)->hProcess,&IOCounter);		
			ULONGLONG CurrentIO = IOCounter.ReadTransferCount+IOCounter.WriteTransferCount - pListData->IOLast;
			NetUsage =(double)((CurrentIO/theApp.AppSettings.TimerStep )-DiskUsageBytePerSec)*0.9;
			pListData->IOLast = IOCounter.ReadTransferCount+IOCounter.WriteTransferCount;

			

			if(NetUsage<0.001) NetUsage = 0;  //防止显示错乱的 数据


			CString StrOut;
			StrItem.Format(L"%0.2f",  NetUsage/1024 );

			

			StrItem=StrItem+L" KB/s";
			mTaskList.MySetItemText(i,PROCLIST_NETWORK,StrItem);



		}


		//-------------------所有更新完成----------------------


		i++; //注意不可删除！！！


	}

 
	

	if(UpdateWndList)ReSort();  //timer update





	CString StrColheaderTemp;
	int FlagRedrawHeacerCtrl = 0;
	if(COL_SAT_PROC[PROCLIST_CPU].Redraw)
	{
		StrColheaderTemp.Format(L"%.0f%%",theApp.PerformanceInfo.CpuUsage);
		StringCchCopy(COL_SAT_PROC[PROCLIST_CPU].StrItem,5,StrColheaderTemp);
		COL_SAT_PROC[PROCLIST_CPU].Percents = (float)theApp.PerformanceInfo.CpuUsage;
		FlagRedrawHeacerCtrl++;
	}

	if(COL_SAT_PROC[PROCLIST_MEMORY].Redraw)
	{
		StrColheaderTemp.Format(L"%d%%",(int)theApp.PerformanceInfo.MemoryUsage);
		StringCchCopy(COL_SAT_PROC[PROCLIST_MEMORY].StrItem,5,StrColheaderTemp);
		COL_SAT_PROC[PROCLIST_MEMORY].Percents = (float)theApp.PerformanceInfo.MemoryUsage;
		FlagRedrawHeacerCtrl++;
	}

	if(COL_SAT_PROC[PROCLIST_DISK].Redraw)
	{
		//double UsePercents = pPerformanceMon->GetTotalDiskIO();
		StrColheaderTemp.Format(L"%.0f%%", (float)theApp.PerformanceInfo.TotalDiskUsage   );

		StringCchCopy(COL_SAT_PROC[PROCLIST_DISK].StrItem,5,StrColheaderTemp);
		COL_SAT_PROC[PROCLIST_DISK].Percents = (float)theApp.PerformanceInfo.TotalDiskUsage ;
		FlagRedrawHeacerCtrl++;
	}

	if(COL_SAT_PROC[PROCLIST_NETWORK].Redraw)
	{
		StrColheaderTemp.Format(L"%.0f%%",theApp.PerformanceInfo.TotalNetUsage );
		StringCchCopy(COL_SAT_PROC[PROCLIST_NETWORK].StrItem,5,StrColheaderTemp);
		COL_SAT_PROC[PROCLIST_NETWORK].Percents = (float)theApp.PerformanceInfo.TotalNetUsage;
		FlagRedrawHeacerCtrl++;
	}

	if(FlagRedrawHeacerCtrl!=0)
		this->mTaskList.CoolheaderCtrl.Invalidate(0);





	nApp = NewAppCount   ;
	nBkg = NewBkgPrcCount ;
	nWin =  NewWinPrcCount ;



	UpdateGroupText();






//	mTaskList.XXX=1;
	//	mTaskList.Invalidate();


	return 0;
	

}

HBRUSH CPageProcesses::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{


	return theApp.BkgBrush;
}





void CPageProcesses::_AddGroupItem(void)
{
	mTaskList.SetRedraw(0);


	APPLISTDATA *pNewAppListData = NULL;

	CString StrGroupTitle;

	StrGroupTitle.Format(L"%s (%d)",STR_GROUP_APP,nApp);
		
	mTaskList.InsertItem(0, StrGroupTitle);

	pNewAppListData   =    new  APPLISTDATA  ;
	mTaskList.SetItemData(0,(DWORD_PTR)pNewAppListData);





	pNewAppListData->iImage =-1;
	pNewAppListData->pPData = NULL ;
	pNewAppListData->ItemType = 0;
	pNewAppListData->SubType = -1;

	//i++;


	StrGroupTitle.Format(L"%s(%d)",STR_GROUP_BKG,nBkg);
	mTaskList.InsertItem(0, StrGroupTitle);
	
	pNewAppListData   =    new  APPLISTDATA  ;
	mTaskList.SetItemData(0,(DWORD_PTR)pNewAppListData);

	pNewAppListData->iImage =-1;
	pNewAppListData->pPData = NULL ;
	pNewAppListData->ItemType  = 2;
	pNewAppListData->SubType = -1;
	

	//i++;


	StrGroupTitle.Format(L"%s (%d)",STR_GROUP_WIN,nWin);
	mTaskList.InsertItem(0, StrGroupTitle);
	
	pNewAppListData   =    new  APPLISTDATA  ;
	mTaskList.SetItemData(0,(DWORD_PTR)pNewAppListData);

	pNewAppListData->iImage =-1;
	pNewAppListData->pPData = NULL ;
	pNewAppListData->ItemType   =  4;
	pNewAppListData->SubType = -1;

	mTaskList.SetRedraw(1);

}

BOOL CPageProcesses::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	//	if(pMsg->message == UM_PROCEXIT) 上一级总体处理避免数据删除时机不对造成错误


	if(pMsg->message == UM_PROCSTART)
	{
		mTaskList.SetRedraw(0);


		PROCLISTDATA * pData = (PROCLISTDATA *) pMsg->lParam;

		AddNewItem( pData);
		
		if( pData->Type  == APP )
		{
		
			nApp++;


		}
		else  if( pData->Type == WINPROC )
		{


			nWin++;
		}
		else
		{

			nBkg++;		

		}


		//--------------------及时更新 分组数量信息 -------------------
		UpdateGroupText();




		ReSort(FALSE);

		

		mTaskList.SetRedraw(1);

	}



	//---------------------------------------------------

	if(pMsg->message == UM_BASELISTOK) //详细列表 基本信息 列举完成  可以开始生成第一页的 高级列表
	{
		AfxBeginThread(Thread_GenProcesses,this);

	}

	//---------------------------------------------------

	if(pMsg->message == UM_ALLINFO_OK) //详细列表具有完整信息  可以开始完善 第一页的 高级列表
	{
		AfxBeginThread(Thread_FillAllItemsData,this);

		

	}

	//---------------------------------------------------



	if(pMsg->message ==  UM_DBCLICK_LSIT)
	{
		int nItem = mTaskList.GetNextItem(-1,LVNI_SELECTED);
		APPLISTDATA *pData = NULL;
		pData=(APPLISTDATA * )mTaskList.GetItemData(nItem);
		if(pData!=NULL)
		{
			if(pData->SubType == PARENT_ITEM_CLOSE )
			{

				_OpenSubList(nItem);

			}
			else  if(pData->SubType == PARENT_ITEM_OPEN )
			{
				_CloseSubList(nItem);
			}
		}

	}




	if(pMsg->message == UM_HEADER_LCLICK)
	{
		int n=(int)pMsg->wParam;
		if(FlagEnableRefresh)//更新完毕前不允许点击排序
		{
			Sort(n);
		}

	}

	if(pMsg->message == UM_HEADER_RCLICK)
	{
		CPoint pt;  
		GetCursorPos(&pt);  
		//mTaskList.ScreenToClient(&pt)
		CMenu PopMenu;
		CMenu *pMenu = NULL;
		PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_COLUMN) );
		pMenu = PopMenu.GetSubMenu(0);  //0是 这个 对应的 菜单 和标签顺序对应


		//MSB_S (L"DDDDDD")
		if(pMenu->GetMenuItemID(0) == ID_PROCESSES_TYPE )
		{
			int i=0;

			int nItem = pMenu->GetMenuItemCount();

			if(nItem>3) //保证只对第一级菜单有效
			{
				for(i=1;i<11;i++)
				{
					if(! (COL_SAT_PROC[i].IsHiddenColumn)  ) 
					{
						pMenu->CheckMenuItem(i-1,MF_BYPOSITION|MF_CHECKED);
					}
				}

			}

		}
		 
	
		
		pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y, &(mTaskList.CoolheaderCtrl));
		
		//MSB_S (L"XXXXXX")

	}

	//-----------------
	if(pMsg->message == UM_ITEMCHANGED_COOLLIST)
	{
		_OnListItemChaged();
	}

 




	return CFormView::PreTranslateMessage(pMsg);
}

APPLISTDATA* CPageProcesses::AddNewItem(PROCLISTDATA * pDetailData, int ID,BOOL SetBaseInfoOnly)
{

	//不必再 判断、设定 每项都 分类 只需按分类 分组即可！！！！！

	//———————————————————————————————————————————————————



	APPLISTDATA * pNewAppListData;
	pNewAppListData   =    new  APPLISTDATA  ;

	CString  StrWndCaption;
	CString StrItem;


	//一定要在最前面！ 否则InsertItem时候效果已经显示
	pNewAppListData->CoolUsageArray[PROCLIST_CPU]=pNewAppListData->CoolUsageArray[PROCLIST_MEMORY]=pNewAppListData->CoolUsageArray[PROCLIST_DISK]=pNewAppListData->CoolUsageArray[PROCLIST_NETWORK]=0;


	StrItem=pDetailData->Description;

	StrItem.Remove(L' ');

	if(StrItem.Compare(L"")==0)//描述为空用进程名代替
	{
		pNewAppListData->StrTitle = pDetailData->Name.Left(pDetailData->Name.GetLength()-4);
	}
	else
	{
		pNewAppListData->StrTitle = pDetailData->Description;
	}


	mTaskList.SetRedraw(0);

	mTaskList.InsertItem(ID,StrItem);



	pNewAppListData->pPData = pDetailData ;
	pNewAppListData->iImage = pDetailData->IconIndex ;
	mTaskList.SetItemData(ID,(DWORD_PTR)pNewAppListData);	


	IO_COUNTERS  IOCounter;
	GetProcessIoCounters(pDetailData->hProcess,&IOCounter);	
	pNewAppListData->IOLast = IOCounter.ReadTransferCount+IOCounter.WriteTransferCount;




	pNewAppListData->ItemType = pDetailData->Type;
	pNewAppListData->SubType = PARENT_ITEM_NOSUB;    //PARENT_ITEM_NOSUB
	pNewAppListData->StrWnd  =L"";
	pNewAppListData->nSubItem = 0;


	pNewAppListData->CoolUsageArray[PROCLIST_MEMORY] = 0.001;



	if( pDetailData->Type  == APP )
	{
		//::GetWindowTextW(pDetailData->hMainWnd ,StrWndCaption.GetBuffer(MAX_PATH),MAX_PATH);						
		//StrWndCaption.ReleaseBuffer();
		//pDetailData->WindowCaption = StrWndCaption;
	

		mTaskList.SetItemText(ID,PROCLIST_TYPE,L"App");
		pNewAppListData->SubType = PARENT_ITEM_CLOSE; 

	    // nApp++;


	}
	else  if( pDetailData->Type == WINPROC )
	{
		
		mTaskList.SetItemText(ID,PROCLIST_TYPE,STR_GROUP_WIN);
		//nWin++;
	}

	else
	{
		 
		mTaskList.SetItemText(ID,PROCLIST_TYPE,STR_GROUP_BKG);

		// nBkg++;

		//if(pNewAppListData->SubType = PARENT_ITEM_CLOSE; )
		//pNewAppListData->SubType = PARENT_ITEM_CLOSE; 


	}
	CString StrDescription=mProcInfo.GetVerInfoString(mProcInfo.GetPathName(pDetailData->hProcess),L"FileDescription");
	pDetailData->Description=StrDescription;
	if(StrDescription==L"") StrDescription = pDetailData->Name;
	mTaskList.SetItemText(ID,PROCLIST_NAME, StrDescription );





	//--------------------------------  Process name ---------------------------


	mTaskList.SetItemText(ID,PROCLIST_PNAME, pDetailData->Name );
	//------------------------------------   PID   --------------------------------------


	StrItem.Format(L"%d",pDetailData->PID);
	mTaskList.SetItemText(ID,PROCLIST_PID, StrItem );


	if(SetBaseInfoOnly)
	{
		//写入初始数据 为了视觉效果 不停顿
		mTaskList.SetItemText(ID,PROCLIST_CPU,L"0.00%");
		if(theApp.AppSettings.ProcList_MemPercents)
		{
			mTaskList.SetItemText(ID,PROCLIST_MEMORY,L"0%");
		}
		else
		{
			mTaskList.SetItemText(ID,PROCLIST_MEMORY,L"0.0 MB");
		}
		mTaskList.SetItemText(ID,PROCLIST_DISK,L"0.00 MB/s");
		mTaskList.SetItemText(ID,PROCLIST_NETWORK,L"0.00 KB/s");

		return pNewAppListData;
	}




	//---------------------------------    Publisher   ------------------------------
	StrItem = mProcInfo.GetVerInfoString(mProcInfo.GetPathName(pDetailData->hProcess),L"CompanyName") ;
	mTaskList.SetItemText(ID,PROCLIST_PUB,StrItem );

	//--------------------------------    command line    ----------------------------------

	mTaskList.SetItemText(ID,PROCLIST_CMDLINE, mProcInfo.GetProcCommandLine(pDetailData->PID) );

	//-----------------------------------CPU-----------------------------------

	mTaskList.SetItemText(ID,PROCLIST_CPU,L"0.00%");



	//---------------------------------内存------------------------

	_GetMemDataAndSetItemText(ID,(APPLISTDATA*)pNewAppListData );
	
	/*if(MemUsage>1.0)
	{
		CString StrOut;
		StrItem.Format(L"%d",  (int)MemUsage);
		GetNumberFormat(LOCALE_USER_DEFAULT,LOCALE_NOUSEROVERRIDE,StrItem,NULL, StrOut.GetBuffer(MAX_PATH),MAX_PATH);
		StrOut.ReleaseBuffer();
		StrItem = StrOut;
		StrItem = StrItem.Left(StrItem.GetLength()-3); 
		StrItem=StrItem+L" KB";
	}*/
	/*else
	{
		StrItem.Format(L"%.2f KB",MemUsage);
	}*/


	mTaskList.SetItemText(ID,PROCLIST_MEMORY,StrItem);

	//---------------------------------------Disk----------------------------------


	mTaskList.SetItemText(ID,PROCLIST_DISK,L"0.00 MB/s");

	//-----------------------------------Network----------------------------------


	mTaskList.SetItemText(ID,PROCLIST_NETWORK,L"0.00 KB/s");




	mTaskList.SetRedraw(1);


	ReSort();

	return pNewAppListData;

}


int CPageProcesses::_OpenSubList(int ID,BOOL LockDraw)
{

	int n=0; 
	CWnd *pDeskTop=CWnd::GetDesktopWindow();
	CWnd  *pAppWnd = pDeskTop->GetWindow(GW_CHILD);

	CString  StrWndCaption;

	DWORD WindowPID ;
	DWORD ItemPID ;
	APPLISTDATA *pData = NULL;
	APPLISTDATA *pDataNew = NULL;
	pData=(APPLISTDATA * )mTaskList.GetItemData(ID);
	ItemPID = ((PROCLISTDATA * )pData->pPData)->PID ;

	int Pos=ID+1;

	if(LockDraw) mTaskList.SetRedraw(0);

	while( pAppWnd!=NULL)
	{



		GetWindowThreadProcessId(pAppWnd->m_hWnd,&WindowPID);
		if(ItemPID == WindowPID)
		{	
			if( ((pData->ItemType== APP) && (pAppWnd->IsWindowVisible()))  || (pData->ItemType==BKGPROC) )
			{
				if((!(pAppWnd->GetExStyle()&WS_EX_TOOLWINDOW))&&(pAppWnd->GetParent()==NULL))
				{
					pAppWnd->GetWindowTextW(StrWndCaption);
					if(StrWndCaption!= L"")
					{
						pDataNew =  AddNewItem((PROCLISTDATA * )pData->pPData,Pos);

						pDataNew->CoolUsageArray[ PROCLIST_CPU]=pData->CoolUsageArray[ PROCLIST_CPU];
						pDataNew->CoolUsageArray[ PROCLIST_MEMORY]=pData->CoolUsageArray[ PROCLIST_MEMORY];
						pDataNew->CoolUsageArray[ PROCLIST_DISK]=pData->CoolUsageArray[ PROCLIST_DISK];
						pDataNew->CoolUsageArray[ PROCLIST_NETWORK]=pData->CoolUsageArray[ PROCLIST_NETWORK];




						pDataNew->StrWnd = StrWndCaption;
						pDataNew->hMainWnd = pAppWnd->m_hWnd;
						mTaskList.SetItemText(Pos,0,StrWndCaption);
						pDataNew->SubType = SUB_ITEM;
						pDataNew->ItemType =pData->ItemType;

						Pos++;
						n++;	
					}
				}
			}


		}



		pAppWnd=pAppWnd->GetWindow(GW_HWNDNEXT);

	}


	pData->SubType = PARENT_ITEM_OPEN ;

	pData->nSubItem = n;

	ReSort(FALSE);

	if(LockDraw)mTaskList.SetRedraw(1);
	mTaskList.Invalidate();


	return n;
}

int CPageProcesses::_CloseSubList(int ID,BOOL LockDraw)
{
	APPLISTDATA *pData = NULL;
	APPLISTDATA *pDelData = NULL;

	pData=(APPLISTDATA * )mTaskList.GetItemData(ID);


	int Pos = ID+1;
	if(LockDraw)mTaskList.SetRedraw(0);
	while(1)  //注意删除后 会造成位置变化所以只需要删除同以位置项即可
	{

		pDelData = (APPLISTDATA * )mTaskList.GetItemData(Pos);
		if(pDelData->SubType == SUB_ITEM)
		{
			mTaskList.DeleteItem(Pos);

		}
		else
		{
			break ;
		}

	}

	pData->SubType = PARENT_ITEM_CLOSE ;
	if(LockDraw)mTaskList.SetRedraw(1);
	mTaskList.Invalidate();



	return 0;
}

int CPageProcesses::ListItemWindows(int nItem,APPLISTDATA * pListData )
{
	 

	APPLISTDATA *  pData = NULL ;
	if(pListData == NULL)
	{
		pData = (APPLISTDATA *) mTaskList.GetItemData(nItem);
	}
	else
	{
		pData = pListData;
	}

	if(pData == NULL) return -1;



	//----------------------------------


	DWORD PID = ((PROCLISTDATA *)pData->pPData)->PID;

	CWnd *pDeskTop=CWnd::GetDesktopWindow();

	CWnd  *pAppWnd = pDeskTop->GetWindow(GW_CHILD);

	CString  StrWndCaptionF;
	CString  StrWndCaptionB;
	CString Strtemp;
	DWORD WindowPID ;


	int nAppWnd=0;
	int nBkgWnd=0;



	while( pAppWnd!=NULL)
	{
		
		GetWindowThreadProcessId(pAppWnd->m_hWnd, &WindowPID); // 获得找到窗口所属的进程 

		if(WindowPID ==PID)
		{
			DWORD  WndExStyle = pAppWnd->GetExStyle();
			if(pAppWnd->IsWindowVisible()&& (!(WndExStyle&WS_EX_TOOLWINDOW))&&(pAppWnd->GetParent()==NULL)&&(!(WndExStyle&0x200000)))
			{				

				nAppWnd ++;
			}
			else  if(pAppWnd->GetParent()==NULL)
			{
				pAppWnd->GetWindowText(Strtemp);
				//StrWndCaptionB = StrWndCaptionB+L"   "+Strtemp+L"\n";
				if(Strtemp!=L"")nBkgWnd ++ ;
			}

		}
		
		pAppWnd=pAppWnd->GetWindow(GW_HWNDNEXT);

	}

	CString StrItem;


	if(nAppWnd>0 )
	{
		if(pData->ItemType!=APP) //类别改变删除展开项
		{
			_CloseSubList(nItem,FALSE);
			mTaskList.SetItemText(nItem,PROCLIST_TYPE,L"App");
			pData->ItemType = APP;
		}

		if((pData->nSubItem != nAppWnd) &&(pData->SubType == PARENT_ITEM_OPEN) )
		{		
			_CloseSubList(nItem,FALSE);
			_OpenSubList(nItem,FALSE);
		}

		pData->nSubItem = nAppWnd;


	}
	else 
	{

		if(pData->ItemType!=BKGPROC) //类别改变删除展开项
		{
			_CloseSubList(nItem,FALSE);
			pData->ItemType = BKGPROC;
			mTaskList.MySetItemText(nItem,PROCLIST_TYPE,L"Background Process");

		}


		if( (pData->nSubItem != nBkgWnd ) &&(pData->SubType == PARENT_ITEM_OPEN) )
		{
			_CloseSubList(nItem,FALSE);
			_OpenSubList(nItem,FALSE);			
		}

		pData->nSubItem = nBkgWnd; 




	}

	if(pData->ItemType == BKGPROC && pData->nSubItem == 0)
	{
		pData->SubType = PARENT_ITEM_NOSUB;
	}
	else if(pData->SubType != PARENT_ITEM_OPEN)
	{
		pData->SubType = PARENT_ITEM_CLOSE;
	}


	((PROCLISTDATA *)pData->pPData)->Type = pData->ItemType;// 一定要更新这个 否是 排序会错！！！

	//重设文字

 



	if(pData->nSubItem>1)
	{
		
			StrItem.Format(L"%s (%d)",pData->StrTitle, pData->nSubItem);
			mTaskList.MySetItemText(nItem,PROCLIST_NAME,StrItem);
		
	}
	else
	{
	
	 
			mTaskList.MySetItemText(nItem,PROCLIST_NAME,pData->StrTitle);
		
	}



	return 0;
}



BOOL CPageProcesses::CheckWndChange(void)
{


	int n=0; 
	CWnd *pDeskTop=CWnd::GetDesktopWindow();
	CWnd  *pAppWnd = pDeskTop->GetWindow(GW_CHILD);

	CString  StrWndCaption;

	CString StrNewWndList;

	CString StrTemp ;

	DWORD   hWndNum;

	while( pAppWnd!=NULL)
	{

		DWORD  WndExStyle =  pAppWnd->GetExStyle(); 
		if( !( WndExStyle & WS_EX_TOOLWINDOW)  )  //      WS_CAPTION   pAppWnd->IsWindowVisible() && 
		{

			hWndNum = (DWORD) pAppWnd->GetSafeHwnd();
			pAppWnd->GetWindowTextW(StrWndCaption);
			StrTemp.Format(L"%x",hWndNum);
			StrTemp=StrTemp+StrWndCaption;

			if(pAppWnd->IsWindowVisible()&&(pAppWnd->GetParent()==NULL)&& (!(WndExStyle & 0x200000)) )  //含有0x200000标记视为后台窗口
			{				 
				StrTemp = StrTemp+L"{APP}";
			}
			else if(pAppWnd->GetParent()==NULL)
			{
				StrTemp = StrTemp+L"{BKG}";
			}
			StrNewWndList= StrNewWndList+StrTemp;


		}

		pAppWnd=pAppWnd->GetWindow(GW_HWNDNEXT);

	}

	BOOL Ret = TRUE;

	//if(StrNewWndList.Compare(StrWndList) ==0 ) //没变化 返回FALSE
	if(	wcscmp(StrNewWndList,StrWndList)==0)
	{
		Ret =FALSE;
	}

	StrWndList= StrNewWndList;





	return Ret;
}

//BOOL CPageProcesses::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	if ((((LPNMHDR)lParam)->code == NM_RCLICK))  
//	{  
//		CPoint pt, pt2;  
//		GetCursorPos(&pt);  
//		pt2 = pt;  
//		mTaskList.ScreenToClient(&pt);  
//		CWnd* pWnd = mTaskList.ChildWindowFromPoint(pt);  
//		CHeaderCtrl* pHeader = mTaskList.GetHeaderCtrl();  
//		if(pWnd && (pWnd->GetSafeHwnd() == pHeader->GetSafeHwnd()))  
//		{  
//			//----------注释代码 可检测 在哪列按右键
//			//HDHITTESTINFO info = {0};  
//			//info.pt = pt;  
//			//pHeader->SendMessage(HDM_HITTEST, 0, (LPARAM)&info);  
//			//-------------------------------------------------------------------
//			
//
//		}  
//	}  
//	return CFormView::OnNotify(wParam, lParam, pResult);
//}

void CPageProcesses::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFormView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	if(pPopupMenu == NULL) return;





	pPopupMenu->SetDefaultItem(ID_PROCESSESLIST_EXPAND);

	//----------------------
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	APPLISTDATA *pListData = (APPLISTDATA *)mTaskList.GetItemData(nSel);
	CString Str;
	if(pListData->SubType == PARENT_ITEM_CLOSE)
	{			
		Str.LoadStringW(IDS_STRING_POPMENU_EXP);

	}

	else
	{
		Str.LoadStringW(IDS_STRING_POPMENU_COLL);
	}

	pPopupMenu->ModifyMenuW(ID_PROCESSESLIST_EXPAND,MF_BYCOMMAND,ID_PROCESSESLIST_EXPAND,Str);

	if(pListData->SubType == PARENT_ITEM_NOSUB)
	{
		pPopupMenu->DeleteMenu(ID_PROCESSESLIST_EXPAND,MF_BYCOMMAND);
	}

	 


	//----------------------

	//Menu status : percents Or Values   

	if(theApp.AppSettings.ProcList_MemPercents)
	{		 
		pPopupMenu->CheckMenuRadioItem(ID_MEMORY_PERCENTS_PLIST,ID_MEMORY_VALUES_PLIST,ID_MEMORY_PERCENTS_PLIST,MF_BYCOMMAND);
	}
	else
	{
		pPopupMenu->CheckMenuRadioItem(ID_MEMORY_PERCENTS_PLIST,ID_MEMORY_VALUES_PLIST,ID_MEMORY_VALUES_PLIST,MF_BYCOMMAND);
	}
		






}

void CPageProcesses::PlaceAllCtrl(void)
{

	CRect rc;	
	CRect rcHeader;
	CRect rcParent;
	GetClientRect(rc);



	if(rc.Height()<110)
	{
		if(mTaskList.CoolheaderCtrl.IsWindowVisible())
			mTaskList.CoolheaderCtrl.ShowWindow(SW_HIDE);
	}
	else
	{
		if(!mTaskList.CoolheaderCtrl.IsWindowVisible())
			mTaskList.CoolheaderCtrl.ShowWindow(SW_SHOW);
	}




	rc.InflateRect(1,1);


	//不用管headerCtrl


	rc.bottom-=50;

	rc.top=mTaskList.CoolheaderCtrl.Height;	
	mTaskList.MoveWindow(rc);


	CWnd *pBtn= GetDlgItem(IDC_BTN_ENDTASK);

	if(pBtn!=NULL)
	{
		CRect rcBtn(rc.right-95-15,rc.bottom+25-12,rc.right-20,rc.bottom+25+12);
		pBtn->MoveWindow(rcBtn);


	}

	if(theApp.pButtonFOM!=NULL)
	{
		if(theApp.AppSettings.TaskManMode == 0)
		{
			rc.bottom+=theApp.MenuAndTabHeight;
		}

		theApp.pButtonFOM->MoveWindow(rc.left+15,rc.bottom+25-12,95,25);

	}





}

void CPageProcesses::OnNMRClickProcesslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	int  nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );

	APPLISTDATA *pData =(APPLISTDATA *)mTaskList.GetItemData(nSel);

	if(pData->pPData == NULL )return ; //分组标题不弹出菜单！！！


	CMenu PopMenu;
	CMenu *pMenu = NULL;

	PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_BASE) );

	if(pData->SubType==SUB_ITEM)
	{
		pMenu = PopMenu.GetSubMenu(6);  //6是 子项右键 对应的 菜单  
	}
	else
	{
		pMenu = PopMenu.GetSubMenu(0);  //0是 这个 对应的 菜单  
	}


	CPoint CurPos ;
	GetCursorPos(&CurPos); 

	pMenu->TrackPopupMenu(TPM_LEFTALIGN,CurPos.x,CurPos.y,this);



	//---------------



	*pResult = 0;
}



void CPageProcesses::PreListItems(void)
{




	nApp=nBkg=nWin=0;
	/*

	Tips !  方案1    /

	所有的排序隐含都先按条目 类型优先 包括分组的 标题 类型 （就是 APP/Background Processes /Windows Processes 三组）

	条目类型设置如下：（暂定）

	APP分组的标题           0, 6               APP具体项目          1  
	BGKProc 分组的标题       2 , 4              BGKProc具体项目       3
	WinProc 分组的标题       4  ,2                WinProc具体项目      5



	所有 分组的标题        iImage =-1; 



	*/



	PROCLISTDATA *pDetailListData = NULL;


	mTaskList.SetRedraw(0); //必须的 因为在线程中

	for(int  i= 0;i<pPageDetails->mDetailsList.GetItemCount() ;i++)
	{	 
		pDetailListData =( PROCLISTDATA *) ( pPageDetails->mDetailsList.GetItemData(i));

		if(pDetailListData!=NULL)
		{
			if(pDetailListData->PID == 0) //不包含此项 sis idle
			{

				continue  ;

			}
			AddNewItem(pDetailListData,mTaskList.GetItemCount(),TRUE);

			if( pDetailListData->Type  == APP )
			{
				nApp++;
			}
			else  if( pDetailListData->Type == WINPROC )
			{
				nWin++;
			}
			else
			{
				nBkg++;		
			}


			AfxBeginThread(Thread_GetItemName,pDetailListData);



		}

	}




	//-------------分组标题 -------------

	_AddGroupItem();

	//AfxBeginThread(Thread_AddGroupItem,this);


	//------------------------------------------------------------------------

	//排序

	mTaskList.FlagSortUp = TRUE;
	mTaskList.CurrentSortColumn = PROCLIST_NAME;

	Sort(PROCLIST_NAME,FALSE);

	//-------------------------------------------------------------------------

	mTaskList.SetRedraw(1);

	mTaskList._GetRedrawColumn();
	mTaskList.Invalidate();
	mTaskList.CoolheaderCtrl.Invalidate();

	//MSB(0)
	

	




}




void CPageProcesses::FillAllItemData(BOOL LoadAllTrueData)
{

	APPLISTDATA *pData = NULL;
	PROCLISTDATA *pDetailListData = NULL;

	//mTaskList.SetRedraw(0);

	CString StrItem;


	// 数量判断 防止过程中有增减进程
	for( int i =0;i<mTaskList.GetItemCount() ;i++)
	{	 
		pData = ( APPLISTDATA *) mTaskList.GetItemData(i);

		if(pData==NULL) continue ;

		pDetailListData = (( PROCLISTDATA *)pData->pPData) ;

		if(pDetailListData==NULL) continue ;



		//pData->npDetailListData->Name   ;

		
		

		

		if(( pDetailListData->Name.CompareNoCase(L"svchost.exe") == 0) && (pDetailListData->SessionID == 0))
		{
			pData->iImage = SvchostIconIndex;
		}
		else
		{
			pData->iImage = pDetailListData->IconIndex;
		}
		 



     	mTaskList.SetItem(i,0, LVIF_IMAGE, NULL,pDetailListData->IconIndex,0,0,0); 
		

		//--------------------------------  第一列 Title  ---------------------------
		if(	pData->SubType != SUB_ITEM )
		{
			CString StrTemp=pDetailListData->Description;

			StrTemp.Remove(L' ');

			if(StrTemp.Compare(L"")==0)//描述为空用进程名代替
			{
				pData->StrTitle = pDetailListData->Name.Left(pDetailListData->Name.GetLength()-4);
				mTaskList.SetItemText(i,PROCLIST_NAME, pData->StrTitle);
			}
			else
			{
				mTaskList.SetItemText(i,PROCLIST_NAME, pDetailListData->Description);
				pData->StrTitle = pDetailListData->Description;
			}


			

			
		}

		//--------------------------------  Process name ---------------------------

		mTaskList.SetItemText(i,PROCLIST_PNAME, pDetailListData->Name );

		//--------------------------------  Process Status  ---------------------------

		//StrItem.Format(L"%d",pDetailListData->IconIndex);
		//mTaskList.SetItemText(i,PROCLIST_STATUS, StrItem );

		mTaskList.SetItemText(i,PROCLIST_PNAME, pDetailListData->Name );

		//------------------------------------   PID   --------------------------------------

		StrItem.Format(L"%d",pDetailListData->PID);
		mTaskList.SetItemText(i,PROCLIST_PID, StrItem );


		//---------------------------------    Publisher   ------------------------------
		StrItem = mProcInfo.GetVerInfoString(mProcInfo.GetPathName(pDetailListData->hProcess),L"CompanyName") ;
		mTaskList.SetItemText(i,PROCLIST_PUB,StrItem );


		//--------------------------------    command line    ----------------------------------
		mTaskList.SetItemText(i,PROCLIST_CMDLINE, mProcInfo.GetProcCommandLine(pDetailListData->PID) );


		//-----------------------------------CPU-----------------------------------

		mTaskList.SetItemText(i,PROCLIST_CPU,L"0.00%");
		pDetailListData->CPU_Usage = 0; //防止错乱
		pData->CoolUsageArray[PROCLIST_CPU] = 0;

		//---------------------------------内存------------------------


		_GetMemDataAndSetItemText(i,pData);

		pData->CoolUsageArray[PROCLIST_MEMORY] = 0.001;



		//---------------------------------------Disk----------------------------------


		mTaskList.SetItemText(i,PROCLIST_DISK,L"0.00 MB/s");

		//-----------------------------------Network----------------------------------


		mTaskList.SetItemText(i,PROCLIST_NETWORK,L"0.00 KB/s");

	

	}

	mTaskList.CoolheaderCtrl.Invalidate();






	//排序

	ReSort(FALSE);

	//-------------------------------------------------------------------------



	//	mTaskList.SetRedraw(1);



}

void CPageProcesses::DeleteProcessItem(PROCLISTDATA * pDetailsListData)
{
	int n = mTaskList.GetItemCount();
	APPLISTDATA *pListData = NULL;

	for(int i = 0;i<n;i++)
	{
		pListData =(APPLISTDATA *) mTaskList.GetItemData(i);

		if(pListData == NULL) continue;

		if( pListData->SubType != -1) //不是分组标题
		{
			if( (pListData->pPData ==  pDetailsListData)  || ( (pListData->pPData==NULL) && (pListData->SubType<SUB_ITEM) ) ) //SubType<SUB_ITEM) 是排除子项因为他不用独立处理 会自动随父项删除  || ( (pListData->pPData==NULL) && (pListData->SubType<SUB_ITEM) )
			{

				switch(pListData->ItemType)
				{
				case  APP:
					nApp--;
					break;
				case  BKGPROC:
					nBkg--;
					break;
				case  WINPROC:
					nWin--;
					break;
				}

				mTaskList.DeleteItemAndSub(i);
				break ;
			}
		}


	}//for


	ReSort();
	
	UpdateGroupText();
}

void CPageProcesses::SetProcessStatusInfo(DWORD PID, int Status)
{

	APPLISTDATA *pData = NULL;
	CString  StrStatus ;

	switch(Status)
	{
	case PS_RUNNING:StrStatus = L"";break;
	case PS_NOTRESPONDING:StrStatus = L"Not Responding";break;
		//case 2:StrStatus = L"Running";break;
	}


	for(int i=0;i<mTaskList.GetItemCount();i++)
	{
		pData = (APPLISTDATA *)mTaskList.GetItemData(i);

		if(pData ==NULL) continue ;

		if(pData->pPData ==NULL) continue ;

		if( ((PROCLISTDATA *)(pData->pPData))->PID == PID)
		{
			if( (pData->SubType !=  SUB_ITEM)&&(pData->pPData!=NULL))
			{
				mTaskList.SetItemText(i,PROCLIST_STATUS,StrStatus);
			}
			break;
		}
	}


}


//void CPageProcesses::OnSysCommand(UINT nID, LPARAM lParam)
//{
//	// TODO: Add your message handler code here and/or call default
//
//
//	CFormView::OnSysCommand(nID, lParam);
//}

BOOL CPageProcesses::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

 


	UINT ID= (UINT)wParam;
	switch(ID)
	{
	case ID_VIEW_EXPANDALL:
		
		break;
	case ID_PROCESSES_TYPE:
	case ID_PROCESSES_STATUS:
	case ID_PROCESSES_PUBLISHER:
	case ID_PROCESSES_PID:
	case ID_PROCESSES_PROCESSNAME:
	case ID_PROCESSES_COMMANDLINE:
	case ID_PROCESSES_CPU:
	case ID_PROCESSES_MEMORY:
	case ID_PROCESSES_DISK:
	case ID_PROCESSES_NETWORK:

		CMenu PopMenu;
		CMenu *pMenu;
		PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_COLUMN) );
		pMenu = PopMenu.GetSubMenu(0);  //0是 这个 对应的 菜单 和标签顺序对应

		//		if((HMENU)(pMsg->wParam)  == pMenu->m_hMenu) MSBOX(999)

		int nItem = pMenu->GetMenuItemCount();
		int iClick = -1;
		for(int i= 0;i<nItem;i++)
		{
			if(pMenu->GetMenuItemID(i)==ID)
			{
				iClick= i;
				break;
			}
		}

		int iCol = iClick+1;

		
		BOOL IsTurnColShow;
		IsTurnColShow =mTaskList.ShowOrHideColumn(iCol);
		if(IsTurnColShow)RefreshList();

		 
		break;

	}





	return CFormView::OnCommand(wParam, lParam);
}

void CPageProcesses::Sort(int nCol,BOOL InvertSort)
{

	APPLISTDATA *pListData = NULL;
	int nCount = mTaskList.GetItemCount();






	if(nCol == PROCLIST_NAME )//点击第 PROCLIST_NAME 列
	{
		if(mTaskList.CurrentSortColumn  == PROCLIST_NAME  ) //目前也是按此列排序
		{
			BYTE  ResetType ;
			if(InvertSort)
			{
				nCount = mTaskList.GetItemCount();

				if(mTaskList.FlagSortUp)//目前是升序排列 标题type分别为 0,2,4转为降序2,4,6
				{
					ResetType = 2;
					for(int i=0;i<nCount;i++)
					{
						pListData = (APPLISTDATA *)mTaskList.GetItemData(i);
						if(pListData==NULL) continue ;
						if(pListData->pPData == NULL)
						{
							pListData->ItemType =  ResetType;	
							ResetType+=2;
						}
					}
				}
				else//目前是降序排列 标题type分别为 2,4,6转为升序 0,2,4 //注意此时大的在上面！！！
				{
					ResetType = 4;
					for(int i=0;i<nCount;i++) //
					{
						pListData = (APPLISTDATA *)mTaskList.GetItemData(i);
						if(pListData==NULL) continue ;
						if(pListData->pPData == NULL)
						{
							pListData->ItemType =  ResetType;	
							ResetType-=2;
						}
					}
				}


			}





		}
		else //目前不是是按第0列排序
		{

			//-------------添加 分组标题 -------------
			if(theApp.AppSettings.GroupByType)
			_AddGroupItem();


		}

	}
	else //不按第0列排序 移除分组标题项
	{
		if(theApp.AppSettings.GroupByType)
		_RemoveGroupTitle();

	}



	//-------------------------------------------------
	//   必须放在   移除/添加 分组标题     增减列表项之后

	int LastParentItemID = -1 ;
	nCount = mTaskList.GetItemCount();
	for(int i= 0;i<nCount;i++)
	{
		pListData = (APPLISTDATA *) mTaskList.GetItemData(i);
		//mDetailsList.SetItemData(i,(DWORD_PTR)i);  //列表项相关项设为项号

		if(pListData->SubType == PARENT_ITEM_OPEN)
		{
			LastParentItemID = i;
		}	

		if(pListData->SubType == SUB_ITEM && (nCol != PROCLIST_NAME)) //由于子项很多列无数据所以 排序是填充子项对应列 注意不可在排序过程中 因为那时id 可能已经变了
		{
			CString StrItemText;
			StrItemText=mTaskList.GetItemText(LastParentItemID,nCol);
			mTaskList.SetItemText(i,nCol,StrItemText);
		}

		pListData->SortID = i;
	}


	if(InvertSort)
	{
		if(mTaskList.CurrentSortColumn == nCol)
		{
			mTaskList.FlagSortUp = !mTaskList.FlagSortUp;
		}
		else
		{
			mTaskList.FlagSortUp = TRUE;
		}
	}


	mTaskList.CurrentSortColumn =  nCol;


	//设置回调函数的参数和入口地址   
	mTaskList.SortItems(Sort_Processes, nCol);




}

void CPageProcesses::ReSort(BOOL SkipStaticColumn)
{
	//注意增减进程时候要马上调用一次本函数 SkipStaticColumn 要设为 FALSE否则静态列不会自动排序！！！

	if(mTaskList)
	{
		switch(mTaskList.CurrentSortColumn)
		{//以下列 内容 相对于进程 固定 不需要每次刷新重新排序

		case PROCLIST_TYPE:
		case PROCLIST_PUB:
		case PROCLIST_PID:
		case PROCLIST_PNAME:
		case PROCLIST_CMDLINE:
			return;

		}
	}



	Sort(mTaskList.CurrentSortColumn,FALSE);

}

void CPageProcesses::OnPop_ProcesseslistExpand()
{

	this->PostMessageW(UM_DBCLICK_LSIT);


}


void CPageProcesses::OnPop_ProcesseslistEndtask()
{
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	APPLISTDATA *pListData = (APPLISTDATA *)mTaskList.GetItemData(nSel);
	CString Str;
	if(pListData->SubType != SUB_ITEM)
	{			
	
		if(theApp.Global_ShowOperateTip(((PROCLISTDATA*)(pListData->pPData))->Name,STR_ENDPROC_MAINTIP,STR_ENDPROC_CONTENT,STR_ENDPROC_BTN) ==IDOK)
		{
			TerminateProcess(((PROCLISTDATA*)(pListData->pPData))->hProcess, 4);
		}


	}

}

void CPageProcesses::OnPop_ProcesseslistGotoDetails()
{

	APPLISTDATA *pData = NULL;
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mTaskList.GetItemData(nSel);


	//由于允许多选 所以要先清除之前选中的 

	pPageDetails->ClearSelecet();





	if(pData!=NULL)
	{
		if(pData->pPData ==NULL )return;
		DWORD PID = ((PROCLISTDATA *)(pData->pPData))->PID;
		for(int i=0;i<pPageDetails->mDetailsList.GetItemCount();i++)
		{
			PROCLISTDATA * pDetailsData =(PROCLISTDATA *) pPageDetails->mDetailsList.GetItemData(i);
			if(pDetailsData!=NULL)
			{
				if(pDetailsData->PID == PID)
				{
					pPageDetails->mDetailsList.EnsureVisible(i,FALSE);
					pPageDetails->mDetailsList.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
					break;
				}
			}

		}
	}



	int PageID=3;
	theApp.pMainTab->SetCurSel(PageID);
	//--------------触发实际动作---------- 
	NMHDR nmhdr; 
	nmhdr.code = TCN_SELCHANGE;  
	nmhdr.hwndFrom = theApp.pMainTab->GetSafeHwnd();  
	nmhdr.idFrom= theApp.pMainTab->GetDlgCtrlID();  
	::SendMessage(theApp.pMainTab->GetSafeHwnd(), WM_NOTIFY,MAKELONG(TCN_SELCHANGE,PageID), (LPARAM)(&nmhdr));
}


void CPageProcesses::OnBnClickedBtnEndTask()
{
	OnPop_ProcesseslistEndtask();
}

void CPageProcesses::_OnListItemChaged(void)
{
	CWnd * pButton=	this->GetDlgItem(IDC_BTN_ENDTASK);
	if(pButton==NULL) return;

	if(mTaskList.GetSelectedCount()>0)
	{	
		
		int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED ); 
		APPLISTDATA * pData =(APPLISTDATA *) mTaskList.GetItemData(nSel); 
		if(pData!=NULL)
		{
			if(pData->SubType != -1)
			{			
				pButton->EnableWindow(TRUE);
			}	
			else
			{
				pButton->EnableWindow(FALSE);		
			}
		}

	}
	else
	{	
		pButton->EnableWindow(FALSE);		
	}

	
}

void CPageProcesses::_RemoveGroupTitle(void)
{

	mTaskList.SetRedraw(0);

	int nCount = mTaskList.GetItemCount();
		BYTE  ResetType = 0;
		for(int i=nCount-1;i>=0;i--)  //从后删除防止行号变化造成的麻烦
		{
			APPLISTDATA *pListData = (APPLISTDATA *)mTaskList.GetItemData(i);
			if(pListData->pPData ==NULL)
			{
				mTaskList.DeleteItem(i);  //数据自动删除

			}
		}
	mTaskList.SetRedraw(1);
}

void CPageProcesses::OnLvnKeydownProcesslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here

	if(pLVKeyDow->wVKey == VK_DELETE)
	{
		//AfxBeginThread(Thread_FillAllItemsData,this);
		CWnd * pCtrl= NULL;
		pCtrl = this->GetDlgItem(IDC_BTN_ENDTASK);
		if(pCtrl)
		{
			if(pCtrl->IsWindowEnabled())
			{
				OnPop_ProcesseslistEndtask();
			}
		}
	}

	*pResult = 0;
}


void CPageProcesses::RefreshList(void)
{

	AfxBeginThread(Thread_FillAllItemsData,this);
 
}

void CPageProcesses::OnPop_OpenFileLocation()
{
	APPLISTDATA *pData = NULL;
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mTaskList.GetItemData(nSel);

	if(pData!=NULL)
	{
		if(pData->pPData ==NULL )return;

		CString StrTargetPath = mProcInfo.GetPathName(((PROCLISTDATA *)(pData->pPData))->hProcess);
		OpenFileLocation(StrTargetPath);
	}



}

void CPageProcesses::OnPop_Properties()
{
	APPLISTDATA *pData = NULL;
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mTaskList.GetItemData(nSel);

	if(pData!=NULL)
	{
		if(pData->pPData ==NULL )return;

		CString StrTargetPath = mProcInfo.GetPathName(((PROCLISTDATA *)(pData->pPData))->hProcess);
		OpenPropertiesDlg(StrTargetPath);
	}
}

void CPageProcesses::OnPop_SwitchTo()
{
	APPLISTDATA *pData = NULL;
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mTaskList.GetItemData(nSel);
	if(pData!=NULL)
	{
		if(pData->hMainWnd!=NULL)
		{
			::BringWindowToTop(pData->hMainWnd);	
			::ShowWindow(pData->hMainWnd,SW_RESTORE);
			::SetActiveWindow(pData->hMainWnd);
			if(theApp.AppSettings.MiniOnUse)
			{
				theApp.m_pMainWnd->ShowWindow(SW_MINIMIZE);
			}
		}
	}

}

void CPageProcesses::OnPop_Bringtofront()
{
	APPLISTDATA *pData = NULL;
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mTaskList.GetItemData(nSel);
	if(pData!=NULL)
	{
		if(pData->hMainWnd!=NULL)
		{
		
			::BringWindowToTop(pData->hMainWnd);	
			::ShowWindow(pData->hMainWnd,SW_RESTORE);
			::SetForegroundWindow(pData->hMainWnd);
		}
	}
}

void CPageProcesses::OnPop_Maximize()
{
	APPLISTDATA *pData = NULL;
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mTaskList.GetItemData(nSel);
	if(pData!=NULL)
	{
		if(pData->hMainWnd!=NULL)
		{		
				
			::ShowWindow(pData->hMainWnd,SW_MAXIMIZE);
			
		}
	}
}

void CPageProcesses::OnPop_Minimize()
{
	APPLISTDATA *pData = NULL;
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mTaskList.GetItemData(nSel);
	if(pData!=NULL)
	{
		if(pData->hMainWnd!=NULL)
		{		
				
			::ShowWindow(pData->hMainWnd,SW_MINIMIZE);
			
		}
	}
}

void CPageProcesses::OnPop_Endtask() //这是某项展开后针对进程窗口的操作
{
	APPLISTDATA *pData = NULL;
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mTaskList.GetItemData(nSel);
	if(pData!=NULL)
	{
		if(pData->hMainWnd!=NULL)
		{		
			::PostMessage(pData->hMainWnd, WM_DESTROY,0,0);
		
		}
	}
}

void CPageProcesses::SetWndItem(void)
{

	//int n=0; 
	//CWnd *pDeskTop=CWnd::GetDesktopWindow();
	//CWnd  *pAppWnd = pDeskTop->GetWindow(GW_CHILD);

	//CString  StrWndCaption;
	//CString StrNewWndList;

	//CString StrTemp ;

	//DWORD   hWndNum;

	// 
	//APPLISTDATA *pData;

	//while( pAppWnd!=NULL)
	//{

	//	if(!(pAppWnd->GetExStyle()&WS_EX_TOOLWINDOW) )  //      WS_CAPTION   pAppWnd->IsWindowVisible() && 
	//	{

	//		hWndNum = (DWORD) pAppWnd->GetSafeHwnd();
	//		pAppWnd->GetWindowTextW(StrWndCaption);
	//		StrTemp.Format(L"%x",hWndNum);
	//		StrTemp=StrTemp+StrWndCaption;
	//		DWORD PID;
	//		::GetWindowThreadProcessId(hWndNum,&PID);

	//		for(int i=0;i<mTaskList.GetItemCount();i++)
	//		{
	//			pData = (APPLISTDATA *)mTaskList.GetItemData(i);
	//			if(pData==NULL) continue;
	//			if(pData->pPData == NULL) continue;	
	//			if(pData->ItemType == WINPROC) continue;		
	//			if(((PROCLISTDATA*)pData->pPData)->PID ==PID )
	//			{
	//				pData->ItemType = = BKGPROC;

	//				if(pAppWnd->IsWindowVisible())
	//				{
	//					pData->ItemType = APP;
	//					
	//				}
	//				if()

	//				
	//			}


	//		}


	//	}

	//	pAppWnd=pAppWnd->GetWindow(GW_HWNDNEXT);

	//}



}

void CPageProcesses::OnPop_Memory_ShowAsPercents()
{
	if(theApp.AppSettings.ProcList_MemPercents==0)
	{
		theApp.AppSettings.ProcList_MemPercents = 1;
		RefreshList();
	}
}

void CPageProcesses::OnPop_Memory_ShowAsValues()
{
	if(theApp.AppSettings.ProcList_MemPercents!=0)
	{
		theApp.AppSettings.ProcList_MemPercents = 0;
		RefreshList();
	}
}

void CPageProcesses::_GetMemDataAndSetItemText(int iItem, APPLISTDATA * PData)
{
	
	double  MemUsage;
	MemUsage = mProcInfo.GetWsPrivate_PDH( (PROCLISTDATA*)PData->pPData );

	CString StrItem =L"";



	if(MemUsage<= (double)theApp.PerformanceInfo.TotalPhysMem   &&   MemUsage>0 && theApp.PerformanceInfo.InUsePhysMem>0)//防止显示错乱的 数据
	{
		
		((PROCLISTDATA*)PData->pPData)->Mem_PrivateWS = MemUsage; 

		double Percents =  (double)( MemUsage/(theApp.PerformanceInfo.InUsePhysMem )*100);
		if(Percents<0.1&&Percents>0)Percents=0.1;

		if(theApp.AppSettings.ProcList_MemPercents)//按百分比显示 
		{		
			
			StrItem.Format(L"%.1f%%",  Percents );
			mTaskList.MySetItemText(iItem,PROCLIST_MEMORY,StrItem);
			

		}
		else
		{	 	
			StrItem.Format(L"%.1f",  MemUsage/1024/1024);
			mTaskList.MySetItemText(iItem,PROCLIST_MEMORY,StrItem+L" MB");

		}

		if(Percents>1&&Percents<10)Percents=20;//否则很多都不显示变色
		PData->CoolUsageArray[PROCLIST_MEMORY]= Percents/100; //用于变色显示！！！
	}

}

void CPageProcesses::OnPop_Searchonline()
{
	APPLISTDATA *pData = NULL;
	int nSel = mTaskList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mTaskList.GetItemData(nSel);

	if(pData!=NULL)
	{
		if(pData->pPData ==NULL )return;		 
		SearchOnline(((PROCLISTDATA *)(pData->pPData))->Description);
	}


	
}

void CPageProcesses::UpdateGroupText(void)
{
	int i= 0;
	while(1)
	{
		if(mTaskList.GetItemCount() == i) break ;

		APPLISTDATA *pListData = (APPLISTDATA *)mTaskList.GetItemData(i);	 

		if( (pListData == NULL) || (pListData->pPData == NULL ) )  //注意排除分组标题
		{ 	

			CString StrGroupTitle  = mTaskList.GetItemText(i, 0);
			CString StrNewGroupTitle = NULL  ;

			if(StrGroupTitle.Find(STR_GROUP_APP)>=0)
			{
				StrNewGroupTitle.Format(L"%s (%d)",STR_GROUP_APP,nApp);						 

			}
			else if(StrGroupTitle.Find(STR_GROUP_BKG)>=0)
			{
				StrNewGroupTitle.Format(L"%s (%d)",STR_GROUP_BKG,nBkg);			

			}
			else if(StrGroupTitle.Find(STR_GROUP_WIN)>=0)
			{
				StrNewGroupTitle.Format(L"%s (%d)",STR_GROUP_WIN,nWin);		
			}

			mTaskList.SetItemText(i,0,StrNewGroupTitle);

		} 

		i++;  

	}
}
