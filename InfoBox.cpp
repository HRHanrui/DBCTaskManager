// InfoBox.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "InfoBox.h"


// CInfoBox

IMPLEMENT_DYNAMIC(CInfoBox, CStatic)

CInfoBox::CInfoBox()
{
	int W = 80 ;
	
	CRect rc(0,0,W,14);

	for(int i=0;i<7;i++)
	{
		Info[i].rc.CopyRect(rc);
		rc.OffsetRect(W,0);
		if(i%2!=0)
		{
			rc.OffsetRect(-(W+W),48);
		}
		Info[i].Type = 0;
			 
	}

	Info[6].rc.SetRect(215,0,215+180,90);



}

CInfoBox::~CInfoBox()
{
}


BEGIN_MESSAGE_MAP(CInfoBox, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CInfoBox message handlers



void CInfoBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	CRect rc;

	this->GetClientRect(rc);


  
  
	CDC MemDC;//ÄÚ´æID±í  
	CBitmap MemBitmap;
	CBitmap *pOldBm;

	MemDC.CreateCompatibleDC(&dc);  
	MemBitmap.CreateCompatibleBitmap(&dc,rc.Width(),rc.Height());
	pOldBm=MemDC.SelectObject(&MemBitmap);

	MemDC.SelectObject(this->GetFont()); 
  	MemDC.FillSolidRect(0,0,rc.Width(),rc.Height(),theApp.WndBkgColor);  


	#ifdef  IS_CHS
	  Gdiplus::Font  FontSmall(STR_FONTNAME, 12, FontStyleRegular, UnitPixel);
	#else
	  Gdiplus::Font  FontSmall(STR_FONTNAME, 8, FontStyleRegular, UnitPoint);
    #endif

	Gdiplus::Font  FontLarge(STR_FONTNAME, 18, FontStyleRegular, UnitPixel);
 

	Graphics graphics(MemDC.m_hDC );
	graphics.SetSmoothingMode(SmoothingModeHighQuality );
	graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	StringFormat stringFormat;  
	stringFormat.SetFormatFlags(  StringFormatFlagsNoWrap|StringFormatFlagsDisplayFormatControl    );
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment( StringAlignmentNear);

	Gdiplus::Color InfoTextColor(255,GetRValue(theApp.WndTextColor),GetGValue(theApp.WndTextColor),GetBValue(theApp.WndTextColor));

	



	int dx=0;
	for(int i=0;i<6;i++)
	{
		if(Info[i].Type == 0)	
	   {
			dx = 0;
		}
		else 
		{
			dx = 5;
			float Top = (float)Info[i].rc.top+3;
			if(Info[i].Type == 1)
			{
				 				 
				graphics.DrawLine(&Pen(LineClrA,2),(float)Info[i].rc.left+1,Top,(float)Info[i].rc.left+1,Top+40);
			}
			else
			{
				Gdiplus::Pen  MyDashPen(Color(LineClrB),2);
				MyDashPen.SetDashStyle(DashStyleDash);
				graphics.DrawLine(&MyDashPen,(float)Info[i].rc.left+1,Top,(float)Info[i].rc.left+1,Top+40);

			}
		}


		
		graphics.DrawString(Info[i].StrTitle,   (INT)Info[i].StrTitle.GetLength(),&FontSmall,PointF((float)Info[i].rc.left+dx,(float)Info[i].rc.top),&stringFormat,&SolidBrush(Color(255, 112 ,112,112)));
		graphics.DrawString(Info[i].StrInfo,   (INT)Info[i].StrInfo.GetLength(),&FontLarge,PointF((float)Info[i].rc.left+dx,(float)Info[i].rc.top+20),&stringFormat,&SolidBrush(InfoTextColor));
 
		

	}

	
	
	PointF pos[8];
	float dy = 0;

	int n=0;

	WCHAR  StrTemp[MAX_PATH];
	WCHAR * pStr;
	WCHAR  * Temp   ;

	StringCchCopy(StrTemp,MAX_PATH,Info[6].StrTitle);	 

	pStr=wcstok_s(StrTemp,L"\n",&Temp);


	for(int i=0;i<8;i++)
	{
		
		if(pStr!=NULL)
		graphics.DrawString(pStr,   (INT)wcslen(pStr),&FontSmall,PointF((float)Info[6].rc.left,(float)Info[6].rc.top+dy),&stringFormat,&SolidBrush(Color(255,112,112,112)));
	
		pStr=wcstok_s(NULL,L"\n",&Temp);
		dy+=17;

	}

	StringCchCopy(StrTemp,MAX_PATH,Info[6].StrInfo);	
	pStr=wcstok_s(StrTemp,L"\n",&Temp);
	dy=0;
	float x= (float) (Info[6].rc.left+115);
	 
	for(int i=0;i<8;i++)
	{
			
		if(pStr!=NULL)
		graphics.DrawString(pStr,   (INT)wcslen(pStr),&FontSmall,PointF(x ,(float)Info[6].rc.top+dy),&stringFormat,&SolidBrush(InfoTextColor));
	
		pStr=wcstok_s(NULL,L"\n",&Temp);
		dy+=17;

	}

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &MemDC, 0, 0, SRCCOPY);

   MemDC.SelectObject(pOldBm); 
    MemBitmap.DeleteObject();

	graphics.ReleaseHDC(MemDC.m_hDC);
	 


 
	ReleaseDC(&MemDC);
 
    MemDC.DeleteDC();




}

void CInfoBox::SetColor(void)
{
	if(Type == PM_DISK )
	{
		if(theApp.AppSettings.DiskColor.LineAColor.GetA()>theApp.AppSettings.DiskColor.FillAColor.GetA())
		{
			LineClrA =  theApp.AppSettings.DiskColor.LineAColor;
		}
		else
		{
			LineClrA =  theApp.AppSettings.DiskColor.FillAColor;

		}
		if(theApp.AppSettings.DiskColor.LineBColor.GetA()>theApp.AppSettings.DiskColor.FillBColor.GetA())
		{
			LineClrB = theApp.AppSettings.DiskColor.LineBColor;
		}
		else
		{
			LineClrB = theApp.AppSettings.DiskColor.FillBColor;
		}
		
	}
	else if(Type == PM_ETHERNET)
	{

		if(theApp.AppSettings.NetworkColor.LineAColor.GetA()>theApp.AppSettings.NetworkColor.FillAColor.GetA())
		{
			LineClrA = theApp.AppSettings.NetworkColor.LineAColor;
		}
		else
		{
			LineClrA = theApp.AppSettings.NetworkColor.FillAColor;
		}
		if(theApp.AppSettings.NetworkColor.LineBColor.GetA()>theApp.AppSettings.NetworkColor.FillBColor.GetA())
		{
			LineClrB = theApp.AppSettings.NetworkColor.LineBColor;
		}
		else
		{
			LineClrB = theApp.AppSettings.NetworkColor.FillBColor;
		}

	}
}
