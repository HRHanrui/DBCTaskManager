// DBCBtn.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "DBCBtn.h"


// CDBCBtn

IMPLEMENT_DYNAMIC(CDBCBtn, CButton)

CDBCBtn::CDBCBtn()
: Type(0)
, MouseTrackNow(FALSE)
{
	TextColor = ::GetSysColor(COLOR_BTNTEXT );


}

CDBCBtn::~CDBCBtn()
{	 
	
}

BEGIN_MESSAGE_MAP(CDBCBtn, CButton)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEHOVER()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CDBCBtn message handlers



 
void CDBCBtn::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CButton::OnPaint() for painting messages



	CDC MemDC;//内存ID表  
	CBitmap MemMap;
	CBitmap *pOldBm;


	CRect rc ;
	this->GetClientRect(rc);

	MemDC.CreateCompatibleDC(&dc); 
	MemMap.CreateCompatibleBitmap(&dc,rc.Width(),rc.Height());
	pOldBm=MemDC.SelectObject(&MemMap);

    	
	 
	MemDC.FillSolidRect(rc,theApp.WndBkgColor);

	CString Str;
	this->GetWindowTextW(Str);
	CFont * pUIFont =this->GetFont();
	CFont *pOldFont = MemDC.SelectObject(pUIFont);
	if(Type == 0)//普通  //区别按钮种类 1 代表是 Fewer details  /More Details  切换按钮 0 是普通的 图标按钮
	{
	
		if(hIcon!=NULL)
		DrawIconEx(MemDC.m_hDC,0+10,(rc.Height()-16)/2,hIcon,16,16,0,NULL,DI_NORMAL);		 
		MemDC.FillSolidRect(0,(rc.Height()-15)/2,1,15,GetSysColor(COLOR_WINDOWFRAME));
		rc.left+=10;
		 
	}
	else  //   Fewer details/More Details切换模式按钮
	{
		
		int PartW,PartH;
		PartW = PartH =16 ;		

		BOOL  ThemeActive = IsThemeActive();

		HTHEME hTheme ;		 
		hTheme=OpenThemeData(this->GetSafeHwnd(), L"TASKDIALOG");
		SIZE rcSize;
		if(GetThemePartSize(hTheme,MemDC.m_hDC,TDLG_EXPANDOBUTTON,TDLGEBS_NORMAL,NULL,TS_TRUE,&rcSize)== S_OK)
		{
			PartW = rcSize.cx ;
			PartH =rcSize.cy;
		}
		
		CRect rcIcon(0,(rc.Height()-PartH)/2,PartW,(rc.Height()-PartH)/2+PartH);	 
		 

		if(ThemeActive)
		{
			int ExpandBtnStateID = 0;//圆形展开按钮具体状态			

			if(theApp.AppSettings.TaskManMode == 1)
			{
				ExpandBtnStateID = MouseTrackNow ? TDLGEBS_EXPANDEDHOVER:TDLGEBS_EXPANDEDNORMAL;
			}
			else
			{
				ExpandBtnStateID = MouseTrackNow ? TDLGEBS_HOVER:TDLGEBS_NORMAL;				
			}

			DrawThemeBackground(hTheme,MemDC.m_hDC,  TDLG_EXPANDOBUTTON ,   ExpandBtnStateID , rcIcon,NULL);  //绘制圆形展开按钮 



		}
		else //经典界面
		{
			Graphics  Grap(MemDC.m_hDC);
			Grap.SetSmoothingMode(SmoothingModeHighQuality );


			PartH = PartW = 18;
			Grap.FillEllipse(&SolidBrush(Color(255,245,245,245)),0,(rc.Height()-PartH)/2,PartW,PartH);
			Grap.DrawEllipse(&Pen(Color(255,0,0,0)),0,(rc.Height()-PartH)/2,PartW,PartH);
								
            PointF PointArray[3];

			if(theApp.AppSettings.TaskManMode == 1)
			{				
				PointArray[0].X = 4.0;                            PointArray[0].Y = (Gdiplus::REAL)(rc.Height()-PartH)/2+11;
				PointArray[1].X =(Gdiplus::REAL)(PartW/2.0);      PointArray[1].Y =(Gdiplus::REAL) (PointArray[0].Y-5);
				PointArray[2].X = (Gdiplus::REAL)(PartW-4.0);     PointArray[2].Y = PointArray[0].Y;				
			}
			else
			{	
				PointArray[0].X = 4.0;                            PointArray[0].Y = (Gdiplus::REAL)(rc.Height()-PartH)/2+6 ;
				PointArray[1].X =(Gdiplus::REAL)(PartW/2.0);      PointArray[1].Y =(Gdiplus::REAL) (PointArray[0].Y+5);
				PointArray[2].X = (Gdiplus::REAL)(PartW-4.0);     PointArray[2].Y = PointArray[0].Y;
				
			}

			Grap.DrawLines(&Pen(Color(255,99,99,99),2),PointArray,3);
			Grap.ReleaseHDC(MemDC.m_hDC);
		
		}
	

		CloseThemeData(hTheme);



	}


	MemDC.SetBkColor(theApp.WndBkgColor);
	MemDC.SetTextColor(TextColor);
	rc.left+=26;
	MemDC.DrawText(Str,rc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	

	

	this->GetClientRect(rc);

	dc.BitBlt(0,0,rc.Width(),  rc.Height(),&MemDC,0, 0,SRCCOPY);  

	
	MemDC.SelectObject(pOldFont);
	MemDC.SelectObject(pOldBm);
	   
   	MemDC.DeleteDC();
	MemMap.DeleteObject();





}


void CDBCBtn::OnMouseMove(UINT nFlags, CPoint point)
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
		MouseTrackNow=TRUE ;   
	}


	
	CButton::OnMouseMove(nFlags, point);
}


void CDBCBtn::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
    if(Type != 0) 
	{		
		theApp.GetMainWnd()->PostMessageW(UM_TOGGLE_TMMODE);
	}
	CButton::OnLButtonDown(nFlags, point);
}

void CDBCBtn::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default

	TextColor = ::GetSysColor(COLOR_BTNTEXT );
	Invalidate();
	MouseTrackNow=FALSE;
	CButton::OnMouseLeave();
}

void CDBCBtn::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	

	if(theApp.FlagThemeActive)
	{
		HTHEME hTheme = OpenThemeData(this->GetSafeHwnd(), L"BUTTON"); 	
		GetThemeColor(hTheme,BP_COMMANDLINK,CMDLS_HOT,TMT_TEXTCOLOR ,&TextColor);
		::CloseThemeData(hTheme);
	}
	else
	{
		TextColor = ::GetSysColor(COLOR_HOTLIGHT);
	}
	Invalidate(0);
	CButton::OnMouseHover(nFlags, point);
}

BOOL CDBCBtn::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
// TODO: Add your message handler code here and/or call default
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}
