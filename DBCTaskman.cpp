// DBCTaskman.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "DBCTaskmanDlg.h"
#include <WinBase.h>


#include <Aclapi.h>
 
 

 



#define SE_MIN_WELL_KNOWN_PRIVILEGE     2;
#define SE_CREATE_TOKEN_PRIVILEGE     2;
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE    3;
#define SE_LOCK_MEMORY_PRIVILEGE       4;
#define SE_INCREASE_QUOTA_PRIVILEGE                     5;
#define SE_UNSOLICITED_INPUT_PRIVILEGE                     6;
#define SE_MACHINE_ACCOUNT_PRIVILEGE                     6;
#define SE_TCB_PRIVILEGE                     7;
#define SE_SECURITY_PRIVILEGE                     8;
#define SE_TAKE_OWNERSHIP_PRIVILEGE                     9;
#define SE_LOAD_DRIVER_PRIVILEGE                     10;
#define SE_SYSTEM_PROFILE_PRIVILEGE                     11;
#define SE_SYSTEMTIME_PRIVILEGE                     12;
#define SE_PROF_SINGLE_PROCESS_PRIVILEGE                     13;
#define SE_INC_BASE_PRIORITY_PRIVILEGE                     14;
#define SE_CREATE_PAGEFILE_PRIVILEGE                     15;
#define SE_CREATE_PERMANENT_PRIVILEGE                     16;
#define SE_BACKUP_PRIVILEGE                     17;
#define SE_RESTORE_PRIVILEGE                     18;
#define SE_SHUTDOWN_PRIVILEGE                     19;
#define SE_DEBUG_PRIVILEGE                     20;
#define SE_AUDIT_PRIVILEGE                     21;
#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE                     22;
#define SE_CHANGE_NOTIFY_PRIVILEGE                     23;
#define SE_REMOTE_SHUTDOWN_PRIVILEGE                     24;
#define SE_UNDOCK_PRIVILEGE                     25;
#define SE_SYNC_AGENT_PRIVILEGE                     26;
#define SE_ENABLE_DELEGATION_PRIVILEGE                     27;
#define SE_MANAGE_VOLUME_PRIVILEGE                     28;
#define SE_IMPERSONATE_PRIVILEGE                     29;
#define SE_CREATE_GLOBAL_PRIVILEGE                     30;
#define SE_INC_WORKING_SET_PRIVILEGE        (33L)








