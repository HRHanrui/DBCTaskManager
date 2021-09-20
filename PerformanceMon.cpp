#include "StdAfx.h"
#include "PerformanceMon.h"
#include "DBCTaskman.h"


#define SystemBasicInformation    0
#define SystemPerformanceInformation    2
#define SystemTimeInformation    3
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

CPerformanceMon::CPerformanceMon(void)
 
{
}

CPerformanceMon::~CPerformanceMon(void)
{
}

void CPerformanceMon::Init(void)
{
	 
	MEMORYSTATUSEX  MemStatus;
	MemStatus.dwLength = sizeof(MemStatus);
	BOOL Ret = GlobalMemoryStatusEx(&MemStatus);

	if(Ret)
	{
		theApp.PerformanceInfo.TotalPhysMem = MemStatus.ullTotalPhys;
	}

	

}


int CPerformanceMon::StopMon(void)
{




	//    Close the query.

 


	return 0;
}

 

double CPerformanceMon::GetDiskUsage(int nDisk)
{
	return 0;
}


DWORD CPerformanceMon::_CountSetBits(ULONG_PTR bitMask)
{
	DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;    
    DWORD i;
    
    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
	
	 
}

void CPerformanceMon::GetCpuInfo(void)
{
	
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
    DWORD returnLength = 0;
    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount = 0;
    DWORD processorCoreCount = 0;
    DWORD processorL1CacheCount = 0;
    DWORD processorL2CacheCount = 0;
    DWORD processorL3CacheCount = 0;
    DWORD processorPackageCount = 0;
    DWORD byteOffset = 0;
    PCACHE_DESCRIPTOR Cache;


   
    while (!done)
    {
        DWORD rc = GetLogicalProcessorInformation(buffer, &returnLength);

        if (FALSE == rc) 
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
            {
                if (buffer)free(buffer);

                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);

                if (NULL == buffer) 
                {
                   // _tprintf(TEXT("\nError: Allocation failure\n"));
                    return  ;
                }
            } 
            else 
            {
                //_tprintf(TEXT("\nError %d\n"), GetLastError());
                return  ;
            }
        } 
        else
        {
            done = TRUE;
        }
    }

    ptr = buffer;

	DWORD SizeCacheL1,SizeCacheL2,SizeCacheL3 ;

    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) 
    {
        switch (ptr->Relationship) 
        {
        case RelationNumaNode:
            // Non-NUMA systems report a single record of this type.
            numaNodeCount++;
            break;

        case RelationProcessorCore:
            processorCoreCount++;

            // A hyperthreaded core supplies more than one logical processor.
            logicalProcessorCount += _CountSetBits(ptr->ProcessorMask);
            break;

        case RelationCache:
            // Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
            Cache = &ptr->Cache;
			
            if (Cache->Level == 1)
            {
				SizeCacheL1 = Cache->Size;
                processorL1CacheCount++;
            }
            else if (Cache->Level == 2)
            {
				SizeCacheL2 = Cache->Size;
                processorL2CacheCount++;
            }
            else if (Cache->Level == 3)
            {
				SizeCacheL3 = Cache->Size;
                processorL3CacheCount++;
            }
            break;

        case RelationProcessorPackage:
            // Logical processors share a physical package.
            processorPackageCount++;
            break;

        default:
           // _tprintf(TEXT("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n"));
            break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }

	

	/*CString Str;
	CString StrTemp;

	StrTemp = L"\nGetLogicalProcessorInformation results:\n" ;

	Str=Str+StrTemp;

	StrTemp.Format(L"Number of NUMA nodes: %d\n",  numaNodeCount);
	Str=Str+StrTemp;

    StrTemp.Format(L"Number of physical processor packages: %d\n" ,processorPackageCount);
	Str=Str+StrTemp;

    StrTemp.Format(L"Number of processor cores: %d\n" ,processorCoreCount);
	Str=Str+StrTemp;

    StrTemp.Format(L"Number of logical processors: %d\n",logicalProcessorCount);
	Str=Str+StrTemp;

    StrTemp.Format(L"Number of processor L1/L2/L3 caches: %d/%d/%d\n",processorL1CacheCount,processorL2CacheCount,processorL3CacheCount);    
	Str=Str+StrTemp;*/


	CString StrUnit1=L"KB";
	CString StrUnit2=L"KB";
	CString StrUnit3=L"KB";

	SizeCacheL1 = SizeCacheL1/1024*processorL1CacheCount;
	
	SizeCacheL2 = SizeCacheL2/1024*processorL2CacheCount;
	if(SizeCacheL2>1024) 
	{
		StrUnit2 = L"MB";
		SizeCacheL2=SizeCacheL2/1024;
	}

	SizeCacheL3 = SizeCacheL3/1024*processorL3CacheCount;
	if(SizeCacheL3>1024) 
	{
		StrUnit3 = L"MB";
		SizeCacheL3=SizeCacheL3/1024;
	}


	theApp.PerformanceInfo.nNUMA_node = 	numaNodeCount;
	theApp.PerformanceInfo.nPhysicalProcessorPackages = processorPackageCount;
	theApp.PerformanceInfo.nProcessorCores = processorCoreCount;
	theApp.PerformanceInfo.nLogicalProcessor = logicalProcessorCount;
	theApp.PerformanceInfo.StrL1Cache.Format(L"%d%s",SizeCacheL1,StrUnit1);
	theApp.PerformanceInfo.StrL2Cache.Format(L"%d%s",SizeCacheL2,StrUnit2);
	theApp.PerformanceInfo.StrL3Cache.Format(L"%d%s",SizeCacheL3,StrUnit3);








	//StrTemp.Format(L"%d%s/%d%s/%d%s\n",SizeCacheL1,StrUnit1,SizeCacheL2,StrUnit2,SizeCacheL3,StrUnit3);    
	//Str=Str+StrTemp;
 
    free(buffer);
}

