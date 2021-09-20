#pragma once
#include "afxcmn.h"


// CCfgDlg dialog

class CCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CCfgDlg)

public:
	CCfgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCfgDlg();

// Dialog Data
	enum { IDD = IDD_CFGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl ColorList;
};
