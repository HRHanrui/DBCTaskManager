#pragma once


// CNormalHeaderCtrl

class CNormalHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CNormalHeaderCtrl)

public:
	CNormalHeaderCtrl();
	virtual ~CNormalHeaderCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	BOOL *pFlagSortUp;
	int *pCurrentSortCol;
};


