#pragma once


#include "afxcmn.h"
#include "CoolListCtrl.h"
 


 

#define     USERLIST_USER            0   
#define     USERLIST_ID              1   
#define     USERLIST_SESSION         2   
#define     USERLIST_CLIENTNAME      3   
#define     USERLIST_STATUS          4   
#define     USERLIST_CPU             5   
#define     USERLIST_MEMORY          6   
#define     USERLIST_DISK            7   
#define     USERLIST_NETWORK         8   







// CUserView form view

class CPageUsers : public CFormView
{
	DECLARE_DYNCREATE(CPageUsers)

protected:
	CPageUsers();           // protected constructor used by dynamic creation
	virtual ~CPageUsers();

public:
	enum { IDD = IDD_PAGE_USERS };
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
	void InitList(void);
	CCoolListCtrl  mUserList;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int ListUsers(void);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CListCtrl *pDetailList;
	CFormView  *pPageDetails;
	afx_msg void OnSessionChange(UINT nSessionState, UINT nId);
	int AddUserItem(int nItem, UINT SessionID);
	void UpdateUserStatus(UINT nSessionID,UINT nSessionState);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int _OpenSubList(int ID,BOOL Lock = TRUE);
	int AddProcessItem(PVOID  pListData, int nID=0);
	int _CloseSubList(int ID,BOOL Lock = TRUE);
	
	void _SetMemUsage(PVOID pData, int nID,BOOL Update = FALSE);// 
	void ReSort(void);

public:
	afx_msg LRESULT  OnUMTimer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
//	afx_msg void OnNMDblclkUserlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickUserlist(NMHDR *pNMHDR, LRESULT *pResult);
	void DeleteProcessItem(PVOID pDetailsListData);
	
	PVOID pPerformanceMon;
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnUserslistManageUserAccounts();
	void Sort(int nCol,BOOL InvertSort=TRUE);
	afx_msg void OnPop_Endtask();
	afx_msg void OnPop_Openfilelocation();
	afx_msg void OnPop_Properties();
	afx_msg void OnPop_Disconnect();
	afx_msg void OnPop_Gotodetails();
	afx_msg void OnPop_Searchonline();
};


