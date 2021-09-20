// UserView.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "UsersView.h"
//
#include <lm.h>   // LPUSER_INFO_0  等 
#pragma comment(lib,"Netapi32.lib") 

#include <Wtsapi32.h>   //WTSQuerySessionInformation 
#pragma comment(lib,"Wtsapi32.lib") 

#include "processinfo.h"
#include "DetailsView.h"



static  CPageUsers *pThisPage;

 




//------------------------------------------------------------------------------------------------------------------------------------





//排序的回调函数--升序 使用 虚拟列表无效 
int CALLBACK Sort_Users(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)  
{  
	CCoolListCtrl* pList= &( ((CPageUsers*)theApp.pSelPage)->mUserList);

	int nCol =(int) lParamSort;
	int result = 0;     //返回值   

	USERLISTDATA * pData1 = (USERLISTDATA * )lParam1;
	USERLISTDATA * pData2 = (USERLISTDATA * )lParam2;

//	int Data1,Data2;
 
	CString Str1,Str2;

	Str1= pList->GetItemText(pData1->SortID,nCol);
	Str2= pList->GetItemText(pData2->SortID,nCol);
	




	switch(nCol)
	{
	case USERLIST_USER:
	case USERLIST_SESSION:
	case USERLIST_CLIENTNAME:
	case USERLIST_STATUS:

		//所有列都要 先按 User  排列 否则但子项展开后排序子项与父项有可能错开！！！！		

		result=lstrcmp(( (PROCLISTDATA *) pData1->pPData)->User,( (PROCLISTDATA *) pData2->pPData)->User);
		if(result==0)
		{
			result = (pData1->SubType  - pData2->SubType);  //确定子项和父项关系，子项 SubType 值最大！
			if(result==0)
			{
				result =lstrcmp(Str1,Str2);
			}
			else
			{
				if( pList->FlagSortUp == FALSE)//如果有父子之分 根据情况预先颠倒防止被最后总的颠倒所颠倒
				{
					result = -result;
				}
			}
		}	

		break;

	default:

	
		CString StrL1,StrL2;
		StrL1.Format(L"%16s",Str1);//转为同样长度字符串前面补空格
		StrL2.Format(L"%16s",Str2);//转为同样长度字符串前面补空格

		//所有列都要 先按 User  排列 否则但子项展开后排序子项与父项有可能错开！！！！		
		result=lstrcmp(( (PROCLISTDATA *) pData1->pPData)->User,( (PROCLISTDATA *) pData2->pPData)->User);
		if(result==0)
		{
			result = (pData1->SubType  - pData2->SubType);  //确定子项和父项关系，子项 SubType 值最大！
			if(result==0)
			{
				result =lstrcmp(StrL1,StrL2);
			}	
			else
			{
				if( pList->FlagSortUp == FALSE)//如果有父子之分 根据情况预先颠倒防止被最后总的颠倒所颠倒
				{
					result = -result;
				}
			}
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






// CUserView

IMPLEMENT_DYNCREATE(CPageUsers, CFormView)

CPageUsers::CPageUsers()
	: CFormView(CPageUsers::IDD)
	, pDetailList(NULL)
{
	

}

CPageUsers::~CPageUsers()
{
}

void CPageUsers::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USERLIST, mUserList);
}

BEGIN_MESSAGE_MAP(CPageUsers, CFormView)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_WTSSESSION_CHANGE()
	ON_MESSAGE(UM_TIMER,OnUMTimer)

	ON_WM_INITMENUPOPUP()

	ON_NOTIFY(NM_RCLICK, IDC_USERLIST, &CPageUsers::OnNMRClickUserlist)
	ON_COMMAND(ID_USERSLIST_MANAGEUSERACCOUNTS, &CPageUsers::OnUserslistManageUserAccounts)
	ON_COMMAND(ID_PROCESSESLIST_ENDTASK, &CPageUsers::OnPop_Endtask)
	ON_COMMAND(ID_PROCESSESLIST_OPENFILELOCATION, &CPageUsers::OnPop_Openfilelocation)
	ON_COMMAND(ID_PROCESSESLIST_PROPERTIES, &CPageUsers::OnPop_Properties)
	ON_COMMAND(ID_USERSLIST_DISCONNECT, &CPageUsers::OnPop_Disconnect)
	ON_COMMAND(ID_PROCESSESLIST_GOTODETAILS, &CPageUsers::OnPop_Gotodetails)
	ON_COMMAND(ID_PROCESSESLIST_SEARCHONLINE, &CPageUsers::OnPop_Searchonline)
END_MESSAGE_MAP()


// CUserView diagnostics

#ifdef _DEBUG
void CPageUsers::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPageUsers::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CUserView message handlers

void CPageUsers::InitList(void)
{

	//mUserList.SetExtendedStyle( (mUserList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES)&(~LVS_EX_INFOTIP)  ); //| LVS_EX_GRIDLINES
	mUserList.SetExtendedStyle(mUserList.GetExtendedStyle()|LVS_EX_FULLROWSELECT| LVS_OWNERDRAWFIXED|LVS_EX_DOUBLEBUFFER |LVS_EX_HEADERDRAGDROP   );  //| LVS_EX_GRIDLINES |LVS_EX_CHECKBOXES

	if(!theApp.FlagThemeActive)
	{
		mUserList.ModifyStyleEx(WS_EX_CLIENTEDGE,0);
		mUserList.ModifyStyle(0,WS_BORDER);
	}
	

	
	
	mUserList.IsProcList = FALSE;

	mUserList.DrawAllColForSubItem = TRUE; //设置子列表项仍然绘制全部 列  这是与第一页的不同之处
//-------------------
	
	mUserList.InitAllColumn(COL_SAT_USER,STR_COLUMN_USERS,COL_COUNT_USER);  //9 是全部的列 数 5 是黄色起始项

	//设置在子项中允许显示数据的列，默认只有第0列显示
	mUserList.pColStatusArray[USERLIST_CPU].DrawInSubItem = TRUE;
	mUserList.pColStatusArray[USERLIST_MEMORY].DrawInSubItem = TRUE;
	mUserList.pColStatusArray[USERLIST_DISK].DrawInSubItem = TRUE;
	mUserList.pColStatusArray[USERLIST_NETWORK].DrawInSubItem = TRUE;
//-----------------------------------------------------------------

	
	mUserList.SetImageList(theApp.mImagelist.m_hImageList);  //决定了可以自绘表头 必须放在InsertColumn之后!!!!!!!!!

	ListUsers();

}

void CPageUsers::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	CRect rc;	
	CRect rcParent;

	GetClientRect(rc);
	 

	rc.InflateRect(1,1);
	
	rc.bottom-=50;
	rc.top=mUserList.CoolheaderCtrl.Height;	
	mUserList.MoveWindow(rc);
	



//---------------------------

	CWnd *pBtn= GetDlgItem(IDC_BTN_DISCONNECT);

	if(pBtn!=NULL)
	{
		CRect rcBtn(rc.right-95-15,rc.bottom+25-12,rc.right-20,rc.bottom+25+12);
		pBtn->MoveWindow(rcBtn);
	}


	mUserList._GetRedrawColumn();//重要！！必须马上更新否则显示跟不上


}