#ifdef _DEBUG
#define new DEBUG_NEW
#endif

 
static BOOL  EnableSpecificPrivilege(LPCTSTR lpPrivilegeName)
{

	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES Token_Privilege;
	BOOL bRet = TRUE;

	do 
	{
		if (0 == OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		{
			MessageBox(NULL,L"OpenProcessToken Error",NULL,MB_OK|MB_ICONSTOP);
			bRet = FALSE;
			break;
		}

		if (0 == LookupPrivilegeValue(NULL, lpPrivilegeName, &Token_Privilege.Privileges[0].Luid))
		{
			MessageBox(NULL,L"LookupPrivilegeValue Error",NULL,MB_OK|MB_ICONSTOP); 
			bRet = FALSE;
			break;
		}

		Token_Privilege.PrivilegeCount = 1;
		Token_Privilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//Token_Privilege.Privileges[0].Luid.LowPart=17;//SE_BACKUP_PRIVILEGE
		//Token_Privilege.Privileges[0].Luid.HighPart=0;


		if (0 == AdjustTokenPrivileges(hToken, FALSE, &Token_Privilege, sizeof(Token_Privilege), NULL,NULL))
		{
			
			MessageBox(NULL,L"AdjustTokenPrivileges Error",NULL,MB_OK|MB_ICONSTOP);

			bRet = FALSE;
			break;
		}

	} while (false);

	if (NULL != hToken)
	{
		CloseHandle(hToken);
	}

	return bRet;

}




//--------------------


static LONG CrashTip(EXCEPTION_POINTERS *pException)   
{      
    // 在这里添加处理程序崩溃情况的代码     //   
  
    // 这里以弹出一个对话框为例子   
    //  
	CString  StrError =L"whoops!";

	//StrError.Format(L"Error!   %d",::GetLastError());

	MessageBox(NULL, StrError, _T("DBC Task Manager"), MB_OK);     
    
	 
    return EXCEPTION_EXECUTE_HANDLER;   
}   










// CDBCTaskmanApp

BEGIN_MESSAGE_MAP(CDBCTaskmanApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDBCTaskmanApp construction

CDBCTaskmanApp::CDBCTaskmanApp()
: CurrentPID(0)
, nAppWnd(0)
, nBkgWnd(0)
, UpTimeSec(0)
, UpTimeMin(0)
, UpTimeHour(0)
, UpTimeDay(0)

, StartPerformancePageTimer(FALSE)
, pButtonFOM(NULL)
, FlagWindowStatus(0)
, pMainTab(NULL)
, pSelPage(NULL)
, MenuAndTabHeight(0)
, StrCfgFileName(_T(""))

, FlagThemeActive(TRUE)
, FlagIsX64(FALSE)
, FlagIsAdminNow(FALSE)
, FlagSummaryView(FALSE)
, StrAppFullPath(_T(""))
, StrCfgToolsPath(_T(""))
, FlagSysIs32Bit(FALSE)
, IsChineseEdition(FALSE)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDBCTaskmanApp object

CDBCTaskmanApp theApp;

map<DWORD,PVOID> Map_PidToData;
map<CWnd*,int> HungWndMap;
map<int, int> NetAdapterList;


// CDBCTaskmanApp initialization

BOOL CDBCTaskmanApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.


		




	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();





	AfxEnableControlContainer();




	HRESULT hres = 0;  
  
 //  hres = CoInitializeEx(0,  COINIT_APARTMENTTHREADED    );   //COINIT_MULTITHREADED  

	CoInitializeEx(0,  COINIT_APARTMENTTHREADED    );   //COINIT_MULTITHREADED
 //  hres = CoInitialize(0);   //
    if (hres!=S_OK)  
    {  
       AfxMessageBox(L"Failed to initialize COM library. "   );
       
    }  





	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	 SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	



 


	 //-------------------------------------------
	 InitAll();
	 //-----------------------------



//======================================================


//              以下实现只运行一个实例
//              配合 主对话框  OnCreate  OnDestory 

	 if( AppSettings.OnlyOneInstance )
	 {
		 if( IsInstanceExist()) {return FALSE;}
	 }

	

//==========================================================================











//	 if( !_IsAdministratorNow()) 
	 {

		// MSB(0);
		//  ElevateCurrentProcess(L"Administrator"); //NT AUTHORITY\SYSTEM"
		  
		//  return 0;

	 }

   



 
	

	CDBCTaskmanDlg dlg;
	 
	dlg.MoveWindow(theApp.AppSettings.rcWnd_Simple);

	m_pMainWnd = &dlg;	
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CDBCTaskmanApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class


	SaveAppSettings();


	CoUninitialize();  
	GdiplusShutdown(m_gdiplusToken);
	return CWinApp::ExitInstance();
}


/*
WriteProfileBinary	Writes binary data to an entry in the application's .INI file.
WriteProfileInt    	Writes an integer to an entry in the application's .INI file.
WriteProfileString	Writes a string to an entry in the application's .INI file.

GetProfileBinary	Retrieves binary data from an entry in the application's .INI file.
GetProfileInt	Retrieves an integer from an entry in the application's .INI file.
GetProfileString

*/

void CDBCTaskmanApp::LoadAppSettings(BOOL ReLoad)
{
	 
  
	BOOL Ret;

	if(!ReLoad)
	{
		AppSettings.TimerStep = 1.0 ;
		AppSettings.ProcessorDisplayMode = 0;
		AppSettings.ShowKernelTime = FALSE;	 

		AppSettings.ActiveTab=0;
		AppSettings.TaskManMode=0; //默认简洁模式

		AppSettings.PerformanceListShowGraph = 1;

		AppSettings.GroupByType = TRUE;
		//-----------------
		AppSettings.ProcList_MemPercents=0;
		AppSettings.ProcList_DiskPercents=0;
		AppSettings.ProcList_NetPercents=0;

		AppSettings.UserList_MemPercents=0;
		AppSettings.UserList_DiskPercents=0;
		AppSettings.UserList_NetPercents=0;
	}

	//-------------------------

	CFile CfgFile;
	Ret = CfgFile.Open(StrCfgFileName,CFile::modeRead);

	//-------------------------



	
	int i;
	 
	if(Ret)
	{

			WCHAR Mark[] =L"DBCSTUDIOTASKMAN";
			WCHAR Mark2[] =L"DDDDDDDDDDDDDDDD";
			DWORD  DataVer;

			
			


			CfgFile.Read(&Mark2,sizeof(Mark) 	);	
			if(ReLoad)
			{
				APPSETTINGS SettingsReload;
				CfgFile.Read(&SettingsReload,sizeof(APPSETTINGS));
				if(SettingsReload.Ver!= CFG_DATA_VER || (lstrcmp(Mark2,Mark)!=0) ) //防止数据错乱！！！
				{
					return;
				}
				memcpy(&AppSettings.CpuColor,&SettingsReload.CpuColor,sizeof(GRAPHCOLOR));
				memcpy(&AppSettings.MemoryColor,&SettingsReload.MemoryColor,sizeof(GRAPHCOLOR));
				memcpy(&AppSettings.DiskColor,&SettingsReload.DiskColor,sizeof(GRAPHCOLOR));			
				memcpy(&AppSettings.NetworkColor,&SettingsReload.NetworkColor,sizeof(GRAPHCOLOR));
				CfgFile.Close();
				return;

			}
			else
			{
				CfgFile.Read(&AppSettings,sizeof(APPSETTINGS));
			}
			CfgFile.Close();

			DataVer = AppSettings.Ver;
			if(DataVer!= CFG_DATA_VER || (lstrcmp(Mark2,Mark)!=0) ) //防止数据错乱！！！
			{
				
					goto LOADDEFAULT;
				
			}
		
		 

	}
	else //载入失败设为默认值
	{
LOADDEFAULT:
		AppSettings.rcWnd.SetRect(0,0,690,590);
		AppSettings.rcWnd_Simple.SetRect(0,0,490,515);
		AppSettings.TimerStep = 1.0 ; 


		//CPU
		AppSettings.CpuColor.BackgroundColor=RGB(255,255,255);
		AppSettings.CpuColor.BorderColor = RGB(17,125,187);
		AppSettings.CpuColor.GridColorRGB = RGB(17,125,187);
		AppSettings.CpuColor.GridColorAlpha = 30;
		AppSettings.CpuColor.LineAColor =  Gdiplus::Color(255,17,125,187);
		AppSettings.CpuColor.LineBColor =  Gdiplus::Color(255,17,125,187);
		AppSettings.CpuColor.FillAColor =  Gdiplus::Color(20,17,125,187);
		AppSettings.CpuColor.FillBColor =  Gdiplus::Color(16,17,125,187);

		//Mem
		AppSettings.MemoryColor.BackgroundColor=RGB(255,255,255);
		AppSettings.MemoryColor.BorderColor = RGB(139,18,174);
		AppSettings.MemoryColor.GridColorRGB = RGB(139,18,174);
		AppSettings.MemoryColor.GridColorAlpha = 30;
		AppSettings.MemoryColor.LineAColor =  Gdiplus::Color(255,139,18,174);
		AppSettings.MemoryColor.LineBColor =  Gdiplus::Color(255,139,18,174);
		AppSettings.MemoryColor.FillAColor =  Gdiplus::Color(20,139,18,174);
		AppSettings.MemoryColor.FillBColor =  Gdiplus::Color(16,139,18,174);

		//Disk

		AppSettings.DiskColor.BackgroundColor=RGB(255,255,255);
		AppSettings.DiskColor.BorderColor = RGB(77,166,12);
		AppSettings.DiskColor.GridColorRGB = RGB(77,166,12);
		AppSettings.DiskColor.GridColorAlpha = 30;
		AppSettings.DiskColor.LineAColor =  Gdiplus::Color(255,77,166,12);
		AppSettings.DiskColor.LineBColor =  Gdiplus::Color(255,77,166,12);
		AppSettings.DiskColor.FillAColor =  Gdiplus::Color(20,77,166,12);
		AppSettings.DiskColor.FillBColor =  Gdiplus::Color(16,77,166,12);

		//Net
		AppSettings.NetworkColor.BackgroundColor=RGB(255,255,255);
		AppSettings.NetworkColor.BorderColor = RGB(167,79,1);
		AppSettings.NetworkColor.GridColorRGB = RGB(167,79,1);
		AppSettings.NetworkColor.GridColorAlpha = 30;
		AppSettings.NetworkColor.LineAColor =  Gdiplus::Color(255,167,79,1);
		AppSettings.NetworkColor.LineBColor =  Gdiplus::Color(255,167,79,1);
		AppSettings.NetworkColor.FillAColor =  Gdiplus::Color(20,167,79,1);
		AppSettings.NetworkColor.FillBColor =  Gdiplus::Color(16,167,79,1);


	
		//----------------

		AppSettings.ProcessorDisplayMode = 0;	
		AppSettings.ShowKernelTime = FALSE;	 
		AppSettings.PerformanceListShowGraph = 1;

		AppSettings.TopMost = FALSE;
		AppSettings.MiniOnUse = FALSE;
		AppSettings.OnlyOneInstance = TRUE;


		for( i = 0;i< COL_COUNT_PROC;i++)
		{
			AppSettings.ColIDS_ProcList[i].ColWidth =0 ;
			AppSettings.ColIDS_ProcList[i].IsHiddenColumn =TRUE ;
			AppSettings.ColIDS_ProcList[i].Redraw = 0;
			AppSettings.ColIDS_ProcList[i].Align = (BYTE)DT_LEFT;
			if(i>=7)//黄色
			{
				AppSettings.ColIDS_ProcList[i].Cool=TRUE;
				AppSettings.ColIDS_ProcList[i].Align = (BYTE)DT_RIGHT;
				
			}
			if(i==PROCLIST_PID )AppSettings.ColIDS_ProcList[i].Align = (BYTE)DT_RIGHT;

		}
		for( i = 0;i< COL_COUNT_USER ;i++)
		{
			AppSettings.ColIDS_UserList[i].ColWidth =0 ;
			AppSettings.ColIDS_UserList[i].Redraw = 0;
			AppSettings.ColIDS_UserList[i].IsHiddenColumn =TRUE ;
			AppSettings.ColIDS_UserList[i].Align = (BYTE)DT_LEFT;

			if(i>=5)//黄色
			{
				AppSettings.ColIDS_UserList[i].Cool=TRUE;
				AppSettings.ColIDS_UserList[i].Align = (BYTE)DT_RIGHT;
				
			}

		}
		for( i = 0;i< COL_COUNT_DETAIL;i++)
		{
			AppSettings.ColIDS_DetailList[i].ColWidth=0;
			AppSettings.ColIDS_DetailList[i].Redraw=0;
		 
				

			if(i>=DETAILLIST_CPU && i<=DETAILLIST_IO_OB)
			{
				AppSettings.ColIDS_DetailList[i].Align = (BYTE)DT_RIGHT;
			}
			else
			{
				AppSettings.ColIDS_DetailList[i].Align = (BYTE)DT_LEFT;
			}
			 
		}

		//默认的显示项

		// P1-process
		COL_SAT_PROC[PROCLIST_NAME].ColWidth= COL_SAT_PROC[PROCLIST_STATUS].ColWidth=COL_SAT_PROC[PROCLIST_CPU].ColWidth = COL_SAT_PROC[PROCLIST_MEMORY].ColWidth= COL_SAT_PROC[PROCLIST_DISK].ColWidth = COL_SAT_PROC[PROCLIST_NETWORK].ColWidth=80;
		COL_SAT_PROC[PROCLIST_NAME].ColWidth= 200;
		COL_SAT_PROC[PROCLIST_NAME].Redraw= COL_SAT_PROC[PROCLIST_STATUS].Redraw=COL_SAT_PROC[PROCLIST_CPU].Redraw = COL_SAT_PROC[PROCLIST_MEMORY].Redraw= COL_SAT_PROC[PROCLIST_DISK].Redraw = COL_SAT_PROC[PROCLIST_NETWORK].Redraw=1;
        COL_SAT_PROC[PROCLIST_NAME].IsHiddenColumn= COL_SAT_PROC[PROCLIST_STATUS].IsHiddenColumn=COL_SAT_PROC[PROCLIST_CPU].IsHiddenColumn = COL_SAT_PROC[PROCLIST_MEMORY].IsHiddenColumn= COL_SAT_PROC[PROCLIST_DISK].IsHiddenColumn = COL_SAT_PROC[PROCLIST_NETWORK].IsHiddenColumn= FALSE;
		

		// P3-User
		COL_SAT_USER[USERLIST_USER].ColWidth  = COL_SAT_USER[USERLIST_STATUS].ColWidth = COL_SAT_USER[USERLIST_CPU].ColWidth= COL_SAT_USER[USERLIST_MEMORY].ColWidth=COL_SAT_USER[USERLIST_DISK].ColWidth=COL_SAT_USER[USERLIST_NETWORK].ColWidth=80;
		COL_SAT_USER[USERLIST_USER].ColWidth = 200;
		COL_SAT_USER[USERLIST_USER].Redraw  = COL_SAT_USER[USERLIST_STATUS].Redraw = COL_SAT_USER[USERLIST_CPU].Redraw= COL_SAT_USER[USERLIST_MEMORY].Redraw=COL_SAT_USER[USERLIST_DISK].Redraw=COL_SAT_USER[USERLIST_NETWORK].Redraw=1;
		COL_SAT_USER[USERLIST_USER].IsHiddenColumn  = COL_SAT_USER[USERLIST_STATUS].IsHiddenColumn = COL_SAT_USER[USERLIST_CPU].IsHiddenColumn= COL_SAT_USER[USERLIST_MEMORY].IsHiddenColumn=COL_SAT_USER[USERLIST_DISK].IsHiddenColumn=COL_SAT_USER[USERLIST_NETWORK].IsHiddenColumn=FALSE;


		// P4-DETAILS
		COL_SAT_DETAIL[DETAILLIST_NAME].ColWidth  = COL_SAT_DETAIL[DETAILLIST_TYPE].ColWidth = COL_SAT_DETAIL[DETAILLIST_PID].ColWidth= COL_SAT_DETAIL[DETAILLIST_STATUS].ColWidth=COL_SAT_DETAIL[DETAILLIST_USER].ColWidth=COL_SAT_DETAIL[DETAILLIST_CPU].ColWidth=COL_SAT_DETAIL[DETAILLIST_MEM_PWS].ColWidth=COL_SAT_DETAIL[DETAILLIST_DESCRIPTION].ColWidth=123;
		COL_SAT_DETAIL[DETAILLIST_NAME].Redraw  = COL_SAT_DETAIL[DETAILLIST_TYPE].Redraw = COL_SAT_DETAIL[DETAILLIST_PID].Redraw= COL_SAT_DETAIL[DETAILLIST_STATUS].Redraw=COL_SAT_DETAIL[DETAILLIST_USER].Redraw=COL_SAT_DETAIL[DETAILLIST_CPU].Redraw=COL_SAT_DETAIL[DETAILLIST_MEM_PWS].Redraw=COL_SAT_DETAIL[DETAILLIST_DESCRIPTION].Redraw=1;
 

		
	}

	//------------------------


	//永远显示的项

	COL_SAT_PROC[PROCLIST_NAME].Redraw = COL_SAT_USER[USERLIST_USER].Redraw = COL_SAT_DETAIL[DETAILLIST_NAME].Redraw = 1;



	//安装的物理内存总数

	ULONGLONG  TotalMemoryInKilobytes;

	API_GETINSTALLMEM GetPhysicallyInstalledSystemMemory ;

	
	HINSTANCE hDll= ::LoadLibrary(L"Kernel32.dll");	

	if(hDll!=NULL)
	{
		GetPhysicallyInstalledSystemMemory = NULL; 
		GetPhysicallyInstalledSystemMemory=(API_GETINSTALLMEM)GetProcAddress(hDll, "GetPhysicallyInstalledSystemMemory");
		FreeLibrary(hDll); 
	}




    Ret = GetPhysicallyInstalledSystemMemory(&TotalMemoryInKilobytes);
 

	if(Ret)
	{
		PerformanceInfo.InstalledMemKB = TotalMemoryInKilobytes;
	}
	else
	{
		MEMORYSTATUSEX  MemStatus;
		MemStatus.dwLength = sizeof(MemStatus);
		BOOL Ret = GlobalMemoryStatusEx(&MemStatus);

		if(Ret)
		{
			theApp.PerformanceInfo.TotalPhysMem = MemStatus.ullTotalPhys;
		}
		PerformanceInfo.InstalledMemKB = PerformanceInfo.TotalPhysMem/1024;
	}



	theApp.PerformanceInfo.TotalNetUsage = 0;
	theApp.PerformanceInfo.TotalDiskUsage = 0;
	



	//-------------------



//	AppSettings.CpuColor.LineColor=GetPrivateProfileInt(L"CPU",L"Border",0x0,L"Color.ini");
 //
	 

	

	



}

void CDBCTaskmanApp::SaveAppSettings(void)
{
	

	CFile CfgFile;
	BOOL Ret = CfgFile.Open(StrCfgFileName,CFile::modeCreate |CFile::modeReadWrite );

	if(Ret)
	{
		WCHAR Mark[] =L"DBCSTUDIOTASKMAN";
		
		AppSettings.Ver = CFG_DATA_VER;

		CfgFile.Write(Mark,sizeof(Mark));	
		CfgFile.Write(&AppSettings,sizeof(APPSETTINGS));
		

		CfgFile.Close();



	}


	//WriteProfileBinary(L"",L"AppSettings",(LPBYTE)&AppSettings,sizeof(AppSettings));



}

void CDBCTaskmanApp::_UpgradeRights(void)
{


 
	MyAdjustPrivilege RtlAdjustPrivilege = (MyAdjustPrivilege)GetProcAddress(GetModuleHandle(L"ntdll.dll"),"RtlAdjustPrivilege");
  
 
	BOOLEAN s;
	 RtlAdjustPrivilege(0x14 ,1,0,&s);
 

	 FlagIsAdminNow = _IsAdministratorNow();


//------------







	/*



	 HANDLE tokenHandle;



    if (NT_SUCCESS(PhOpenProcessToken(&tokenHandle,TOKEN_ADJUST_PRIVILEGES,NtCurrentProcess())))
    {
        CHAR privilegesBuffer[FIELD_OFFSET(TOKEN_PRIVILEGES, Privileges) + sizeof(LUID_AND_ATTRIBUTES) * 8];
        PTOKEN_PRIVILEGES privileges;
        ULONG i;

        privileges = (PTOKEN_PRIVILEGES)privilegesBuffer;
        privileges->PrivilegeCount = 8;

        for (i = 0; i < privileges->PrivilegeCount; i++)
        {
            privileges->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
            privileges->Privileges[i].Luid.HighPart = 0;
        }

        privileges->Privileges[0].Luid.LowPart = SE_DEBUG_PRIVILEGE;
        privileges->Privileges[1].Luid.LowPart = SE_INC_BASE_PRIORITY_PRIVILEGE;
        privileges->Privileges[2].Luid.LowPart = SE_INC_WORKING_SET_PRIVILEGE;
        privileges->Privileges[3].Luid.LowPart = SE_LOAD_DRIVER_PRIVILEGE;
        privileges->Privileges[4].Luid.LowPart = SE_PROF_SINGLE_PROCESS_PRIVILEGE;
        privileges->Privileges[5].Luid.LowPart = SE_RESTORE_PRIVILEGE;
        privileges->Privileges[6].Luid.LowPart = SE_SHUTDOWN_PRIVILEGE;
        privileges->Privileges[7].Luid.LowPart = SE_TAKE_OWNERSHIP_PRIVILEGE;


		MyNtAdjustPrivilegeToken   NtAdjustPrivilegesToken;
		NtAdjustPrivilegesToken = (MyNtAdjustPrivilegeToken)GetProcAddress(GetModuleHandle(L"ntdll"),"NtAdjustPrivilegesToken");
		MyNtClose NtClose = (MyNtClose)GetProcAddress(GetModuleHandle(L"ntdll"),"NtClose");
		 

        NtAdjustPrivilegesToken(
            tokenHandle,
            FALSE,
            privileges,
            0,
            NULL,
            NULL
            );

        NtClose(tokenHandle);
    }*/




	// DuplicateTokenEx


 



 
}

BOOL  CDBCTaskmanApp::ElevateCurrentProcess(CString  StrCmdLine)
{
	// IFileOperation ifile;


//return 0;

	 

//-------------------------------------

	TCHAR szPath[MAX_PATH] = {0};

	if (::GetModuleFileName(NULL, szPath, MAX_PATH))

	{

		// Launch itself as administrator.

		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };

		sei.lpVerb = _T("runas");

		sei.lpFile = szPath;

		sei.lpParameters = (LPCTSTR)StrCmdLine;

		// sei.hwnd = hWnd;

		sei.nShow = SW_SHOWNORMAL;



		if (!ShellExecuteEx(&sei))

		{

			DWORD dwStatus = GetLastError();

			if (dwStatus == ERROR_CANCELLED)

			{

				// The user refused to allow privileges elevation.

				return FALSE;

			}

			else

				if (dwStatus == ERROR_FILE_NOT_FOUND)

				{

					// The file defined by lpFile was not found and

					// an error message popped up.

					return FALSE;

				}



				return FALSE;



}

return TRUE;

}

return FALSE;
}

