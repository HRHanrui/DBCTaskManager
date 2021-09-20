#include "StdAfx.h"
#include "SMBIOS.h"

CSMBIOS::CSMBIOS(void)
{
}

CSMBIOS::~CSMBIOS(void)
{
}

BOOL CSMBIOS::InitRawSMBIOS_Data(void)
{

	memset(GlobalBuffer.smbios_table_data,0,MAX_DATA);

	BOOL ret = FALSE;
	HRESULT h_result;

    //不用Initialize COM  因为整体已经有 .

    // Obtain the initial locator to Windows Management
    // on a particular host computer.
    
	IWbemLocator* p_locator = 0;

    h_result = CoCreateInstance(CLSID_WbemLocator, 0,  CLSCTX_INPROC_SERVER,IID_IWbemLocator, (LPVOID *) &p_locator);
 
    if (h_result<0)  {  return FALSE;  }  // Program has failed.

    IWbemServices* p_service = 0;

    // Connect to the root\cimv2 namespace with the
    // current user and obtain pointer pSvc
    // to make IWbemServices calls.

    h_result = p_locator->ConnectServer(
        _T("ROOT\\WMI"),		 // WMI namespace
        NULL,                    // User name
        NULL,                    // User password
        0,                       // Locale
        NULL,                    // Security flags                 
        0,                       // Authority       
        0,                       // Context object
        &p_service                 // IWbemServices proxy
    );                              
    
    if (h_result<0)
    {
        p_locator->Release(); 
        return FALSE;                // Program has failed.
    }

    // Set the IWbemServices proxy so that impersonation
    // of the user (client) occurs.

    h_result = CoSetProxyBlanket(
       p_service,                      // the proxy to set
       RPC_C_AUTHN_WINNT,            // authentication service
       RPC_C_AUTHZ_NONE,             // authorization service
       NULL,                         // Server principal name
       RPC_C_AUTHN_LEVEL_CALL,       // authentication level
       RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
       NULL,                         // client identity 
       EOAC_NONE                     // proxy capabilities     
    );

    if (h_result<0)
    {
        p_service->Release();
        p_locator->Release();     
        return FALSE;               // Program has failed.
    }

    IEnumWbemClassObject* p_enumerator = NULL;
    h_result = p_service->CreateInstanceEnum( _T("MSSMBios_RawSMBiosTables"), 0, NULL, &p_enumerator);
    
    if (h_result<0)
    {
        p_service->Release();
        p_locator->Release();     
      
        return FALSE;               // Program has failed.
    }
    else
    { 
        do
        {
            IWbemClassObject* p_instance = NULL;
            ULONG dw_count = NULL;

            h_result = p_enumerator->Next(
                WBEM_INFINITE,
                1,
                &p_instance,
                &dw_count);      
			if(h_result>=0)
			{
				VARIANT variant_bios_data;
				VariantInit(&variant_bios_data);
				CIMTYPE type;

				h_result = p_instance->Get(_T("SmbiosMajorVersion"),0,&variant_bios_data,&type,NULL);
				if(h_result <0)
				{
					VariantClear(&variant_bios_data);
				}
				else
				{
					GlobalBuffer.smbios_major_version = (unsigned char)variant_bios_data.iVal;
					VariantInit(&variant_bios_data);
					h_result = p_instance->Get(_T("SmbiosMinorVersion"),0,&variant_bios_data,&type,NULL);
					if(h_result<0)
					{
						VariantClear(&variant_bios_data);
					}
					else
					{
						GlobalBuffer.smbios_minor_version = (unsigned char)variant_bios_data.iVal;
						VariantInit(&variant_bios_data);
						h_result = p_instance->Get(_T("SMBiosData"),0,&variant_bios_data,&type,NULL);
						if(h_result>=0)
						{
							if ( ( VT_UI1 | VT_ARRAY  ) != variant_bios_data.vt )
							{
							}
							else
							{
								SAFEARRAY* p_array = NULL;
								p_array = V_ARRAY(&variant_bios_data);
								unsigned char* p_data = (unsigned char *)p_array->pvData;

								GlobalBuffer.length = p_array->rgsabound[0].cElements;
								if (GlobalBuffer.length >= MAX_DATA) 
								{
									p_service->Release();
									p_locator->Release();   									
									return FALSE;
								}
								memcpy(GlobalBuffer.smbios_table_data,p_data,GlobalBuffer.length);
							}
						}
						VariantClear(&variant_bios_data);
					}
				}
				break;
			}
    
        } while (h_result == WBEM_S_NO_ERROR);
    }
	
    p_service->Release();
    p_locator->Release();     
   

	return true;
}

