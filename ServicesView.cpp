// ServicesView.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "ServicesView.h"
//#include <stdio.h>
#include <locale.h>
#include  <Winsvc.h>
// CPageServices


//------------------------------------------------------------------------------------


 
//排序   升降 续 由 类中 SortType  变量控制
int CALLBACK Sort_Services(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)  
{  
	int result = 0;     //返回值   
	ListSortData * SortData = (ListSortData * )lParamSort;

	CString Str1,Str2;

	Str1= SortData->pList->GetItemText((int)lParam1,SortData->nCol);
	Str2= SortData->pList->GetItemText((int)lParam2,SortData->nCol);



	//排序   
	  if(SortData->nCol == 1) //PID 为数字其他均为字符串
	  {
		  int PID1 = _wtoi(Str1);
		  int PID2 = _wtoi(Str2);

		  result = PID1-PID2;


	  }
	  else
	  {
		  result =lstrcmp(Str1,Str2);
	  }

	 
	  if(SortData->FlagSortUp == FALSE)
	  {
		  result = -result;
	  }
		
	 return result;  
}  




//----------------------------------------------------------------------------------------------




IMPLEMENT_DYNCREATE(CPageServices, CFormView)

CPageServices::CPageServices()
	: CFormView(CPageServices::IDD)
	, FlagSortUp(TRUE)
	, CurrentSortColumn(0)

{

	mServicesList.mHdCtrl.pCurrentSortCol =  &CurrentSortColumn;
	mServicesList.mHdCtrl.pFlagSortUp =  &FlagSortUp ;
	

	CString StrLoad;
	StrLoad.LoadString(STR_SVR_STATUS);
	for(int i=0;i<8;i++)
	{		
		::AfxExtractSubString(StrSvrStatus[i],StrLoad,i,L';');	
	}

	StrLoad.Empty();
	StrLoad.LoadString(STR_SVR_STARTTYPE);
	for(int i=0;i<6;i++)
	{
		::AfxExtractSubString(StrSvrStartType[i],StrLoad,i,L';');		
	}

 

}

CPageServices::~CPageServices()
{
}

void CPageServices::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVICESLIST, mServicesList);
	DDX_Control(pDX, IDC_BTN_OPENSERVICES, mBtnOpenService);
}

BEGIN_MESSAGE_MAP(CPageServices, CFormView)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_OPENSERVICES, &CPageServices::OnBnClickedBtnOpenServices)
	ON_NOTIFY(NM_RCLICK, IDC_SERVICESLIST, &CPageServices::OnNMRClickServicesList)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CPageServices::OnHdnItemclickServiceslist)
	ON_COMMAND(ID_SERVICESLIST_OPENSERVICES, &CPageServices::OnPop_ServiceslistOpenServices)
	ON_COMMAND(ID_SERVICESLIST_GOTODETAILS, &CPageServices::OnPop_ServiceslistGotoDetails)
	ON_COMMAND(ID_SERVICESLIST_START, &CPageServices::OnPop_ServiceListStart)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ENDSCROLL, IDC_SERVICESLIST, &CPageServices::OnLvnEndScrollServiceslist)
	ON_COMMAND(ID_SERVICESLIST_STOP, &CPageServices::OnPop_ServicesListStop)
	ON_COMMAND(ID_SERVICESLIST_SEARCHONLINE, &CPageServices::OnPop_Searchonline)
END_MESSAGE_MAP()


// CPageServices diagnostics

#ifdef _DEBUG
void CPageServices::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPageServices::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPageServices message handlers