BOOL CDBCTaskmanApp::_IsAdministratorNow(void)
{
 
	BOOL bIsElevated = FALSE;
	HANDLE hToken = NULL;
	UINT16 uWinVer = LOWORD(GetVersion());
	uWinVer = MAKEWORD(HIBYTE(uWinVer),LOBYTE(uWinVer));

	if (uWinVer < 0x0600) //不是VISTA、Windows7
		return(FALSE);

	if (OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hToken))
	{

		struct {
			DWORD TokenIsElevated;
		} /*TOKEN_ELEVATION*/te;
		DWORD dwReturnLength = 0;

		if (GetTokenInformation(hToken,/*TokenElevation*/(_TOKEN_INFORMATION_CLASS)20,&te,sizeof(te),&dwReturnLength)) {
			if (dwReturnLength == sizeof(te))
				bIsElevated = te.TokenIsElevated;
		}
		CloseHandle( hToken );
	}
	return bIsElevated;
 
}

void CDBCTaskmanApp::InitAll(void)
{
	
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CrashTip);
	 	

	//----------------------测试是否 为x64版-------------------
	
	LPVOID   Test;
	int nTest= sizeof(Test);   //x64 下nTest == 8 x86 ==4;
	if(nTest == 8)   
	{
		FlagIsX64 = TRUE;		
	}
	else
	{
		FlagIsX64=FALSE;
	}
	 

	// ----------- 测试是否为中文版 导出 html 或 文本文件 及一些显示需要根据语言版本不同处理 ----------- 
	CString StrCharCode;
	StrCharCode.LoadString(STR_CHARCODE);
	IsChineseEdition =(StrCharCode.CompareNoCase(L"gb18030")==0);







	 //--------------  Set Full Path name for Config file  ------------------
	 DWORD BuffSize=MAX_PATH;

	 GetModuleFileName(NULL,StrAppFullPath.GetBuffer(BuffSize),BuffSize);  
	 StrAppFullPath.ReleaseBuffer();

	 CString StrTemp;
	 StrCfgFileName = StrAppFullPath;
	 StrTemp = StrAppFullPath;

	 ::ExtractIconEx(StrAppFullPath,0,&hBigIcon,NULL,1);
	 StrTemp = StrCfgFileName.Left(StrCfgFileName.ReverseFind(L'\\'));
	 StrCfgFileName = StrTemp + L"\\DBCTaskman.CFG";
	 StrCfgToolsPath = StrTemp+L"\\DTMCFG.exe";

	 WCHAR WcWow64Path[MAX_PATH];
	 FlagSysIs32Bit = FALSE;
	 if ( GetSystemWow64Directory(WcWow64Path,MAX_PATH) == 0)
	 {
		 if(GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) FlagSysIs32Bit = TRUE;
	 }



	 


	  //---------------获取  逻辑处理器数量 ---------------
		
	 SYSTEM_INFO sysInfo;
	 GetSystemInfo(&sysInfo);
	 LogicalProcessorsCount = sysInfo.dwNumberOfProcessors;

 



	 //   MSB_S(StrCfgFileName)


	 LoadAllString();


	 _UpgradeRights();


	 LoadAppSettings();


	//---------------------- 打开列表主题 ------------------------
	//---------------这个程序主要用到这个 所以在此一次性打开-----------------
	FlagThemeActive  = IsThemeActive();
	hTheme=OpenThemeData(NULL, L"Explorer::ListView"); // ◆ 如果想自绘这个效果 就不能再设置SetWindowTheme(mProcessList.GetSafeHwnd(),L"explorer", NULL);了 ！！！


	//------------------------

	SHFILEINFO sfi;
	HIMAGELIST himlSmall;

	CString  StrSysDir;
	GetWindowsDirectory(StrSysDir.GetBuffer(MAX_PATH), MAX_PATH);
	StrSysDir.ReleaseBuffer();	
	himlSmall = (HIMAGELIST)SHGetFileInfo(StrSysDir, 0, &sfi, sizeof(SHFILEINFO),SHGFI_SYSICONINDEX | SHGFI_SMALLICON |SHGFI_ICON   );

		
	if(himlSmall != NULL)
	{	
		mImagelist.Attach(himlSmall);
	}

	//-----------特殊图标载入----------------------
	HICON hIcon = theApp.LoadIcon( MAKEINTRESOURCE(IDI_SVCHOST) );
	mImagelist.Add(hIcon);
	DestroyIcon(hIcon);

	//-----------------------------------------


	HINSTANCE hDll= ::LoadLibrary(L"Kernel32.dll");
	GetProcessDEPPolicy = NULL; 
	GetProcessDEPPolicy=(MYAPIGETPROCDEP)GetProcAddress(hDll, "GetProcessDEPPolicy");
	
	FreeLibrary(hDll); 

	 


