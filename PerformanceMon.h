#pragma once

#include <Pdh.h>
#pragma comment ( lib, "Pdh.lib" )
#include <pdhmsg.h>

 
class CPerformanceMon
{
public:
	CPerformanceMon(void);
	~CPerformanceMon(void);
	void Init(void);

public:
	PDH_STATUS Status;
    

	int StopMon(void);
	 
	/*void AddProcessCpuMon(CString StrName=L"SYSTEM");*/
	
	double GetDiskUsage(int nDisk);

 
	DWORD _CountSetBits(ULONG_PTR bitMask);
	void GetCpuInfo(void);
	double PdhGetInfo(CString Path);
	
	double GetTotalDiskIO(void);
	double GetTotalNetworkIO(void);
	DISK_PERFORMANCE GetDiskPerformance(int DiskID);
};
