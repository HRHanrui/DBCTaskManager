#include "StdAfx.h"
#include "ProcessInfo.h"
#include "DBCTaskman.h"
#include "PerformanceMon.h"

 


//---------------------------

struct LANGANDCODEPAGE
	{
		WORD wLanguage;
		WORD wCodePage;
	};


struct  WORKING_SET
	{
		double  WsPrivate;
		double  WsShareable;
	};

//---------------------------

static  CPerformanceMon PM;
static UINT Thread_PDHGetMemInfo(LPVOID lparam)
{
	PROCLISTDATA * pData =  (PROCLISTDATA * )lparam;

	if(pData!=NULL)
	{
		//CString StrTemp,StrPDHProcessName;

		//StrPDHProcessName = pData->Name;

		//if(StrPDHProcessName.Right(4).CompareNoCase(L".exe") == 0)  //exe去掉 .dll则不去掉 Pdh 要求这样格式
		//{	
		//	StrPDHProcessName=StrPDHProcessName.Left(StrPDHProcessName.GetLength()-4);
		//}

		//if(pData->SameNameID != 0)
		//{
		//
		//	StrTemp.Format(L"%s#%d",StrPDHProcessName,pData->SameNameID)  ;
		//	StrPDHProcessName = StrTemp;
		//}


		////特殊的 

		//if(pData->PID == 0){	StrPDHProcessName = L"Idle";	}
		//if(pData->PID == 4){	StrPDHProcessName = L"System";	}

		//StrTemp.Format(L"\\Process(%s)\\Working Set", StrPDHProcessName );
		//
		//double WS = PM.PdhGetInfo(StrTemp);

	//	pData->Mem_WorkingSet = WS;






	}

	::AfxEndThread(0);

	return 0;
}







 




CProcess::CProcess(void)
{


	NtQueryVirtualMemory = (MyNtQueryVirtualMemory)GetProcAddress(GetModuleHandle(L"ntdll.dll"),"NtQueryVirtualMemory");
	MyNtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(GetModuleHandle(L"ntdll"),"NtQueryInformationProcess");

	
	

}

CProcess::~CProcess(void)
{
}

CString CProcess::GetUser(HANDLE hProcess,UINT PID,WCHAR *StrDomain)
{

	if (  (PID == 0) || (PID == 4) )
	{
		 return L"SYSTEM" ;
	}
 

	HANDLE hToken = NULL;
	BOOL bFuncReturn = FALSE;
	CString strUserName = _T("");
	PTOKEN_USER pToken_User = NULL;
	DWORD dwTokenUser = 0;
	TCHAR szAccName[MAX_PATH] = {0};
	TCHAR szDomainName[MAX_PATH] = {0};


	if(hProcess != NULL)
	{
		// 提升本进程的权限

		bFuncReturn = ::OpenProcessToken(hProcess,TOKEN_QUERY,&hToken);

		if( bFuncReturn == 0) // 失败
		{
			return strUserName;
		}

		if(hToken != NULL)
		{
			::GetTokenInformation(hToken, TokenUser, NULL,0L, &dwTokenUser);

			if(dwTokenUser>0)
			{
				pToken_User = (PTOKEN_USER)::GlobalAlloc( GPTR, dwTokenUser );
			}

			if(pToken_User != NULL)
			{
				bFuncReturn = ::GetTokenInformation(hToken, TokenUser, pToken_User, dwTokenUser, &dwTokenUser);
			}

			if(bFuncReturn != FALSE && pToken_User != NULL)
			{
				SID_NAME_USE eUse  = SidTypeUnknown;

				DWORD dwAccName    = 0L;  
				DWORD dwDomainName = 0L;

				PSID  pSid = pToken_User->User.Sid;

				bFuncReturn = ::LookupAccountSid(NULL, pSid, NULL, &dwAccName,NULL,&dwDomainName,&eUse );
				if(dwAccName>0 && dwAccName < MAX_PATH && dwDomainName>0 && dwDomainName <= MAX_PATH)
				{
					bFuncReturn = ::LookupAccountSid(NULL,pSid,szAccName,&dwAccName,szDomainName,&dwDomainName,&eUse );
				}

				if( bFuncReturn != 0)
				{
					strUserName = szAccName;
					if(StrDomain !=NULL)
					{
						StringCchCopy(StrDomain,dwDomainName,szDomainName);
					}
				}
			}
		}
	}

	if (pToken_User != NULL)
	{
		::GlobalFree( pToken_User );
	}

	if(hToken != NULL)
	{
		::CloseHandle(hToken);
	}





	



	return strUserName;


}

 
double CProcess::GetWsPrivate_PDH(ProcessListData *pData)//byte
{
	//------------------------------------------

	double MemUsage;
	 MemUsage  = _GetPDHData(pData->hQueryMem,pData->hCounterMem);

	if(MemUsage == 0)
	{
		MemUsage =  GetWsPrivate(pData->PID) ; //采用另一种方法获取
	}



	return   MemUsage;
}

