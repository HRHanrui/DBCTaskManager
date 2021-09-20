#pragma once
#include "gdiplusbrush.h"


// CWaveBox

class CWaveBox : public CStatic
{
	DECLARE_DYNAMIC(CWaveBox)

public:
	CWaveBox();
	virtual ~CWaveBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

	int nCol;
	int nRow;
	void _DrawWave(Gdiplus::Graphics  * pGraph,  Gdiplus::PointF * pArray,Gdiplus::PointF * pArray2,CRect rc,int nBox = 0);


	float  LineVar;

 	float (*Num)[61] ;
	float (*Num2)[61] ;
	 
	Gdiplus::Color  LineAColor;
	Gdiplus::Color  LineBColor; 
	Gdiplus::Color  FillAColor;
	Gdiplus::Color FillBColor; 
	COLORREF  BkgColor;
	COLORREF  GridColor;
	COLORREF  BorderColor;

	void SetColor(GRAPHCOLOR GrphColor );
	void SetLineColumn(int Row, int Column);
	BOOL DrawSecondWave;
	float dX;

//	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	 
};


