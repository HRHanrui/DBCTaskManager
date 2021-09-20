// PItemList.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "PItemList.h"






// CPItemList

IMPLEMENT_DYNAMIC(CPItemList, CListCtrl)

CPItemList::CPItemList()
: NumArray(NULL)
, ItemHeight(60)
, nSel(0)

{



}

CPItemList::~CPItemList()
{
}


BEGIN_MESSAGE_MAP(CPItemList, CListCtrl)
	ON_WM_MEASUREITEM_REFLECT() 
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &CPItemList::OnLvnDeleteitem)
	
//	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CPItemList::OnLvnItemchanged)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CPItemList message handlers


void CPItemList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)  //只有自绘风格这个才有效
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//MSB(0);

	lpMeasureItemStruct->itemHeight   = ItemHeight ;

}
void CPItemList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	CDC *pDC;
	//不可以用  GetDC 否则启用半透明选区时 显示有问题 ！！！！ 
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);  

	CRect rcItem;
	CRect rc;
	this->GetClientRect(rc);

	CRect rcMain;
	theApp.m_pMainWnd->GetClientRect(rcMain);

	CDC MemDC;//内存ID表  
	CBitmap MemMap;
	CBitmap *pOldBm;


	rcItem.SetRect(lpDrawItemStruct->rcItem.left,  lpDrawItemStruct->rcItem.top,  rc.right,  lpDrawItemStruct->rcItem.bottom);


	MemDC.CreateCompatibleDC(pDC);  
	MemMap.CreateCompatibleBitmap(pDC,rcItem.Width(),rcItem.Height());
	pOldBm=MemDC.SelectObject(&MemMap);

	MemDC.SelectObject(this->GetFont());

 
	BOOL  ThemeActive = IsThemeActive();

	Graphics graphics(MemDC.m_hDC );
	graphics.SetSmoothingMode(SmoothingModeHighQuality  );


	CRect rcMem;

	rcMem.CopyRect(rcItem);
	rcMem.top=0;
	rcMem.bottom=rcItem.Height();

	//DrawThemeBackground(hThe,MemDC.m_hDC,  LVP_COLLAPSEBUTTON ,    LVCB_HOVER,rcMem ,NULL);  圆形展开按钮
	if ( lpDrawItemStruct->itemState & ODS_SELECTED )
	{
		if(theApp.FlagSummaryView)
		{
			rcMem.right= rcMem.left+rcMain.Width()-6;
		}

		MemDC.FillSolidRect(0,0,rcItem.Width(),rcItem.Height(),theApp.WndBkgColor); //预先填充窗口背景色 底防止重叠
		if(theApp.FlagThemeActive)  
		{
			
			DrawThemeBackground(theApp.hTheme,MemDC.m_hDC,  LVP_LISTITEM ,    LISS_SELECTED ,rcMem ,NULL);
		}
		else
		{	

			MemDC.FillSolidRect(rcMem,::GetSysColor(COLOR_HIGHLIGHT ));
			//graphics.FillRectangle(&SolidBrush(Color(60,112,192,231)),rcMem.left,rcMem.top,rcMem.Width(),rcMem.Height());
			//MemDC.Draw3dRect(rcMem,RGB(112,192,231),RGB(112,192,231));
		}


		
	}
	else
	{
		MemDC.FillSolidRect(0,0,rcItem.Width(),rcItem.Height(),theApp.WndBkgColor);  
		//DrawThemeBackground(hTheme,MemDC.m_hDC,  LVP_COLLAPSEBUTTON ,    LVCB_HOVER,rcMem ,NULL); 
	}


	CRect rcText;

	PerferListData *pData = NULL;
	pData = (PerferListData*) GetItemData(lpDrawItemStruct->itemID);

	///----------------------//波形图标方式-----

	int IconWidth = 80;	//波形图标方式
	

	CRect rcIcon;
	rcIcon.CopyRect(rcMem);

	if(theApp.AppSettings.PerformanceListShowGraph != 0)
	{
		IconWidth = 80;

		rcIcon.right= IconWidth;
		rcIcon.DeflateRect(10,10);

		

		COLORREF BorderColor = pData->pWaveBox->BorderColor;



		 
		MemDC.FillSolidRect(rcIcon,pData->pWaveBox->BkgColor);

		//波形绘制不包含背景及边框

		if(lpDrawItemStruct->itemID == 0)
		{
			DrawWaveBox(&graphics,rcIcon,NumArray,pData);
		}
		else	
		{
			DrawWaveBox(&graphics,rcIcon,pData->pWaveBox->Num[0],pData);
			if(pData->Type == PM_ETHERNET)
			{
				DrawWaveBox(&graphics,rcIcon,pData->pWaveBox->Num2[0],pData,TRUE);
			}
		}


		//SelectObject(MemDC, GetStockObject(NULL_BRUSH));
		//MemDC.Rectangle(rcIcon);
		MemDC.Draw3dRect(rcIcon,BorderColor,BorderColor);

		

	}
	else 
	{


		IconWidth = 40;

		Color BorderColor;
		BorderColor.SetFromCOLORREF(pData->pWaveBox->BorderColor);
	
		//LinearGradientBrush( Point & point1, Point & point2,   Color & color1, Color & color2);
		int X,Y;
		X=rcIcon.left+13;
		Y=rcIcon.top+11;
		graphics.DrawEllipse(&Pen(BorderColor),X,Y,14,14);
	//	BorderColor.SetValue(
	 	graphics.FillEllipse(&LinearGradientBrush(Rect(X,Y,14,14),BorderColor,Color(50,255,255,255),LinearGradientModeVertical) ,X,Y,14,14);

		
		
	}


	graphics.ReleaseHDC(MemDC);
 
 


	//------------------------------------------------------------




	rcText.CopyRect(rcMem);
	rcText.DeflateRect(0,10);//只需要纵向缩小
	rcText.left+=IconWidth; 
	rcText.bottom = rcMem.Height()/2;




	UINT nFormat=DT_TOP|DT_SINGLELINE|DT_WORD_ELLIPSIS|DT_LEFT;


	CString StrTip;


	CFont *mFontSmall = (CFont*)MemDC.SelectObject(&theApp.mTitleFont);


	MemDC.SetBkMode(TRANSPARENT);

	StrTip=GetItemText(lpDrawItemStruct->itemID,0);


	COLORREF TextColor(RGB(0,0,0));


	if( ThemeActive )
	{
		if( lpDrawItemStruct->itemState & ODS_SELECTED )
		{
			GetThemeColor(theApp.hTheme,LVP_LISTITEM,LISS_HOTSELECTED,TMT_TEXTCOLOR ,&TextColor);
			
		}
		else
		{
			GetThemeColor(theApp.hTheme,LVP_LISTITEM,LISS_NORMAL,TMT_TEXTCOLOR ,&TextColor);
		}
		MemDC.SetTextColor(TextColor);
	}
	else
	{
		if( lpDrawItemStruct->itemState & ODS_SELECTED )
		{
			MemDC.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT  ));
		}
		else
		{
			MemDC.SetTextColor(theApp.WndTextColor);
		}
	}




	 
	if(pData->Type == PM_DISK)
	{
		if(pData->StrOther0.GetAt(1)!=L' ')
		StrTip =StrTip+L" ("+pData->StrOther0+L")" ;
	}


	MemDC.DrawText(StrTip ,rcText,nFormat);

	
	MemDC.SelectObject(mFontSmall);
	
 



	//第二行字

	rcText.OffsetRect(0,	rcText.Height()+2);
	StrTip=GetItemText(lpDrawItemStruct->itemID,1);
	MemDC.DrawText(StrTip,	rcText,nFormat);


	MemDC.SetTextColor(theApp.WndTextColor);



