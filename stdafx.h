// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once


#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT





#include <afxdlgs.h>


#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment( lib,  "gdiplus.lib" ) // function


#include <Winternl.h>

#include "psapi.h"
#pragma comment(lib,"psapi.lib")



//-----------------------------------------------------------------------
using namespace std;  
#include <comdef.h>  
#include <Wbemidl.h> 
# pragma comment(lib, "wbemuuid.lib") 
//-----------------------------------------------------------------------
//#include <vector>
#include<map>
using namespace std;
 

#include "strsafe.h"

#pragma   comment(lib, "UxTheme.lib ")



//磁盘枚举
#include "setupapi.h"  
#include <WinIoCtl.h>  
#include <ntddscsi.h>  
#define DISK_PATH_LEN 512







#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif






 


#define     CFG_DATA_VER  (140731)

#define     NORMAL_WND   0
#define     MAX_WND    1
#define     MIN_WND    2


#define  COL_COUNT_PROC 11
#define  COL_COUNT_USER 9
#define  COL_COUNT_DETAIL 33


#define  PS_RUNNING   (0)
#define  PS_NOTRESPONDING  (1)




#define  LINE_H1   (22)
#define  LINE_H2   (28)

#define   SORT_UP    0    //升序排列
#define   SORT_DOWN    1 //降序排列


#define  APP   (1)
#define  BKGPROC   (3)
#define  WINPROC   (5)
#define  USER   (1)

#define  BOX_W_MIN   (420)


#define  UM_PROCSTART   (WM_USER+100)
#define  UM_PROCEXIT   (WM_USER+101)
#define  UM_DBCLICK_LSIT   (WM_USER+102)
#define  UM_ADD_STARTUPITEM   (WM_USER+103)
#define  UM_BASELISTOK   (WM_USER+104) //最基本的进程列表载入完成！
#define  UM_ALLINFO_OK   (WM_USER+105) //进程全部信息载入完成！
#define  UM_TOGGLE_TMMODE   (WM_USER+106) 

#define  UM_ADD_PROC_SIMPEL_LIST   (WM_USER+107) //添加到simple list 
//#define  UM_DEL_PROC_SIMPEL_LIST   (WM_USER+108) //从 simple list  删除



#define  UM_HEADER_LCLICK   (WM_USER+108)  
#define  UM_HEADER_RCLICK   (WM_USER+109)  
#define  UM_NOTIFYICON   (WM_USER+110)  
#define  UM_ITEMCHANGED_COOLLIST   (WM_USER+111) 
#define  UM_TIMER   (WM_USER+112) 
#define  UM_REFRESH   (WM_USER+113) 
#define  UM_SUMMARYVIEW   (WM_USER+114) 
#define  UM_POPUP_MEMDETAILSDLG   (WM_USER+115) 
#define  UM_RELOADSETINGS   (WM_USER+116) 


//--------------
#define  UM_HOOK_WNDCREATED    (WM_USER+150)
#define  UM_HOOK_WNDDESTROYED    (WM_USER+151)




 



#define  PARENT_ITEM_CLOSE  0  
#define  PARENT_ITEM_OPEN  1 
#define  PARENT_ITEM_NOSUB  2 
#define  SUB_ITEM  3

#define  PM_CPU  0
#define  PM_MEMORY  1
#define  PM_DISK  2
#define  PM_ETHERNET 3


#define  mbox(x)   {CString StrXXX; StrXXX.Format(L"%d",x); AfxMessageBox(StrXXX);}
#define  mboxdb(x)   {CString StrXXX; StrXXX.Format(L"%f",x); AfxMessageBox(StrXXX);}
#define  mboxstr(x)   {  AfxMessageBox(x);}
#define  setwndtext(x)   {CString StrTTT;  StrTTT.Format(L"%d",x);theApp.m_pMainWnd->SetWindowTextW(StrTTT);}  
#define  setwndtextdb(x)   {CString StrTTT;  StrTTT.Format(L"%f",(float)x);theApp.m_pMainWnd->SetWindowTextW(StrTTT);}  
 


#define  COL_SAT_PROC   theApp.AppSettings.ColIDS_ProcList
#define  COL_SAT_USER    theApp.AppSettings.ColIDS_UserList

 
//******************************************************************************************************************




