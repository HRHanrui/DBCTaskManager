// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "AboutDlg.h"
#include "VersionNo.h"
#include <commoncontrols.h>

// CAboutDlg dialog

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{

}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	
END_MESSAGE_MAP()


// CAboutDlg message handlers

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return NULL;
}



BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

		
	 


	
	SHFILEINFOW sfi = { 0 };
	SHGetFileInfo(theApp.StrAppFullPath, -1, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);
	// Retrieve the system image list.
	// To get the 48x48 icons, use SHIL_EXTRALARGE	  To get the 256x256 icons (Vista only), use SHIL_JUMBO

	int iImageList = SHIL_JUMBO;//(OSVersion<6.0)?SHIL_EXTRALARGE:SHIL_JUMBO;


	HIMAGELIST* ImageList;
	HRESULT hResult = SHGetImageList(iImageList, IID_IImageList, (void**)&ImageList);
	if (hResult == S_OK) 
	{			 
		hResult = ((IImageList*)ImageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &LogoIcon);
	}

		
	





	




	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	//DefWindowProc(WM_PAINT,(WPARAM)dc.m_hDC,(LPARAM)0);


	
	CRect Rc;
	
	 
	CDC MemDC;//内存ID表  
	CBitmap MemMap;
	GetClientRect(Rc);   
	Rc.bottom-=60;
	MemDC.CreateCompatibleDC(&dc);  
	MemMap.CreateCompatibleBitmap(&dc,Rc.Width(),Rc.Height());
	MemDC.SelectObject(&MemMap);

	//MemDC.FillSolidRect(0,0,Rc.Width(),Rc.Height()-60, RGB(255,255,255));  

	MemDC.FillSolidRect(Rc, RGB(255,255,255));  

    RectF rectImg(0,0,415,81);

	Image *BarLogoImg = LoadPNG(MAKEINTRESOURCE(PNG_ABOUT) ,theApp.m_hInstance);
	Graphics graphics(MemDC.m_hDC );
	



	CString StrVerInfo;

	WCHAR compile_date[] =  _T(__DATE__);
    WCHAR compile_time[] = _T( __TIME__);
 
	CString StrPDVer = TEXT(STRPRODUCTVER);

	StrVerInfo.Format(L"Version %s   ( %s %s )",StrPDVer,compile_date,compile_time);


	MemDC.SelectObject(this->GetFont());
	MemDC.SetTextColor(RGB(99,99,99));

	CRect RcText;
	RcText.CopyRect(Rc);
	RcText.top = 170;
	RcText.bottom = 190;
	RcText.DeflateRect(35,0);

	MemDC.DrawText(StrVerInfo,RcText,DT_LEFT|DT_TOP|DT_NOCLIP);
	
	graphics.DrawImage(BarLogoImg,148,50); 


	CString Str64Or32  =  theApp.FlagIsX64 ? L"64":L"32";
	MemDC.DrawText(Str64Or32+L"-bit Edition",CRect(150,95,100,25),DT_LEFT|DT_TOP|DT_NOCLIP);

	
 
	 


	ICONINFO IconInfo;
	int IconSize = 256;//(OSVersion<6.0)?48:
	GetIconInfo(LogoIcon, &IconInfo); 
	Rect RcSrcIcon(0, 0, IconSize, IconSize ); 
	Bitmap BmpIcon(IconInfo.hbmColor, NULL);  
	
 	Gdiplus::BitmapData bmData; 
	BmpIcon.LockBits(&RcSrcIcon, ImageLockModeRead,BmpIcon.GetPixelFormat(), &bmData); 
	Gdiplus::Bitmap BmpAlpha(bmData.Width, bmData.Height, bmData.Stride, PixelFormat32bppARGB, (BYTE*)bmData.Scan0); 
	BmpIcon.UnlockBits(&bmData); 	
 
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality); 
	graphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality); 
	graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver); 

	graphics.DrawImage(&BmpAlpha, 25,25,100,100);

	


	MemDC.FillSolidRect(35,150,Rc.Width()-70,1,RGB(200,200,200));
	MemDC.FillSolidRect(0,Rc.bottom-1,Rc.Width(),1,RGB(200,200,200));

   

	//这一句是调用默认的OnPaint(),把图形画在内存DC表上  
	DefWindowProc(WM_PAINT,(WPARAM)MemDC.m_hDC,(LPARAM)0);     
	//输出  

	 
	dc.BitBlt(0,0,Rc.Width(),  Rc.Height(),&MemDC,0, 0,SRCCOPY);  
 	
	
	delete BarLogoImg;
	MemDC.DeleteDC();
	MemMap.DeleteObject();

}




BOOL CAboutDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	
	LPNMHDR pNMHDR = (LPNMHDR)lParam;
	if(pNMHDR->code == NM_CLICK)
	{
		switch(pNMHDR->idFrom)
		{
		case IDC_HOME:
			::ShellExecute(NULL,_T("open"),_T("http://dbcstudio.net/software/taskman/main.html"), NULL,_T(""),SW_SHOWNA);	
			break;
		case IDC_BLOG:
			::ShellExecute(NULL,_T("open"),_T("http://dbc-studio.blog.163.com/blog/static/757510502013412101333965/"), NULL,_T(""),SW_SHOWNA);
			break;
		case IDC_DONATE:
			::ShellExecute(NULL,_T("open"),_T("http://dbcstudio.net/donate.html"), NULL,_T(""),SW_SHOWNA);
			break;
		}
		
	}

	//---------------
	return CDialog::OnNotify(wParam, lParam, pResult);
}