int CPageUsers::ListUsers(void)
{
 


	BOOL bRet = FALSE;
	CString  strUserName = _T("");
	//for xp or above
	TCHAR *szLogName = NULL;

	WTS_SESSION_INFO *sessionInfo = NULL;
	DWORD sessionInfoCount;
	BOOL result = WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &sessionInfo, &sessionInfoCount);

	DWORD dwSize = 0;
	unsigned int userCount(0);
	CString StrTemp;


	COOLLISTDATA  *pUserData = NULL;
	PROCLISTDATA  *pProcListData = NULL;

	
	for(unsigned int i = 0; i < sessionInfoCount; ++i)
	{
		
		if( ((sessionInfo[i].State == WTSActive)   || (sessionInfo[i].State == WTSDisconnected) ) &&(sessionInfo[i].SessionId!=0) )        //  WTSActive
		{
			userCount++;

			if(WTSQuerySessionInformation( WTS_CURRENT_SERVER_HANDLE,sessionInfo[i].SessionId,WTSUserName,&szLogName,&dwSize))//WTS_CURRENT_SESSION
			{
				strUserName = szLogName;
				//CString  Str;
				///Str.Format(L"%d",userCount);
				//MessageBox(szLogName);
				int nItem = mUserList.GetItemCount();
				mUserList.InsertItem(nItem,szLogName);
				if(sessionInfo[i].State == WTSActive)  //WTS_CONNECTSTATE_CLASS::WTSActive
				{
					mUserList.SetItemText(nItem,USERLIST_STATUS,L"Active");

				}
				else if(sessionInfo[i].State ==  WTSDisconnected)  //WTS_CONNECTSTATE_CLASS:: WTSDisconnected
				{
					mUserList.SetItemText(nItem,USERLIST_STATUS,L"Disconnected");
				}

				StrTemp.Format(L"%d",sessionInfo[i].SessionId);

				mUserList.SetItemText(nItem,USERLIST_ID,StrTemp);

				pUserData= new USERLISTDATA;
				pProcListData = new PROCLISTDATA; //不允许为空 且排序需要！！！！
				pProcListData->User = strUserName;

				pUserData->iImage = -1;
				pUserData->ItemType = USER;
				pUserData->pPData =  pProcListData;
				pUserData->SubType =PARENT_ITEM_CLOSE;				
		
				mUserList.SetItemData(nItem, (DWORD_PTR)pUserData);
			

				WTSFreeMemory(szLogName);
				bRet = TRUE;

			}
		}
	}


 //Sort(0,FALSE);