//**************************************************


extern CString     STR_FONTNAME;

extern CString     MAIN_CAPTION ;
extern CString     STR_GROUP_APP ;
extern CString     STR_GROUP_BKG ;
extern CString     STR_GROUP_WIN ;
//-----------Cpu-----------
extern CString     STR_CPUINFO_0  ;
extern CString     STR_CPUINFO_1 ;
extern CString     STR_CPUINFO_2 ;
extern CString     STR_CPUINFO_3 ;
extern CString     STR_CPUINFO_4  ;
extern CString     STR_CPUINFO_5 ;
extern CString     STR_CPUINFO_6  ;
extern CString     STR_TIP1_CPU_TOTAL ;
extern CString     STR_TIP1_CPU_LOGICAL  ;
extern CString     STR_TIP1_CPU_NUMA  ;

//------------Memory
extern CString     STR_MEMINFO_0 ;
extern CString     STR_MEMINFO_1;
extern CString     STR_MEMINFO_2;
extern CString     STR_MEMINFO_3;
extern CString     STR_MEMINFO_4 ;
extern CString     STR_MEMINFO_5;
extern CString     STR_MEMINFO_6;
extern CString     STR_TIP3_MEMORY;
extern CString     STR_TIP5_MEMORY;
//----------Disk-----------
extern CString     STR_DISKINFO_0;
extern CString     STR_DISKINFO_1;
extern CString     STR_DISKINFO_2;
extern CString     STR_DISKINFO_3;
extern CString     STR_DISKINFO_6;

extern CString     STR_TIP5_DISK;
//-------------Network-----------
extern CString     STR_NETWORKINFO_0;
extern CString     STR_NETWORKINFO_2 ;
extern CString     STR_NETWORKINFO_6;
extern CString     STR_TIP3_NETWORK;

//---------  properties dialog  of Process -----------

extern CString      STR_PROPERTIESDLG_CAPTION;

//------------------------List Text Color ------------------------

extern COLORREF     List_NormalTextColor;
extern COLORREF     List_HotTextColor;




extern  int     LogicalProcessorsCount;




//******************************************************************************************************************


typedef struct  ServicesListData
{
	int PID;
 

} SERVICESLISTDATA ;



struct ListSortData
{
	CListCtrl *pList;
	int nCol;
	BOOL FlagSortUp;
};



//-----------------------------------------------------------------------------------





#define  PROCESS_DEP_ENABLE 0x00000001



struct  COLUMNSTATUS
{
	int ColWidth;
	BYTE Redraw; //一个总体标志控制是否重绘此列
	BYTE Align;
	BYTE iOrder;
	int ColWStore;//用于保存恢复显示列时候列的宽度
};


struct  COLUMNSTATUS_EX
{
	int  ColWidth;
	BYTE Redraw;//一个总体标志控制是否重绘此列
	BOOL Cool;
	BOOL IsHiddenColumn; //未启用的列
	BYTE iOrder;
	//CString StrItem;
	WCHAR StrItem[5]; // headerctrl 显示的 百分比 
	BYTE Align;
	BOOL DrawInSubItem ; //如果是子项 是否显示此列中文字！
	float Percents;
	int ColWStore;//用于保存恢复显示列时候列的宽度

};


//static int UUU = 0;

extern int GetDiskVolFailCount;

//******************************************************************************************************************



 

typedef enum _OBJECT_INFORMATION_CLASS
{


    ObjectBasicInformation,
    ObjectNameInformation,
    ObjectTypeInformation,
    ObjectAllInformation,
    ObjectDataInformation



} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

 

//******************************************************************************************************************
//            导入 函数 


typedef DWORD  (WINAPI *API_NtQuerySystemInformation)(DWORD, PVOID, DWORD, PDWORD);
static	API_NtQuerySystemInformation  MyNtQuerySystemInformation;

typedef LONG  (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);
static  PROCNTQSIP MyNtQueryInformationProcess;

typedef LONG (WINAPI *MyAdjustPrivilege)(ULONG Privilege,BOOLEAN Enable,BOOLEAN CurrentThread,PBOOLEAN Enabled);

