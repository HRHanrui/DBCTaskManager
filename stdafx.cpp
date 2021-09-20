// stdafx.cpp : source file that includes just the standard includes
// DBCTaskman.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


int GetDiskVolFailCount = 0;



API_NtSuspendProcess   NtSuspendProcess;
API_NtResumeProcess    NtResumeProcess;






CString   STR_FONTNAME;

CString   MAIN_CAPTION ;
CString   STR_GROUP_APP ;
CString   STR_GROUP_BKG ;
CString   STR_GROUP_WIN ;
//-----------Cpu
CString   STR_CPUINFO_0  ;
CString   STR_CPUINFO_1 ;
CString   STR_CPUINFO_2 ;
CString   STR_CPUINFO_3 ;
CString   STR_CPUINFO_4  ;
CString   STR_CPUINFO_5 ;
CString   STR_CPUINFO_6  ;
CString   STR_TIP1_CPU_TOTAL ;
CString   STR_TIP1_CPU_LOGICAL  ;
CString   STR_TIP1_CPU_NUMA  ;

//------------Memory------------------------
CString   STR_MEMINFO_0 ;
CString   STR_MEMINFO_1;
CString   STR_MEMINFO_2;
CString   STR_MEMINFO_3;
CString   STR_MEMINFO_4 ;
CString   STR_MEMINFO_5;
CString   STR_MEMINFO_6;
CString   STR_TIP3_MEMORY;
CString   STR_TIP5_MEMORY;
//----------Disk------------------------
CString   STR_DISKINFO_0;
CString   STR_DISKINFO_1;
CString   STR_DISKINFO_2;
CString   STR_DISKINFO_3;
CString   STR_DISKINFO_6;

CString   STR_TIP5_DISK;
//-------------Network------------------------
CString   STR_NETWORKINFO_0;
CString   STR_NETWORKINFO_2 ;
CString   STR_NETWORKINFO_6;
CString   STR_TIP3_NETWORK;

//---------  properties dialog  of Process -----------

CString    STR_PROPERTIESDLG_CAPTION;



//------------------------List Text Color ------------------------
COLORREF     List_NormalTextColor;
COLORREF     List_HotTextColor;



int      LogicalProcessorsCount = 1;