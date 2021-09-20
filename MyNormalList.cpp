// MyNormalList.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "MyNormalList.h"


// CMyNormalList

IMPLEMENT_DYNAMIC(CMyNormalList, CListCtrl)

CMyNormalList::CMyNormalList()
{

}

CMyNormalList::~CMyNormalList()
{
}


BEGIN_MESSAGE_MAP(CMyNormalList, CListCtrl)
END_MESSAGE_MAP()



// CMyNormalList message handlers



void CMyNormalList::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CListCtrl::PreSubclassWindow();
	mHdCtrl.SubclassWindow(::GetDlgItem(m_hWnd,0) );
}