return userCount;



}

HBRUSH CPageUsers::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
		return theApp.BkgBrush;
}



void CPageUsers::OnSessionChange(UINT nSessionState, UINT nId)
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.
	// TODO: Add your message handler code here and/or call default



	if( nSessionState == WTS_SESSION_LOGON)
	{
		AddUserItem(0,nId);
	}
	else
	{
		UpdateUserStatus(nId,nSessionState);
	}



	//CString Str,Str1;

	//switch(nSessionState)
	//{


	//case WTS_CONSOLE_CONNECT: //The session identified by lParam was connected to the console terminal or RemoteFX session.	
	//case WTS_SESSION_LOGON://A user has logged on to the session identified by lParam.
	//	AddUserItem(0,nId);
	//	break;
	//case WTS_CONSOLE_DISCONNECT://The session identified by lParam was disconnected from the console terminal or RemoteFX session.
	//case WTS_REMOTE_DISCONNECT://	The session identified by lParam was disconnected from the remote terminal.
	//case WTS_SESSION_LOGOFF://A user has logged off the session identified by lParam.
	//case WTS_SESSION_LOCK://The session identified by lParam has been locked.
	//case WTS_SESSION_UNLOCK://The session identified by lParam has been unlocked.
	//case WTS_SESSION_REMOTE_CONTROL://The session identified by lParam has changed its remote controlled status. To determine the status, call GetSystemMetrics and check the SM_REMOTECONTROL metric.
	//	UpdateUserStatus(nId,nSessionState);
	//	break;

	//}



	//Str.Format(L"%s    ID= %d",Str1,nId);
	//MessageBox(Str);


	



	CFormView::OnSessionChange(nSessionState, nId);
}

int CPageUsers::AddUserItem(int nItem, UINT SessionID)
{

	WCHAR * szLogName = NULL;
	DWORD dwSize = 0;
	CString StrUserName;
	CString StrTemp;

	//WTS_SESSION_INFO   SessionInfo; 

	COOLLISTDATA  *pUserData;
	PROCLISTDATA  *pProcListData;

	if(WTSQuerySessionInformation( WTS_CURRENT_SERVER_HANDLE,SessionID,WTSUserName,&szLogName,&dwSize))//WTS_CURRENT_SESSION
	{
		StrUserName = szLogName;
		
		mUserList.InsertItem(0,StrUserName);
		 
		mUserList.SetItemText(0,2,L"Active");
 
		StrTemp.Format(L"%d",SessionID);

		mUserList.SetItemText(0,1,StrTemp);

		pUserData= new USERLISTDATA;
		pProcListData = new PROCLISTDATA;
		pUserData->iImage = -1;
		pUserData->ItemType = USER;
		pUserData->pPData =  pProcListData;
		pUserData->SubType = PARENT_ITEM_CLOSE; 
		pProcListData->User =  StrUserName;

	
		 
		//一定要在最前面！ 否则InsertItem时候效果已经显示
	   pUserData->CoolUsageArray[USERLIST_CPU]=pUserData->CoolUsageArray[USERLIST_MEMORY]=pUserData->CoolUsageArray[USERLIST_DISK]=pUserData->CoolUsageArray[USERLIST_NETWORK]=0;

		mUserList.SetItemData(0, (DWORD_PTR)pUserData);

		WTSFreeMemory(szLogName);
		 

	}


	return 0;
}