void CPageServices::InitList(void)
{ 

	// 如果想自绘   这个效果 就不能再设置了 ！！！
	SetWindowTheme(mServicesList.GetSafeHwnd(),L"explorer", NULL);

	// LVSIL_SMALL

	HIMAGELIST IList;

	IList=::ImageList_Create(LINE_H1,LINE_H1,ILC_COLOR32,1,1);


	::ImageList_AddIcon(IList,theApp.LoadIcon( MAKEINTRESOURCE(IDI_SERVICE) ) );

	::SendMessage(mServicesList.m_hWnd, LVM_SETIMAGELIST,(WPARAM)LVSIL_SMALL, (LPARAM)IList);

	mServicesList.SetExtendedStyle( mServicesList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES|LVS_EX_HEADERDRAGDROP|LVS_EX_DOUBLEBUFFER|LVS_EX_TRANSPARENTSHADOWTEXT  ); // 加 LVS_EX_TRANSPARENTSHADOWTEXT 没有虚线框  | LVS_EX_GRIDLINES       &(~LVS_EX_INFOTIP)


	CString StrColumn,StrLoad;
	StrLoad.LoadStringW(STR_COLUMN_SERVER);

	for(int i=0;i<6;i++)
	{	
		AfxExtractSubString(StrColumn,StrLoad,i,L';');
		mServicesList.InsertColumn(i,StrColumn,0,100);
	}

	mServicesList.SetColumnWidth(0,200);
	mServicesList.SetColumnWidth(0,150);





	ListServices();

	//设置按钮图标

	//mBtnOpenService.hIcon = theApp.LoadIcon( MAKEINTRESOURCE(IDI_SERVICE) );
	//CString StrPath;
	//GetModuleFileName(NULL,StrPath.GetBuffer(MAX_PATH),MAX_PATH);
	//StrPath.ReleaseBuffer();
 
	 ::ExtractIconEx(L"filemgmt.dll",0,NULL,&mBtnOpenService.hIcon,1);

	 this->SetTimer(0,2000,NULL);

}

void CPageServices::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here


	CRect rc;	
	CRect rcParent;
	GetClientRect(rc);
	 

	rc.InflateRect(1,1);	 
	rc.bottom-=50;
	 
	mServicesList.MoveWindow(rc);
	mBtnOpenService.MoveWindow(115,rc.bottom+25-11, 200, 25 );// 此时 rc.bottom 已经上移
 
  
}