CString CProcess::GetPathName(HANDLE hProcess,BOOL FullPath)
{
	DWORD  nSize=MAX_PATH;
	CString StrPath;
	CString StrRet;
	QueryFullProcessImageName(hProcess,0,StrPath.GetBuffer(MAX_PATH),&nSize);
	StrPath.ReleaseBuffer();

	if(FullPath)
	{
		StrRet =  StrPath;
	}
	else
	{
		int n=StrPath.ReverseFind(L'\\');
		StrRet= StrPath.Right(StrPath.GetLength()-n-1);
	}

	return StrRet;
}




CString CProcess::GetFileName(CString FullPathName)
{
	CString StrRet;
	int n=FullPathName.ReverseFind(L'\\');
	StrRet= FullPathName.Right(FullPathName.GetLength()-n-1);

	return StrRet;
}

ULONGLONG CProcess::GetDiskIO(HANDLE hProcess,ULONGLONG * pOther)
{
	if(hProcess<0)  return 0;

	ULONGLONG  DiskIOByte=0;

	IO_COUNTERS  IOCounter;
	GetProcessIoCounters(hProcess,&IOCounter);
	DiskIOByte = IOCounter.ReadTransferCount+IOCounter.WriteOperationCount ;//Byte
	//DiskIOByte = IOCounter.OtherTransferCount;
	*pOther = IOCounter.OtherTransferCount ; 

	

	return DiskIOByte;
}

double CProcess::GetIOUsage(CString SrtProcessNameID,  HQUERY   hQuery, HCOUNTER hCounter)
{

//	PdhCollectQueryData(hQuery);
//
//	// 获得当前实例的“ID Process”计数器的值
//	DWORD ctrType;
//	PDH_FMT_COUNTERVALUE fmtValue;
//	PdhGetFormattedCounterValue(hCounter, PDH_FMT_DOUBLE, &ctrType, &fmtValue);
//
//	double IOUsage  =  fmtValue.doubleValue  ;  
//
//	//
//
////	PIP_PER_ADAPTER_INFO  AdapterInfo;
//
//	return IOUsage;

	return 0;


}


