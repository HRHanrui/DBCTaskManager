

#pragma once
#include "wavebox.h"
#include "pitemlist.h"
#include "InfoBox.h"
#include "performancemon.h"
#include "HotboxView.h"
#include "afxcmn.h"

struct LOGICALCPUPDH
{
	HQUERY Query  ;
    HCOUNTER Counter;
};

 

// CPerformanceBox form view

class   CPerformanceBox: public CFormView
{
	DECLARE_DYNCREATE(CPerformanceBox)

protected:
	CPerformanceBox();           // protected constructor used by dynamic creation
	virtual ~CPerformanceBox();

public:
	enum { IDD = IDD_PAGE_PERFORMANCE_BOX };
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
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);


 


	afx_msg void OnSize(UINT nType, int cx, int cy);
	//CWaveList mCPUGraphic;
	void Init(void);
//	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void GetCPUInfo(void);
	CString StrCPUName;
 
	CPItemList mPItemList;

	afx_msg LRESULT  OnUMTimer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnItemchangedListPerformanceitem(NMHDR *pNMHDR, LRESULT *pResult);
	
	int AddEthernetAdapterToList(void);
	CInfoBox *pInfoBoxCpu;
	CInfoBox *pInfoBoxMemory;
	CWaveBox * pTotalCpuBox;
	CWaveBox * pCpuBox;

 

	
	int AddDiskToList(int LastID =0);
	void PlaceAllCtrl(void);
	BOOL _NumberIsPN(int Number);
	int nLogicalProcessor;
	void _GetDiskOtherStaticInfo( );
		
	
	void UpdateInfoBox(void);
	
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	int UpdateAllPMInfo(void);
	afx_msg void OnPop_ChangeGraphToOverallutilization();
	afx_msg void OnPop_ChangeGraphToLogicalprocessors();
	afx_msg BOOL SetTipText( UINT ID, NMHDR * pTTTStruct, LRESULT * pResult );
	CPerformanceMon PM;

	void _UpdateCpuInfoBox(BOOL UpdateAll = FALSE);
	void _UpdateMemoryInfoBox(BOOL UpdateAll = FALSE);
	void _UpdateNetworkInfoBox( PerferListData* pData,double Received,double Sent,BOOL UpdateAll = FALSE);
	void _UpdateDiskInfoBox(PerferListData* pData,double Read,double Write,double ActiveTime,double Other,BOOL UpdateAll = FALSE);

//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//----------------

	void _GetLogicalProcessorUsage(CWaveBox *pBox,double * pTotalUsage,double * pTotalKernelUsage);	
 	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION * spi_old;
	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION * spi;

	void _DetermineRowCol(int nLogicalProcessor , int * pNRow, int * pNCol);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	
	afx_msg void OnPerformanceShowkerneltimes();
	afx_msg void OnUpdatePerformanceShowkerneltimes(CCmdUI *pCmdUI);
	
	
	afx_msg void OnViewCpu();
	afx_msg void OnViewMemory();
	PERFORMANCE_INFORMATION MyPMInfo;
	int nStep;
	double _TryToChangeDiskMaxVar( double DataA,double DataB,PerferListData *pPData   );
	double _TryToChangeNetworkMaxVar(double DataA,double DataB,PerferListData *pPData );
	BOOL FlagFirstUpdate;
	void _LoadDiskStaticInfo(PerferListData* pData);
	afx_msg void OnNMRClickListPerformanceitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPop_PerformanceListShowHideGraphs();
	
	BOOL ShowThisPage;
protected:
//	virtual void PreSubclassWindow();
public:
	double MaxCPUSpeed;
//	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnUpdatePerformancetypeHidegraphs(CCmdUI *pCmdUI);
	PerferListData * _InsertNetAdapterItem(CString StrType, CString StrDescription,int IfIndex);
	CRuntimeClass *pViewClass;
	CHotboxView *pHotBox;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPop_ChangeGraphToNumaNodes();

//	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	int _TestDiskID(int TestID);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	afx_msg void OnPop_GraphSummaryView();	
	afx_msg void OnPop_PerformanceListSummaryView();
	CToolTipCtrl mToolTip;
	int CurrentDiskCount;
	void _GetDiskLetterUseWmi(void);
	void MySetItem(int iItem, CString Strtext);

	size_t ArraySize;
	
	
	void _FakeSelPItem(int ID);
	double _GetSurrentCpuSpeed(void);
};