int CPageServices::ListServices(void)
{

/*
	SC_HANDLE hSCM;
    LPENUM_SERVICE_STATUS lpEnumServiceStatus;
    DWORD dwBytesNeeded;
    DWORD dwServicesReturned;
    DWORD i;

    setlocale(LC_ALL, setlocale(LC_CTYPE, ""));

    hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);



    EnumServicesStatus(hSCM, SERVICE_WIN32 | SERVICE_DRIVER,SERVICE_STATE_ALL,NULL,0,&dwBytesNeeded,&dwServicesReturned,0);
	//EnumServicesStatusEx(  hSCM,  SC_ENUM_PROCESS_INFO,  SERVICE_WIN32,  SERVICE_STATE_ALL,  NULL,  dwBufSize,  &dwBufNeed,  &dwNumberOfService,  NULL,  NULL);
	

 
    lpEnumServiceStatus = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, dwBytesNeeded);
    if (lpEnumServiceStatus == NULL) {
        return 1;
    }


  
    EnumServicesStatus(hSCM,         SERVICE_WIN32 ,    SERVICE_STATE_ALL,     lpEnumServiceStatus,        dwBytesNeeded,        &dwBytesNeeded,        &dwServicesReturned,        0);  // SERVICE_DRIVER 驱动 





	

    for (i = 0; i < dwServicesReturned; i++)
	{

		mServicesList.InsertItem(0,lpEnumServiceStatus[i].lpServiceName);

	    mServicesList.SetItemText(0,2, lpEnumServiceStatus[i].lpDisplayName);
		 mServicesList.SetItemText(0,3,_GetCurrentStateString(lpEnumServiceStatus[i].ServiceStatus.dwCurrentState));
 


		 SC_HANDLE hService = NULL;
		 DWORD     nRet = 0;

		 // 打开服务
		 hService = ::OpenService(hSCM,lpEnumServiceStatus[i].lpServiceName,SERVICE_QUERY_CONFIG);


		 ::QueryServiceConfig(hService, sc, 4 * 1024, &nRet);


		 
		 mServicesList.SetItemText(0,4,_GetStartypeString( sc->dwStartType));

		 CString StrGroup = sc->lpBinaryPathName;

		 int n= StrGroup.Find(L"-k");

		 if(n== -1)
		 {
			 
			 StrGroup =L"";
		 }
		 else
		 {
			 StrGroup.Delete(0,n+3);
			 
		 }



		 mServicesList.SetItemText(0,5,StrGroup);



		 ::CloseServiceHandle(hService);

		 
    
   
	   
//  _tprintf(TEXT("DisplayName: %s\n"),      lpEnumServiceStatus[i].lpDisplayName);
        
     
      //  _tprintf(TEXT("ServiceName: %s\n"),             lpEnumServiceStatus[i].lpServiceName);
        
     
     //   _tprintf(TEXT("CurrentState: %s\n"),             GetCurrentStateString(lpEnumServiceStatus[i].ServiceStatus.dwCurrentState));

      
    //    _tprintf(TEXT("ServiceType: %s\n"),        GetServiceTypeString(lpEnumServiceStatus[i].ServiceStatus.dwServiceType));
        
       
    }

    LocalFree(lpEnumServiceStatus);
    CloseServiceHandle(hSCM);

	CString StrN;

	StrN.Format(L"%d Services",i);
	//MessageBox(StrN);
*/



	SC_HANDLE hSCM    = NULL;
	PUCHAR  pBuf    = NULL;
	ULONG  dwBufSize   = 0x00;
	ULONG  dwBufNeed   = 0x00;
	ULONG  dwNumberOfService = 0x00;

	LPENUM_SERVICE_STATUS_PROCESS pInfo = NULL;

	CString StrTemp;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_CONNECT);

	if (hSCM == NULL)
	{
		//printf_s("OpenSCManager fail \n");
		return 0xffff0001;
	}

	EnumServicesStatusEx(hSCM,SC_ENUM_PROCESS_INFO,SERVICE_WIN32,SERVICE_STATE_ALL,NULL,dwBufSize,&dwBufNeed,&dwNumberOfService,NULL,NULL);


	if (dwBufNeed < 0x01)
	{
		//printf_s("EnumServicesStatusEx fail ?? \n");
		return 0xffff0002;
	}

	dwBufSize = dwBufNeed + 0x10;
	pBuf  = (PUCHAR) malloc(dwBufSize);

	EnumServicesStatusEx(hSCM,SC_ENUM_PROCESS_INFO,SERVICE_WIN32,SERVICE_STATE_ALL,pBuf,dwBufSize,&dwBufNeed,&dwNumberOfService,NULL,NULL); //SERVICE_STATE_ALL代表各种状态都包括

	pInfo = (LPENUM_SERVICE_STATUS_PROCESS)pBuf;


	LPQUERY_SERVICE_CONFIG sc; 
	sc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 4 * 1024);

	for (ULONG i=0;i<dwNumberOfService;i++)
	{
		
		mServicesList.InsertItem(0,pInfo[i].lpServiceName);
	    mServicesList.SetItemText(0,2, pInfo[i].lpDisplayName);
		mServicesList.SetItemText(0,3,_GetCurrentStateString(pInfo[i].ServiceStatusProcess.dwCurrentState));


		StrTemp.Format(L"%d",pInfo[i].ServiceStatusProcess.dwProcessId);
		if(pInfo[i].ServiceStatusProcess.dwProcessId !=0 )
		{
			mServicesList.SetItemText(0,1,StrTemp);
		}
		
		 SC_HANDLE hService = NULL;
		 DWORD     nRet = 0;

		
		 // 打开服务
		 hService = ::OpenService(hSCM,pInfo[i].lpServiceName,SERVICE_QUERY_CONFIG);

		 ::QueryServiceConfig(hService, sc, 4 * 1024, &nRet);
		 
		 mServicesList.SetItemText(0,4,_GetStartypeString( sc->dwStartType));

		 CString StrGroup = sc->lpBinaryPathName;

		 int n= StrGroup.Find(L"-k");

		 if(n== -1)
		 {
			 
			 StrGroup =L"";
		 }
		 else
		 {
			 StrGroup.Delete(0,n+3);
			 
		 }


		 mServicesList.SetItemText(0,5,StrGroup);
		 ::CloseServiceHandle(hService); ///一定要每个都关闭！！！！不然内存泄漏





	}

	free(pBuf);

	CloseServiceHandle(hSCM);

	LocalFree(sc);





	return 0;
}

