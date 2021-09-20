#pragma once
#include "PerformanceBox.h"
//#include "PMListView.h"
#include "DBCSplitter.h"
#include "afxwin.h"
#include "dbcbtn.h"
// CPagePerformance form view

class CPagePerformance : public CFormView
{
	DECLARE_DYNCREATE(CPagePerformance)

protected:
	CPagePerformance();           // protected constructor used by dynamic creation
	virtual ~CPagePerformance();

public:
	enum { IDD = IDD_PAGE_PERFORMANCE };
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
	 CRuntimeClass *pViewClass;
     CPerformanceBox *pViewBox;
	// PMListView *pViewList;
	 CDBCSplitterCtrl mSplitter;
 

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);



	void Init(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	

	CDBCBtn mBtnOpenResMon;

	afx_msg void OnBnClickedBtnOpenResourceMonitor();



};


