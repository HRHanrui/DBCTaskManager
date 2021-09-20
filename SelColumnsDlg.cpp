// SelColumnsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "SelColumnsDlg.h"


// CSelColumnsDlg dialog

IMPLEMENT_DYNAMIC(CSelColumnsDlg, CDialog)

CSelColumnsDlg::CSelColumnsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelColumnsDlg::IDD, pParent)
{

}

CSelColumnsDlg::~CSelColumnsDlg()
{
}

void CSelColumnsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SELCOLUMN_LIST, mList);
}


BEGIN_MESSAGE_MAP(CSelColumnsDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSelColumnsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelColumnsDlg message handlers

BOOL CSelColumnsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	 SetWindowTheme(mList.GetSafeHwnd(),L"explorer", NULL);

	mList.SetExtendedStyle(mList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES| LVS_EX_DOUBLEBUFFER|LVS_EX_CHECKBOXES   );  // 	//加 LVS_EX_TRANSPARENTSHADOWTEXT 没有虚线框 | LVS_EX_GRIDLINES |LVS_EX_CHECKBOXES  

	CRect rc;
	mList.GetClientRect(rc);
	mList.InsertColumn(0,L"",0,rc.Width());

	CString StrItem,StrLoad;	 
	int i = 0;
	StrLoad.LoadString(STR_COLUMN_DETAILS);	

	while(1)
	{
		AfxExtractSubString(StrItem,StrLoad,i,L';');	
		if(StrItem.IsEmpty()) { break;}

		mList.InsertItem(i,StrItem);
		if(theApp.AppSettings.ColIDS_DetailList[i].ColWidth != 0)
		{
			//mList.SetItemState (i,0x2000, LVIS_STATEIMAGEMASK);
			//判断是否选中:m_yktlist.GetItemState(行数,LVIS_STATEIMAGEMASK)==0x2000
			mList.SetCheck(i);
		}
		i++;		

	}





	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelColumnsDlg::OnBnClickedOk()
{
	int nCount = mList.GetItemCount();
	LVCOLUMNW  Col;
	Col.mask = LVCF_ORDER |LVCF_SUBITEM ;

	 

	pDetailList->SetRedraw(0);

	for(int iCol = 0;iCol<nCount;iCol++)
	{

	
		if(mList.GetCheck(iCol))
		{
			
			//暂时定位方法！！！！
			pDetailList->GetColumn(0,&Col);//先获取name列的iOrder 然后添加在他后面 因为前面隐藏了几个所以不能从零算起！

			theApp.AppSettings.ColIDS_DetailList[iCol].ColWidth = 123;
			theApp.AppSettings.ColIDS_DetailList[iCol].Redraw =1;

			Col.iOrder=Col.iOrder+1;
			pDetailList->SetColumn(iCol,&Col);
			pDetailList->SetColumnWidth(iCol,theApp.AppSettings.ColIDS_DetailList[iCol].ColWidth );


		}
		else
		{
			theApp.AppSettings.ColIDS_DetailList[iCol].ColWidth = 0 ; 
			theApp.AppSettings.ColIDS_DetailList[iCol].Redraw = 0;
			Col.iOrder=0;
			pDetailList->SetColumn(iCol,&Col);
			pDetailList->SetColumnWidth(iCol,0 );
		}
	}
	pDetailList->SetRedraw(1);
 
	OnOK();
}
