// DBCListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "DBCListCtrl.h"



// CDBCListCtrl

IMPLEMENT_DYNAMIC(CDBCListCtrl, CListCtrl)

CDBCListCtrl::CDBCListCtrl()
: RedrawAllColumn(FALSE)
, FullColumnCount(0)
, HotItemID(-1)
, FlagSortUp(FALSE)
, CurrentSortColumn(0)
, pColStatusArray(NULL)
{

	mHdCtrl.pCurrentSortCol = &CurrentSortColumn;
	mHdCtrl.pFlagSortUp = &FlagSortUp;

	

	NormalTextColor = List_NormalTextColor;
	HotTextColor = List_HotTextColor;
	








}

CDBCListCtrl::~CDBCListCtrl()
{
}


BEGIN_MESSAGE_MAP(CDBCListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_SIZE()

	ON_WM_SIZING()
	ON_NOTIFY_REFLECT(LVN_BEGINSCROLL, &CDBCListCtrl::OnLvnBeginScroll)
	ON_NOTIFY_REFLECT(LVN_ENDSCROLL, &CDBCListCtrl::OnLvnEndScroll)
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(LVN_HOTTRACK, &CDBCListCtrl::OnLvnHotTrack)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CDBCListCtrl::OnLvnItemchanged)
END_MESSAGE_MAP()



// CDBCListCtrl message handlers





void CDBCListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{



	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast <NMLVCUSTOMDRAW*> ( pNMHDR );

	CRect rc;

	this->GetClientRect(rc);

	CDC*  pDC = CDC::FromHandle ( pLVCD->nmcd.hdc );

	CHeaderCtrl  *pHeaderCtrl = GetHeaderCtrl();
	if ( CDDS_PREPAINT  == pLVCD-> nmcd.dwDrawStage )   //整体背景处理
	{ 

		int nCol =pHeaderCtrl->GetItemCount();

		CRect rcCol;

		for(int i=0;i<nCol; i++)
		{
			GetSubItemRect(0,i,LVIR_LABEL,rcCol);//不能用  pHeaderCtrl 因为他不会随滚动改变位置
			DrawThemeBackground(theApp.hTheme,pDC->m_hDC,  LVP_COLUMNDETAIL ,    0 ,CRect(rcCol.right-1,0,rcCol.right,rc.bottom) ,NULL); 	 //!!!主题中列表列分割线	
		}


		*pResult = CDRF_NOTIFYITEMDRAW; 
	} 
	else if ( CDDS_ITEMPREPAINT == pLVCD-> nmcd.dwDrawStage )   
	{ 

		LVITEM lvItem; 

		int  iItem = static_cast <int> ( pLVCD-> nmcd.dwItemSpec ); 


		pDC-> SetBkMode ( TRANSPARENT );
		pDC->SetTextColor( theApp.WndTextColor );

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


		if ( lvItem.state & LVIS_SELECTED ) 
		{ 

			if(theApp.FlagThemeActive) 
			{
				rcItem.bottom++;
				DrawThemeBackground(theApp.hTheme,pDC->m_hDC,  LVP_LISTITEM ,    LISS_SELECTED ,rcItem ,NULL); 
				rcItem.bottom--;
			}
			else
				pDC->FillSolidRect(rcItem,::GetSysColor(COLOR_HIGHLIGHT ));


		}

		if ( (iItem  ==  HotItemID )  &&  (!( lvItem.state & LVIS_SELECTED ) ))
		{		
			//不判断 经典界面下 刚好画不上
			DrawThemeBackground(theApp.hTheme,pDC->m_hDC,  LVP_LISTITEM ,    LISS_HOT ,rcItem ,NULL); 			
		}

		// --------------------------------------------Draw the icon. -----------------------------------------------------


	

		CRect rcCol0;
	    GetSubItemRect(iItem,0,LVIR_ICON  ,rcCol0);				
		::ImageList_Draw( theApp.mImagelist.m_hImageList, lvItem.iImage,    pDC->m_hDC,rcCol0.left,rcCol0.top+4 ,ILD_TRANSPARENT); //16是图标实际尺寸
			

		for(int i=0;i<FullColumnCount; i++)
		{
			if(pColStatusArray[i].ColWidth == 0) continue ; //没有的直接跳过

			if ( ! RedrawAllColumn )
			{				
				if(pColStatusArray[i].Redraw == 0   )
				{
					continue ;
				}				
			}

			StrSubItem = GetItemText(iItem,i) ;

			GetSubItemRect(iItem,i,LVIR_LABEL,rcText);       

			rcText.DeflateRect(6,0);

		

			LVCOLUMNW   LvCol;
			LvCol.mask=LVCF_FMT;
			GetColumn(i,&LvCol);

			UINT Align = ( LvCol.fmt &LVCFMT_RIGHT ) ? DT_RIGHT: DT_LEFT;


			//---draw Texts  -----
			
			COLORREF TextColor  =  ( lvItem.state & LVIS_SELECTED ) ? HotTextColor:NormalTextColor;

			pDC->SetTextColor( TextColor );
			pDC->DrawText(StrSubItem, rcText ,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|Align );

			


		}


	} 





	pDC =NULL;
}


void CDBCListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	RedrawAllColumn = TRUE;
	InvalidateIfVisible(this);
	RedrawAllColumn = FALSE;



}




void CDBCListCtrl::OnSizing(UINT fwSide, LPRECT pRect)
{
	CListCtrl::OnSizing(fwSide, pRect);
	RedrawAllColumn = TRUE;
	InvalidateIfVisible(this);
	RedrawAllColumn = FALSE;
	// TODO: Add your message handler code here
}



void CDBCListCtrl::OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Internet Explorer 5.5 or greater.
	// The symbol _WIN32_IE must be >= 0x0560.
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	// TODO: Add your control notification handler code here
	RedrawAllColumn = TRUE;
	this->Invalidate(0);
	*pResult = 0;
}

void CDBCListCtrl::OnLvnEndScroll(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Internet Explorer 5.5 or greater.
	// The symbol _WIN32_IE must be >= 0x0560.
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	// TODO: Add your control notification handler code here

	this->Invalidate(0);
	RedrawAllColumn = FALSE;
	*pResult = 0;
}

void CDBCListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);

	_GetRedrawColumn();
	//RedrawAllColumn = TRUE;
	this->Invalidate(0);
	//RedrawAllColumn = FALSE;
}

CRect CDBCListCtrl::_GetRedrawColumn(void)
{
	CRect rc,rcSub,rcTemp;
	GetClientRect(rc);

	if(pColStatusArray==NULL) return  rc;





	for(int n=0;n<FullColumnCount;n++)
	{
		if(pColStatusArray[n].ColWidth!= 0)
		{
			GetSubItemRect(0,n,LVIR_BOUNDS,rcSub);
			rcSub.top=0;rcSub.bottom=20;
			if(IntersectRect(rcTemp,rc,rcSub))
			{
				pColStatusArray[n].Redraw = 1;
			}
			else
			{
				pColStatusArray[n].Redraw = 0;
			}
		}
		else
		{
			pColStatusArray[n].Redraw = 0;
		}

	}

	//当前排序列一定要设为允许刷新状态！
	if(pColStatusArray!=NULL)
	{
		pColStatusArray[CurrentSortColumn].Redraw = 1;   //!!!!!!!!!!!!!!出处位置
	}


	return  rc;
}

void CDBCListCtrl::InitAllColumn(COLUMNSTATUS * pStatusArray, UINT ColumnStringID,const int ColumnCount)
{
	this->FullColumnCount = ColumnCount;

	
	
	CString StrCol,StrLoad;

	int SubID= 0;  
	
	LVCOLUMNW  Col;
	Col.mask = LVCF_ORDER |LVCF_SUBITEM ;
	Col.iOrder = 0;


	UINT Align;

	StrLoad.LoadString(ColumnStringID);
	
	for( int iArray=0; iArray<ColumnCount; iArray++ )
	{
		Align = (pStatusArray[iArray].Align ==DT_RIGHT ) ? LVCFMT_RIGHT:LVCFMT_LEFT;
				
		AfxExtractSubString(StrCol,StrLoad,iArray,L';');
		InsertColumn(iArray,StrCol,Align,pStatusArray[iArray].ColWidth); 
		if(pStatusArray[iArray].ColWidth == 0)
		{
			SetColumn(iArray,&Col);
		}
	} 

	pColStatusArray = pStatusArray;





}

BOOL CDBCListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	HD_NOTIFY     *pHDNotify   =   (HD_NOTIFY*)lParam;     

	switch   (((NMHDR*)lParam)->code)      
	{ 
	case   HDN_BEGINTRACKW:      
	case   HDN_BEGINTRACKA:      
	case   HDN_DIVIDERDBLCLICKA:      
	case   HDN_DIVIDERDBLCLICKW:       //   pHDNotify->iItem —设定为自己不想改变的列值，比如pHDNotify->iItem=0，就是第一列     //固定列宽
		if   (pColStatusArray[pHDNotify->iItem].ColWidth  ==  0   )
		{      
			*pResult   =   TRUE;                                 //   disable   tracking      
			return   TRUE;      
		}  


		break;    
	}


	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

void CDBCListCtrl::OnLvnHotTrack(NMHDR *pNMHDR, LRESULT *pResult)
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

	int NewHotID =(CurPos.y-rcItem.top) /LINE_H1;



	if(HotItemID != NewHotID)
	{		
		HotItemID = NewHotID;
		Invalidate(0);

	}




	*pResult = 0;
}

void CDBCListCtrl::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CListCtrl::PreSubclassWindow();
	mHdCtrl.SubclassWindow(::GetDlgItem(m_hWnd,0) );
}

void CDBCListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	Invalidate(0);
	*pResult = 0;
}
