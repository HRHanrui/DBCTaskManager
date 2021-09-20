// WaveBox.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "WaveBox.h"


// CWaveBox

IMPLEMENT_DYNAMIC(CWaveBox, CStatic)

CWaveBox::CWaveBox()
:nCol(1)
,nRow(1)
, DrawSecondWave(FALSE)
, dX(0)
, LineVar(0)

{
	 
	BkgColor =RGB(255,255,255);
	nCol=nRow =1;

}

CWaveBox::~CWaveBox()
{
}


BEGIN_MESSAGE_MAP(CWaveBox, CStatic)
	ON_WM_PAINT()
//	ON_WM_SIZE()
//ON_WM_SIZING()
ON_WM_SIZE()
END_MESSAGE_MAP()



// CWaveBox message handlers



void CWaveBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here

	


	CRect rc;

	this->GetClientRect(rc);

	CRect rcNode;

	int NodeW,NodeH;
	//NodeW=static_cast<int>((float)rc.Width()/nCol + 0.5f);
//	NodeH=static_cast<int>((float)rc.Height()/nLine + 0.5f);


	NodeW=rc.Width()/nCol;
	NodeH=rc.Height()/nRow;


	rcNode.SetRect(0,0,NodeW, NodeH  );


	
	CDC MemDC;//内存ID表  
	CBitmap MemBitmap;
	CBitmap *pOldBm;

	MemDC.CreateCompatibleDC(&dc);  
	MemBitmap.CreateCompatibleBitmap(&dc,rc.Width(),rc.Height());
	pOldBm=MemDC.SelectObject(&MemBitmap);

	MemDC.SelectObject(this->GetFont());
 
  	MemDC.FillSolidRect(0,0,rc.Width(),rc.Height(),BkgColor);  

//MemDC.FillSolidRect  (0,0,rcItem.Width(),rcItem.Height(),RGB(rand()%255,rand()%255,rand()%255));

  //	DefWindowProc(WM_PAINT,(WPARAM)MemDC.m_hDC,(LPARAM)0); 
 


	//COLORREF SelbkColor = ::GetSysColor(COLOR_HIGHLIGHT);
	//COLORREF SeltextColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);




 
	
	Graphics graphics(MemDC.m_hDC );
 
	//graphics.SetSmoothingMode(SmoothingModeHighQuality );
	graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
	PointF  PArray[60+1+2];
	PointF  PArray2[60+1+2];




	



		//画横线

	    int BroderSize=2;

		CPen Pen(0,1,GridColor);

		MemDC.SelectObject(&Pen);

		int nStep_X = 20 ;

		if(rcNode.Width()<BOX_W_MIN-40)
		{
			nStep_X =6 ;
		}
		else if(rcNode.Width()<600)
		{
			nStep_X =10;
		}
		else
		{
			nStep_X =20 ;
		}



		float StepX=((float)rcNode.Width())/nStep_X;
		float StepY=((float)(rcNode.Height()-BroderSize*2))/10;


		//PenColor.SetFromCOLORREF(RGB(217,234,244));
       

		float  PosX= StepX;
		float  PosY;


		
		
		
			
		for(int  n=0;n<nRow;n++)
		{
			PosY=rcNode.Height()*n+BroderSize+StepY;
			for(int  i=0;i<9;i++)
			{
				MemDC.MoveTo(rcNode.left+3,(int)PosY);
				MemDC.LineTo(rcNode.Width()*nCol,(int)PosY);
				//graphics.DrawLine(&Gdiplus::Pen(PenColor,0.5), (int)(rcNode.left+4),(int)PosY,(int)rcNode.Width()*nCol,(int)PosY);


				PosY+=StepY;

			}
			
			

		}

		
		float  nXsegments =(float)rcNode.Width()/60;
		dX = dX+nXsegments;

		 

		if(dX>StepX)
		{
			dX = dX-StepX;

		}

		float  NodeRight;

		//画竖线
		for(int n=0;n<nCol;n++)
		{


			PosX=rcNode.Width()*n+BroderSize+StepX-dX;
			NodeRight = PosX+rcNode.Width();
			//for(int i= 0;i<nStep_X-1;i++)
		 
			while(1)
			{
				MemDC.MoveTo((int)PosX,rcNode.top+3);
				MemDC.LineTo((int)PosX,rcNode.Height()*nCol);
				PosX+=StepX;
				if(PosX>=NodeRight) break;

			}

		}

		int nBox = 0;

		for(int i=0;i<nRow;i++)
		{
			for(int j=0;j<nCol;j++)
		{


			//rcSubItem.top = rcSubItem.Height()*lpDrawItemStruct->itemID;
			

			rcNode.DeflateRect( 2, 2);



			_DrawWave(&graphics,PArray,PArray2,rcNode,nBox++);


			//MemDC.FillSolidRect(rcSubItem,RGB(rand()%255,rand()%255,rand()%255));
			MemDC.Draw3dRect(rcNode,BorderColor,BorderColor);
			rcNode.InflateRect( 1, 1);
			MemDC.Draw3dRect(rcNode,theApp.WndBkgColor,theApp.WndBkgColor);
			rcNode.InflateRect( 1, 1);
			MemDC.Draw3dRect(rcNode,theApp.WndBkgColor,theApp.WndBkgColor);
		
			rcNode.OffsetRect(rcNode.Width(),0);

			}
			rcNode.OffsetRect(-rcNode.Width()*nCol,rcNode.Height());


		}

		 


	if(nCol)
	{
		MemDC.Draw3dRect(rc,theApp.WndBkgColor,theApp.WndBkgColor);
	}


	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &MemDC, 0, 0, SRCCOPY);

	

	Pen.DeleteObject();

	graphics.ReleaseHDC(MemDC.m_hDC);


	MemDC.SelectObject(pOldBm);
	MemBitmap.DeleteObject();
	ReleaseDC(&MemDC);
	MemDC.DeleteDC();




	// Do not call CStatic::OnPaint() for painting messages
}

