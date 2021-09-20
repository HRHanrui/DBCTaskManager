#pragma once
#include "afxcmn.h"
#include "dbcbtn.h"
#include "DetailsView.h"
#include "MyNormalList.h"



// CPageServices form view

class CPageServices : public CFormView
{
	DECLARE_DYNCREATE(CPageServices)

protected:
	CPageServices();           // protected constructor used by dynamic creation
	virtual ~CPageServices();

public:
	enum { IDD = IDD_PAGE_SERVICES };
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CMyNormalList  mServicesList;
	int ListServices(void);
	CString _GetCurrentStateString(DWORD dwCurrentState);
	CString _GetStartypeString(DWORD dwType);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CDBCBtn mBtnOpenService;
	afx_msg void OnBnClickedBtnOpenServices();
	afx_msg void OnNMRClickServicesList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclickServiceslist(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL FlagSortUp;
	int CurrentSortColumn;
	afx_msg void OnPop_ServiceslistOpenServices();
	afx_msg void OnPop_ServiceslistGotoDetails();
	afx_msg void OnPop_ServiceListStart();
	void UpDateAllItemStatus(BOOL UpdateVisibleOnly = TRUE);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLvnEndScrollServiceslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPop_ServicesListStop();
	
 
	afx_msg void OnPop_Searchonline();
	CString StrSvrStatus[8];
	CString StrSvrStartType[6];
	 
};