void CPageUsers::UpdateUserStatus(UINT nSessionID,UINT nSessionState)
{

	int i=0;

	CString StrItem;
	UINT ID ;
	while(1)
	{

		if(i==mUserList.GetItemCount()) break;
		
		//----------------------------------------

		StrItem = mUserList.GetItemText(i,1);
		ID =_wtoi(StrItem);

		if(ID == nSessionID)
		{
			switch(nSessionState)
			{
			case WTS_CONSOLE_CONNECT: //The session identified by lParam was connected to the console terminal or RemoteFX session.
				
				break;
			case	WTS_CONSOLE_DISCONNECT://The session identified by lParam was disconnected from the console terminal or RemoteFX session.
				mUserList.SetItemText(i,2,L"Disconnected");
				break;
			case	WTS_REMOTE_CONNECT://The session identified by lParam was connected to the remote terminal.
				mUserList.SetItemText(i,2,L"Active");
				break;
			case	WTS_REMOTE_DISCONNECT://	The session identified by lParam was disconnected from the remote terminal.
				mUserList.SetItemText(i,2,L"Disconnected");
				break;
			case	WTS_SESSION_LOGOFF://A user has logged off the session identified by lParam.				
				
				mUserList.DeleteItemAndSub(i);

				break;
			case	WTS_SESSION_LOCK://The session identified by lParam has been locked.
				mUserList.SetItemText(i,2,L"Locked");
				break;
			case	WTS_SESSION_UNLOCK://The session identified by lParam has been unlocked.
				mUserList.SetItemText(i,2,L"Active");
				break;
			case	WTS_SESSION_REMOTE_CONTROL://The session identified by lParam has changed its remote controlled status. To determine the status, call GetSystemMetrics and check the SM_REMOTECONTROL metric.
				
				break;

			}
			break ;
		}

		i++;

	}


	






}

BOOL CPageUsers::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if(pMsg->message ==  UM_DBCLICK_LSIT)
	{
		int nItem = mUserList.GetNextItem(-1,LVNI_SELECTED);
		APPLISTDATA *pData = NULL;
		pData=(APPLISTDATA * )mUserList.GetItemData(nItem);
		if(pData->SubType == PARENT_ITEM_CLOSE )
		{
			 
			_OpenSubList(nItem);
			

		}
		else  if(pData->SubType == PARENT_ITEM_OPEN )
		{
			_CloseSubList(nItem);
			 
		}

	}

	//-------------

	if(pMsg->message == UM_PROCSTART)
	{
		int n = mUserList.GetItemCount();

		PROCLISTDATA *pPorcListData=(PROCLISTDATA*)(pMsg->lParam);
		CString StrUserName;
		APPLISTDATA *pData = NULL;
		for(int i=0;i<n;i++)
		{
			pData=(APPLISTDATA * )mUserList.GetItemData(i);
			if(pData->SubType == PARENT_ITEM_OPEN)
			{
				StrUserName = mUserList.GetItemText(i,0);
				if(StrUserName.CompareNoCase(pPorcListData->User)==0 )
				{
					mUserList.SetRedraw(0);
					AddProcessItem(pPorcListData,i+1);
					mUserList.SetRedraw(1);
					break;
				}

			}
		}
		 

	}


	//--------------


 
	

	if(pMsg->message == UM_HEADER_LCLICK)
	{
		int n=(int)pMsg->wParam;
		Sort(n);

	}



	//----------------------menu message --------------------
	


	if(pMsg->message == UM_HEADER_RCLICK)
	{
		CPoint pt;  
		GetCursorPos(&pt);  
		//mTaskList.ScreenToClient(&pt)
		CMenu PopMenu;
		CMenu *pMenu = NULL;
		PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_COLUMN) );
		pMenu = PopMenu.GetSubMenu(1);  //1是 这个 对应的 菜单 和标签顺序对应
	

		if(pMenu->GetMenuItemID(0) == ID_USERS_ID )
		{
			int i=0;
			int nItem = pMenu->GetMenuItemCount();

			if(nItem>3)
			{
				for(i=1;i<9;i++)
				{
					if(COL_SAT_USER[i].ColWidth != 0) 
					{
						pMenu->CheckMenuItem(i-1,MF_BYPOSITION|MF_CHECKED);
					}		
				}
			}
		}

		pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,&(mUserList.CoolheaderCtrl));

	}




	return CFormView::PreTranslateMessage(pMsg);
}

int CPageUsers::_OpenSubList(int ID ,BOOL Lock)
{
	CString StrUser =  mUserList.GetItemText(ID,USERLIST_USER);

	USERLISTDATA *pUserList = (USERLISTDATA*)mUserList.GetItemData(ID);

	int i = 0;

	PROCLISTDATA *pData = NULL;
	if(Lock)mUserList.SetRedraw(0);
//


	for (map<DWORD,PVOID>::iterator i=Map_PidToData.begin(); i!=Map_PidToData.end();  i++ ) 
	{  
		pData = (PROCLISTDATA *) i->second;
		if(pData->User.CompareNoCase(StrUser)==0)
		{
			AddProcessItem( pData,ID+1) ;
		}	
		
	}




	if(Lock)mUserList.SetRedraw(1);

	pUserList->SubType = PARENT_ITEM_OPEN;

	ReSort();


	return 0;
}