typedef LONG  (WINAPI *MyNtAdjustPrivilegeToken)(HANDLE TokenHandle,BOOL bVal,PTOKEN_PRIVILEGES NewS,DWORD nSize,PVOID p1 ,  PVOID p2);

//typedef LONG  (WINAPI *MyNtDeviceIoControlFile)(_In_   HANDLE FileHandle,HANDLE Event,     PIO_APC_ROUTINE ApcRoutine,     PVOID ApcContext,    PIO_STATUS_BLOCK IoStatusBlock,     ULONG IoControlCode,     PVOID InputBuffer,     ULONG InputBufferLength,    PVOID OutputBuffer,     ULONG OutputBufferLength);

typedef BOOL  (WINAPI  *MYAPIGETPROCDEP)( HANDLE hProcess ,     LPDWORD lpFlags,     PBOOL lpPermanent);

typedef BOOL  (WINAPI  *API_GETINSTALLMEM)( PULONGLONG TotalMemoryInKilobytes);

typedef NTSTATUS   (WINAPI  *API_NtQueryObject)( IN HANDLE ObjectHandle,IN OBJECT_INFORMATION_CLASS ObjectInformationClass,OUT PVOID ObjectInformation,IN ULONG Length,OUT PULONG ResultLength);


//typedef LONG (WINAPI *MyNtClose)( _In_  HANDLE Handle);
//
//typedef LONG (WINAPI *MyNtOpenProcessToken)(  HANDLE FileHandle, DWORD Mask, HANDLE * pHandle);
//
//typedef LONG (WINAPI *MyNtOpenProcess)( __out PHANDLE ProcessHandle,    __in ACCESS_MASK DesiredAccess,    __in POBJECT_ATTRIBUTES ObjectAttributes,    __in_opt PVOID ClientId);
//
//
//typedef HANDLE  (WINAPI *MyGetProcessHandleFromHwnd)( _In_  HWND hwnd);


//----------------------------------------------
 

typedef DWORD (WINAPI *API_NtSuspendProcess)(HANDLE ProcessHandle); 
typedef DWORD (WINAPI *API_NtResumeProcess)(HANDLE hProcess);

extern API_NtSuspendProcess  NtSuspendProcess;
extern API_NtResumeProcess   NtResumeProcess;



//******************************************************************************************************************


typedef LONG    NTSTATUS;


#define NT_SUCCESS(status)          ((NTSTATUS)(status)>=0)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_ACCESS_DENIED        ((NTSTATUS)0xC0000022L)

//--------------------------------------------------------------------------------------------------------
#define NT_PROCESSTHREAD_INFO        0x05
#define MAX_INFO_BUF_LEN             0x500000
#define STATUS_SUCCESS               ((NTSTATUS)0x00000000L)
#define STATUS_INFO_LENGTH_MISMATCH  ((NTSTATUS)0xC0000004L)

typedef LONG NTSTATUS;




typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
}CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;
typedef LONG KPRIORITY;


