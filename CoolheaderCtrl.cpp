// CoolheaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "CoolheaderCtrl.h"


// CCoolheaderCtrl

IMPLEMENT_DYNAMIC(CCoolheaderCtrl, CHeaderCtrl)

CCoolheaderCtrl::CCoolheaderCtrl()
: Height(0)
, PosX(0)
, Width(0)
, MouseTrackNow(FALSE)
, FlagLeftBtnDown(FALSE)
, StartDragCurPos(0)
, WillSwapToID(0)
, pFlagSortUp(NULL)
, pCurrentSortCol(NULL)
{
	//MyFont.CreateFont(17,0,0,0,FW_NORMAL,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, 	DEFAULT_PITCH | FF_SWISS,_T("Tahoma"));

	hTheme=OpenThemeData(this->GetSafeHwnd(), L"HEADER");
	 
	ARGB aRGB(0  );
	GColor1.SetValue(aRGB);
	GColor2.SetFromCOLORREF(theApp.WndFrameColor);
	
	
}

CCoolheaderCtrl::~CCoolheaderCtrl()
{
	::CloseThemeData(hTheme);
}


BEGIN_MESSAGE_MAP(CCoolheaderCtrl, CHeaderCtrl)

	ON_MESSAGE(HDM_LAYOUT, OnLayout)
	ON_WM_PAINT() 
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEMOVE()
	
	ON_WM_MOUSELEAVE()
	
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CCoolheaderCtrl message handlers



void CCoolheaderCtrl::DrawItem(LPDRAWITEMSTRUCT  lpDrawItemStruct )
{
	 
	
	int nItem = lpDrawItemStruct->itemID;
 
	WCHAR StrTitle[MAX_PATH];

	HDITEM              hdItem;
    hdItem.mask  =  HDI_FORMAT|HDI_TEXT;   
	hdItem.pszText = StrTitle;
	hdItem.cchTextMax = MAX_PATH;
	 
	this->GetItem(nItem,&hdItem);


	if(ColStatusArray[nItem].ColWidth == 0) return;


	CRect  rcItem ;

	rcItem.SetRect(lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.top,lpDrawItemStruct->rcItem.right,lpDrawItemStruct->rcItem.bottom);



//	hTheme




	//rcItem.InflateRect

	//::DrawFrameControl(lpDrawItemStruct->hDC,       &lpDrawItemStruct->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH);


  

	CDC   *pDC   =   new   CDC   ;   
    pDC->Attach(lpDrawItemStruct->hDC)   ;   



 
	
	pDC->FillSolidRect(rcItem,theApp.WndBkgColor);

	



	Graphics  Grap(pDC->m_hDC);

	RectF rcF((float)rcItem.left,(float)rcItem.top+10,(float)rcItem.Width(),(float)rcItem.Height());

	


	CRect rcDrageImage;
	rcDrageImage.CopyRect(rcItem);
	rcDrageImage.DeflateRect(1,1);
	pDC->FillSolidRect(rcDrageImage,theApp.WndBkgColor);

	pDC->SetBkColor(theApp.WndBkgColor);
	// pDC->SetBkMode(0);


	UINT Align = DT_LEFT ;

	Align =ColStatusArray[nItem].Align; 


	

	CRect rcText,rcArrow;
	

	rcText.CopyRect(&rcItem);
	
	rcText.top+=10;
	rcText.InflateRect(-8,-5);
 


	if(*pCurrentSortCol == nItem) //正按此列排序
	{
		rcArrow.CopyRect(&rcItem);

		rcArrow.bottom=rcArrow.top+10;

		if(theApp.FlagThemeActive)
		{
			int SortType = HSAS_SORTEDUP;
			if(!(*pFlagSortUp)) SortType = HSAS_SORTEDDOWN;
			DrawThemeBackground(hTheme,pDC->m_hDC,  HP_HEADERSORTARROW ,    SortType ,rcArrow ,NULL); 

		}
		else
		{
		}
	}



	


	//pDC->SetTextColor(::GetSysColor(COLOR_HOTLIGHT ));


	if(theApp.FlagThemeActive)
	{
	
		pDC->SetTextColor(theApp.CoolHdrColor);
	}
	else
	{
		pDC->SetTextColor(::GetSysColor(COLOR_HOTLIGHT ));
	}
	
	
	pDC->DrawText(StrTitle,&rcText,DT_BOTTOM|DT_SINGLELINE|DT_END_ELLIPSIS|Align);//内容从定义数据中取
	 


	if(ColStatusArray[nItem].Cool)
	{
		
		
		
		CFont *pOldFont = pDC->SelectObject(&theApp.mTitleFont);

		rcText.top=16;
		pDC->SetTextColor(RGB(99,99,99));
		pDC->DrawText(ColStatusArray[nItem].StrItem,&rcText,DT_TOP|DT_SINGLELINE|DT_END_ELLIPSIS|Align);// 
		pDC->SelectObject(pOldFont);
		//MyFont.DeleteObject();


	}


//	pDC->SelectObject(pOldBrush);

	Grap.ReleaseHDC(pDC->m_hDC);
	
    pDC->Detach(); 
	pDC->DeleteDC();
     delete   pDC; 
	 pDC = NULL;

}





