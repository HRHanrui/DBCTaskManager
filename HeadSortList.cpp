// HeadSortList.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "HeadSortList.h"



//**************************************




int CALLBACK Sort_HeadSortList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)  
{

	CHeadSortList *pList =(CHeadSortList*)lParamSort;

	int iCol =pList->NewSortCol;
	int result = 0;     //返回值   

	
 
	if(pList==NULL)return result;
	//	int Data1,Data2;

	CString Str1,Str2;

	Str1= pList->GetItemText((int)lParam1,iCol);
	Str2= pList->GetItemText((int)lParam2,iCol);

	result =lstrcmp(Str1,Str2);

 
	if(!pList->IsSortUp)
	{
		result=-result;
	}
	

	return result;


}




//*********************************************









// CHeadSortList

IMPLEMENT_DYNAMIC(CHeadSortList, CListCtrl)

CHeadSortList::CHeadSortList()
: IsSortUp(TRUE)
, CurrentSortCol(9999)
, NewSortCol(0)
, CompareFunc(NULL)
{

}

CHeadSortList::~CHeadSortList()
{
}


BEGIN_MESSAGE_MAP(CHeadSortList, CListCtrl)
	ON_NOTIFY(HDN_ITEMCLICKA, 0, &CHeadSortList::OnHdnItemclick)
	ON_NOTIFY(HDN_ITEMCLICKW, 0, &CHeadSortList::OnHdnItemclick)
END_MESSAGE_MAP()



// CHeadSortList message handlers



void CHeadSortList::SortListItems(int iCol)
{
	int nCount = GetItemCount();

	 

	for(int i= 0;i<nCount;i++)
	{
		SetItemData(i,(DWORD_PTR)i);  //列表项相关项设为项号
	}

	if(CurrentSortCol==iCol)
	{
		IsSortUp = (!IsSortUp);
	}
	else
	{
		CurrentSortCol = iCol;
		IsSortUp = TRUE;
	}



	NewSortCol = iCol;
	SortItems(Sort_HeadSortList, (DWORD_PTR)this);
}

void CHeadSortList::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	SortListItems(phdr->iItem);
	*pResult = 0;
}