//************************************************** 备用**************************************************
//
//
//typedef struct _VM_COUNTERS
//{
//	ULONG PeakVirtualSize;
//	ULONG VirtualSize;
//	ULONG PageFaultCount;
//	ULONG PeakWorkingSetSize;
//	ULONG WorkingSetSize;
//	ULONG QuotaPeakPagedPoolUsage;
//	ULONG QuotaPagedPoolUsage;
//	ULONG QuotaPeakNonPagedPoolUsage;
//	ULONG QuotaNonPagedPoolUsage;
//	ULONG PagefileUsage;
//	ULONG PeakPagefileUsage;
//}VM_COUNTERS,*PVM_COUNTERS;
//
//
//typedef enum _THREAD_STATE
//{
//	StateInitialized,
//	StateReady,
//	StateRunning,
//	StateStandby,
//	StateTerminated,
//	StateWait,
//	StateTransition,
//	StateUnknown
//}THREAD_STATE;
//
//typedef enum _KWAIT_REASON
//{
//	Executive,
//	FreePage,
//	PageIn,
//	PoolAllocation,
//	DelayExecution,
//	Suspended,
//	UserRequest,
//	WrExecutive,
//	WrFreePage,
//	WrPageIn,
//	WrPoolAllocation,
//	WrDelayExecution,
//	WrSuspended,
//	WrUserRequest,
//	WrEventPair,
//	WrQueue,
//	WrLpcReceive,
//	WrLpcReply,
//	WrVertualMemory,
//	WrPageOut,
//	WrRendezvous,
//	Spare2,
//	Spare3,
//	Spare4,
//	Spare5,
//	Spare6,
//	WrKernel
//}KWAIT_REASON;
//
//typedef struct _SYSTEM_THREADS
//{
//	LARGE_INTEGER KernelTime;
//	LARGE_INTEGER UserTime;
//	LARGE_INTEGER CreateTime;
//	ULONG         WaitTime;
//	PVOID         StartAddress;
//	CLIENT_ID     ClientId;
//	KPRIORITY     Priority;
//	KPRIORITY     BasePriority;
//	ULONG         ContextSwitchCount;
//	THREAD_STATE  State;
//	KWAIT_REASON  WaitReason;
//}SYSTEM_THREADS,*PSYSTEM_THREADS;
//
//
//
//typedef struct _SYSTEM_PROCESSES
//{
//	ULONG NextEntryDelta; //构成结构序列的偏移量;
//	ULONG ThreadCount;  //线程数目;
//	ULONG Reserved1[6]; 
//	LARGE_INTEGER CreateTime; //创建时间;
//	LARGE_INTEGER UserTime; //用户模式(Ring 3)的CPU时间;
//	LARGE_INTEGER KernelTime; //内核模式(Ring 0)的CPU时间;
//	UNICODE_STRING ProcessName; //进程名称;
//	KPRIORITY BasePriority; //进程优先权;
//	ULONG ProcessId; //进程标识符;
//	ULONG InheritedFromProcessId;  //父进程的标识符;
//	ULONG HandleCount; //句柄数目;
//	ULONG Reserved2[2];
//	VM_COUNTERS VmCounters;  //虚拟存储器的结构，见下;
//	IO_COUNTERS IoCounters;  //IO计数结构，见下;
//	SYSTEM_THREADS Threads[1];      //进程相关线程的结构数组，见下;
//}SYSTEM_PROCESSES,*PSYSTEM_PROCESSES;



//--------------------------------------------------------------------------------------------------------


//typedef struct _LDR_DATA_TABLE_ENTRY {
//	PVOID Reserved1[2];
//	LIST_ENTRY InMemoryOrderLinks;
//	PVOID Reserved2[2];
//	PVOID DllBase;
//	PVOID EntryPoint;
//	PVOID Reserved3;
//	UNICODE_STRING FullDllName;
//	BYTE Reserved4[8];
//	PVOID Reserved5[3];
//	union {
//		ULONG CheckSum;
//		PVOID Reserved6;
//	};
//	ULONG TimeDateStamp;
//} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;
//

//


//                                             以上备用
//**************************************************************************************************************************************************