LRESULT CCoolheaderCtrl::OnLayout(WPARAM wParam, LPARAM lParam) 
{
	//return 1;

	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam); 
	HD_LAYOUT &hdl = *( HD_LAYOUT * ) lParam; 
	RECT *prc = hdl.prc; 
	WINDOWPOS *pwpos = hdl.pwpos; 
	//表头高度为原来1.5倍，如果要动态修改表头高度的话，将1.5设成一个全局变量 
	//int nHeight = (int)(pwpos->cy * );


	
	pwpos->y = 0;
	//pwpos->cx = Width ;
	pwpos->cy = Height;//46+10; 
	prc->top = 0;//46+10;  //列表项起始位置


	 
	return  lResult; 
    
}  // OnLayout
void CCoolheaderCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here

	// Do not call CHeaderCtrl::OnPaint() for painting messages
 

	CRect rc,rcItem;
	int n = this->GetItemCount();
	//this->GetItemRect(n-1,rcLastItem); 
	this->GetClientRect(rc);

	//应为可以交换列 所以要求出实际最右侧列的位置

	int  SumColWidth=0 ;



	 




	CDC MemDC;//内存ID表  
	CBitmap MemMap;
	CBitmap *pOldBm;

	MemDC.CreateCompatibleDC(&dc); 
	MemMap.CreateCompatibleBitmap(&dc,rc.Width(),rc.Height());
	pOldBm=MemDC.SelectObject(&MemMap);

	 
	

	MemDC.FillSolidRect(rc,theApp.WndBkgColor);

	//这一句是调用默认的OnPaint(),把图形画在内存DC表上  

	DefWindowProc(WM_PAINT,(WPARAM)MemDC.m_hDC,(LPARAM)0);

	//DrawThemeBackground(hTheme,MemDC.m_hDC,  HP_HEADERITEM ,   HIS_PRESSED ,rc ,NULL); 

	





	Graphics  Grap(MemDC.m_hDC);

	RectF rcF((float)rc.left,(float)rc.top+10,1.0,(float)rc.Height());
	LinearGradientBrush  LBr(rcF,GColor1,GColor2,LinearGradientModeVertical );
	LinearGradientBrush  LBrRed(rcF,Color(0,255,0,0),Color(128,255,0,0),LinearGradientModeVertical );
	//Grap.FillRectangle(&LBr,rc.left,10,1,rc.Height());
	

	CRect rcFix;
	rcFix.CopyRect(rc);

	if(!theApp.FlagThemeActive) 
	{		
		rcFix.top= rcFix.bottom-2;
		MemDC.FillSolidRect(rcFix,theApp.WndBkgColor);
	}

	n = this->GetItemCount();

	CRect rcStore;
	for(int i=0;i<n;i++)
	{

		
		this->GetItemRect(i,rcItem);
		rcStore.CopyRect(rcItem);
		SumColWidth=SumColWidth+rcItem.Width();
		rcItem.left=rcItem.right-6;
		MemDC.FillSolidRect(rcItem,theApp.WndBkgColor);
		if(!theApp.FlagThemeActive){rcItem.right-=1;}//实测结果
		Grap.FillRectangle(&LBr,rcItem.right,10,1,rc.Height());
		

		if(ColStatusArray[i].Percents>90)
		{
			Grap.FillRectangle(&LBrRed,rcStore.left,10,rcStore.Width(),rcStore.Height());
			//MemDC.Draw3dRect(rcStore.left,rcStore.top,rcStore.Width(),rcStore.Height(),RGB(200,99,0),RGB(200,99,0));
		}

		
	}





	
	
	//if(theApp.FlagThemeActive)
	//{
	//	//rc.left=SumColWidth+1;  
	//}
	//else
	//{
	//	//rc.left=SumColWidth;
	//}



	rc.left=SumColWidth;

	MemDC.FillSolidRect(rc,theApp.WndBkgColor);