//-------------------------------

	
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

//-----------get system colors ---------------------

	SetUIColor();
//-----------------------------------------------------

	PerformanceInfo.ProcessCount =0;



	CurrentPID = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess( PROCESS_SET_INFORMATION     ,FALSE, CurrentPID);  //注意 PPROCESS_SET_INFORMATION 要设置优先级 要这个权限
	BOOL Ret=SetPriorityClass( hProcess, HIGH_PRIORITY_CLASS );
	CloseHandle(hProcess);	


	UpTimeSec= 0;
    UpTimeMin = UpTimeHour = UpTimeDay=0;


//-------------------------

	
	 CString StrFontName;

	 StrFontName.LoadStringW(IDS_FONTNAME);
	 mTitleFont.CreateFont(17,   
						 0,                         // nWidth
						 0,                         // nEscapement
						 0,                         // nOrientation
						 FW_NORMAL,                 // nWeight     FW_NORMAL,     FW_BOLD
						 FALSE,                     // bItalic
						 FALSE,                     // bUnderline下划线标记，需要下划线把这里设置成TRUE
						 0,                         // cStrikeOut
						 DEFAULT_CHARSET,              // nCharSet
						 OUT_DEFAULT_PRECIS,        // nOutPrecision
						 CLIP_DEFAULT_PRECIS,       // nClipPrecision
						 DEFAULT_QUALITY,           // nQuality
						 DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
						 StrFontName);                 // lpszFacename





}