void CWaveBox::_DrawWave(Gdiplus::Graphics  * pGraph,  Gdiplus::PointF * pArray,Gdiplus::PointF * pArray2,CRect rc,int nBox)
{

 
	pGraph->SetSmoothingMode(SmoothingModeHighQuality );
	
	int  Seg=60; //标准速度 60  高 120  低 30  

	for(int i=0;i<=Seg;i++)
	{
		//Num[i] =rc.Height()/100* (::GetTickCount()%100);
		pArray[i].X=rc.left+(float)rc.Width()/Seg*i;
		pArray[i].Y=rc.bottom-(float)rc.Height()* Num[nBox][i] ;
		if( Num[nBox][i]>=1){ pArray[i].Y = (float)rc.top+1; }

		if(DrawSecondWave)
		{
			pArray2[i].X=rc.left+(float)rc.Width()/Seg*i;
			pArray2[i].Y=rc.bottom-(float)rc.Height()* Num2[nBox][i] ;
			if( Num2[nBox][i]>=1){ pArray2[i].Y = (float)rc.top+1; }
		}

	}



	pArray[0].X=(float)rc.left;
	pArray[Seg].X=(float)rc.right;

	pArray[Seg+1].X=(float)rc.right;
	pArray[Seg+1].Y=(float)rc.bottom;

	pArray[Seg+2].X=(float)rc.left;
	pArray[Seg+2].Y=(float)rc.bottom;


	if(DrawSecondWave)
	{
		pArray2[0].X=(float)rc.left;
		pArray2[Seg].X=(float)rc.right;

		pArray2[Seg+1].X=(float)rc.right;
		pArray2[Seg+1].Y=(float)rc.bottom;

		pArray2[Seg+2].X=(float)rc.left;
		pArray2[Seg+2].Y=(float)rc.bottom;
	}




	

	
	 

	GraphicsPath Path;
	Path.AddLines(pArray,Seg+3);		
	 
	pGraph->FillPath(&SolidBrush( FillAColor),&Path);  //Color(100,0,222,0) a=20
	pGraph->DrawLines(&Gdiplus::Pen(LineAColor ,1),pArray,Seg+1);  //Color(255,0,180,0)
 


	if(DrawSecondWave)
	{
		
		Gdiplus::Pen  MyDashPen(LineBColor,1);
		MyDashPen.SetDashStyle(DashStyleDash);
		
		GraphicsPath Path2;
		Path2.AddLines(pArray2,Seg+3);


		pGraph->FillPath(&SolidBrush( FillBColor) ,&Path2);  //Color(100,0,222,0) a=16
	
		pGraph->DrawLines(&MyDashPen,pArray2,Seg+1);  //Color(255,0,180,0)

	

	}


	if(LineVar !=0)
	{
	pGraph->SetSmoothingMode(SmoothingModeDefault  );
	float MaxLineY = (float)(rc.top+rc.Height()*(1-LineVar));
	pGraph->DrawLine(&Gdiplus::Pen(LineAColor,1),(float)rc.left,MaxLineY,(float)rc.right,MaxLineY);  //Color(255,0,180,0)
    
	}
 



	 
	
	//pGraph->DrawLine(&Gdiplus::Pen(Color(20,0,255,0),15),rc.left,MaxLineY,rc.right,MaxLineY);  //Color(255,0,180,0)
    

    //pGraph->DrawCurve(&Gdiplus::Pen(Color(255,GetRValue(LineColor),GetGValue(LineColor),GetBValue(LineColor)),1),pArray,Seg+1);
}

void CWaveBox::SetColor(GRAPHCOLOR GrphColor )
{
	BkgColor = GrphColor.BackgroundColor;
	BorderColor = GrphColor.BorderColor;

	

	LineAColor = GrphColor.LineAColor ;
	LineBColor=  GrphColor.LineBColor; 
	FillAColor= GrphColor.FillAColor;
	FillBColor=  GrphColor.FillBColor; 
 

	float A= (float)GrphColor.GridColorAlpha/255;
 


	GridColor = AlphaBlendColor(BkgColor,GrphColor.GridColorRGB,A);
}

void CWaveBox::SetLineColumn(int Row, int Column)
{
	nRow = Row;
	nCol = Column;
	int n=nRow*nCol;
	Num= new float[n][61];
	Num2= new float[n][61];
	
	/*for(int i=0;i<n;i++)
	{
		for(int j=0;j<61;j++)
		{
			Num[i][j] = 0;
			Num2[i][j] = 0;
		}
	}*/


	memset(Num,0,sizeof(float)* n*61);
	memset(Num2,0,sizeof(float)* n*61);



}



void CWaveBox::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	dX = 0;

}
