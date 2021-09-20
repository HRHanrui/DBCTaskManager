#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSetAffinityDlg dialog

class CSetAffinityDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetAffinityDlg)

public:
	CSetAffinityDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetAffinityDlg();

// Dialog Data
	enum { IDD = IDD_AFFINITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl mCpuList;
	CString StrProcName;
	CStatic mTitle;
	HANDLE  hProcess;
	DWORD PID;
	afx_msg void OnLvnItemchangedAffinitycpulist(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL BlockItemChangeMessage;
	afx_msg void OnBnClickedOk();
};
