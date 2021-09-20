#pragma once
#include "coolheaderctrl.h"
#include "afxwin.h"

 




typedef  struct  CoolListData 
{

	BYTE   ItemType;	
	int   SubType;
	DWORD iImage;
	void  * pPData;
	CString  StrWnd;
	HWND   hMainWnd;
	int   nSubItem;
	ULONGLONG  IOLast;


//-------------
	double CoolUsageArray[11] ; //仅限于此程序 11个足够了！！！//记录每列使数值热度！！！用于控制颜色深度
	//-------------
	int SortID;
	CString StrTitle;

	
	
 
}COOLLISTDATA,APPLISTDATA,USERLISTDATA;



// CCoolListCtrl

class CCoolListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CCoolListCtrl)

public:
	CCoolListCtrl();
	virtual ~CCoolListCtrl();
	afx_msg void OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct); 

	 

protected:
	DECLARE_MESSAGE_MAP()
public:
	COLUMNSTATUS_EX *pColStatusArray;
	HIMAGELIST hImageList;
	void SetImageList(HIMAGELIST hIMGList);
	HTHEME hTheme;
	
	afx_msg void OnLvnHotTrack(NMHDR *pNMHDR, LRESULT *pResult);
	DWORD nHot;
 	CCoolheaderCtrl CoolheaderCtrl;
protected:
	virtual void PreSubclassWindow();
public:
 

	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	int DeleteItemAndSub(int iItem);
	BOOL DrawAllColForSubItem;	 //重要标记 
 	

	BOOL IsProcList;

public:
	CRect _GetRedrawColumn(void);
	int FullColumnCount;
	void InitAllColumn(COLUMNSTATUS_EX * pStatusArray,UINT ColumnStringID,const int ColumnCount,int iStartCool=-1);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

public:



	afx_msg void OnHdnItemchanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	
	void SetHeaderPosSize(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
	
	BOOL PopMenuAction;
 
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
 

 
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
 
	afx_msg void OnHdnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnEnddrag(NMHDR *pNMHDR, LRESULT *pResult);
	
	BOOL FlagSortUp;
	int CurrentSortColumn;
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);

	//int XXX;


	COLORREF GetCoolColor(double Hot);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
protected:
//	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	BOOL FlagDrawAllColumns;
	BOOL ShowOrHideColumn(int iCol);
	void MySetItemText(int iItem, int iCol, CString StrToSet);
	void _DrawGroupIconClassic(CDC * pdc,CRect Rc,BOOL Open = FALSE);
	
};