void CSMBIOS::GetStringResourceForID(int target_id, BYTE * in_buf, _TCHAR * out_string)
{
	/*char array_of_string_entries[MAX_STRING_TABLE][MAX_STRING];
	int index = (int)*(in_buf+1);
	int id=0;
	int still_more_data = *(in_buf+index) + *(in_buf+index+1);
	while (still_more_data)
	{
		int start_of_this_string = index;
		while (*(in_buf+index)!=0)
		{
			index++;
		}
		int len = index - start_of_this_string;
		index++;
		if (len > 0)
		{
			memcpy(array_of_string_entries[id],in_buf+start_of_this_string,len+1);
		}
		id++;
		still_more_data = *(in_buf+index) + *(in_buf+index+1);
	}
	target_id-=1;
	if (target_id > id)
	{
		*(out_string)=NULL;
	}
	else
	{  

		 size_t nConvert;
		mbstowcs_s(&nConvert,out_string,MAX_STRING*sizeof(_TCHAR),array_of_string_entries[target_id],MAX_STRING*sizeof(_TCHAR));	
	}*/
}

//--------------- 此函数 返回值为内存类型----------------
CString CSMBIOS::GetMemoryInfo(int * pUse,int *pSlot,UINT *pSpeed,WCHAR *FormFactor)
{
	InitRawSMBIOS_Data();

	CString StrMemType ;
	CString StrFormFactor;
	UINT Speed ;


	int nUse,nSlot;

	nUse=nSlot=0;
	BOOL  Result= TRUE;
	int len=MAX_DATA;
	unsigned char Buff[MAX_DATA];
	unsigned char* p_buf = Buff;
	memset(Buff,0,MAX_DATA);
	//there maybe multiple memory devices so...
	int DeviceCount=0;
	memory_device_type_17 t17;

	bool FoundDevice=false;

	do 
	{
		
		
		memset(&t17, 0, sizeof(memory_device_type_17));
		len=MAX_DATA;
		//build the table up
		FoundDevice=false;
		if (DeviceCount < 1)
		{
			if (ReturnNextTableOfType(mem_dev,&Buff,&len,true))
			{
				FoundDevice=true;
				DeviceCount++;
				 
			}
			
		}
		else
		{
			if (ReturnNextTableOfType(mem_dev,&Buff,&len,false))
			{
			
				DeviceCount++;
				FoundDevice=true;
			}
			
		}
		if (FoundDevice)
		{
			
			//load up this table
		
			t17.memory_type=Buff[0x12];
				
			CString StrTemp  ;
			StrTemp = _MemoryTypeToString(t17.memory_type);
  
			if(StrTemp != L"Unknown") 
			{
				StrMemType  = StrTemp ;
				nUse++;
			}



			//---------------------


			t17.form_factor=Buff[0x0E];
			StrFormFactor =_FormFactorToString(t17.form_factor);

			//------Speed------------
			memcpy(&t17.speed,p_buf+0x15,2);
			if(t17.speed>0)
			{
				Speed =t17.speed;
			}

				nSlot++;
	
		}

	


	} while (FoundDevice);
	if (DeviceCount < 1)
	{
		 
		Result = FALSE;
	}
	 
	

	*pUse = nUse; *pSlot = nSlot; *pSpeed = Speed;


	 StringCchCopy(FormFactor,MAX_PATH,StrFormFactor) ;


	return StrMemType;
}

bool CSMBIOS::ReturnNextTableOfType(supported_tables table_type_to_find, void* p_return_buffer, int* return_buffer_length, bool reset)
{
	static int index;
	int FormattedSectionLength=0;
	int UnformattedSectionLength=0;
	int CurrentTableLength=0;

	if (reset) index=0;

	while(true)
	{
		
		FormattedSectionLength = _GetFormattedSectionLength(index);
		UnformattedSectionLength = _GetUnformattedSectionLength(index);
		CurrentTableLength = FormattedSectionLength+UnformattedSectionLength;
		
		//check the first 
		if (GlobalBuffer.smbios_table_data[index]==table_type_to_find)
		{ 
			if (CurrentTableLength < *return_buffer_length)
			{
				memcpy(p_return_buffer,&(GlobalBuffer.smbios_table_data[index]),CurrentTableLength);
				*return_buffer_length=CurrentTableLength;
			}
			else
			{
				break;
				//indicates an error even though table was found...your buffer was too small
			}
			index+=CurrentTableLength;
			return true;
		}
		
		index+=CurrentTableLength;

		if (CurrentTableLength==0) break;
	}
	*return_buffer_length=0;
	return false;
}

