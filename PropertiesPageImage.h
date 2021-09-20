#pragma once
#include "afxwin.h"


// CPropertiesPageImage dialog

class CPropertiesPageImage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertiesPageImage)

public:
	CPropertiesPageImage();
	virtual ~CPropertiesPageImage();

// Dialog Data
	enum { IDD = IDD_PROCESS_PROPERTIES_IMAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString mPath;
	virtual BOOL OnInitDialog();
	PVOID pData;


	CString mCmdLine;
	CString mDescription;
	CString mPublisher;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CStatic mIcon;
	afx_msg void OnDestroy();
	SHFILEINFO sfi;
	CString mVersion;
	CString mParent;
	CString mUser;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	HICON  hIcon;
	
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};