//	MemDC.FillSolidRect(CRect(SumColWidth,0,SumColWidth+2,10),RGB(255,255,255));

	
	GetItemRect(0,rcItem);//修补最左侧不该画出的线
	rcItem.right=rcItem.left+1;
	MemDC.FillSolidRect(rcItem,theApp.WndBkgColor);

	Grap.FillRectangle(&LBr,rc.left,10,1,rc.Height());







	//横线
	
	CPen mPen;

	mPen.CreatePen(0,1,theApp.WndFrameColor);

	CPen *OldPen= MemDC.SelectObject(&mPen);
 
	//MemDC.MoveTo(0,rc.bottom-1);
	//MemDC.LineTo(rc.right,rc.bottom-1);






	
	
	rc.left=0;

	dc.BitBlt(0,0,rc.Width(),  rc.Height(),&MemDC,0, 0,SRCCOPY);  


	Grap.ReleaseHDC(MemDC);

	MemDC.SelectObject(pOldBm);

	mPen.DeleteObject();    

   	MemDC.DeleteDC();
	MemMap.DeleteObject();






}


BOOL CCoolheaderCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
	//return CHeaderCtrl::OnEraseBkgnd(pDC);
}


void CCoolheaderCtrl::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//CRect rc;
	//this->GetParent()->GetClientRect(rc);
	lpwndpos->y =0; //防止列表 水平滚动时 向上移动
	lpwndpos->x=PosX;
	//lpwndpos->cx=PosX+rc.Width();


	//CHeaderCtrl::OnWindowPosChanging(lpwndpos);
}




//BOOL CCoolheaderCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	HD_NOTIFY* pHDN=(HD_NOTIFY*)lParam; 
//	this->GetParent()->SendNotifyMessageW(pHDN->hdr.code,wParam,lParam);
//
//	return CHeaderCtrl::OnNotify(wParam, lParam, pResult);
//}

void CCoolheaderCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	
	
	GetParent()->PostMessageW(UM_HEADER_RCLICK);

	 

}




void CCoolheaderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (!MouseTrackNow)     //  允许开始追踪  
	{
		TRACKMOUSEEVENT csTME;
		csTME.cbSize = sizeof (csTME);
		csTME.dwFlags = TME_LEAVE|TME_HOVER;
		csTME.hwndTrack = m_hWnd ;// 指定要 追踪 的窗口 
		csTME.dwHoverTime = 10;  // 鼠标在按钮上停留超过 10ms ，才认为状态为 HOVER
		::_TrackMouseEvent (&csTME); // 开启 Windows 的 WM_MOUSELEAVE ， WM_MOUSEHOVER 事件支持 
		MouseTrackNow=TRUE ;   // 若已经 追踪 ，则停止 追踪 
	}

	
	//---------------------------------------------


	//if(GetCursor()==AfxGetApp()->LoadStandardCursor(IDC_ARROW)) 
	//{
	//	int n = this->GetItemCount();
	//	CRect rcCol,rcThisCol;
	////	GetItemRect(CurrentCol,rcThisCol);
	//	

	//	 
	//	for(int i=0;i<n;i++)
	//	{
	//		this->GetItemRect(i,rcCol);
	//		if(point.x<StartDragCurPos)
	//		{
	//			rcCol.right=rcCol.left+rcCol.Width()/2;
	//			if(rcCol.PtInRect(point))
	//			{
	//				WillSwapToID = i; //setwndtext(i)

	//				this->GetDC()->FillSolidRect(rcCol,RGB(222,0,0));
	//				

	//			}
	//		}

	//	}

	//}








	CHeaderCtrl::OnMouseMove(nFlags, point);
}



void CCoolheaderCtrl::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default

	FlagLeftBtnDown = FALSE;
	MouseTrackNow=FALSE;
	CHeaderCtrl::OnMouseLeave();
}



void CCoolheaderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default


	if(FlagLeftBtnDown)
	{
			
		FlagLeftBtnDown = FALSE;
		int n = this->GetItemCount();
		CRect rc;

		int iClick = -1;
		for( int i=0;i<n;i++ )
		{
			GetItemRect(i,rc);
			if(rc.PtInRect(point))
			{
				iClick = i;
				Invalidate();//刷新排序箭头状态
				GetParent()->PostMessageW(UM_HEADER_LCLICK,iClick);
				//其他地方点击无效
				break;
			}
		}


		
		
		
	}
	CHeaderCtrl::OnLButtonUp(nFlags, point);
}

void CCoolheaderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default



	 



	if(GetCursor()==AfxGetApp()->LoadStandardCursor(IDC_ARROW)) 
	{
		FlagLeftBtnDown = TRUE;
		StartDragCurPos = point.x;
	}

	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

BOOL CCoolheaderCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
 
	this->GetParent()->PostMessageW(WM_COMMAND, wParam,  lParam);
	return CHeaderCtrl::OnCommand(wParam, lParam);
}