CString CSMBIOS::_MemoryTypeToString(unsigned char device)
{

	CString StrMemType ;

	switch (device)
	{
		case OTHER1:
			StrMemType=_T("Other");
			break;
		case DRAM:
			StrMemType=_T("DRAM");
			break;
		case EDRAM:
			StrMemType=_T("EDRAM");
			break;
		case VRAM:
			StrMemType=_T("VRAM");
			break;
		case SRAM:
			StrMemType=_T("SRAM");
			break;
		case RAM:
			StrMemType=_T("RAM");
			break;
		case ROM:
			StrMemType=_T("ROM");
			break;
		case FLASH:
			StrMemType=_T("FLASH");
			break;
		case EEPROM:
			StrMemType=_T("EEPROM");
			break;
		case FEPROM:
			StrMemType=_T("FEPROM");
			break;
		case EPROM:
			StrMemType=_T("EPROM");
			break;
		case CDRAM:
			StrMemType=_T("CDRAM");
			break;
		case _3DRAM:
			StrMemType=_T("3DRAM");
			break;
		case SDRAM:
			StrMemType=_T("SDRAM");
			break;
		case SGRAM:
			StrMemType=_T("SGRAM");
			break;
		case RDRAM:
			StrMemType=_T("RDRAM");
			break; 
		case DDR:
			StrMemType=_T("DDR");
			break;
		case DDR2:
			StrMemType=_T("DDR2");
			break;
		case DDR2_FB_DIMM:
			StrMemType=_T("DDR2 FB-DIMM");
			break;
		case DDR3:
			StrMemType=_T("DDR3");
			break;
		default:
			StrMemType=_T("Unknown");
	}


	return StrMemType;
}

CString CSMBIOS::_FormFactorToString(unsigned char form)
{
	CString StrRet;
	switch (form)
	{
	case OTHER1:
		StrRet=_T("Other");
		break;
	case SIMM:
		StrRet=_T("SIMM");
		break;
	case SIP:
		StrRet=_T("SIP");
		break;
	case CHIP:
		StrRet=_T("CHIP");
		break;
	case DIP:
		StrRet=_T("DIP");
		break;
	case ZIP:
		StrRet=_T("ZIP");
		break;
	case PROPRIETARY8:
		StrRet=_T("Proprietary Card");
		break;
	case DIMM:
		StrRet=_T("DIMM");
		break;
	case TSOP:
		StrRet=_T("TSOP");
		break;
	case ROW:
		StrRet=_T("ROW OF CHIPS");
		break;
	case RIMM:
		StrRet=_T("RIMM");
		break;
	case SODIMM:
		StrRet=_T("SODIMM");
		break;
	case SRIMM:
		StrRet=_T("SRIMM");
		break;
	case FB_DIMM:
		StrRet=_T("FB-DIMM");
		break;
	default:
		StrRet=_T("Unknown");
	}

	return StrRet;
}

int CSMBIOS::_GetFormattedSectionLength(int Index)
{
	int Type = GlobalBuffer.smbios_table_data[Index];
	if (( Type <0) || (Type > 127)) return 0; //error
	int Length = (int)GlobalBuffer.smbios_table_data[Index+1];
	return Length;
}

int CSMBIOS::_GetUnformattedSectionLength(int Index)
{
	int start_point=Index;
	int unformatted_section_length=(int)GlobalBuffer.smbios_table_data[Index+1];
	Index+=unformatted_section_length;
	while (Index < MAX_DATA-1)
	{
		if ((GlobalBuffer.smbios_table_data[Index]==0) && (GlobalBuffer.smbios_table_data[Index+1]==0))
		{
			return Index-unformatted_section_length+2-start_point;
		}
		Index++;
	}
	return 0;
}