CString CPageServices::_GetCurrentStateString(DWORD dwCurrentState)
{


	//switch (dwCurrentState) 
	//{

	//case SERVICE_STOPPED:
	//	return TEXT("Stopped");

	//case SERVICE_START_PENDING:
	//	return TEXT("SERVICE_START_PENDING");

	//case SERVICE_STOP_PENDING:
	//	return TEXT("SERVICE_STOP_PENDING");

	//case SERVICE_RUNNING:
	//	return TEXT("Running");

	//case SERVICE_CONTINUE_PENDING:
	//	return TEXT("SERVICE_CONTINUE_PENDING");

	//case SERVICE_PAUSE_PENDING:
	//	return TEXT("SERVICE_PAUSE_PENDING");

	//case SERVICE_PAUSED:
	//	return TEXT("Paused");

	//default:
	//	return TEXT("Unkown State");

	//}


	if(dwCurrentState>SERVICE_PAUSED || SERVICE_PAUSED<=0)
	{
		return StrSvrStatus[7];
	}
	else
	{
		return StrSvrStatus[dwCurrentState-1];
	}



	return L"";
}



CString CPageServices::_GetStartypeString(DWORD dwType)
{


//	#define ADS_SERVICE_BOOT_START          SERVICE_BOOT_START
//#define ADS_SERVICE_SYSTEM_START        SERVICE_SYSTEM_START
//#define ADS_SERVICE_AUTO_START          SERVICE_AUTO_START
//#define ADS_SERVICE_DEMAND_START        SERVICE_DEMAND_START
//#define ADS_SERVICE_DISABLED           SERVICE_DISABLED
 

	//switch (dwType) 
	//{
	//case SERVICE_BOOT_START:	
	//case SERVICE_SYSTEM_START:		
	//case SERVICE_AUTO_START:
	//case SERVICE_DEMAND_START: //Manual
	//case SERVICE_DISABLED:
	//	return StrSvrStartType[dwType];
	//	
	//default:
	//	return TEXT("Unkown");

	//}

	if(dwType>SERVICE_DISABLED || dwType<0) 
	{
		return StrSvrStartType[5];//Unkown
	}
	else
	{
		return StrSvrStartType[dwType];
	}

	
	
	
}

HBRUSH CPageServices::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
		return theApp.BkgBrush;
}

void CPageServices::OnBnClickedBtnOpenServices()
{
	ShellExecute(CWnd::GetDesktopWindow()->m_hWnd, L"open", L"mmc.exe",L"services.msc" ,_T(""),SW_SHOW);

}

void CPageServices::OnNMRClickServicesList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	int SelCount = mServicesList.GetSelectedCount();


	if(SelCount == 1)
	{
		CMenu PopMenu;
		CMenu *pMenu = NULL;
		PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_BASE) );
		pMenu = PopMenu.GetSubMenu(4);  //4是 这个 对应的 菜单 和标签顺序对应

		CPoint CurPos ;
		GetCursorPos(&CurPos); 
		
		
		
		int nSel = mServicesList.GetNextItem( -1, LVNI_SELECTED );
		CString StrSevName = mServicesList.GetItemText(nSel,0);
		// 打开服务管理对象 &  打开服务。
		   
		SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, GENERIC_EXECUTE);if( hSC == NULL)return;
		SC_HANDLE hSvc = ::OpenService(hSC, StrSevName,SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
		if( hSvc == NULL){ 	::CloseServiceHandle( hSC);	return;	}
		// 获得服务的状态
		SERVICE_STATUS status;
		if( ::QueryServiceStatus( hSvc, &status) == FALSE)
		{     
			::CloseServiceHandle( hSvc);
			::CloseServiceHandle( hSC);
			return;
		}

		if(status.dwCurrentState == SERVICE_STOPPED)//选中项为停止状态
		{
			pMenu->EnableMenuItem(ID_SERVICESLIST_STOP,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND );
			pMenu->EnableMenuItem(ID_SERVICESLIST_GOTODETAILS,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND );	
			pMenu->EnableMenuItem(ID_SERVICESLIST_RESTART,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND );	
		}
		else //选中项为运行状态
		{
			pMenu->EnableMenuItem(ID_SERVICESLIST_START,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND );
		}


		pMenu->TrackPopupMenu(TPM_LEFTALIGN,CurPos.x,CurPos.y,this);
	}

	//---------------

	*pResult = 0;
}

void CPageServices::OnHdnItemclickServiceslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here



	 
	if(CurrentSortColumn == phdr->iItem)
	{
		FlagSortUp = !FlagSortUp;
	}
	else
	{
		FlagSortUp = TRUE;
	}

	ListSortData SortData;
	SortData.pList = &mServicesList;
	SortData.nCol = phdr->iItem ;
	SortData.FlagSortUp = FlagSortUp;




	CurrentSortColumn = phdr->iItem;

	int nCount = mServicesList.GetItemCount();

	for(int i= 0;i<nCount;i++)
	{
		mServicesList.SetItemData(i,(DWORD_PTR)i);  //列表项相关项设为项号
	}
	mServicesList.SortItems(Sort_Services,(LPARAM)&SortData );
	


	*pResult = 0;
}

void CPageServices::OnPop_ServiceslistOpenServices()
{
	OnBnClickedBtnOpenServices();
}

void CPageServices::OnPop_ServiceslistGotoDetails()
{
		//由于允许多选 所以要先清除之前选中的 

	//pPageDetails->ClearSelecet();


	int PageID=3;
	theApp.pMainTab->SetCurSel(PageID);
	//--------------触发实际动作---------- 
	NMHDR nmhdr; 
	nmhdr.code = TCN_SELCHANGE;  
	nmhdr.hwndFrom = theApp.pMainTab->GetSafeHwnd();  
	nmhdr.idFrom= theApp.pMainTab->GetDlgCtrlID();  
	::SendMessage(theApp.pMainTab->GetSafeHwnd(), WM_NOTIFY,MAKELONG(TCN_SELCHANGE,PageID), (LPARAM)(&nmhdr));

}

void CPageServices::OnPop_ServiceListStart()
{
	// 打开服务管理对象
    SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, GENERIC_EXECUTE);
    if( hSC == NULL)return;   
    

	int nSel = mServicesList.GetNextItem( -1, LVNI_SELECTED );
	CString StrSevName = mServicesList.GetItemText(nSel,0);


	// 打开服务。
    SC_HANDLE hSvc = ::OpenService(hSC, StrSevName,SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if( hSvc == NULL)
    { 
        ::CloseServiceHandle( hSC);
        return;
    }
    // 获得服务的状态
    SERVICE_STATUS status;
    if( ::QueryServiceStatus( hSvc, &status) == FALSE)
    {     
        ::CloseServiceHandle( hSvc);
        ::CloseServiceHandle( hSC);
        return;
    }

	if( status.dwCurrentState == SERVICE_STOPPED)
	{
		 // 启动服务
        if( ::StartService( hSvc, NULL, NULL) == FALSE)
        {
            AfxMessageBox( L"Start service error.",MB_ICONEXCLAMATION|MB_OK);
                      
        }
				
	}

	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);



}

