#pragma once
#include "afxcmn.h"
#include "ProcessInfo.h"

typedef map<DWORD, THREADLISTDATA>  THREADDATA ;


// CPropertiesPageThreads dialog

class CPropertiesPageThreads : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertiesPageThreads)

public:
	CPropertiesPageThreads();
	virtual ~CPropertiesPageThreads();

// Dialog Data
	enum { IDD = IDD_PROCESS_PROPERTIES_THREADS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	DWORD PID;
	CListCtrl mThreadsList;
	virtual BOOL OnInitDialog();
	int PlaceAllCtrl(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int LoadThreadList(BOOL Init = FALSE);
	//FILETIME CPUTime0;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	THREADDATA   ThreadListData;
};
