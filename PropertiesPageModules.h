#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "HeadSortList.h"

// CPropertiesPageModules dialog

class CPropertiesPageModules : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertiesPageModules)

public:
	CPropertiesPageModules();
	virtual ~CPropertiesPageModules();

// Dialog Data
	enum { IDD = IDD_PROCESS_PROPERTIES_MODULES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CHeadSortList mModulesList;
	virtual BOOL OnInitDialog();
	DWORD PID;
	int ListModules(BOOL Sort = TRUE);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CBrush Br;




	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
};
