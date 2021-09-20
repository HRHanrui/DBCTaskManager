// TaskList.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "TaskList.h"


// CTaskList

IMPLEMENT_DYNAMIC(CTaskList, CListCtrl)

CTaskList::CTaskList()
: HotItemID(-1)
{

}

CTaskList::~CTaskList()
{
}


BEGIN_MESSAGE_MAP(CTaskList, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(LVN_HOTTRACK, &CTaskList::OnLvnHotTrack)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CTaskList::OnLvnItemchanged)
END_MESSAGE_MAP()



// CTaskList message handlers




void CTaskList::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
{

	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast <NMLVCUSTOMDRAW*> ( pNMHDR );

	CRect rc;

	this->GetClientRect(rc);

	CDC*  pDC = CDC::FromHandle ( pLVCD->nmcd.hdc );

	if ( CDDS_PREPAINT  == pLVCD-> nmcd.dwDrawStage )   //整体背景处理
	{ 

		*pResult = CDRF_NOTIFYITEMDRAW; 
	} 
	else if ( CDDS_ITEMPREPAINT == pLVCD-> nmcd.dwDrawStage )   
	{ 
		LVITEM lvItem; 



		int  iItem = static_cast <int> ( pLVCD-> nmcd.dwItemSpec ); 
		pDC-> SetBkMode ( TRANSPARENT );



		CRect  rcItem; 
		CRect  rcText; 
		CString  StrSubItem; 


		ZeroMemory ( &lvItem, sizeof(LVITEM) ); 

		lvItem.mask  = LVIF_IMAGE | LVIF_STATE ; 
		lvItem.iItem = iItem; 

		lvItem.iImage = iItem;
		lvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED; 
		GetItem ( &lvItem );

		GetItemRect ( iItem, rcItem, LVIR_BOUNDS ); // 		


		rcItem.top+=1;
		pDC->FillSolidRect(rcItem,::GetSysColor(COLOR_WINDOW) );
		rcItem.top-=1;

		rcItem.bottom++;

		if ( lvItem.state & LVIS_SELECTED ) 
		{ 
			if(theApp.FlagThemeActive)
				DrawThemeBackground(theApp.hTheme,pDC->m_hDC,  LVP_LISTITEM ,    LISS_SELECTED ,rcItem ,NULL); 
			else
				pDC->FillSolidRect(rcItem,::GetSysColor(COLOR_HIGHLIGHT) );

		}


		if ( (iItem  ==  HotItemID )  &&  (!( lvItem.state & LVIS_SELECTED ) ))
		{

			DrawThemeBackground(theApp.hTheme,pDC->m_hDC,  LVP_LISTITEM ,    LISS_HOT ,rcItem ,NULL); 

		}


		// --------------------------------------------Draw the icon. -----------------------------------------------------



		::ImageList_Draw( theApp.mImagelist.m_hImageList, lvItem.iImage,    pDC->m_hDC,rcItem.left+8,rcItem.top+5 ,ILD_TRANSPARENT); //16是图标实际尺寸


		StrSubItem = GetItemText(iItem,0) ;



		rcText.CopyRect( rcItem);
		rcText.left=rcText.left+6+LINE_H2;
		rcText.right=rcText.right-6;	


	
		COLORREF TextColor = ( lvItem.state & LVIS_SELECTED )  ? List_HotTextColor : List_NormalTextColor;

		pDC->SetTextColor(TextColor);
		pDC->DrawText(StrSubItem, rcText ,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT );
		StrSubItem = GetItemText(iItem,1) ;

		//无响应
		pDC->SetTextColor(RGB(200,0,0));
		pDC->DrawText(StrSubItem, rcText ,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_RIGHT);
		pDC->SetTextColor(List_NormalTextColor);



	} 


	::ReleaseDC(this->GetSafeHwnd(),pDC->m_hDC); 
	pDC =NULL;






}

void CTaskList::OnLvnHotTrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here



	//检测鼠标hot

	CRect rcItem,rcList,rcOldHot;
	this->GetClientRect(rcList);
	this->GetItemRect(0,rcItem,LVIR_BOUNDS);

	CPoint CurPos;
	::GetCursorPos(&CurPos);

	this->ScreenToClient(&CurPos);

	int NewHotID =(CurPos.y-rcItem.top) /LINE_H2;


	if(HotItemID != NewHotID)
	{		
		HotItemID = NewHotID;
		Invalidate(0);

	}




	*pResult = 0;
}

void CTaskList::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	Invalidate(0);
	*pResult = 0;
}
