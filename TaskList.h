#pragma once
#include "afxwin.h"


// CTaskList

class CTaskList : public CListCtrl
{
	DECLARE_DYNAMIC(CTaskList)

public:
	CTaskList();
	virtual ~CTaskList();

protected:
	DECLARE_MESSAGE_MAP()
public:
 
	afx_msg void OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
	          
	CImageList ImgList;
	afx_msg void OnLvnHotTrack(NMHDR *pNMHDR, LRESULT *pResult);
	int HotItemID;
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