int CPageUsers::AddProcessItem(PVOID pListData, int nID)
{
	PROCLISTDATA * pData =( PROCLISTDATA * )pListData ;

	USERLISTDATA *pUserListData = new USERLISTDATA;

	//一定要在最前面！ 否则InsertItem时候效果已经显示
	pUserListData->CoolUsageArray[USERLIST_CPU]=pUserListData->CoolUsageArray[USERLIST_MEMORY]=pUserListData->CoolUsageArray[USERLIST_DISK]=pUserListData->CoolUsageArray[USERLIST_NETWORK]=0;



 
	pUserListData->pPData = pData;
	pUserListData->SubType = SUB_ITEM;
	pUserListData->iImage = pData->IconIndex;
	pUserListData->ItemType = APP ; //不能是分组标题否则文字只绘制第一列

	IO_COUNTERS  IOCounter;
	GetProcessIoCounters(pData->hProcess,&IOCounter);	
	pUserListData->IOLast = IOCounter.ReadTransferCount+IOCounter.WriteTransferCount;

	CString StrItem=pData->Description;
	StrItem.Remove(L' ');

	if(StrItem.Compare(L"")==0)//描述为空用进程名代替
	{
		pUserListData->StrTitle = pData->Name.Left(pData->Name.GetLength()-4);
	}
	else
	{
		pUserListData->StrTitle = pData->Description;
	}


	mUserList.InsertItem(nID,pUserListData->StrTitle);

	//写入初始数据 为了视觉效果 不停顿
	mUserList.SetItemText(nID,USERLIST_CPU,L"0.00%");
	mUserList.SetItemText(nID,USERLIST_MEMORY,L"0.00 MB");
	mUserList.SetItemText(nID,USERLIST_DISK,L"0.00 MB/s");
	mUserList.SetItemText(nID,USERLIST_NETWORK,L"0.00 KB/s");

	

		
	mUserList.SetItemData(nID,(DWORD_PTR)pUserListData);
	_SetMemUsage(pData,nID,TRUE);

	 
 


	return 0;
}

int CPageUsers::_CloseSubList(int ID,BOOL Lock )
{

	int nDelID = ID+1;

	USERLISTDATA *pData = NULL;

	if(Lock)mUserList.SetRedraw(0);

	while(nDelID <mUserList.GetItemCount())  //i的位置总不变 因为是删除 后面会上来
	{
		
		pData = (USERLISTDATA *) mUserList.GetItemData(nDelID);
		if(pData->SubType != SUB_ITEM )
		{
			break;
		}
		else
		{
			mUserList.DeleteItem(nDelID);
		}	
		 
	}

	if(Lock)mUserList.SetRedraw(1);

	pData= (USERLISTDATA *) mUserList.GetItemData(ID);

	pData->SubType = PARENT_ITEM_CLOSE;

	mUserList.Invalidate();
	return 0;
}