typedef struct _PEB_LDR_DATA {
	BYTE       Reserved1[8];
	PVOID      Reserved2[3];
	LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;




typedef struct
{
	ULONG          AllocationSize;
	ULONG          ActualSize;
	ULONG          Flags;
	ULONG          Unknown1;
	UNICODE_STRING Unknown2;
	HANDLE         InputHandle;
	HANDLE         OutputHandle;
	HANDLE         ErrorHandle;
	UNICODE_STRING CurrentDirectory;
	HANDLE         CurrentDirectoryHandle;
	UNICODE_STRING SearchPaths;
	UNICODE_STRING ApplicationName;
	UNICODE_STRING CommandLine;
	PVOID          EnvironmentBlock;
	ULONG          Unknown[9];
	UNICODE_STRING Unknown3;
	UNICODE_STRING Unknown4;
	UNICODE_STRING Unknown5;
	UNICODE_STRING Unknown6;
} PROCESS_PARAMETERS, *PPROCESS_PARAMETERS; 




typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE           Reserved1[16];
	PVOID          Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;



typedef struct 
{
	BYTE                          Reserved1[2];
	BYTE                          BeingDebugged;
	BYTE                          Reserved2[1];
	PVOID                         Reserved3[2];
	PPEB_LDR_DATA                 Ldr;
	PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
	BYTE                          Reserved4[104];
	PVOID                         Reserved5[52];
	//  PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE                          Reserved6[128];
	PVOID                         Reserved7[1];
	ULONG                         SessionId;
} PEB86, *PPEB86;




typedef struct 
{
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[21];
	PPEB_LDR_DATA LoaderData;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	BYTE Reserved3[520];
	// PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE Reserved4[136];
	ULONG SessionId;
} PEB64;






typedef struct MY_SYSTEM_INTERRUPT_INFORMATION
{
    ULONG ContextSwitches;
    ULONG DpcCount;
    ULONG DpcRate;
    ULONG TimeIncrement;
    ULONG DpcBypassCount;
    ULONG ApcBypassCount;
} MY_SYSTEM_INTERRUPT_INFORMATION, *MY_PSYSTEM_INTERRUPT_INFORMATION;



//****************************************** 通用函数 ********************************************************************************************************

static  void OpenPropertiesDlg(CString Path)
{

	SHELLEXECUTEINFO sei;
	ZeroMemory(&sei,sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.lpFile = (LPTSTR)(LPCTSTR)(Path);
	sei.lpVerb = _T("properties");
	sei.fMask = SEE_MASK_INVOKEIDLIST;
	ShellExecuteEx(&sei);

}
//----------------------------------------------------
static  void OpenFileLocation(CString Path)
{

	CString StrTargetPath = Path;

	CString StrLocation; 

	int n = StrTargetPath.ReverseFind(L'\\');

	StrLocation = StrTargetPath.Left(n);
	StrLocation = L"/select,\""+StrTargetPath;

	StrLocation+="\"" ;	
	ShellExecute(CWnd::GetDesktopWindow()->m_hWnd, L"open", L"explorer.exe",StrLocation ,_T(""),SW_SHOW);
}


//----------------------------------------------------
static  void SearchOnline(CString Name)
{

	CString Str=L"";
	Str =L"http://www.bing.com/search?q="+ Name ;	
	ShellExecute(CWnd::GetDesktopWindow()->m_hWnd, L"open", Str,L"" ,_T(""),SW_SHOW);


	Str =L"https://www.google.com/search?q="+ Name ;
	ShellExecute(CWnd::GetDesktopWindow()->m_hWnd, L"open", Str,L"" ,_T(""),SW_SHOW);



}



//-------------------------------------------------------------
static IEnumWbemClassObject*  GetWmiObject(CString StrClass,WCHAR *StrUser = NULL, WCHAR *StrPassword= NULL)
{
	
	//return NULL;
	
	CoInitializeEx(0,  COINIT_APARTMENTTHREADED    );   //COINIT_MULTITHREADED 
	 
	IEnumWbemClassObject* pEnumerator = NULL;
	HRESULT hres;

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(     CLSID_WbemLocator,0,  CLSCTX_INPROC_SERVER,        IID_IWbemLocator, (LPVOID *) &pLoc);

	if (FAILED(hres))
	{
		return pEnumerator;                 // Program has failed.
	}

	IWbemServices *pSvc = NULL;
	hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),StrUser,StrPassword,0,NULL,0,0,&pSvc);  // User name. NULL = current user       User password. NULL = current          Locale. NULL indicates current    Security flags.     Authority (for example, Kerberos)     Context object     pointer to IWbemServices proxy

	if (FAILED(hres))
	{        
		pLoc->Release();
		return pEnumerator;                // Program has failed.
	}

	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hres))
	{

		pSvc->Release();
		pLoc->Release();     
		// CoUninitialize();
		return  pEnumerator;               // Program has failed.
	}



	CString StrClassName;

	StrClassName.Format(L"SELECT * FROM  %s",StrClass);

	pEnumerator = NULL;

	hres = pSvc->ExecQuery(bstr_t("WQL"),         bstr_t(StrClassName),        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,         NULL,        &pEnumerator);
	if (FAILED(hres))  {   pSvc->Release();pLoc->Release();    return  NULL;     }        // Program has failed.

	pSvc->Release();
	pLoc->Release();





    if (FAILED(hres))  
    {  
       AfxMessageBox(L"Failed to initialize COM library. "   );
       
    }  


 
	return pEnumerator;
}

//--------------------------------------------------------------------------------------

//    磁盘相关

