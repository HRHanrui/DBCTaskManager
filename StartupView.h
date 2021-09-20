#pragma once
#include "afxcmn.h"
#include "CoolListCtrl.h"
 
#define COL_COUNT_STARTUP  5



#define STARTUPLIST_NAME           0   
#define STARTUPLIST_STATUS         1   
#define STARTUPLIST_PUB            2
#define STARTUPLIST_TYPE           3 
#define STARTUPLIST_CMDLINE        4 


struct STARTUPITEMDATA
{
	CString  FilePath;
	CString  EntryPath;
};





// CPageStartup form view

class CPageStartup : public CFormView
{
	DECLARE_DYNCREATE(CPageStartup)

protected:
	CPageStartup();           // protected constructor used by dynamic creation
	virtual ~CPageStartup();

public:
	enum { IDD = IDD_PAGE_STARTUP };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CCoolListCtrl mStartupList;
	void InitList(void);
	void ListStartupItem(CString StrRegPath);
	COLUMNSTATUS_EX     ColIDS_StartupList[COL_COUNT_STARTUP];
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMRClickStartup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPop_Openfilelocation();
	afx_msg void OnPop_Properties();
	int ListStartupFromMenu(void);
	void AddItem(int ID,CString StrTitle,CString StrCmd,CString StrFilePath,CString StrEntryPath,CString StrStartupType,int IconID);
	CString _GetShortcuTarget(CString StrLnkFile);


	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void _JumpToRegKey(CString StrTargetKey);
	afx_msg void OnPop_JumpToEntry();
	afx_msg void OnPop_Searchonline();
};


