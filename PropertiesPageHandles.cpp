// PropertiesPageHandles.cpp : implementation file
//

#include "stdafx.h"
#include "DBCTaskman.h"
#include "PropertiesPageHandles.h"




//***********************************************************************************************************************************


#define SystemHandleInformation  16

typedef struct _SYSTEM_HANDLE
{
    ULONG ProcessId;
    BYTE ObjectTypeNumber;
    BYTE Flags;
    USHORT Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG HandleCount; /* Or NumberOfHandles if you prefer. */
    SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;


 typedef struct _OBJECT_TYPE_INFORMATION
{
    UNICODE_STRING TypeName;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG TotalPagedPoolUsage;
    ULONG TotalNonPagedPoolUsage;
    ULONG TotalNamePoolUsage;
    ULONG TotalHandleTableUsage;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
    ULONG HighWaterPagedPoolUsage;
    ULONG HighWaterNonPagedPoolUsage;
    ULONG HighWaterNamePoolUsage;
    ULONG HighWaterHandleTableUsage;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG ValidAccessMask;
    BOOLEAN SecurityRequired;
    BOOLEAN MaintainHandleCount;
    ULONG PoolType;
    ULONG DefaultPagedPoolCharge;
    ULONG DefaultNonPagedPoolCharge;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;



//***********************************************************************************************************************************



 
int CALLBACK Sort_HandlesList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)  
{

	CPropertiesPageHandles *pWnd =(CPropertiesPageHandles*)lParamSort;

	int iCol =pWnd->mHandlesList.NewSortCol;
	int result = 0;     //·µ»ØÖµ   

	 
	if(pWnd==NULL)return result;
	//	int Data1,Data2;

	CString Str1,Str2;

	Str1= pWnd->mHandlesList.GetItemText((int)lParam1,iCol);
	Str2= pWnd->mHandlesList.GetItemText((int)lParam2,iCol);

	result =lstrcmp(Str1,Str2);
 
	if(!pWnd->mHandlesList.IsSortUp)
	{
		result=-result;
	}
	

	return result;


}



//************************************************************************************



// CPropertiesPageHandles dialog

IMPLEMENT_DYNAMIC(CPropertiesPageHandles, CPropertyPage)

CPropertiesPageHandles::CPropertiesPageHandles()
	: CPropertyPage(CPropertiesPageHandles::IDD)
	, PID(0)
{

}

CPropertiesPageHandles::~CPropertiesPageHandles()
{
}

void CPropertiesPageHandles::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_HANDLES, mHandlesList);
}


BEGIN_MESSAGE_MAP(CPropertiesPageHandles, CPropertyPage)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPropertiesPageHandles message handlers

BOOL CPropertiesPageHandles::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
 
 

	mHandlesList.SetExtendedStyle( mHandlesList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER   ); //mModulesList

	CString StrLoad,StrColumn;
	StrLoad.LoadString(STR_COLUMN_PROP_HANDLES);
	int ColumnWidth[2] = {150,330};
	for(int i=0 ; i<2; i++)
	{
		AfxExtractSubString(StrColumn,StrLoad,i,L';');
		mHandlesList.InsertColumn(i,StrColumn,0,ColumnWidth[i]);
	}

	mHandlesList.CompareFunc = Sort_HandlesList;
 
	ListHandles();

	mHandlesList.SortListItems(0);

	SetTimer(0,1000,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertiesPageHandles::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	//--------------------------
	CRect rc;
	this ->GetClientRect(rc);
	rc.DeflateRect(9,9);
	rc.top+=3;
	mHandlesList.MoveWindow(rc);

}