//
//void CProcess::GetProcessAllInfo(UINT dwPID)
//{
//	MyNtQuerySystemInformation =(LPFN_NtQuerySystemInformation) GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQuerySystemInformation");	 
//
//	
//	DWORD bytesreturned;
// 
//	UINT   DEF_BUF_SIZE = sizeof(SYSTEM_PROCESSES);
//	NTSTATUS    status ;
//	UINT        nSize = DEF_BUF_SIZE ;
//	LPBYTE     lpBuf = NULL ;
//
//	DWORD dwNumberBytes = MAX_INFO_BUF_LEN;
//
//	while ( TRUE )
//	{
//		// 动态分配空间，用来存储进程信息
//		if ( ( lpBuf = new BYTE [ nSize ] ) == NULL )	{	return  ;	}
//
//		status = MyNtQuerySystemInformation ( SystemProcessInformation, lpBuf, nSize, &bytesreturned ) ;	// 枚举进程信息
//
//		if ( !NT_SUCCESS(status) )
//		{			
//			if ( status == STATUS_INFO_LENGTH_MISMATCH ) // 检测是否返回缓冲区不够大
//			{
//				nSize += DEF_BUF_SIZE ;
//				delete lpBuf ;
//				lpBuf =NULL;
//				continue ;
//			}
//			else
//			{
//				return  ;
//			}
//		}		
//		else
//		{
//			break ;
//		}
//	}
//
//
//
//
//	PSYSTEM_PROCESSES pSysProcess = (PSYSTEM_PROCESSES)lpBuf ;
//	int n=0;
//	CString Str,StrTemp;
//
//
//	while ( pSysProcess->NextEntryDelta != 0 )
//	{
//		StrTemp = L"";
//		if ( pSysProcess->ProcessName.Buffer != NULL )	// printf ( "ProcessName:\t%30S\n", pSysProcess->ProcessName.Buffer ) ;
//		{	
//			StrTemp.Format(L"%s\t%d\t%d\n",pSysProcess->ProcessName.Buffer,  pSysProcess->ProcessId,pSysProcess->HandleCount);
//
//
//		}
//
//		Str = Str+StrTemp;
//		n++;
//
//		pSysProcess = (PSYSTEM_PROCESSES)( (DWORD)pSysProcess + pSysProcess->NextEntryDelta ) ;
//
//	}
//
//	MSB_S(Str)
//
//
//
//
	//StrTemp.Format(L"%d\n\n%s",n,Str);

	//AfxMessageBox(StrTemp);
	//delete lpBuf ;
	//*/



	//PSYSTEM_PROCESSES  pSystemProc    = NULL;
	//PSYSTEM_THREADS    pSystemThre    = NULL; 
	//HMODULE            hNtDll         = NULL;
	//LPVOID             lpSystemInfo   = NULL;
	//DWORD              dwNumberBytes  = MAX_INFO_BUF_LEN;
	//DWORD              dwTotalProcess = 0;
	//DWORD              dwReturnLength;
	//NTSTATUS           Status;
	//LONGLONG           llTempTime;
	//ULONG              ulIndex;





	//lpSystemInfo = (LPVOID)malloc(dwNumberBytes);
	//Status = MyNtQuerySystemInformation(NT_PROCESSTHREAD_INFO,
	//	lpSystemInfo,
	//	dwNumberBytes,
	//	&dwReturnLength);
	//if(Status == STATUS_INFO_LENGTH_MISMATCH)
	//{
	//	AfxMessageBox(L"STATUS_INFO_LENGTH_MISMATCH\n");
	//	;
	//}
	//else if(Status != STATUS_SUCCESS)
	//{
	//	AfxMessageBox(L"NtQuerySystemInformation Error: %d\n",GetLastError());
	//	;
	//}


	//pSystemProc = (PSYSTEM_PROCESSES)lpSystemInfo;

	//while(pSystemProc->NextEntryDelta != 0)
	//{
	//	if(pSystemProc->ProcessId == dwPID)
	//	{
	//		AfxMessageBox(L"ProcessName:\t\t ");
	//		if(pSystemProc->ProcessId != 0)
	//		{
	//			wprintf(L"%-20s\n",pSystemProc->ProcessName.Buffer);

	//		}
	//		else
	//		{
	//			wprintf(L"%-20s\n",L"System Idle Process");
	//		}
	//		printf("ProcessID:\t\t %d\t\t",pSystemProc->ProcessId);
	//		printf("ParentProcessID:\t%d\n",pSystemProc->InheritedFromProcessId);

	//		printf("KernelTime:\t\t ");
	//		llTempTime  = pSystemProc->KernelTime.QuadPart;
	//		llTempTime /= 10000;
	//		printf("%d:",llTempTime/(60*60*1000));
	//		llTempTime %= 60*60*1000;
	//		printf("%.2d:",llTempTime/(60*1000));
	//		llTempTime %= 60*1000;
	//		printf("%.2d.",llTempTime/1000);
	//		llTempTime %= 1000;
	//		printf("%.3d\t",llTempTime);

	//		printf("UserTime:\t\t");
	//		llTempTime  = pSystemProc->UserTime.QuadPart;
	//		llTempTime /= 10000;
	//		printf("%d:",llTempTime/(60*60*1000));
	//		llTempTime %= 60*60*1000;
	//		printf("%.2d:",llTempTime/(60*1000));
	//		llTempTime %= 60*1000;
	//		printf("%.2d.",llTempTime/1000);
	//		llTempTime %= 1000;
	//		printf("%.3d\n",llTempTime);

	//		printf("Privilege:\t\t %d%%\t\t",(pSystemProc->KernelTime.QuadPart * 100)/
	//			(pSystemProc->KernelTime.QuadPart + pSystemProc->UserTime.QuadPart));
	//		printf("User:\t\t\t%d%%\n",(pSystemProc->UserTime.QuadPart * 100)/(pSystemProc->KernelTime.QuadPart + pSystemProc->UserTime.QuadPart));

	//		printf("ThreadCount:\t\t %d\t\t",pSystemProc->ThreadCount);
	//		printf("HandleCount:\t\t%d\n",pSystemProc->HandleCount);

	//		printf("BasePriority:\t\t %-2d\t\t",pSystemProc->BasePriority);
	//		printf("PageFaultCount:\t\t%d\n\n",pSystemProc->VmCounters.PageFaultCount)
	//			;

	//		printf("PeakWorkingSetSize(K):\t %-8d\t",pSystemProc->VmCounters.PeakWorkingSetSize/1024);
	//		printf("WorkingSetSize(K):\t%-8d\n",pSystemProc->VmCounters.WorkingSetSize/1024);

	//		printf("PeakPagedPool(K):\t %-8d\t",pSystemProc->VmCounters.QuotaPeakPagedPoolUsage/1024);
	//		printf("PagedPool(K):\t\t%-8d\n",pSystemProc->VmCounters.QuotaPagedPoolUsage/1024);

	//		printf("PeakNonPagedPook(K):\t %-8d\t",pSystemProc->VmCounters.QuotaPeakNonPagedPoolUsage/1024);
	//		printf("NonePagedPook(K):\t%-8d\n",pSystemProc->VmCounters.QuotaNonPagedPoolUsage/1024);

	//		printf("PeakPagefile(K):\t %-8d\t",pSystemProc->VmCounters.PeakPagefileUsage/1024);
	//		printf("Pagefile(K):\t\t%-8d\n",pSystemProc->VmCounters.PagefileUsage/1024);

	//		printf("PeakVirtualSize(K):\t %-8d\t",pSystemProc->VmCounters.PeakVirtualSize/1024);
	//		printf("VirtualSize(K):\t\t%-8d\n\n",pSystemProc->VmCounters.VirtualSize/1024);

	//		printf("ReadTransfer:\t\t %-8d\t",pSystemProc->IoCounters.ReadTransferCount);
	//		printf("ReadOperationCount:\t%-8d\n",pSystemProc->IoCounters.ReadOperationCount);

	//		printf("WriteTransfer:\t\t %-8d\t",pSystemProc->IoCounters.WriteTransferCount);
	//		printf("WriteOperationCount:\t%-8d\n",pSystemProc->IoCounters.WriteOperationCount);

	//		printf("OtherTransfer:\t\t %-8d\t",pSystemProc->IoCounters.OtherTransferCount);
	//		printf("OtherOperationCount:\t%-8d\n\n",pSystemProc->IoCounters.OtherOperationCount);

	//		printf("%-5s%3s%4s%5s%5s%11s%12s%12s%7s%6s%9s\n","TID","Pri","BPr","Priv","User","KernelTime","UserTime","StartAddr","CSwitC","State","WtReason");
	//		printf("-------------------------------------------------------------------------------\n");

	//		for(ulIndex = 0; ulIndex < pSystemProc->ThreadCount; ulIndex++)
	//		{
	//			pSystemThre = &pSystemProc->Threads[ulIndex];
	//			printf("%-5d",pSystemProc->Threads[ulIndex].ClientId.UniqueThread);

	//			printf("%3d",pSystemProc->Threads[ulIndex].Priority);
	//			printf("%4d",pSystemProc->Threads[ulIndex].BasePriority);

	//			printf("%4d%%",(pSystemProc->Threads[ulIndex].KernelTime.QuadPart * 100)/(pSystemProc->KernelTime.QuadPart + pSystemProc->UserTime.QuadPart));
	//			printf("%4d%%",(pSystemProc->Threads[ulIndex].UserTime.QuadPart * 100)/(pSystemProc->KernelTime.QuadPart + pSystemProc->UserTime.QuadPart));

	//			llTempTime  = pSystemProc->Threads[ulIndex].KernelTime.QuadPart;
	//			llTempTime /= 10000;
	//			printf("%2d:",llTempTime/(60*60*1000));
	//			llTempTime %= 60*60*1000;
	//			printf("%.2d.",llTempTime/(60*1000));
	//			llTempTime %= 60*1000;
	//			printf("%.2d.",llTempTime/1000);
	//			llTempTime %= 100;
	//			printf("%.2d ",llTempTime);

	//			llTempTime  = pSystemProc->Threads[ulIndex].UserTime.QuadPart;
	//			llTempTime /= 10000;
	//			printf("%2d:",llTempTime/(60*60*1000));
	//			llTempTime %= 60*60*1000;
	//			printf("%.2d.",llTempTime/(60*1000));
	//			llTempTime %= 60*1000;
	//			printf("%.2d.",llTempTime/1000);
	//			llTempTime %= 100;
	//			printf("%.2d ",llTempTime);

	//			printf(" 0x%.8X",pSystemProc->Threads[ulIndex].StartAddress);
	//			printf("%7d",pSystemProc->Threads[ulIndex].ContextSwitchCount);

	//			switch(pSystemProc->Threads[ulIndex].State)
	//			{
	//			case StateInitialized:
	//				printf("%6s","Init.");
	//				break;
	//			case StateReady:
	//				printf("%6s","Ready");
	//				break;
	//			case StateRunning:
	//				printf("%6s","Run");
	//				break;
	//			case StateStandby:
	//				printf("%6s","StBy.");
	//				break;
	//			case StateTerminated:
	//				printf("%6s","Term.");
	//				break;
	//			case StateWait:
	//				printf("%6s","Wait");
	//				break;
	//			case StateTransition:
	//				printf("%6s","Tran.");
	//				break;
	//			case StateUnknown:
	//				printf("%6s","Unkn.");
	//				break;
	//			default:
	//				printf("%6s","Unkn.");
	//				break;
	//			}

	//			switch(pSystemProc->Threads[ulIndex].WaitReason)
	//			{
	//			case Executive:
	//				printf(" %-8s","Executi.");
	//				break;
	//			case FreePage:
	//				printf(" %-8s","FreePag.");
	//				break;
	//			case PageIn:
	//				printf(" %-8s","PageIn");
	//				break;
	//			case PoolAllocation:
	//				printf(" %-8s","PoolAll.");
	//				break;
	//			case DelayExecution:
	//				printf(" %-8s","DelayEx.");
	//				break;
	//			case Suspended:
	//				printf(" %-8s","Suspend.");
	//				break;
	//			case UserRequest:
	//				printf(" %-8s","UserReq.");
	//				break;
	//			case WrExecutive:
	//				printf(" %-8s","WrExect.");
	//				break;
	//			case WrFreePage:
	//				printf(" %-8s","WrFrePg.");
	//				break;
	//			case WrPageIn:
	//				printf(" %-8s","WrPageIn");
	//				break;
	//			case WrPoolAllocation:
	//				printf(" %-8s","WrPoolA.");
	//				break;
	//			case WrSuspended:
	//				printf(" %-8s","WrSuspe.");
	//				break;
	//			case WrUserRequest:
	//				printf(" %-8s","WrUsReq.");
	//				break;
	//			case WrEventPair:
	//				printf(" %-8s","WrEvent.");
	//				break;
	//			case WrQueue:
	//				printf(" %-8s","WrQueue");
	//				break;
	//			case WrLpcReceive:
	//				printf(" %-8s","WrLpcRv.");
	//				break;
	//			case WrLpcReply:
	//				printf(" %-8s","WrLpcRp.");
	//				break;
	//			case WrVertualMemory:
	//				printf(" %-8s","WrVerMm.");
	//				break;
	//			case WrPageOut:
	//				printf(" %-8s","WrPgOut.");
	//				break;
	//			case WrRendezvous:
	//				printf(" %-8s","WrRende.");
	//				break;
	//			case WrKernel:
	//				printf(" %-8s","WrKernel");
	//				break;
	//			default:
	//				printf(" %-8s","Unknown");
	//				break;
	//			}
	//			printf("\n");
	//		}
	//		printf("-------------------------------------------------------------------------------\n\n");
	//		printf("Total %d Thread(s) !\n\n",ulIndex);

	//		dwTotalProcess ++;
	//		break;
	//	}
	//	pSystemProc = (PSYSTEM_PROCESSES)((char *)pSystemProc + pSystemProc->NextEntryDelta);
	//}



	//if(dwTotalProcess == 0)
	//{
	//	CString StrTemp;
	//	StrTemp.Format(L"Could not found the %d Process !\n",dwPID);
	//	AfxMessageBox(StrTemp );
	//}
	//else
	//{
	//	printf("TID:\t\t====>\tThread Identification\n");
	//	printf("Pri:\t\t====>\tPriority\n");
	//	printf("BPr:\t\t====>\tBase Priority\n");
	//	printf("Priv:\t\t====>\tPrivilege\n");
	//	printf("StartAddr:\t====>\tThread Start Address\n");
	//	printf("CSwitC:\t\t====>\tContext Switch Count\n");
	//	printf("WtReason:\t====>\tWait Reason\n");
	//}
	//if(lpSystemInfo != NULL)
	//{
	//	free(lpSystemInfo);
	//}
	//if(hNtDll != NULL)
	//{
	//	FreeLibrary(hNtDll);
	//}


