#pragma once

#include "wavebox.h"
#include "InfoBox.h"

#include <Pdh.h>
#pragma comment ( lib, "Pdh.lib" )
#include <pdhmsg.h>

struct PerferListData
{
	CWaveBox *   pWaveBox;
	CInfoBox *   pInfoBox;
	CWnd *       pOtherWnd;

	/*HQUERY       Query  ;
    HCOUNTER     Counter;

	HQUERY       QueryA  ;
    HCOUNTER     CounterA;

	HQUERY       QueryB  ;
    HCOUNTER     CounterB;

	HQUERY       QueryOther  ;
    HCOUNTER     CounterOther;*/

	

	//----------------
	BYTE Type ;
	CString StrOther0;
	CString StrOther1;

	ULONG64  DataA;
	ULONG64  DataB;
	ULONG64  DataC;
	ULONG64  DataD;
 
	//HANDLE  hDevice; 
	 
	int  ID;
	double  MaxVar;
	 

}   ;


// CPItemList

class CPItemList : public CListCtrl
{
	DECLARE_DYNAMIC(CPItemList)

public:
	CPItemList();
	virtual ~CPItemList();
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct); 

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);




private:
	void DrawWaveBox( Graphics *pGraph, CRect rc,float * pNum, PerferListData *pData,BOOL GraphB = FALSE);
 
public:
	float* NumArray;
	int ItemHeight;
	void SetRowHeight(UINT Height);
	int nSel;
//	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


