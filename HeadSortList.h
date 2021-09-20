#pragma once


// CHeadSortList





class CHeadSortList : public CListCtrl
{
	DECLARE_DYNAMIC(CHeadSortList)

public:
	CHeadSortList();
	virtual ~CHeadSortList();

protected:
	DECLARE_MESSAGE_MAP()


public:
	int CurrentSortCol;
	int NewSortCol;
	BOOL IsSortUp;
	void SortListItems(int iCol);
	afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	PFNLVCOMPARE CompareFunc;
};


