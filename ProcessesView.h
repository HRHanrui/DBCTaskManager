 
#pragma once

 

 
#include "afxcmn.h"
#include "processinfo.h"
#include "DetailsView.h"
#include  "CoolListCtrl.h"





#define  PROCLIST_NAME    0
#define  PROCLIST_TYPE    1
#define  PROCLIST_STATUS   2
#define  PROCLIST_PUB     3
#define  PROCLIST_PID     4
#define  PROCLIST_PNAME    5
#define  PROCLIST_CMDLINE     6
#define  PROCLIST_CPU     7
#define  PROCLIST_MEMORY    8
#define  PROCLIST_DISK     9
#define  PROCLIST_NETWORK     10

 
// CPageProcesses form view

class CPageProcesses : public CFormView
{
	DECLARE_DYNCREATE(CPageProcesses)

protected:
	CPageProcesses();           // protected constructor used by dynamic creation
	virtual ~CPageProcesses();

public:
	enum { IDD = IDD_PAGE_PROCESSES };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CCoolheaderCtrl *CoolheaderCtrl;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	//CListCtrl  mTaskList;

	CPerformanceMon *pPerformanceMon;
	CCoolListCtrl  mTaskList;

	void InitList(void);

	HWND _GetProcessMainWnd(DWORD PID, HWND Dll_hWnd);

//	PROCLISTDATA  *pDetailListDataArray;  //数据来源 进程详细列表

	//afx_msg void OnCustomdrawMyList(NMHDR *pNMHDR, LRESULT *pResult);
	
	//afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	/*APPLISTDATA  *pListDataArray;

	APPLISTDATA  *pAppArray;
	APPLISTDATA  *pBkgArray;
	APPLISTDATA  *pWinArray;*/

	int nApp,nBkg,nWin;// Number of  APP/Background Processes /Windows Processes 

	afx_msg void OnLvnGetdispinfoProcesslist(NMHDR *pNMHDR, LRESULT *pResult);
	
	CProcess mProcInfo;
	
	afx_msg LRESULT  OnUMTimer(WPARAM wParam, LPARAM lParam);

	CPageDetails  *pPageDetails;

	
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	
	void _AddGroupItem(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	APPLISTDATA* AddNewItem(PROCLISTDATA * pDetailData, int ID = 0,BOOL SetBaseInfoOnly = FALSE);
	int _OpenSubList(int ID,BOOL LockDraw = TRUE);
	int _CloseSubList(int ID,BOOL LockDraw = TRUE); //有时不育系改变可重绘状态！！！
	//int ListProcesses(void);
	
	
	int ListItemWindows(int nItem,APPLISTDATA * pListData = NULL );
	
	CString StrWndList;
	BOOL CheckWndChange(void);
   

 
public:
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

	BOOL FlagEnableRefresh;
	void PlaceAllCtrl(void);
	afx_msg void OnNMRClickProcesslist(NMHDR *pNMHDR, LRESULT *pResult);

	void PreListItems(void);
	void FillAllItemData(BOOL LoadAllTrueData = FALSE);
	void DeleteProcessItem(PROCLISTDATA * pDetailsListData);
	void SetProcessStatusInfo(DWORD PID, int Status);

//	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	void Sort(int nCol,BOOL InvertSort = TRUE);
	void ReSort(BOOL SkipStaticColumn = TRUE);
	afx_msg void OnPop_ProcesseslistExpand();
//	afx_msg void OnUpdateProcesseslistExpand(CCmdUI *pCmdUI);
	afx_msg void OnPop_ProcesseslistEndtask();
	afx_msg void OnPop_ProcesseslistGotoDetails();
	
	afx_msg void OnBnClickedBtnEndTask();
	void _OnListItemChaged(void);
	void _RemoveGroupTitle(void);
	afx_msg void OnLvnKeydownProcesslist(NMHDR *pNMHDR, LRESULT *pResult);

	void RefreshList(void);
	afx_msg void OnPop_OpenFileLocation();
	afx_msg void OnPop_Properties();
	afx_msg void OnPop_SwitchTo();
	afx_msg void OnPop_Bringtofront();
	afx_msg void OnPop_Maximize();
	afx_msg void OnPop_Minimize();
	afx_msg void OnPop_Endtask();
	void SetWndItem(void);
	int SvchostIconIndex;
	afx_msg void OnPop_Memory_ShowAsPercents();
	afx_msg void OnPop_Memory_ShowAsValues();
	void _GetMemDataAndSetItemText(int iItem,APPLISTDATA * PData);
	afx_msg void OnPop_Searchonline();
	void UpdateGroupText(void);
};

 