static DWORD GetPhysicalDriveFromPartitionLetter(CString  letter,int *PartitionID )
{

    HANDLE hDevice;               // handle to the drive to be examined
    BOOL result;                 // results flag
    DWORD readed;                   // discard results
    STORAGE_DEVICE_NUMBER number;   //use this to get disk numbers
 
    CString  StrPath ;
   
	StrPath.Format(L"\\\\.\\%s:",letter);

    hDevice = CreateFile(StrPath, // drive to open
                         0,    // access to the drive
                         FILE_SHARE_READ | FILE_SHARE_WRITE,    //share mode
                         NULL,             // default security attributes
                         OPEN_EXISTING,    // disposition
                         0,                // file attributes
                         NULL);            // do not copy file attribute
    if (hDevice == INVALID_HANDLE_VALUE) // cannot open the drive
    {
      //  fprintf(stderr, "CreateFile() Error: %ld\n", GetLastError());
		
        return DWORD(-1);
    }
 
    result = DeviceIoControl(
                hDevice,                // handle to device
                IOCTL_STORAGE_GET_DEVICE_NUMBER, // dwIoControlCode
                NULL,                            // lpInBuffer
                0,                               // nInBufferSize
                &number,           // output buffer
                sizeof(number),         // size of output buffer
                &readed,       // number of bytes returned
                NULL      // OVERLAPPED structure
                  );
    if (!result) // fail
    {
		
		 
       // fprintf(stderr, "IOCTL_STORAGE_GET_DEVICE_NUMBER Error: %ld\n", GetLastError());
        (void)CloseHandle(hDevice);
        return (DWORD)-1;
    }
    //printf("%d %d %d\n\n", number.DeviceType, number.DeviceNumber, number.PartitionNumber);
 
 

	

	*PartitionID =  number.PartitionNumber;
    (void)CloseHandle(hDevice);
    return number.DeviceNumber;




}
//--------------------
static CString GetDrivelettersFormDiskID(int ID)
{
	 

	WCHAR szDrive[2];
	DWORD dwID = GetLogicalDrives();

	CString StrDriveLetter=L"";
	WCHAR szLetter[26*3+1];


	for(int i = 0;i < 26*3;i++)
	{
		szLetter[i] = L'X';//预先填充空格 不是空字符 

	}



	szLetter[26*3] =L'\0';


	int PartitionID;
	int LetterCount=0;

	for(int i = 0;i < 26;i++)
	{
		if( ( ( dwID >> i ) &0x00000001 ) == 1 )
		{
			szDrive[0] =  ( WCHAR)( 65 + i );
			szDrive[1] = 0;

			int nGetID =   GetPhysicalDriveFromPartitionLetter(szDrive,&PartitionID);
			if(nGetID>=0)
			{
				if(ID == nGetID)
				{

					szLetter[PartitionID*3-1] =L' ';//空格
					szLetter[PartitionID*3] = szDrive[0];
					szLetter[PartitionID*3+1] =L':';
					LetterCount ++;

				}
			}

		 
			 
		}

	}


	//	AfxMessageBox( StrDriveLetter);
	StrDriveLetter =szLetter;
	//MSB_S(StrDriveLetter)
	if(LetterCount>0) //找到盘符
	{
		StrDriveLetter.Remove(L'X');
	}
	else  //没找到任何盘符
	{
		GetDiskVolFailCount++;
		return L" ";

	}
	
	return StrDriveLetter;	 

	
}

//**************************************************************************************************************************************************



static COLORREF  AlphaBlendColor(COLORREF BColor,COLORREF FColor,float Alpha)
{
	COLORREF RetColor;

	BYTE FR = GetRValue(FColor);
	BYTE FG = GetGValue(FColor);
	BYTE FB = GetBValue(FColor);
		


	BYTE NewR = (BYTE)(GetRValue(BColor)*(1-Alpha)+ FR*Alpha);
	BYTE NewG = (BYTE)(GetGValue(BColor)*(1-Alpha)+ FG*Alpha);
	BYTE NewB = (BYTE)(GetBValue(BColor)*(1-Alpha)+ FB*Alpha);

	RetColor = RGB(NewR,NewG,NewB);

	return RetColor;

}

//**************************************************************************************************************************************************
static void InvalidateIfVisible(CWnd * pWnd)
{
	if(pWnd == NULL) return;
	if(pWnd->IsWindowVisible())pWnd->Invalidate(0);

}