//	return  ;
//
//}

double CProcess::GetMemUsageInfo(ProcessListData *pData)
{
	HANDLE hProcess =pData->hProcess;
	double MemUsage = 0 ;

	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb=sizeof(PROCESS_MEMORY_COUNTERS_EX);
	BOOL Ret;

 
	if (hProcess!= NULL)//hProcess
	{
		
		Ret = GetProcessMemoryInfo(  hProcess,(PROCESS_MEMORY_COUNTERS*)&pmc,sizeof(pmc));

		pData->Mem_WorkingSet = (double)pmc.WorkingSetSize;
		pData->Mem_PeakWorkingSet =(double)pmc.PeakWorkingSetSize;
		pData->Mem_PagePool = (double)pmc.QuotaPagedPoolUsage;
		pData->Mem_NonPagePool = (double)pmc.QuotaNonPagedPoolUsage;
		pData->Mem_PageFaults = (double)pmc.PageFaultCount;
		pData->Mem_Commit = (double)pmc.PrivateUsage; 
		 
		if(!Ret) 
		{
			 goto MEM_USE_PDH;
		}

	}
	else
	{
MEM_USE_PDH:
		//AfxBeginThread(Thread_PDHGetMemInfo,pData);
	 
		pData->Mem_WorkingSet = _GetPDHData(pData->hQueryWs,pData->hCounterWs);		
	
		pData->Mem_PeakWorkingSet =0;
		pData->Mem_PagePool = 0;
		pData->Mem_NonPagePool = 0;
		pData->Mem_PageFaults = 0;
		pData->Mem_Commit = 0; 


		if(pData->Mem_WorkingSet > (double)theApp.PerformanceInfo.TotalPhysMem) 
			pData->Mem_WorkingSet =0;

		
	
	
	}
 
	 






	return 0;



	//return   MemUsage;


}



