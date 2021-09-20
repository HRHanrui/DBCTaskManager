#pragma once

#include <Pdh.h>
#pragma comment ( lib, "Pdh.lib" )
#include <pdhmsg.h>

#include "TlHelp32.h"

#define MY_LARGE_BUFFER_SIZE  (16 * 1024 * 1024)
#define STATUS_INSUFFICIENT_RESOURCES (0xC0000009A) 

//*********************************************************************************************



typedef struct _MEMORY_WORKING_SET_BLOCK
{
    ULONG_PTR Protection : 5;
    ULONG_PTR ShareCount : 3;
    ULONG_PTR Shared : 1;
    ULONG_PTR Node : 3;
#ifdef _M_X64
    ULONG_PTR VirtualPage : 52;
#else
    ULONG VirtualPage : 20;
#endif
} MEMORY_WORKING_SET_BLOCK, *PMEMORY_WORKING_SET_BLOCK;
typedef struct PROCESS_WS_COUNTERS
{
    ULONG NumberOfPages;
    ULONG NumberOfPrivatePages;
    ULONG NumberOfSharedPages;
    ULONG NumberOfShareablePages;
} PROCESS_WS_COUNTERS, *PPROCESS_WS_COUNTERS;
//
typedef struct _MEMORY_WORKING_SET_INFORMATION
{
    ULONG_PTR NumberOfEntries;
    MEMORY_WORKING_SET_BLOCK WorkingSetInfo[1];
} MEMORY_WORKING_SET_INFORMATION, *PMEMORY_WORKING_SET_INFORMATION;


typedef enum _MEMORY_INFORMATION_CLASS
{
    MemoryBasicInformation,
    MemoryWorkingSetInformation,
    MemoryMappedFilenameInformation,
    MemoryRegionInformation,
    MemoryWorkingSetExInformation
} MEMORY_INFORMATION_CLASS;




//===============================================================



typedef NTSTATUS  (WINAPI  *MyNtQueryVirtualMemory)(  
										  IN HANDLE                  hProcess ,                 //Ŀ����̾��  
										  IN PVOID                   BaseAddress,               //Ŀ���ڴ��ַ  
										  IN MEMORY_INFORMATION_CLASS   MemoryInformationClass,       //��ѯ�ڴ���Ϣ�����  
										  OUT PVOID                   Buffer,                      //���ڴ洢��ȡ�����ڴ���Ϣ�Ľṹ��ַ  
										  IN ULONG                   Length,                      //Buffer����󳤶�  
										  OUT PULONG                   ResultLength OPTIONAL); 
								 


///-----------


static MyNtQueryVirtualMemory NtQueryVirtualMemory;




//*********************************************************************************************

























struct   CPU_USAGE
{	
	//HQUERY hQueryCPU;
	//HCOUNTER hCounterCPU ;

	 ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
//	 int numProcessors;

} ;






typedef struct   ProcessListData
{
	
	CString Name; //������ 
		
	CString Description; //����
	
	int SameNameID; //ͬ��ʱ�����ֱ�� ע��ֻ�б��
	 
	
	DWORD  PID; //����PID
	DWORD  ParentPID; //����PID
	CString  User;

	DWORD SessionID;



//---------------------------
	double  CPU_Usage;

    double  Mem_PrivateWS;
	double  Mem_WorkingSet;
	double  Mem_PeakWorkingSet;
	double  Mem_PagePool;
	double  Mem_NonPagePool;
	double  Mem_PageFaults;
	double  Mem_Commit;

	int ThreadsCount;


	BYTE  Type;// ǰ̨ ��̨ ϵͳ
	 
	//-------------------------
	int IconIndex;



	//-------------------------
	HANDLE hProcess; //PID���
	
	CPU_USAGE CpuUsage;

	ULONGLONG  DiskIO;
	ULONGLONG  OtherIO;	


	//---------PDH-------------



	HQUERY hQueryMem;
	HCOUNTER hCounterMem ;

	/*HQUERY hQueryIO;
	HCOUNTER hCounterIO ;*/


	HQUERY hQueryWs;
	HCOUNTER hCounterWs ;



	//-------------------------



	int SortID;//��������� ID ����ǰ������ֵΪ ��ǰ���б����ǵڼ���


}PROCLISTDATA,*PPROCLISTDATA;

 


struct THREADLISTDATA
{
	double  CPU_Usage;
	HANDLE hThread; //PID���
	CPU_USAGE CpuUsage;

};
//---------------------



class CProcess
{
public:
	CProcess(void);
	~CProcess(void);


public:
	CString GetUser(HANDLE hProcess,UINT PID,WCHAR *StrDomain = NULL);
	double GetWsPrivate_PDH(ProcessListData *pData);

	CString GetPathName(HANDLE hProcess,BOOL FullPath = TRUE);



	


	 CString GetFileName(CString FullPathName);
	 ULONGLONG GetDiskIO(HANDLE hProcess,ULONGLONG * pOther);
	 double GetIOUsage(CString SrtProcessNameID,HQUERY   hQuery, HCOUNTER hCounter);
	 
	//void GetProcessAllInfo(UINT dwPID);
	 double GetMemUsageInfo(ProcessListData *pData);

	 int GetThreadCount(ProcessListData *pData);
	 int GetHandles(HANDLE hProcess);
	 CString GetProcCommandLine(DWORD PID);
	 CString GetVerInfoString(CString FilePathName,CString StrInfType);
	 double GetCpuUsage(PROCLISTDATA* pListData,double * CpuTime = NULL);
	 double GetWsPrivate(DWORD PID);
	 NTSTATUS _GetProcessWorkingSetInformation(HANDLE hProcess, PMEMORY_WORKING_SET_INFORMATION* WorkingSetInformation);
	 DWORD GetPriority(HANDLE hProcess, CString * lpStr);
	 double _GetPDHData(HQUERY   hQuery,HCOUNTER hCounter);
	 DWORD GetParentPID(PROCLISTDATA * pData );  
	 DWORD GetParentPID(DWORD PID);

	 ULONG GetMemoryPriority(HANDLE hProcess);
	 static double GetThreadCpuUsage(THREADLISTDATA * pData);
};