//	COLORREF SelbkColor = ::GetSysColor(COLOR_HIGHLIGHT);
//	COLORREF SeltextColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);





	pDC->BitBlt(rcItem.left,rcItem.top,rcItem.Width(),rcItem.Height(),&MemDC, 0, 0, SRCCOPY);



	// pDC->DeleteDC();
	
	


	//mFontSmall->DeleteObject();
	MemMap.DeleteObject();
	MemDC.DeleteDC();
	 
	
}

void CPItemList::OnSize(UINT nType, int cx, int cy)
{
	//CListCtrl::OnSize(nType, cx, cy);

	
	 SetColumnWidth(0,1);

	 InvalidateIfVisible(this);
	


	// TODO: Add your message handler code here
}

void CPItemList::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	PerferListData *pPData = (PerferListData*)GetItemData(pNMLV->iItem);
	if(pPData!=NULL)
	{
		if(pPData->pWaveBox!=NULL) 
		{pPData->pWaveBox->DestroyWindow();
		delete pPData->pWaveBox;
		pPData->pWaveBox = NULL;
		}
	}


	*pResult = 0;
}



void CPItemList::DrawWaveBox( Graphics *pGraph , CRect rc, float * pNum, PerferListData *pData,BOOL GraphB)
{
	//PerferListData *pData


	PointF  PArray[60+1+2];

	int  Seg=60; //标准速度 60  高 120  低 30  

	float StepX  = (float)rc.Width()/Seg;
 
	for(int i=0;i<=Seg;i++)
	{
		//Num[i] =rc.Height()/100* (::GetTickCount()%100);
		PArray[i].X=rc.left+StepX*i;
		PArray[i].Y=rc.bottom-(float)rc.Height()* pNum[i] ;
		if( pNum[i]>=1){ PArray[i].Y = (float)rc.top+1; }
		if( pNum[i]<=0){ PArray[i].Y = (float)rc.bottom-1; }


	}

	





	PArray[0].X=(float)rc.left;
	PArray[Seg].X=(float)rc.right;

	PArray[Seg+1].X=(float)rc.right;
	PArray[Seg+1].Y=(float)rc.bottom;

	PArray[Seg+2].X=(float)rc.left;
	PArray[Seg+2].Y=(float)rc.bottom;

 

	
	 
	Rect  Grc(rc.left,rc.top,rc.Width(),rc.Height());
	pGraph->SetClip(Grc);


	 

	GraphicsPath Path;
	Path.AddLines(PArray,Seg+3);		


	if(GraphB)
	{
		pGraph->FillPath(&SolidBrush(pData->pWaveBox->FillBColor),&Path);  //Color(100,0,222,0)
		pGraph->DrawLines(&Gdiplus::Pen(pData->pWaveBox->LineBColor,1),PArray,Seg+1);  //Color(255,0,180,0)



	}
	else
	{
		pGraph->FillPath(&SolidBrush(pData->pWaveBox->FillAColor),&Path);  //Color(100,0,222,0)
		pGraph->DrawLines(&Gdiplus::Pen(pData->pWaveBox->LineAColor,1),PArray,Seg+1);  //Color(255,0,180,0)
	}




}

void CPItemList::SetRowHeight(UINT Height)
{
	ItemHeight = Height;
	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd  = m_hWnd;
	wp.cx  = rcWin.Width();
	wp.cy  = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);

	SetWindowPos(NULL,0,0,rcWin.Width(),rcWin.Height(),wp.flags);
}


void CPItemList::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if(theApp.FlagSummaryView)
	{
		theApp.m_pMainWnd->PostMessage(WM_NCLBUTTONDOWN,    HTCAPTION,    MAKELPARAM(point.x, point.y)); 
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

void CPItemList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	theApp.m_pMainWnd->PostMessage(UM_SUMMARYVIEW,(WPARAM)(this));
	CListCtrl::OnLButtonDblClk(nFlags, point);
}