int CProcess::GetThreadCount(ProcessListData *pData )
{
	int nThread = 0;
	// 
	//PROCESSENTRY32 pe32;
	////在使用这个结构前，先设置它的大小
	//pe32.dwSize = sizeof(pe32);
	////给系统内所有的进程拍个快照
	//HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	//if (hProcessSnap == INVALID_HANDLE_VALUE)
	//{
	//	//MSB_S(L"CreateToolhelp32Snapshot 调用失败");
	//	return 0 ;
	//}
	////遍历进程快照，轮流显示每个进程的信息
	//BOOL bMore = ::Process32First(hProcessSnap,&pe32);
	//CString Str,StrT;
	// 
	//while (bMore)
	//{		
	//	if(pe32.th32ProcessID == pData->PID)
	//	{
	//		
	//		nThread = pe32.cntThreads;
	//		break;
	//	}
	//	 
	//	bMore = ::Process32Next(hProcessSnap,&pe32);
	//	 
	//}
	////不要忘记清除掉snapshot对象
 ////	MSB_S(Str)
	//::CloseHandle(hProcessSnap);
	
 
	// nThread = (int)_GetPDHData(pData->hQueryThread,pData->hCounterThread);
		

	return nThread;




}

int CProcess::GetHandles(HANDLE hProcess)
{

	DWORD nHandles=0;
	GetProcessHandleCount(hProcess,&nHandles);
	return nHandles;
}

