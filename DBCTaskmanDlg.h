// DBCTaskmanDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "processinfo.h"
#include "performancemon.h"
#include "wavebox.h"
#include "DetailsView.h"
#include "PerformanceView.h"
#include "servicesview.h"
#include "ProcessesView.h"
#include "usersview.h"
#include "MyTabCtrl.h"
#include "dbcbtn.h"
#include "atltypes.h"
#include "afxwin.h"
#include "TaskList.h"
#include "StartupView.h"





//typedef  void (*PROCSETHOOK)(HWND hWNd);
//typedef  void (*PROCUNHOOK)();





// CDBCTaskmanDlg dialog
class CDBCTaskmanDlg : public CDialog
{
	// Construction
public:
	CDBCTaskmanDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	enum { IDD = IDD_DBCTASKMAN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
public:


	void PlaceAllCtrl(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);



	void InitAll(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CPerformanceMon mPerformanceMon;
	CMyTabCtrl mTab;
	afx_msg void OnTcnSelchangePageTab(NMHDR *pNMHDR, LRESULT *pResult);


	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	CRuntimeClass *pViewClass;

	CPageProcesses *pPageProcesses;   
	CPagePerformance * pPagePerformance;
	CPageDetails *pPageDetails;
	CPageServices *pPageServices;
	CPageUsers *pPageUsers;
	CPageStartup *pPageStartup;


	// PROCSETHOOK  SatrtShellHook;
	//PROCUNHOOK   StopShellHook;

	void InitPages(void);


	afx_msg void OnSessionChange(UINT nSessionState, UINT nId);	
	afx_msg void OnMM_UpdatespeedHigh();
	afx_msg void OnMM_UpdateSpeedNormal();
	afx_msg void OnMM_UpdateSpeedLow();
	afx_msg void OnMM_UpdateSpeedPaused();
	void UpdateUpTime(void);


	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	


public:
	afx_msg void OnClose();	
	void _SetTimeTipStr(UINT nStrID);
	CDBCBtn mBtnFewerOrMore;


	CMenu *pMainMenu;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	 
	afx_msg void OnMM_FileRunNewTask();
	CTaskList mSimpleTaskList;
	int ListTask(void);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void _SetToSimpleMode(BOOL SaveWndPos = TRUE);
	void _SetToAdvanceMode(BOOL SaveWndPos  = TRUE);
	BOOL FirstShow;
	void SortSimpleList(void);
	void DeleteProcessItem(PROCLISTDATA * pDetailsListData);
	afx_msg void OnNMRClickSimpletaskList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPop_SimplelistSwitchTo();
	afx_msg void OnPop_SimplelistEndTask();
	void TestHungProcess(void);
	void SetProcessStatusInfo(DWORD PID, int Status);
	afx_msg void OnExitSizeMove();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	
	void UpdateSimpleList(void);
	BOOL SimpleListSortEnable;
	NOTIFYICONDATA mNotifyIconData;
	void _HICONFromCBitmap(CBitmap& bitmap,HICON *phIcon);
	afx_msg void OnNcDestroy();
	
	void SetTrayIcon(DWORD Message);

	afx_msg void OnPop_NotifyRestore();	
	afx_msg void OnMM_GroupByType();
	
	void _RegisterForDevChange(void);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


public:
	afx_msg void OnPop_SimplelistOpenfilelocation();
	afx_msg void OnPop_SimplelistProperties();
	afx_msg void OnPop_SimplelistAlwaysOnTop();
	
	void InitUpTime(void);
	

	void AddToSimpleList(PROCLISTDATA * pData);
	void DeleteFromSimpleList(PROCLISTDATA * pData);
	LRESULT OnThemeChanged();
	void SetSimpleListItemsIcon(void);
	


	afx_msg void OnFileSaveDetailsList();
	void SaveScreenshot(CString StrFileName);
	int _GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	afx_msg void OnFileSaveScreenshot();
	afx_msg void OnLvnKeydownSimpletaskList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	void SetUpdateSpeed(float UpdateTime, UINT TimeStrID);
	void ExpandOrCollapse(BOOL Collapse );
};
