#pragma once



// CModelessPropertySheet

class CModelessPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CModelessPropertySheet)

public:
	CModelessPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CModelessPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CModelessPropertySheet();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
//	virtual void PostNcDestroy();
public:
	HICON hIcon;
	int iIcon;
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClose();
	void DeleteMyObjects(void);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void PlaceAllCtrl(void);
	static int CALLBACK XmnPropSheetCallback(HWND hWnd, UINT message, LPARAM lParam);
	virtual INT_PTR DoModal();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


