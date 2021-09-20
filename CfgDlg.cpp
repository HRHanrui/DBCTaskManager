// CfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "CfgDlg.h"


// CCfgDlg dialog

IMPLEMENT_DYNAMIC(CCfgDlg, CDialog)

CCfgDlg::CCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCfgDlg::IDD, pParent)
{

}

CCfgDlg::~CCfgDlg()
{
}

void CCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COLORLSIT, ColorList);
}


BEGIN_MESSAGE_MAP(CCfgDlg, CDialog)
END_MESSAGE_MAP()


// CCfgDlg message handlers

BOOL CCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	ColorList.InsertColumn(0,L" ",0,90);
	ColorList.InsertColumn(1,L"CPU",0,100);
	ColorList.InsertColumn(2,L"Memory",0,100);
	ColorList.InsertColumn(3,L"Disk",0,100);
	ColorList.InsertColumn(4,L"Network",0,100);

	int i=0;
	ColorList.InsertItem(i++,L"Background");
	ColorList.InsertItem(i++,L"Border");
	ColorList.InsertItem(i++,L"Grid");
	ColorList.InsertItem(i++,L"LineA");
	ColorList.InsertItem(i++,L"FillA");
	ColorList.InsertItem(i++,L"LineB");
	ColorList.InsertItem(i++,L"FillB");

	
	

	

	




	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
