#pragma once


// CInfoBox

struct INFOTEXT
{
	CString StrTitle;
	CString StrInfo;
	CRect rc;
	BYTE Type; //是否带图例  0 不带 1实线 2虚线
};

class CInfoBox : public CStatic
{
	DECLARE_DYNAMIC(CInfoBox)

public:
	CInfoBox();
	virtual ~CInfoBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	INFOTEXT Info[7];
	BYTE Type;
	Gdiplus::Color  LineClrA;
	Gdiplus::Color  LineClrB;
	void SetColor(void);
};


