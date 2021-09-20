// MemCompBox.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "MemCompBox.h"


// CMemCompBox

IMPLEMENT_DYNAMIC(CMemCompBox, CStatic)

CMemCompBox::CMemCompBox()
: XPosReserve(0)
, XPosInUse(0)
, XPosMod(0)
, XPosStandby(0)
{
	LineColor =RGB(0,0,0);
	BkgColor =theApp.AppSettings.MemoryColor.BackgroundColor;	
	ReserveColor=RGB(95,95,95);
	InUseColor=RGB(0,0,0);
	ModifiedColor=RGB(0,0,0);
	StandbyColor=RGB(0,0,0);

	




}

CMemCompBox::~CMemCompBox()
{
}


BEGIN_MESSAGE_MAP(CMemCompBox, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CMemCompBox message handlers



void CMemCompBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc,rcItem;

	this->GetClientRect(rc);
	rcItem.CopyRect(rc);
	rcItem.right=rcItem.left;

	dc.FillSolidRect(rc,BkgColor);

	int BoxWidth = rc.Width();
	int ClipWidth;

	 


	double InstallMem = (double) (theApp.PerformanceInfo.InstalledMemKB*1024);

	if(InstallMem ==0) return;

	//MSGDB(InstallMem)

	//CBrush Br(3,LineColor);

	//Reserve

	ClipWidth = (int)(BoxWidth*((InstallMem-theApp.PerformanceInfo.TotalPhysMem)/InstallMem));
	rcItem.right=rcItem.left+ClipWidth;
	 dc.FillSolidRect(rcItem,ReserveColor);
	 XPosReserve = rcItem.right;


	// dc.FillRect(rcItem,&Br);
 

	//In use
	ClipWidth = (int)(BoxWidth*( theApp.PerformanceInfo.InUsePhysMem/InstallMem));
	rcItem.left=rcItem.right;
	rcItem.right=rcItem.left+ClipWidth;
	dc.FillSolidRect(rcItem,InUseColor);
	XPosInUse = rcItem.right;
 

	//Modified
	ClipWidth = (int)(BoxWidth*( theApp.PerformanceInfo.Mem_Modified/InstallMem));
	rcItem.left=rcItem.right;
	rcItem.right=rcItem.left+ClipWidth;
	dc.FillSolidRect(rcItem,ModifiedColor);
	XPosMod = rcItem.right;

	//Standby
	ClipWidth = (int)(BoxWidth*( theApp.PerformanceInfo.Mem_Standby/InstallMem));
	rcItem.left=rcItem.right;
	rcItem.right=rcItem.left+ClipWidth;
	if(rcItem.right>rc.right)rcItem.right = rc.right;
	dc.FillSolidRect(rcItem,StandbyColor);

	XPosStandby =	rcItem.right;



//	dc.FillSolidRect(rcItem,FillDarkColor);
	

	dc.Draw3dRect(rc,LineColor,LineColor);
	
}

void CMemCompBox::SetColor()
{
 
	COLORREF BaseColor = theApp.AppSettings.MemoryColor.FillAColor.ToCOLORREF();

 
	//float Alpha = ((float)theApp.AppSettings.MemoryColor.FillAColor.GetA()/255.0);



	LineColor =theApp.AppSettings.MemoryColor.BorderColor;
	BkgColor =theApp.AppSettings.MemoryColor.BackgroundColor;
	ReserveColor = AlphaBlendColor(theApp.AppSettings.MemoryColor.BackgroundColor,BaseColor,(float)0.7) ;
	InUseColor= AlphaBlendColor(theApp.AppSettings.MemoryColor.BackgroundColor,BaseColor,(float)0.5) ;
	ModifiedColor= AlphaBlendColor(theApp.AppSettings.MemoryColor.BackgroundColor,BaseColor,(float)0.35) ;
	StandbyColor= AlphaBlendColor(theApp.AppSettings.MemoryColor.BackgroundColor,BaseColor,(float)0.2) ;

}


void CMemCompBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	theApp.m_pMainWnd->PostMessageW(UM_POPUP_MEMDETAILSDLG);

	CStatic::OnLButtonDblClk(nFlags, point);
}


CString CMemCompBox::GetMemCompType(void)
{
	CString SrtRet=L"";
	CPoint Pt;
	::GetCursorPos(&Pt);

	this->ScreenToClient(&Pt);


	double InstallMem =  (double) (theApp.PerformanceInfo.InstalledMemKB*1024);




	if(Pt.x<XPosReserve)
	{

		if(InstallMem>0)
		{
		SrtRet.Format(L"Reserve (%d MB)\nMemory that is reserved for use by the BIOS and some drivers for other peripherals",(int)((InstallMem-theApp.PerformanceInfo.TotalPhysMem)/1024/1024));
		}
		else
		{
			SrtRet = L"Reserve \nMemory that is reserved for use by\nthe BIOS and\nsome drivers for other peripherals";
		}
	}
	else if(Pt.x>XPosReserve && Pt.x<XPosInUse)
	{
		SrtRet.Format(L"In use (%d MB)\nMemory used by processes,drivers,\nor the operating system",(int)(theApp.PerformanceInfo.InUsePhysMem/1024/1024));
		
		//SrtRet = L"InUse\nMemory used by processes,drivers,\nor the operating system";
	}
	else if(Pt.x>XPosInUse && Pt.x<XPosMod)
	{
		SrtRet.Format(L"Modified (%d MB)\nMemory whose contents must be\nwritten to disk before it can be used\nfor another purpose",(int)(theApp.PerformanceInfo.Mem_Modified/1024/1024));
		
	}
	else if(Pt.x>XPosMod && Pt.x<XPosStandby)
	{
		SrtRet.Format(L"Standby (%d MB)\nMemory that contains cached data\nand code that is not actively in use",(int)(theApp.PerformanceInfo.Mem_Standby/1024/1024));
		
	}
	else  if(Pt.x>XPosStandby)
	{
		
		SrtRet.Format(L"Free (%d MB)\nMemory that is not currently in use,\nand that will be repurposed first when\nprocesses,drivers,or the operating\nsystem need more memory",(int)((theApp.PerformanceInfo.TotalPhysMem-theApp.PerformanceInfo.InUsePhysMem-theApp.PerformanceInfo.Mem_Modified-theApp.PerformanceInfo.Mem_Standby)/1024/1024));
		
	}





	return SrtRet;
}