void CDBCTaskmanApp::SetUIColor(void)
{
	WndBkgColor = ::GetSysColor(COLOR_WINDOW );
	WndTextColor = ::GetSysColor(COLOR_WINDOWTEXT );
	WndFrameColor = ::GetSysColor(COLOR_WINDOWFRAME );
	DisableTextColor=::GetSysColor(COLOR_GRAYTEXT  );

	if(FlagThemeActive)
	{
		GetThemeColor(theApp.hTheme,LVP_LISTITEM,LISS_HOTSELECTED,TMT_TEXTCOLOR ,&List_NormalTextColor);
		GetThemeColor(theApp.hTheme,LVP_LISTITEM,LISS_NORMAL,TMT_TEXTCOLOR ,&List_HotTextColor);

		//header
		HTHEME hTheme = OpenThemeData(NULL, L"TEXTSTYLE"); 	
		GetThemeColor(hTheme,TEXT_HYPERLINKTEXT,TS_HYPERLINK_NORMAL,TMT_TEXTCOLOR ,&CoolHdrColor);		
		::CloseThemeData(hTheme);

		hTheme = OpenThemeData(NULL, L"EXPLORERBAR"); 
		GetThemeColor(hTheme,EBP_NORMALGROUPBACKGROUND,0,TMT_TEXTCOLOR ,&GroupTitleTextColor);
		::CloseThemeData(hTheme);





	}
	else
	{
		List_NormalTextColor = ::GetSysColor(COLOR_WINDOWTEXT );
		List_HotTextColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT );

		CoolHdrColor = WndBkgColor;
	}

	

	if(BkgBrush.m_hObject!=NULL)
	{
		BkgBrush.DeleteObject();
		BkgBrush.CreateSysColorBrush(COLOR_WINDOW);
			 
	}
	else
	{
		BkgBrush.CreateSolidBrush(WndBkgColor);
	}
 

}


