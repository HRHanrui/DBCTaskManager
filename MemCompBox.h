#pragma once

#include "PerformanceMon.h"
#include "afxcmn.h"
// CMemCompBox

class CMemCompBox : public CStatic
{
	DECLARE_DYNAMIC(CMemCompBox)

public:
	CMemCompBox();
	virtual ~CMemCompBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	void SetColor( );
	COLORREF  LineColor;
	COLORREF  BkgColor;
	COLORREF  ReserveColor;
	COLORREF  InUseColor;
	COLORREF  ModifiedColor;
	COLORREF  StandbyColor;

//	afx_msg void OnEnterSizeMove();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	 
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString GetMemCompType(void);
	int XPosReserve;
	int XPosInUse;
	int XPosMod;
	int XPosStandby;
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


