#pragma once



// CHotboxView form view

class CHotboxView : public CFormView
{
	DECLARE_DYNCREATE(CHotboxView)

protected:
	CHotboxView();           // protected constructor used by dynamic creation
	virtual ~CHotboxView();

public:
	enum { IDD = IDD_HOTBOXVIEW };
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
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	int nRow;
	int nCol;
	float *ArrayPerLogicalCpuUsage;
	COLORREF GridColor;
};