void CDBCTaskmanApp::LoadAllString(void)
{
	STR_FONTNAME.LoadString(IDS_STR_FONTNAME);

	MAIN_CAPTION.LoadString(IDS_MAIN_CAPTION);
	STR_GROUP_APP.LoadString(IDS_STR_GROUP_APP);
	STR_GROUP_BKG.LoadString(IDS_STR_GROUP_BKG);
	STR_GROUP_WIN.LoadString(IDS_STR_GROUP_WIN);
	//-----------Cpu
	STR_CPUINFO_0 .LoadString(IDS_STR_CPUINFO_0);
	STR_CPUINFO_1.LoadString(IDS_STR_CPUINFO_1);
	STR_CPUINFO_2.LoadString(IDS_STR_CPUINFO_2);
	STR_CPUINFO_3.LoadString(IDS_STR_CPUINFO_3);
	STR_CPUINFO_4 .LoadString(IDS_STR_CPUINFO_4);
	STR_CPUINFO_5.LoadString(IDS_STR_CPUINFO_5);
	STR_CPUINFO_6 .LoadString(IDS_STR_CPUINFO_6);
	STR_TIP1_CPU_TOTAL.LoadString(IDS_STR_TIP1_CPU_TOTAL);
	STR_TIP1_CPU_LOGICAL.LoadString(IDS_STR_TIP1_CPU_LOGICAL);
	STR_TIP1_CPU_NUMA.LoadString(IDS_STR_TIP1_CPU_NUMA);

	//------------Memory
	STR_MEMINFO_0.LoadString(IDS_STR_MEMINFO_0);
	STR_MEMINFO_1.LoadString(IDS_STR_MEMINFO_1);
	STR_MEMINFO_2.LoadString(IDS_STR_MEMINFO_2);
	STR_MEMINFO_3.LoadString(IDS_STR_MEMINFO_3);
	STR_MEMINFO_4.LoadString(IDS_STR_MEMINFO_4);
	STR_MEMINFO_5.LoadString(IDS_STR_MEMINFO_5);
	STR_MEMINFO_6.LoadString(IDS_STR_MEMINFO_6);
	STR_TIP3_MEMORY.LoadString(IDS_STR_TIP3_MEMORY);
	STR_TIP5_MEMORY.LoadString(IDS_STR_TIP5_MEMORY);
	//----------Disk
	STR_DISKINFO_0.LoadString(IDS_STR_DISKINFO_0);
	STR_DISKINFO_1.LoadString(IDS_STR_DISKINFO_1);
	STR_DISKINFO_2.LoadString(IDS_STR_DISKINFO_2);
	STR_DISKINFO_3.LoadString(IDS_STR_DISKINFO_3);
	STR_DISKINFO_6.LoadString(IDS_STR_DISKINFO_6);

	STR_TIP5_DISK.LoadString(IDS_STR_TIP5_DISK);
	//-------------Network
	STR_NETWORKINFO_0.LoadString(IDS_STR_NETWORKINFO_0);
	STR_NETWORKINFO_2.LoadString(IDS_STR_NETWORKINFO_2);
	STR_NETWORKINFO_6.LoadString(IDS_STR_NETWORKINFO_6);
	STR_TIP3_NETWORK.LoadString(IDS_STR_TIP3_NETWORK);

	//---------  properties dialog  of Process -----------

	STR_PROPERTIESDLG_CAPTION.LoadString(IDS_STR_PROPERTIESDLG_CAPTION);
}

