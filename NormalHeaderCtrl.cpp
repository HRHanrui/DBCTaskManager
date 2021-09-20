// NormalHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "NormalHeaderCtrl.h"


// CNormalHeaderCtrl

IMPLEMENT_DYNAMIC(CNormalHeaderCtrl, CHeaderCtrl)

CNormalHeaderCtrl::CNormalHeaderCtrl()
: pFlagSortUp(NULL)
, pCurrentSortCol(NULL)
{

}

CNormalHeaderCtrl::~CNormalHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CNormalHeaderCtrl, CHeaderCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CNormalHeaderCtrl message handlers



void CNormalHeaderCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DefWindowProc(WM_PAINT,(WPARAM)dc.m_hDC,(LPARAM)0);

	 
 
	CRect rcArrow;
	this->GetItemRect(*pCurrentSortCol,rcArrow);
	rcArrow.bottom = rcArrow.top+10;

	


	if(theApp.FlagThemeActive)
	{
		HTHEME hTheme=OpenThemeData(this->GetSafeHwnd(), L"HEADER");
		SIZE rcSize;
		if(GetThemePartSize(hTheme,dc.m_hDC,HP_HEADERSORTARROW,HSAS_SORTEDUP,NULL,TS_TRUE,&rcSize)== S_OK)
		{			
			rcArrow.bottom =  rcSize.cy;
			
		}

		int SortType = HSAS_SORTEDUP;

		if(!(*pFlagSortUp)) SortType = HSAS_SORTEDDOWN;
		
		DrawThemeBackground(hTheme,dc.m_hDC,  HP_HEADERSORTARROW ,    SortType ,rcArrow ,NULL); 
		::CloseThemeData(hTheme);

	}
	else
	{
	}

	//setwndtext(rand())

	//dc.FillSolidRect(rcItem,RGB(0,222,0));


}
