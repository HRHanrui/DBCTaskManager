#pragma once


// CDBCBtn

class CDBCBtn : public CButton
{
	DECLARE_DYNAMIC(CDBCBtn)

public:
	CDBCBtn();
	virtual ~CDBCBtn();

protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual void DrawItem(LPDRAWITEMSTRUCT  lpDrawItemStruct );
	afx_msg void OnPaint();
	BYTE Type;
//	afx_msg void OnMove(int x, int y);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	COLORREF TextColor;
//	afx_msg void OnBnClicked();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	HICON hIcon;
	BOOL MouseTrackNow;
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


