// PMListView.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "PMListView.h"


// CPMListView

IMPLEMENT_DYNCREATE(CPMListView, CFormView)

CPMListView::CPMListView()
	: CFormView(CPMListView::IDD)
{

}

CPMListView::~CPMListView()
{
}

void CPMListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPMListView, CFormView)
END_MESSAGE_MAP()


// CPMListView diagnostics

#ifdef _DEBUG
void CPMListView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPMListView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPMListView message handlers
