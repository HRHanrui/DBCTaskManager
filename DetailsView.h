#pragma once
#include "afxcmn.h"
#include "processinfo.h"
#include  "CoolListCtrl.h"
#include "performancemon.h"
#include  "DBCListCtrl.h"
 


#include "DbgHelp.h"
#include "afxdlgs.h"
#pragma comment( lib,  "DbgHelp.lib") // function




#define  COL_SAT_DETAIL   theApp.AppSettings.ColIDS_DetailList

#define  DETAILLIST_NAME      0   
#define  DETAILLIST_TYPE      1   
#define  DETAILLIST_PID       2   
#define  DETAILLIST_STATUS      3 
#define  DETAILLIST_USER          4   
#define  DETAILLIST_SESSIONID        5   
#define  DETAILLIST_CPU          6   
#define  DETAILLIST_CPUTIME       7   
#define  DETAILLIST_MEM_WS      8   
#define  DETAILLIST_MEM_PEAKWS      9   
#define  DETAILLIST_MEM_WSDELTA      10   
#define  DETAILLIST_MEM_PWS      11   
#define  DETAILLIST_MEM_COMMITSIZE      12   
#define  DETAILLIST_MEM_PPOOL      13   
#define  DETAILLIST_MEM_NPPOOL      14   
#define  DETAILLIST_MEM_PF      15   
#define  DETAILLIST_MEM_PFDELTA      16   
#define  DETAILLIST_BP     17   
#define  DETAILLIST_HANDLES     18   
#define  DETAILLIST_THREADS     19   
#define  DETAILLIST_USEROBJ     20   
#define  DETAILLIST_GDIOBJ     21   
#define  DETAILLIST_IO_R     22   
#define  DETAILLIST_IO_W     23   
#define  DETAILLIST_IO_O     24   
#define  DETAILLIST_IO_RB     25   
#define  DETAILLIST_IO_WB     26   
#define  DETAILLIST_IO_OB     27   
#define  DETAILLIST_IMAGEPATH     28   
#define  DETAILLIST_COMMANDLINE     29   
#define  DETAILLIST_UAC     30  
#define  DETAILLIST_DESCRIPTION     31   
#define  DETAILLIST_DEP    32   




//----------------------------------
 



// CPageDetails form view

class CPageDetails : public CFormView
{
	DECLARE_DYNCREATE(CPageDetails)

protected:
	CPageDetails();           // protected constructor used by dynamic creation
	virtual ~CPageDetails();

public:
	enum { IDD = IDD_PAGE_DETAILS };
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
	CDBCListCtrl   mDetailsList;
	CCoolListCtrl *pTaskList;
	CFormView *pUsersView;
	CFormView *pServiceView;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void InitList(void);
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//int ListProcesses(void);
	void AddNewProcessToList(DWORD PID );
public:
	CProcess mProcInfo;
	
	
	afx_msg LRESULT  OnUMTimer(WPARAM wParam, LPARAM lParam);

	void UpdateProcessInfo(  int nListID );
	int AllProcessCpuUsage;

	HQUERY hQuery_SYS ;
	HCOUNTER  hCounter_SYS;
	afx_msg void OnHdnItemclickProcesslist(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	int _GetIconIndex(LPCTSTR lpszPath);

	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void Refresh(void);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void _StartProcessMon(void);

	 
	//-------------

	IWbemLocator *pLoc ;  
	IWbemServices *pSvc ;  
	
	void _RemoveProcessFromList(DWORD PID);
	void _AddToTaskList( PROCLISTDATA *pListData);
	void _AddToUserList( PROCLISTDATA *pListData);
	void _AddToSimpleList(PROCLISTDATA *pListData);

	afx_msg void OnNMRClickDetailslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPop_EndProcess();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnDetailsSelectColumns();
	CString _FormatSizeString(double SizeNum);
	
private:
	 
	//int nSub;
public:
	CPerformanceMon PerformanceMon;

	afx_msg void OnPop_Openfilelocation();
	CString GetProcessInfoUseWMI(DWORD PID,PROCLISTDATA * pData);
	BOOL FlagEnableRefresh;
	 
	afx_msg void OnPop_GotoService();
	void PreLoadProcesses(void);
	void SetAllListItemData(BOOL NewAdd);
	void _FillAllDataOfItem(int Index,PROCLISTDATA *pPLdata = NULL,BOOL NewAdd = FALSE);
	BOOL _IsAppProcess(int PID);

	void SetProcessStatusInfo(DWORD PID,int Status);
	void ReSort(BOOL SkipStaticColumn = TRUE);
	afx_msg void OnLvnItemchangedDetailslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnEndProcess();
	afx_msg void OnPop_SetPriorityRealtime();
	afx_msg void OnPop_SetPriorityLow();
	void _SetSelItemPriority(DWORD Priority);
	afx_msg void OnPop_SetPriorityNormal();
	afx_msg void OnPop_SetPriorityHigh();
	afx_msg void OnPop_SetPriorityAboveNormal();
	afx_msg void OnPop_SetPriorityBelowNormal();
	afx_msg void OnPop_SetAffinity();
	
//---------------------------------------------------------
	map<CString,int>InstanceNameID; // 记录了 每个 进程名CString 有几个实例 int 
	afx_msg void OnPop_Properties();
	
	void GetThreadsConutAllItem(void);
	afx_msg void OnPop_EndProcessTree();
	void _EndProcessTree(void);
	afx_msg void OnPop_HideColumn();
	int CurrentRightClickCol;
	afx_msg void OnPop_CreateMiniDump();
	PROCLISTDATA * _GetSelectionItemData(void);
	void _CreateDumpForSelectionItem(MINIDUMP_TYPE TYPE);
	afx_msg void OnPop_CreateFullDump();
	afx_msg void OnPop_Searchonline();
	
//	afx_msg void OnNMRDblclkDetailslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkDetailslist(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnKeydownDetailslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileSaveList();
	void SaveHtmlFile(CString StrFileName);
	void SaveTxtFile(CString StrFileName);
	void ClearSelecet(void);
};


