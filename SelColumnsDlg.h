#pragma once
#include "afxcmn.h"


// CSelColumnsDlg dialog

class CSelColumnsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelColumnsDlg)

public:
	CSelColumnsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelColumnsDlg();

// Dialog Data
	enum { IDD = IDD_SEL_COLUMNS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl mList;
	CListCtrl *pDetailList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