LRESULT  CPageUsers::OnUMTimer(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default

	


	CRect rc,rcTemp,rcItem;

	//mUserList.GetClientRect(rc);
	

	CString StrItem;


	rc = mUserList._GetRedrawColumn();

	

 

	int i= 0;

	//mUserList.SetRedraw(0);


	CProcess mProcInfo;
	int nCount =mUserList.GetItemCount();
	while(i<nCount)
	{	 

		

		mUserList.GetItemRect(i,rcItem,LVIR_BOUNDS);
		BOOL RedrawItem =IntersectRect (rcTemp,rcItem,rc);


		USERLISTDATA *pListData = (USERLISTDATA *)mUserList.GetItemData(i);
		//if(RedrawItem)


	 	if(pListData->SubType!= SUB_ITEM){i++; continue ;}

		if(!RedrawItem){i++; continue ;}
 
	 

		if(COL_SAT_USER[USERLIST_CPU].Redraw)
		{			

			//((CPageDetails *)pPageDetails)->_GetCpuUsage( (PROCLISTDATA*)pListData->pPData );
			mProcInfo.GetCpuUsage((PROCLISTDATA*)pListData->pPData);

			StrItem.Format(L"%0.2f%%", ( (PROCLISTDATA*)pListData->pPData )->CPU_Usage );
			pListData->CoolUsageArray[USERLIST_CPU]= ((PROCLISTDATA*)pListData->pPData )->CPU_Usage/100; //用于变色显示！！！
			mUserList.SetItemText(i,USERLIST_CPU,StrItem);


		}

		if(COL_SAT_USER[USERLIST_MEMORY].Redraw)
		{		

			
			double OldWorkingSet = ((PROCLISTDATA*)pListData->pPData )->Mem_WorkingSet;
			mProcInfo.GetMemUsageInfo((PROCLISTDATA*)pListData->pPData);
			double WSDelta = ((PROCLISTDATA*)pListData->pPData )->Mem_WorkingSet - OldWorkingSet;

			if(WSDelta!=0)
			{
				_SetMemUsage(pListData->pPData,i);
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


		if(COL_SAT_USER[USERLIST_DISK].Redraw)
		{

			DiskUsageMBPerSec = ((double)(NewIO-((PROCLISTDATA*)pListData->pPData)->DiskIO))/1048576/theApp.AppSettings.TimerStep ;

			if(DiskUsageMBPerSec<0.01) DiskUsageMBPerSec = 0;
			

			StrItem.Format(L"%.2f MB/s",DiskUsageMBPerSec);    //  除以1024/1024/0.5
			mUserList.SetItemText(i,USERLIST_DISK,StrItem);
			((PROCLISTDATA*)pListData->pPData)->DiskIO = NewIO;


			
		}






		//-------------- Network-----------------

		if(COL_SAT_USER[USERLIST_NETWORK].Redraw)
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
			mUserList.SetItemText(i,USERLIST_NETWORK,StrItem);
		}


	//-------------------所有更新完成----------------------
	
		i++;


	}

	ReSort();
//	mUserList.SetRedraw(1);




	CString StrColheaderTemp;
	int FlagRedrawHeacerCtrl = 0;

	if(COL_SAT_USER[USERLIST_CPU].Redraw)
	{
		StrColheaderTemp.Format(L"%d%%",(int)theApp.PerformanceInfo.CpuUsage);
		StringCchCopy(COL_SAT_USER[USERLIST_CPU].StrItem,5,StrColheaderTemp);
		COL_SAT_USER[USERLIST_CPU].Percents = (float)theApp.PerformanceInfo.CpuUsage;
		FlagRedrawHeacerCtrl++;
	}

	if(COL_SAT_USER[USERLIST_MEMORY].Redraw)
	{


		StrColheaderTemp.Format(L"%d%%",(int)theApp.PerformanceInfo.MemoryUsage);
		StringCchCopy(COL_SAT_USER[USERLIST_MEMORY].StrItem,5,StrColheaderTemp);
		COL_SAT_USER[USERLIST_MEMORY].Percents = (float)theApp.PerformanceInfo.MemoryUsage;
		FlagRedrawHeacerCtrl++;
	}

	if(COL_SAT_USER[USERLIST_DISK].Redraw)
	{
		StrColheaderTemp.Format(L"%.0f%%",theApp.PerformanceInfo.TotalDiskUsage);
		StringCchCopy(COL_SAT_USER[USERLIST_DISK].StrItem,5,StrColheaderTemp);
		COL_SAT_USER[USERLIST_DISK].Percents = (float)theApp.PerformanceInfo.TotalDiskUsage;
		FlagRedrawHeacerCtrl++;
	}

	if(COL_SAT_USER[USERLIST_NETWORK].Redraw)
	{
		StrColheaderTemp.Format(L"%.0f%%",theApp.PerformanceInfo.TotalNetUsage );
		StringCchCopy(COL_SAT_USER[USERLIST_NETWORK].StrItem,5,StrColheaderTemp);
		COL_SAT_USER[USERLIST_NETWORK].Percents = (float)theApp.PerformanceInfo.TotalNetUsage;
		FlagRedrawHeacerCtrl++;
	}



	
	if(FlagRedrawHeacerCtrl!=0)
		this->mUserList.CoolheaderCtrl.Invalidate(0);

	return 0;


}

void CPageUsers::_SetMemUsage(PVOID pData, int nID,BOOL Update) // Update 表示强制更新不论是否有变化
{
	CProcess mProcInfo;

	PROCLISTDATA * pListData =( PROCLISTDATA * )pData ;

	double  MemUsage;
	CString StrItem;
	MemUsage = mProcInfo.GetWsPrivate_PDH(pListData);
	{

		pListData->Mem_PrivateWS = MemUsage;
	
		{
			//CString StrOut;
			StrItem.Format(L"%.1f",  MemUsage/1024/1024);
			//   wprintf(_T("%n KB"),MemUsage);
			//GetNumberFormat(LOCALE_USER_DEFAULT,LOCALE_NOUSEROVERRIDE,StrItem,NULL, StrOut.GetBuffer(MAX_PATH),MAX_PATH);
			//StrOut.ReleaseBuffer();
			//StrItem = StrOut;
			//StrItem=StrItem.Left(StrItem.GetLength()-3);
			StrItem=StrItem+L" MB";
		}
	

		mUserList.SetItemText(nID,USERLIST_MEMORY,StrItem);
	}		
}


void CPageUsers::ReSort(void)
{
	
	Sort(mUserList.CurrentSortColumn,FALSE);
}


void CPageUsers::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFormView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	if(pPopupMenu == NULL) return;

	pPopupMenu->SetDefaultItem(ID_USERSLIST_EXPAND);


	//----------------------
	int nSel = mUserList.GetNextItem( -1, LVNI_SELECTED );
	APPLISTDATA *pListData = (APPLISTDATA *)mUserList.GetItemData(nSel);
	CString Str;
	if(pListData->SubType == PARENT_ITEM_CLOSE)
	{			
		Str.LoadStringW(IDS_STRING_POPMENU_EXP);

	}

	else
	{
		Str.LoadStringW(IDS_STRING_POPMENU_COLL);
	}

	pPopupMenu->ModifyMenuW(ID_USERSLIST_EXPAND,MF_BYCOMMAND,ID_USERSLIST_EXPAND,Str);

 


	
}


