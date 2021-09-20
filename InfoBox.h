#pragma once


// CInfoBox

struct INFOTEXT
{
	CString StrTitle;
	CString StrInfo;
	CRect rc;
	BYTE Type; //�Ƿ��ͼ��  0 ���� 1ʵ�� 2����
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