void CPageServices::UpDateAllItemStatus(BOOL UpdateVisibleOnly)
{
	



	SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, GENERIC_EXECUTE);
	SC_HANDLE hSvc;
    if( hSC == NULL)return;   

	int n=mServicesList.GetItemCount();

	CRect rcList,rcItem,rcTemp;


	mServicesList.GetClientRect(rcList);


	for(int i=0;i<n;i++)
	{
		if(UpdateVisibleOnly)
		{
		mServicesList.GetItemRect(i,rcItem,LVIR_BOUNDS);
		if(!IntersectRect(rcTemp,rcList,rcItem)) continue ;
		}

		CString StrSevName = mServicesList.GetItemText(i,0);
		// 打开服务。
	     hSvc= ::OpenService(hSC, StrSevName,SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
		if( hSvc == NULL)continue ;
		 // 获得服务的状态
		SERVICE_STATUS status;
		if( ::QueryServiceStatus( hSvc, &status) == FALSE)continue ;



		
			
		if( status.dwCurrentState == SERVICE_RUNNING)
		{ 
			CString StrTemp;
			mServicesList.SetItemText(i,3,StrSvrStatus[SERVICE_RUNNING-1]);
			SERVICE_STATUS_PROCESS StatusInfo;
			DWORD dwBytesNeede;	
			QueryServiceStatusEx(hSvc,SC_STATUS_PROCESS_INFO,(LPBYTE) &StatusInfo,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeede);			
			StrTemp.Format(L"%d",StatusInfo.dwProcessId);
			mServicesList.SetItemText(i,1,StrTemp);

			
		}
		else if(status.dwCurrentState == SERVICE_STOPPED)
		{
			mServicesList.SetItemText(i,3,StrSvrStatus[SERVICE_STOPPED-1]);
			mServicesList.SetItemText(i,1,L"");



		}

		::CloseServiceHandle( hSvc);//一定要每个都关闭！！！！不然内存泄漏
 	
	}


	
	::CloseServiceHandle( hSC);



}

BOOL CPageServices::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	////if(pMsg->message == UM_PROCSTART ||pMsg->message ==   UM_PROCEXIT)
	//{
	//	//UpDateAllItemStatus();
	//}

	return CFormView::PreTranslateMessage(pMsg);
}

void CPageServices::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default


	if(theApp.pSelPage == this)
	{
		UpDateAllItemStatus();
	}


//	CFormView::OnTimer(nIDEvent);
}

void CPageServices::OnLvnEndScrollServiceslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Internet Explorer 5.5 or greater.
	// The symbol _WIN32_IE must be >= 0x0560.
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	// TODO: Add your control notification handler code here

	UpDateAllItemStatus( );
	
	*pResult = 0;
}

void CPageServices::OnPop_ServicesListStop()
{
	// 打开服务管理对象
    SC_HANDLE hSC = ::OpenSCManager( NULL,NULL, GENERIC_EXECUTE);
    if( hSC == NULL)return;       

	int nSel = mServicesList.GetNextItem( -1, LVNI_SELECTED );
	CString StrSevName = mServicesList.GetItemText(nSel,0);

	// 打开服务。
    SC_HANDLE hSvc = ::OpenService(hSC, StrSevName,SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if( hSvc == NULL)
    { 
        ::CloseServiceHandle( hSC);
        return;
    }
    // 获得服务的状态
    SERVICE_STATUS status;
    if( ::QueryServiceStatus( hSvc, &status) == FALSE) 
	{   
        ::CloseServiceHandle( hSvc);
		::CloseServiceHandle( hSC);
        return;
    }

	if( status.dwCurrentState == SERVICE_RUNNING)
	{
		 // 停止服务
        if( ::ControlService( hSvc,SERVICE_CONTROL_STOP, &status) == FALSE)
        {
            AfxMessageBox( L"Stop service error.",MB_ICONEXCLAMATION|MB_OK);
                      
        }
				
	}

	::CloseServiceHandle( hSvc);
	::CloseServiceHandle( hSC);
}

 
void CPageServices::OnPop_Searchonline()
{
	int nSel = mServicesList.GetNextItem( -1, LVNI_SELECTED );
	CString StrSevName = mServicesList.GetItemText(nSel,0);
	CString StrSevDescript = mServicesList.GetItemText(nSel,2);
	SearchOnline(StrSevName+L" "+StrSevDescript);

}