void CPageUsers::OnNMRClickUserlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	CMenu PopMenu;
	CMenu *pMenu;
	PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_BASE) );
	

	CPoint CurPos ;
	GetCursorPos(&CurPos); 


	int  nSel = 	pNMItemActivate->iItem;
	APPLISTDATA *pData =(APPLISTDATA *)mUserList.GetItemData(nSel);
	if(pData == NULL )return ; 

	if(pData->SubType == SUB_ITEM)
	{
		pMenu = PopMenu.GetSubMenu(0);  //2是 这个 对应的 菜单 和标签顺序对应
		pMenu->DeleteMenu(ID_PROCESSESLIST_EXPAND,MF_BYCOMMAND);
		 
	}
	else
	{
		pMenu = PopMenu.GetSubMenu(2);  //2是 这个 对应的 菜单 和标签顺序对应
	
	}
	



	pMenu->TrackPopupMenu(TPM_LEFTALIGN,CurPos.x,CurPos.y,this);


	//---------------



	*pResult = 0;
}

void CPageUsers::DeleteProcessItem(PVOID pDetailsListData)
{
	int n = mUserList.GetItemCount();
 
	APPLISTDATA *pData = NULL;
	for(int i=0;i<n;i++)
	{
		pData=(APPLISTDATA * )mUserList.GetItemData(i);
		if( (pData==NULL)||(pDetailsListData==NULL) )continue ;
		if(pData->pPData==NULL)continue ;
		if(pData->SubType == SUB_ITEM )
		{				
			if(((PROCLISTDATA*)pData->pPData)->PID ==  ((PROCLISTDATA *)pDetailsListData)->PID)
			{
				mUserList.DeleteItem(i);
				break;
			}

		}
	}
}

BOOL CPageUsers::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	UINT ID= (UINT)wParam;
	switch(ID)
	{
	case ID_USERSLIST_EXPAND:
		this->PostMessageW(UM_DBCLICK_LSIT);
		break;
	case ID_USERS_ID:
	case ID_USERS_SESSION:
	case ID_USERS_CLIENTNAME:
	case ID_USERS_STATUS:
	case ID_USERS_CPU:
	case ID_USERS_MEMORY:
	case ID_USERS_DISK:
	case ID_USERS_NETWORK:
		{
			int iCol;

			CMenu PopMenu;
			CMenu *pMenu;
			PopMenu.LoadMenuW(MAKEINTRESOURCE( IDR_POPMENU_COLUMN) );
			pMenu = PopMenu.GetSubMenu(1);  //1是 这个 对应的 菜单 和标签顺序对应

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

			iCol = iClick+1;

			BOOL IsTurnColShow;
			IsTurnColShow =mUserList.ShowOrHideColumn(iCol);

			//if(IsTurnColShow)RefreshList();

			break;
		}





	}





	return CFormView::OnCommand(wParam, lParam);
}

void CPageUsers::OnUserslistManageUserAccounts()
{
	CString StrSys;
	::GetSystemDirectory(StrSys.GetBuffer(MAX_PATH),MAX_PATH);
	StrSys.ReleaseBuffer();
	StrSys =StrSys+L"\\control.exe";
	//MSB_S(StrSys) userpasswords
	
 
	ShellExecute(CWnd::GetDesktopWindow()->m_hWnd, L"open", StrSys,L"userpasswords" ,_T(""),SW_SHOW);
	 
}

