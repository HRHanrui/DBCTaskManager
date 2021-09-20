#pragma once

#include "ProcessInfo.h"

// CPropertiesPagePerformance dialog

class CPropertiesPagePerformance : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertiesPagePerformance)

public:
	CPropertiesPagePerformance();
	virtual ~CPropertiesPagePerformance();

// Dialog Data
	enum { IDD = IDD_PROCESS_PROPERTIES_PERFORMANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CString mStrPriority;
	virtual BOOL OnInitDialog();
	CProcess  mProcess;
	PROCLISTDATA *pData;
	
	CString mStrPrivateBytes;
	CString mStrPeakPrivateBytes;
	CString mStrVirtualSize;
	CString mStrPageFaults;
	CString mStrWorkingSet;
	CString mStrPeakWorkingSet;
	CString mStrWSPrivate;
	CString mStrWSShareable;
	CString mStrReads;
	CString mStrWrites;
	CString mStrOther;
	CString mStrTotalTime;
	ULONGLONG ReadsOld;
	ULONGLONG ReadBytesOld;
	ULONGLONG WritesOld;
	ULONGLONG WriteBytesOld;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void UpdateAllPerformanceItems(void);
	
};