double CPerformanceMon::PdhGetInfo(CString Path)
{

	//

	 HQUERY QueryTemp = NULL;

	Status = PdhOpenQuery(NULL, NULL, &QueryTemp);

	if (Status != ERROR_SUCCESS)   { return 0;	}


	HCOUNTER  Counter ;
	
	Status = PdhAddCounter(QueryTemp, Path, 0, &Counter);
 
	if (Status != ERROR_SUCCESS) {PdhCloseQuery ( QueryTemp ) ;	return 0 ;	}
 
	//-----------------------


	PDH_FMT_COUNTERVALUE DisplayValue;
	DWORD CounterType;
 
	Status = PdhCollectQueryData( QueryTemp);	
	if ( Status != ERROR_SUCCESS)	{PdhRemoveCounter(Counter);	PdhCloseQuery ( QueryTemp ) ;		return 0;	}
 

	Status = PdhGetFormattedCounterValue(Counter,  PDH_FMT_DOUBLE,  &CounterType,   &DisplayValue);

	if (Status != ERROR_SUCCESS) 	{PdhRemoveCounter(Counter);	PdhCloseQuery ( QueryTemp ) ;		return 0;	}


	
	double  Ret=DisplayValue.doubleValue;

   PdhRemoveCounter(Counter);
	Status = PdhCloseQuery ( QueryTemp ) ;
	if ( Status != ERROR_SUCCESS ) {	 ;	}

	 


	return Ret;
}


double CPerformanceMon::GetTotalDiskIO(void)
{
	

	
	return 0;
}

double CPerformanceMon::GetTotalNetworkIO(void)
{

	
	return 0;
}

DISK_PERFORMANCE CPerformanceMon::GetDiskPerformance(int DiskID)
{
 

	DISK_PERFORMANCE OutBuffer;

	CString StrDev ;
	StrDev.Format(L"\\\\.\\PhysicalDrive%d",DiskID);


	DWORD FlagAccess = 0;
//	if(theApp.FlagIsAdminNow){FlagAccess}

	HANDLE hDevice=CreateFile(StrDev, FlagAccess,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);

 

	DWORD	nOutBufferSize = sizeof(DISK_PERFORMANCE);

	DWORD BytesReturned = 0;
	OVERLAPPED Overlapped;


	BOOL Rer=DeviceIoControl( hDevice, IOCTL_DISK_PERFORMANCE,NULL,  0, &OutBuffer,nOutBufferSize,  (LPDWORD) &BytesReturned,(LPOVERLAPPED) &Overlapped );



	CloseHandle(hDevice);

	return OutBuffer;
}
