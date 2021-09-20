#pragma once
#include "normalheaderctrl.h"



// CMyNormalList

class CMyNormalList : public CListCtrl
{
	DECLARE_DYNAMIC(CMyNormalList)

public:
	CMyNormalList();
	virtual ~CMyNormalList();

protected:
	DECLARE_MESSAGE_MAP()
public:
	CNormalHeaderCtrl mHdCtrl;
protected:
	virtual void PreSubclassWindow();
};