BOOL CDBCTaskmanApp::IsInstanceExist(void)
{
	 


	// HANDLE m_hMutex = CreateMutex(NULL, FALSE, L"DBC_TASKMAN_MUTE"); 

	 HANDLE hSem=CreateSemaphore(NULL,1,1,L"DBC_TASKMAN_001"); 

	 if(hSem) //信标对象创建成功。 
	 { 		  
		 if(ERROR_ALREADY_EXISTS==GetLastError()) //信标对象已经存在，则程序已有一个实例在运行。
		 { 
			 CloseHandle(hSem); //关闭信号量句柄。 

			 //获取桌面窗口的一个子窗口。 
			 HWND hWndPrev=::GetWindow(::GetDesktopWindow(),GW_CHILD); 
			 while(::IsWindow(hWndPrev)) 
			 { 				 
				 if(::GetProp(hWndPrev,L"DBC_TASKMAN_001")) //判断窗口是否有我们预先设置的标记，如有，则是我们寻找的窗口，并将它激活。 
				 {					
					 if (::IsIconic(hWndPrev)) {::ShowWindow(hWndPrev,SW_RESTORE);} 	 //如果主窗口已最小化，则恢复其大小。 				 
					 ::SetForegroundWindow(hWndPrev); //将应用程序的主窗口激活。 
					 return TRUE; //返回TRUE代表 实例已经运行   退出实例。 
				 } 			
				 hWndPrev = ::GetWindow(hWndPrev,GW_HWNDNEXT); 	 //继续寻找下一个窗口。 
			 } 
			// AfxMessageBox("已有一个实例在运行，但找不到它的主窗口！"); 
		 } 
	 } 
	 else 
	 { 
		 AfxMessageBox(L"Error: CreateSemaphore()"); 
		 return TRUE; 
	 } 

	return FALSE;
}

