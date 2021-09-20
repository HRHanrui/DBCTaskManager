#pragma once



 
#define MAX_DATA 0xFA00 //64K
#define MAX_STRING 0x3E8 //1K
#define MAX_STRING_TABLE 0x19 //25
#define MAX_CONTAINED_ELEMENTS 0xFF //255
#define MAX_CONTAINED_RECORD 0xFF //255
#define SMALL_STRING 0x64 //100
#define BIGGER_STRING 0xFF //255




//table 17.0 form factor
#define OTHER1 0x01
#define UNKNOWN2 0x02
#define SIMM 0x03
#define SIP 0x04
#define CHIP 0x05
#define DIP 0x06
#define ZIP 0x07
#define PROPRIETARY8 0x08
#define DIMM 0x09
#define TSOP 0x0A
#define ROW 0x0B
#define RIMM 0x0C
#define SODIMM 0x0D
#define SRIMM 0x0E
#define FB_DIMM 0x0F

//table 17.1 device type
#define DRAM 0x03
#define EDRAM 0x04
#define VRAM 0x05
#define SRAM 0x06
#define RAM 0x07
#define ROM 0x08
#define FLASH 0x09
#define EEPROM 0x0A
#define FEPROM 0x0B
#define EPROM 0x0C
#define CDRAM 0x0D
#define _3DRAM 0x0E
#define SDRAM 0x0F
#define SGRAM 0x10
#define RDRAM 0x11
#define DDR 0x12
#define DDR2 0x13
#define DDR2_FB_DIMM 0x14
#define DDR3 0x18





struct raw_smbios_data
{
    unsigned char	smbios_major_version;
    unsigned char	smbios_minor_version;
    unsigned int	length;
    unsigned char	smbios_table_data[MAX_DATA];
};

struct required
{
	unsigned char type; //Type of table (spec allows 0 - 127)
	unsigned char length; //Length of formatted area, starting at type (does not include any string tables)
	unsigned short handle; //Unique handle for this structure for later recall
};




struct memory_device_type_17
{
	required header;							
	unsigned short physical_memory_array_handle;			//0x04 Handle to link device to parent array
	unsigned short memory_error_information_handle;		//0x06 instance number associated with an error condition
	unsigned short total_width;								//0x08 width in bits of this memory device including any error bits
	unsigned short data_width;								//0x0A data width in bits
	unsigned short size;									//0x0C see table 17.3
	unsigned char form_factor;								//0x0E see table 17.0
	unsigned char device_set;								//0x0F links a memory device to a larger set
	_TCHAR device_locator[MAX_STRING*sizeof(_TCHAR)];		//0x10 string that identifies socket
	_TCHAR bank[MAX_STRING*sizeof(_TCHAR)];				//0x11 string that identifes bank
	unsigned char memory_type;								//0x12 see table 17.1
	unsigned short type_detail;								//0x13 see table 17.2
	unsigned short speed;									//0x15 speed of device in mhz
	_TCHAR manufactuer[MAX_STRING*sizeof(_TCHAR)];			//0x17 mfg of memory device
	_TCHAR serial_number[MAX_STRING*sizeof(_TCHAR)];		//0x18 serial number of memory device
	_TCHAR asset_tag[MAX_STRING*sizeof(_TCHAR)];			//0x19 asset tag of memory device
	_TCHAR part_number[MAX_STRING*sizeof(_TCHAR)];			//0x1A part number of memory device
	unsigned char rank;										//0x1B see table 17.4
};

enum supported_tables
{
	bios_info = 0x00,
	sys_info = 0x01,
	enclosure = 0x03,
	processor = 0x04,
	cache = 0x07,
	slots = 0x09,
	phys_mem_array = 0x10,
	mem_dev = 0x11,
	mem_array_map_addr = 0x13,
	sys_boot_info = 0x20
};





class CSMBIOS
{
public:
	CSMBIOS(void);
	~CSMBIOS(void);
	raw_smbios_data GlobalBuffer;
	BOOL InitRawSMBIOS_Data(void);
	void GetStringResourceForID(int target_id, BYTE * in_buf, _TCHAR * out_string);
	CString GetMemoryInfo(int * pUse,int *pSlot,UINT *pSpeed,WCHAR *FormFactor);
	bool ReturnNextTableOfType(supported_tables table_type_to_find, void* p_return_buffer, int* return_buffer_length, bool reset);
	CString _MemoryTypeToString(unsigned char device);
	CString _FormFactorToString(unsigned char form);
	int _GetFormattedSectionLength(int Index);
	int _GetUnformattedSectionLength(int Index);
};