static Image * LoadPNG(LPCTSTR nID, HINSTANCE  hInst)
{
 
   HRSRC   hRsrc   =   ::FindResource   (hInst, nID, _T("PNG") );   //   type 
	
	if(!hRsrc) return NULL; 

	// load resource into memory 

	DWORD   len   =   SizeofResource(hInst,   hRsrc); 
	BYTE*   lpRsrc   =   (BYTE*)LoadResource(hInst,   hRsrc); 

	if(!lpRsrc) return NULL; 
	
	HGLOBAL   m_hMem   =   GlobalAlloc(GMEM_FIXED,   len); //Allocate global memory on which to create stream 

	BYTE*   pmem   =   (BYTE*)GlobalLock(m_hMem); 
	memcpy(pmem,lpRsrc,len); 
	IStream*   pStream; 
	CreateStreamOnHGlobal(m_hMem,FALSE,&pStream); 

	//load   from   stream这是关键一句，通过FromStream返回以各Image*，然后在Graphic的DrawImage地方调用就行了！
   
	Image *RetImage;
	RetImage =Gdiplus::Image::FromStream(pStream); 
	
	//free/release  stuff 
	GlobalUnlock(m_hMem); 
	pStream->Release(); 
	FreeResource(lpRsrc); 
	GlobalFree(m_hMem); 

	m_hMem = NULL;  
	
	
	 
	return  RetImage;


}


//
//typedef NTSTATUS (WINAPI *API_GETNEXTPROCESS)(HANDLE ProcessHandle, ACCESS_MASK DesiredAccess, ULONG HandleAttributes, ULONG Flags,PHANDLE NewProcessHandle); 
//typedef NTSTATUS (WINAPI *API_RESUMEPROCESS)(HANDLE ProcessHandle); 
//
//static HANDLE MiniFxOpenProcess(ULONG dwDesiredAccess, ULONG dwProcessId)
//{
//	 
//	API_GETNEXTPROCESS ZwGetNextProcess=(API_GETNEXTPROCESS)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"ZwGetNextProcess");
//	//API_GETPROCESSID GetProcessId=(GETPROCESSID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"GetProcessId");
//
//	HANDLE hCurrent=0, hNext=0;ULONG dwPid=0;long Status=0;
//	if((PVOID)ZwGetNextProcess == NULL || (PVOID)GetProcessId == NULL) return (HANDLE)0;
//
//	Status = ZwGetNextProcess(0, dwDesiredAccess, 0, 0, &hNext);
//	if (Status >= 0)
//	{
//		do{
//			hCurrent = hNext;
//			dwPid = GetProcessId(hCurrent);
//			if (dwPid==dwProcessId) return hCurrent;
//			Status = ZwGetNextProcess(hCurrent, dwDesiredAccess, 0, 0, &hNext);
//			CloseHandle(hCurrent);
//		}while(hCurrent != 0);
//	}
//	return (HANDLE)0;
//}
//
//
//static void TerminateProcessEx2(HANDLE hProc) 
//{ 
//	ULONG(WINAPI*ZwUnmapViewOfSection)(HANDLE ProcessHandle,ULONG BaseAddress); 
//	int i; 
//	ULONG base; 
//	//HANDLE hProc; 
//	(FARPROC&)ZwUnmapViewOfSection=GetProcAddress(GetModuleHandle(L"ntdll.dll"),"ZwUnmapViewOfSection"); 
//	
//	for(i=0x1;i<=0x80;i++) 
//	{ 
//		base=i*0x1000000; 
//		ZwUnmapViewOfSection(hProc,base); 
//	} 
//	CloseHandle(hProc); 
//}
//static void TerminateProcessEx(HANDLE hProc)
//{
//
//
//	long i=0;
//	API_RESUMEPROCESS	ZwResumeProcess=(API_RESUMEPROCESS)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"ZwResumeProcess");
//	for(i=0;i<0x7fffffff;i=i+0x10)
//	{
//		
//		VirtualFreeEx(hProc, (PVOID)i, 0x10, 0x4000);
//		if(ZwResumeProcess(hProc)!=0) //如果调用此函数失败，则证明进程已死
//		{
//			  mbox(hProc)
//			break;
//		}
//	}
//}