void CPageUsers::Sort(int nCol,BOOL InvertSort)
{
	USERLISTDATA *pListData = NULL;
	int nCount = mUserList.GetItemCount();

	
//-------------------------------------------------
 
	int LastParentItemID = -1 ;
	nCount = mUserList.GetItemCount();
	for(int i= 0;i<nCount;i++)
	{
		pListData = (USERLISTDATA *) mUserList.GetItemData(i);
	 

		if(pListData->SubType == PARENT_ITEM_OPEN)
		{
			LastParentItemID = i;
		}	

		//由于子项很多列无数据所以 对于无数据列  排序时填充子项对应列 对于有数据的列则不要填充 
		//注意不可在排序过程中 因为那时id 可能已经变了
		if(pListData->SubType == SUB_ITEM && (nCol != USERLIST_USER)) 
		{
			if(nCol<USERLIST_CPU )
			{
			CString StrItemText;
			StrItemText=mUserList.GetItemText(LastParentItemID,nCol);
			mUserList.SetItemText(i,nCol,StrItemText);
			}
		}
		
		pListData->SortID = i;
	}


//-----------------------

	if(InvertSort)
	{
		if(mUserList.CurrentSortColumn == nCol)
		{
			mUserList.FlagSortUp = !mUserList.FlagSortUp;
		}
		else
		{
			mUserList.FlagSortUp = TRUE;
		}
	}




	mUserList.CurrentSortColumn =  nCol;


	//设置回调函数的参数和入口地址   
	mUserList.SortItems(Sort_Users, nCol);
}




void CPageUsers::OnPop_Endtask()
{
	
	int nSel = mUserList.GetNextItem( -1, LVNI_SELECTED );
	APPLISTDATA *pListData = (APPLISTDATA *)mUserList.GetItemData(nSel);
	CString Str;
	if(pListData->SubType == SUB_ITEM)
	{			
		
		if(theApp.Global_ShowOperateTip(((PROCLISTDATA*)(pListData->pPData))->Name,STR_ENDPROC_MAINTIP,STR_ENDPROC_CONTENT,STR_ENDPROC_BTN) ==IDOK)
		{
			TerminateProcess(((PROCLISTDATA*)(pListData->pPData))->hProcess, 4);
		}

	}
}

void CPageUsers::OnPop_Openfilelocation()
{
	APPLISTDATA *pData = NULL;
	int nSel = mUserList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mUserList.GetItemData(nSel);

	if(pData!=NULL)
	{
		if(pData->pPData ==NULL )return;

		CProcess mProcInfo;
		CString StrTargetPath = mProcInfo.GetPathName(((PROCLISTDATA *)(pData->pPData))->hProcess);
		OpenFileLocation(StrTargetPath);
	}
}

void CPageUsers::OnPop_Properties()
{
	APPLISTDATA *pData = NULL;
	int nSel = mUserList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mUserList.GetItemData(nSel);

	if(pData!=NULL)
	{
		if(pData->pPData ==NULL )return;
		CProcess mProcInfo;
		CString StrTargetPath = mProcInfo.GetPathName(((PROCLISTDATA *)(pData->pPData))->hProcess);
		OpenPropertiesDlg(StrTargetPath);
	}
	 

}

void CPageUsers::OnPop_Disconnect()
{
	APPLISTDATA *pData = NULL;
	int nSel = mUserList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mUserList.GetItemData(nSel);


	if(pData!=NULL)
	{
		if(pData->SubType!= SUB_ITEM)
		{
			CString StrSessionID  = mUserList.GetItemText(nSel,USERLIST_ID);

			int SessionID = _wtoi(StrSessionID);
			WTSDisconnectSession (WTS_CURRENT_SERVER_HANDLE ,SessionID,FALSE );

		}

	}

	
}

void CPageUsers::OnPop_Gotodetails()
{

	//由于允许多选 所以要先清除之前选中的 

	((CPageDetails*)pPageDetails)->ClearSelecet();
	


	APPLISTDATA *pData = NULL;
	int nSel = mUserList.GetNextItem( -1, LVNI_SELECTED );
	pData = (APPLISTDATA *)mUserList.GetItemData(nSel);


	if(pData!=NULL)
	{
		if(pData->pPData ==NULL )return;
		DWORD PID = ((PROCLISTDATA *)(pData->pPData))->PID;
		for(int i=0;i<pDetailList->GetItemCount();i++)
		{
			PROCLISTDATA * pDetailsData =(PROCLISTDATA *) pDetailList->GetItemData(i);
			if(pDetailsData!=NULL)
			{
				if(pDetailsData->PID == PID)
				{
					pDetailList->EnsureVisible(i,FALSE);
					pDetailList->SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
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

void CPageUsers::OnPop_Searchonline()
{

	int nSel = mUserList.GetNextItem( -1, LVNI_SELECTED );
	SearchOnline(mUserList.GetItemText(nSel,0));

}
