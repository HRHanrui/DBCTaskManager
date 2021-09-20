#pragma once
#include "afxcmn.h"
#include "HeadSortList.h"

// CPropertiesPageHandles dialog

class CPropertiesPageHandles : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertiesPageHandles)

public:
	CPropertiesPageHandles();
	virtual ~CPropertiesPageHandles();

// Dialog Data
	enum { IDD = IDD_PROCESS_PROPERTIES_HANDLES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CHeadSortList mHandlesList;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int ListHandles(void);
	DWORD PID;

	 
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