int CDBCTaskmanApp::Global_ShowOperateTip(CString StrProcessName, UINT MainTipStrID,  UINT ContentStrID,  UINT ButtonStrID, UINT GrayBtnID, CString StrCheckBox)
{
	CString StrCaption,StrMainTip ,StrContent,StrBtnOK,StrBtnCancel,StrTemp,StrBtnAll;	


	StrBtnAll.LoadStringW(ButtonStrID);
	AfxExtractSubString(StrBtnOK,StrBtnAll,0,L';');
	AfxExtractSubString(StrBtnCancel,StrBtnAll,1,L';');	
	StrTemp.LoadStringW(MainTipStrID);
	StrContent.LoadStringW(ContentStrID);
	
	//注意 StrTipTitle 带有%s格式 但是这样直接用一个变量可能会出现 内存冲突导致崩溃问题 _debugger_hook_dummy = 0;所以不要直接用一个变量 尤其 后面那个 变量
	StrMainTip.Format(StrTemp,StrProcessName);


	theApp.m_pMainWnd->GetWindowText(StrCaption);	
	TASKDIALOG_BUTTON Buttons[] ={ { IDOK, StrBtnOK },   { IDCANCEL, StrBtnCancel }};	

	

	return NewStyleMessageBox(theApp.m_pMainWnd->GetSafeHwnd(),StrCaption,StrMainTip,StrContent,2,Buttons,GrayBtnID,StrCheckBox);
}

void CDBCTaskmanApp::Global_EndProcessesInList(CListCtrl* pList,BOOL ShowTipWhenOnlyOne)
{
	if(pList==NULL) return;

	BOOL EnableTerminate = FALSE;
	int Selectedcount = pList->GetSelectedCount();
	
	
	PPROCLISTDATA * pDataArray = new PPROCLISTDATA[Selectedcount];		

	POSITION SelPos = pList->GetFirstSelectedItemPosition();		
	CString StrProcessNames= L"";
	int i=0;
	while (SelPos)
	{
		int iItem =  pList->GetNextSelectedItem(SelPos);
		pDataArray[i] = NULL;
		pDataArray[i] = (PROCLISTDATA *) pList->GetItemData(iItem);
		StrProcessNames = StrProcessNames+L"\n"+pDataArray[i]->Name;
		i++;			 
	}

	if(Selectedcount>1)
	{
		EnableTerminate = (theApp.Global_ShowOperateTip(StrProcessNames,STR_ENDMULTPROC_MAINTIP,STR_ENDMULTPROC_CONTENT,STR_ENDMULTPROC_BTN) ==IDOK);


	}
	else //单个
	{
		if(ShowTipWhenOnlyOne)
		{
			 

			StrProcessNames.Remove(L'\n');
			if(( (pDataArray[0]->Name.CompareNoCase(L"csrss.exe")==0 )||(pDataArray[0]->Name.CompareNoCase(L"smss.exe")==0 )||(pDataArray[0]->Name.CompareNoCase(L"wininit.exe")==0 )) && (pDataArray[0]->User.Compare(L"SYSTEM")==0) )
			{				
				CString StrCheckBtn;
				StrCheckBtn.LoadString(STR_ENDSYSPROC_OTHER);
				EnableTerminate = (theApp.Global_ShowOperateTip(StrProcessNames,STR_ENDSYSPROC_MAINTIP,STR_ENDSYSPROC_CONTENT,STR_ENDSYSPROC_BTN,0,StrCheckBtn)==IDOK);	
			}
			else
			{
				EnableTerminate = (theApp.Global_ShowOperateTip(StrProcessNames,STR_ENDPROC_MAINTIP,STR_ENDPROC_CONTENT,STR_ENDPROC_BTN)==IDOK);	
			}
					
			
		}
		else
		{
			EnableTerminate = TRUE;
		}
	}
	
		
	if(EnableTerminate)
	{
		for( i=0;i<Selectedcount;i++)
		{
			if(( (pDataArray[i]->Name.CompareNoCase(L"csrss.exe")==0 )||(pDataArray[i]->Name.CompareNoCase(L"smss.exe")==0 )||(pDataArray[i]->Name.CompareNoCase(L"wininit.exe")==0 )) && (pDataArray[i]->User.Compare(L"SYSTEM")==0) )
			{				
				CString StrCheckBtn;
				StrCheckBtn.LoadString(STR_ENDSYSPROC_OTHER);
				if(theApp.Global_ShowOperateTip(pDataArray[i]->Name,STR_ENDSYSPROC_MAINTIP,STR_ENDSYSPROC_CONTENT,STR_ENDSYSPROC_BTN,0,StrCheckBtn)!=IDOK)continue;	
			}
			
			if(pDataArray[i] != NULL)TerminateProcess(pDataArray[i]->hProcess, 4);
		}
	}

	delete [] pDataArray;
	pDataArray = NULL;

}
