#pragma once



// CPMListView form view

class CPMListView : public CFormView
{
	DECLARE_DYNCREATE(CPMListView)

protected:
	CPMListView();           // protected constructor used by dynamic creation
	virtual ~CPMListView();

public:
	enum { IDD = IDD_LEFTLISTVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};