int CPropertiesPageHandles::ListHandles(void)
{
	
	PSYSTEM_HANDLE_INFORMATION  pHandleInfo=NULL;
	DWORD bytesreturned;

	pHandleInfo =  (PSYSTEM_HANDLE_INFORMATION) malloc(sizeof(SYSTEM_HANDLE_INFORMATION));


	
	MyNtQuerySystemInformation =(API_NtQuerySystemInformation) GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQuerySystemInformation");	
	NTSTATUS Ret = MyNtQuerySystemInformation(SystemHandleInformation,pHandleInfo,  sizeof(SYSTEM_HANDLE_INFORMATION),&bytesreturned);
	
	if ( !NT_SUCCESS(Ret) )
	{		
		if(Ret == STATUS_INFO_LENGTH_MISMATCH  ) 
		{
			free(pHandleInfo);pHandleInfo=NULL;
			pHandleInfo =  (PSYSTEM_HANDLE_INFORMATION) malloc(bytesreturned );
			Ret=MyNtQuerySystemInformation(SystemHandleInformation,pHandleInfo, bytesreturned,&bytesreturned);
			 
			if ( NT_SUCCESS(Ret) )
			{
				HANDLE hProcess =  OpenProcess(  PROCESS_QUERY_INFORMATION |PROCESS_DUP_HANDLE  , FALSE, PID);

		
				for(ULONG i =0;i<pHandleInfo->HandleCount;i++)
				{
					if(pHandleInfo->Handles[i].ProcessId == PID)
					{
						if((pHandleInfo->Handles[i].GrantedAccess == 0x0012019f)|| (pHandleInfo->Handles[i].GrantedAccess == 0x001a019f)|| (pHandleInfo->Handles[i].GrantedAccess == 0x00120189)|| (pHandleInfo->Handles[i].GrantedAccess == 0x00100000))
						{
							continue;
						}
						
												
						HANDLE MyHandle;
						DuplicateHandle(hProcess,(HANDLE)pHandleInfo->Handles[i].Handle, GetCurrentProcess(),&MyHandle, PROCESS_QUERY_INFORMATION, FALSE, 0);

						API_NtQueryObject NtQueryObject = (API_NtQueryObject) GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryObject");

						DWORD BuffLength;
						CString StrName;
						CString StrType;
						PVOID NameInfo;		
						PVOID TypeInfo;
						NameInfo = malloc(1024);
						TypeInfo = malloc(1024);


						//-------------------------

						
						if( !NT_SUCCESS(NtQueryObject(MyHandle,ObjectTypeInformation ,TypeInfo,1024,&BuffLength))  )
						{
							
							free(TypeInfo);
							TypeInfo = malloc(BuffLength);
							if( NT_SUCCESS(NtQueryObject(MyHandle,ObjectTypeInformation ,TypeInfo,BuffLength,NULL))  )
							{								
								StrType = ((OBJECT_TYPE_INFORMATION *)TypeInfo)->TypeName.Buffer ; 
							}
						}
						else
						{							
							StrType = ((OBJECT_TYPE_INFORMATION *)TypeInfo)->TypeName.Buffer ; 
						}
 	  
						//--------------------------


						

						if( !NT_SUCCESS(NtQueryObject(MyHandle,ObjectNameInformation ,NameInfo,1024,&BuffLength))  )
						{
							
							free(NameInfo);
							NameInfo = malloc(BuffLength);
							if( NT_SUCCESS(NtQueryObject(MyHandle,ObjectNameInformation ,NameInfo,BuffLength,NULL))  )
							{
								StrName = ((UNICODE_STRING *)NameInfo)->Buffer ; 
								//StrName = ((OBJECT_TYPE_INFORMATION *)NameInfo)->TypeName.Buffer ; 
							}
						}
						else
						{
							StrName = ((UNICODE_STRING *)NameInfo)->Buffer ; 
							//StrName = ((OBJECT_TYPE_INFORMATION *)NameInfo)->TypeName.Buffer ; 
						}

						
 	  
						 

						if(StrName.GetAt(0) != 0)
						{
							mHandlesList.InsertItem(0,StrType);
							mHandlesList.SetItemText(0,1,StrName);
							
						}


						free(NameInfo);
						free(TypeInfo);
						CloseHandle(MyHandle);

					}

				}//for

				CloseHandle(hProcess);
			
				free(pHandleInfo);

			}
		}
		
	}


	return 0;
}

void CPropertiesPageHandles::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default


	//if(this->IsWindowVisible())
	//{
	//	mHandlesList.SetRedraw(0);
	//	mHandlesList.DeleteAllItems();
	//	ListHandles();
	//	mHandlesList.SetRedraw(1);
	//}


	CPropertyPage::OnTimer(nIDEvent);
}