CString CProcess::GetProcCommandLine(DWORD PID)
{

	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ    ,FALSE,PID);  //注意 PROCESS_ALL_ACCESS 会导致 驱动级别的无法获取信息  //PROCESS_TERMINATE  PROCESS_ALL_ACCESS| PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION   /PROCESS_VM_READ  PROCESS_QUERY_LIMITED_INFORMATION

	if(hProcess == NULL)	return NULL;

	WCHAR StrCmd[2048]={0};

	//----------------------

	PEB64  Peb;
	SIZE_T                     dwDummy;
	SIZE_T                     dwSize;
	PROCESS_PARAMETERS        ProcParam;

	LPVOID                    lpAddress;
	LONG                      status;


	

	PROCESS_BASIC_INFORMATION  ProcBaseInfo;
	status = MyNtQueryInformationProcess(hProcess,ProcessBasicInformation,(PVOID)&ProcBaseInfo,sizeof(PROCESS_BASIC_INFORMATION),NULL );

	//ProcBaseInfo.PebBaseAddress

	if (!status)
	{		

		if (ReadProcessMemory( hProcess,ProcBaseInfo.PebBaseAddress,&Peb,sizeof(PEB),&dwDummy))
		{
			if(	ReadProcessMemory( hProcess,Peb.ProcessParameters,&ProcParam,sizeof(PROCESS_PARAMETERS),&dwDummy))
			{
				//AfxMessageBox(L"dadasdasd");
				lpAddress = ProcParam.CommandLine.Buffer;
				dwSize = ProcParam.CommandLine.Length;

				if(  sizeof(StrCmd)>= dwSize )
				{  
					if(!ReadProcessMemory( hProcess,lpAddress,StrCmd,dwSize,&dwDummy))
					{
						StringCchCopy(StrCmd,2048,L"");
					}
				}

			}
		}
	}

	CloseHandle(hProcess);

	return StrCmd;
}

CString CProcess::GetVerInfoString(CString FilePathName,CString StrInfType)
{
	

	DWORD dwHandle,InfoSize;
	CString StrRet;

	CString StrType=TEXT("\\StringFileInfo\\%04x%04x\\");
	StrType = StrType+StrInfType;


	//首先获得版本信息资源的长度
	InfoSize = GetFileVersionInfoSize(FilePathName,&dwHandle);
	//将版本信息资源读入缓冲区
	if(InfoSize==0)  {return L"" ;}

	BYTE *InfoBuf = new BYTE[InfoSize];
	GetFileVersionInfo(FilePathName,0,InfoSize,(LPVOID)InfoBuf);
	//获得生成文件使用的代码页及文件版本
	unsigned int  cbTranslate = 0;

	LANGANDCODEPAGE *lpTranslate;

	VerQueryValue((LPVOID)InfoBuf, TEXT("\\VarFileInfo\\Translation"),(LPVOID*)&lpTranslate,&cbTranslate);
	// Read the file description for each language and code page.
	for( UINT i=0; i < UINT((cbTranslate/sizeof(struct LANGANDCODEPAGE))); i++ )
	{
		WCHAR  SubBlock[MAX_PATH];
		
		StringCchPrintf(SubBlock,MAX_PATH, StrType,		lpTranslate[i].wLanguage,	lpTranslate[i].wCodePage);
		void *lpBuffer=NULL;

		unsigned int dwBytes=0;
		VerQueryValue(InfoBuf, 	SubBlock, 	&lpBuffer, 	&dwBytes); 
		CString strTemp=(WCHAR *)lpBuffer;
		StrRet+=strTemp;

		 
	}
	delete InfoBuf ;		
	InfoBuf = NULL;


	return  StrRet;
 
}



double CProcess::GetCpuUsage(PROCLISTDATA* pListData,double * CpuTime)
{
		double percent=0;
		if(  pListData == NULL  ) return 0;
		if( pListData->hProcess  < 0 ) return 0;



		if(  pListData->PID >= 4  )
		{

		FILETIME ftime, fsys, fuser;
		ULARGE_INTEGER now, sys, user;        

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&now, &ftime, sizeof(FILETIME));

		GetProcessTimes( pListData->hProcess, &ftime, &ftime, &fsys, &fuser);

		memcpy(&sys, &fsys, sizeof(FILETIME));
		memcpy(&user, &fuser, sizeof(FILETIME));



		if(CpuTime!=NULL){*CpuTime =  (double)(sys.QuadPart+user.QuadPart) ; }


		percent = (double)(sys.QuadPart - pListData->CpuUsage.lastSysCPU.QuadPart) +   (double)( user.QuadPart -  pListData->CpuUsage.lastUserCPU.QuadPart);
		percent = percent/(now.QuadPart -  pListData->CpuUsage.lastCPU.QuadPart);
		percent  =percent/LogicalProcessorsCount;
		pListData->CpuUsage.lastCPU = now;
		pListData->CpuUsage.lastUserCPU = user;
		pListData->CpuUsage.lastSysCPU = sys;


		percent = percent * 100;

	}

	else if(  pListData->PID == 0 )
	{
		percent = 100-theApp.PerformanceInfo.CpuUsage;

	}


	pListData->CPU_Usage = percent;  //保存  ！！！ 
	if(percent<0.0) percent=0.0;
	if(percent>100.0) percent =100.0;
	return  percent ;// _GetCpuUsage ; 
	 
}

double CProcess::GetWsPrivate(DWORD PID )//返回byte数
{
	double WsPrivate = 0;

	HANDLE hProcess =  OpenProcess( PROCESS_QUERY_INFORMATION     ,FALSE,PID);
	
	if(hProcess == NULL)   return WsPrivate;
	
	NTSTATUS status;
    PMEMORY_WORKING_SET_INFORMATION wsInfo;
    PROCESS_WS_COUNTERS wsCounters;
    ULONG i;

  if (!NT_SUCCESS(status = _GetProcessWorkingSetInformation(hProcess,&wsInfo))) return WsPrivate;
       
 
 
    memset(&wsCounters, 0, sizeof(PROCESS_WS_COUNTERS));

    for (i = 0; i < wsInfo->NumberOfEntries; i++)
    {
        wsCounters.NumberOfPages++;

        if (wsInfo->WorkingSetInfo[i].ShareCount > 1)
            wsCounters.NumberOfSharedPages++;
        if (wsInfo->WorkingSetInfo[i].ShareCount == 0)
            wsCounters.NumberOfPrivatePages++;
        if (wsInfo->WorkingSetInfo[i].Shared)
            wsCounters.NumberOfShareablePages++;
    }

    delete wsInfo; wsInfo = NULL;

	 PERFORMANCE_INFORMATION MyPMInfo;

	ZeroMemory(&MyPMInfo, sizeof(PERFORMANCE_INFORMATION));
	MyPMInfo.cb = sizeof(MyPMInfo);
	GetPerformanceInfo(&MyPMInfo,sizeof(PERFORMANCE_INFORMATION));

   WsPrivate = (double)(wsCounters.NumberOfPrivatePages*MyPMInfo.PageSize);

	::CloseHandle(hProcess ); 


	return WsPrivate;


}

NTSTATUS CProcess::_GetProcessWorkingSetInformation(HANDLE hProcess, PMEMORY_WORKING_SET_INFORMATION* WorkingSetInformation)
{
	NTSTATUS status;
    PVOID Buffer;
    ULONG  BufferSize;

    BufferSize = 0x8000;
    Buffer = new BYTE [BufferSize];

	

 
    while ((status = NtQueryVirtualMemory(hProcess, NULL, MemoryWorkingSetInformation, Buffer , BufferSize, NULL )) == STATUS_INFO_LENGTH_MISMATCH   )
    {
		 
       delete  Buffer; Buffer =NULL;
        BufferSize *= 2;

        // Fail if we're resizing the buffer to over  16 MB.
        if (BufferSize > MY_LARGE_BUFFER_SIZE)
		{
            return (NTSTATUS)STATUS_INSUFFICIENT_RESOURCES;
		}
		 Buffer  = new BYTE [BufferSize];
    }

    if (!NT_SUCCESS(status))
    { 		
		//MSB(555)
        delete  Buffer ; Buffer = NULL;
        return status;
    }


    *WorkingSetInformation = (PMEMORY_WORKING_SET_INFORMATION)Buffer;


    return status;
}

DWORD CProcess::GetPriority(HANDLE hProcess, CString * lpStr)
{

	CString StrRet;

	DWORD ProcPriority = GetPriorityClass(hProcess);
	StrRet = L"N/A";
	switch (ProcPriority)
	{
	case ABOVE_NORMAL_PRIORITY_CLASS :StrRet=L"Above Normal";break;
	case BELOW_NORMAL_PRIORITY_CLASS :StrRet=L"Below Normal";break;
	case HIGH_PRIORITY_CLASS :StrRet=L"High";break;
	case IDLE_PRIORITY_CLASS :StrRet=L"Low";break;
	case NORMAL_PRIORITY_CLASS :StrRet=L"Normal";break;
	case REALTIME_PRIORITY_CLASS :StrRet=L"Realtime";break;
	}


	*lpStr = StrRet;

	return ProcPriority;
}

double CProcess::_GetPDHData(HQUERY   hQuery,HCOUNTER hCounter)
{
	


	PDH_STATUS pdhStatus;  
	pdhStatus  = PdhCollectQueryData(hQuery);

	if(pdhStatus != ERROR_SUCCESS) 
	{  
		return 0;
	}

	// 获得当前实例的“ID Process”计数器的值
	DWORD ctrType;
	PDH_FMT_COUNTERVALUE fmtValue;
	pdhStatus=PdhGetFormattedCounterValue(hCounter, PDH_FMT_DOUBLE,   &ctrType,&fmtValue);

	if(pdhStatus != ERROR_SUCCESS) 
	{ 
		return 0;
	}

	double Ret  =  fmtValue.doubleValue  ;  

	if(Ret<0){ Ret=0; }

	return   Ret;
}

DWORD CProcess::GetParentPID(PROCLISTDATA * pData )
{	
	 

	LONG status;
	PROCESS_BASIC_INFORMATION  ProcBaseInfo;
	status = MyNtQueryInformationProcess(pData->hProcess,ProcessBasicInformation,(PVOID)&ProcBaseInfo,sizeof(PROCESS_BASIC_INFORMATION),NULL );	
	if (!status)
	{
		DWORD ParentPID =   (DWORD)ProcBaseInfo.Reserved3;
		pData->ParentPID = ParentPID;
		return ParentPID;
	}
	return -1;
}

DWORD CProcess::GetParentPID(DWORD PID)
{
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, PID); 
	LONG status;
	PROCESS_BASIC_INFORMATION  ProcBaseInfo;
	status = MyNtQueryInformationProcess( hProcess,ProcessBasicInformation,(PVOID)&ProcBaseInfo,sizeof(PROCESS_BASIC_INFORMATION),NULL );	
	if (!status)
	{
		DWORD ParentPID = (DWORD)ProcBaseInfo.Reserved3;		
		return ParentPID;
	}
	return -1;
}

ULONG CProcess::GetMemoryPriority(HANDLE hProcess)
{
	
	//DWORD ErrorCode;
 //   BOOL Success;
 //   MEMORY_PRIORITY_INFORMATION MemPrio;
 //   
 //   // Query process memory priority.  

 //   Success = GetProcessInformation(hProcess,ProcessMemoryPriority,&MemPrio,sizeof(MemPrio));
 //   if (Success) 
	//{
	//	return MemPrio.MemoryPriority;
 //   } 


	






	return 0;
}

double CProcess::GetThreadCpuUsage(THREADLISTDATA * pData)
{
	double percent=0;
	if( pData == NULL  ) return 0;
	if( pData->hThread  < 0 ) return 0;


	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;        

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));
	GetThreadTimes( pData->hThread, &ftime, &ftime, &fsys, &fuser);


 

	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));



	//if(CpuTime!=NULL){*CpuTime =  (double)(sys.QuadPart+user.QuadPart) ; }

	percent = (double)(sys.QuadPart - pData->CpuUsage.lastSysCPU.QuadPart) +   (double)( user.QuadPart -  pData->CpuUsage.lastUserCPU.QuadPart);
	percent = percent/(now.QuadPart -  pData->CpuUsage.lastCPU.QuadPart);
	percent  = percent/LogicalProcessorsCount;
	pData->CpuUsage.lastCPU = now;
	pData->CpuUsage.lastUserCPU = user;
	pData->CpuUsage.lastSysCPU = sys;

	percent = percent * 100;

	pData->CPU_Usage = percent;  //保存  ！！！ 
	if(percent<0.0) percent=0.0;
	if(percent>100.0) percent =100.0;
	return  percent ;// _GetCpuUsage ; 
	 
}
