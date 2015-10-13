#pragma once

//----------------------------------------------------------
//--                        header                        --
//----------------------------------------------------------
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

//----------------------------------------------------------
//--                        marco                         --
//----------------------------------------------------------

#define _WIN32_DCOM

#define WMI_BIOS_SN_LENGTH	16
#define WMI_MB_UUID_LENGTH	36

//----------------------------------------------------------
//--                        struct                        --
//----------------------------------------------------------

struct wmi_sys_info	//total size must be 16*n
{
	unsigned char mb_uuid[WMI_MB_UUID_LENGTH+1];
	unsigned char bios_sn[WMI_BIOS_SN_LENGTH+1];
	unsigned char useless[10];
};

//----------------------------------------------------------
//--                        class                         --
//----------------------------------------------------------

class WmiInfo
{
public:
	static bool getWmiInfoEnc(wmi_sys_info &wmiDst);
	WmiInfo() : m_pLoc(NULL), 
				m_pSvc(NULL),
				m_pEnumerator_bios_sn(NULL),
				m_pEnumerator_mb_uuid(NULL),
				m_pclsObj(NULL),
				m_uReturn(0)
				{}
	~WmiInfo()	{}

	// func
	bool initial(void);
	void release(void);
	void getBIOS_SN(wmi_sys_info &infoDst);
	void getMB_UUID(wmi_sys_info &infoDst);
		
private:

	// func
	bool initCom(void);
	bool setComLevel(void);
	bool initLoc(void);
	bool connWMI(void);
	bool setProxy(void);
	bool queryCommand(const bstr_t cmd, IEnumWbemClassObject **obj);
	void getWmiData(const LPCWSTR cmd, IEnumWbemClassObject *obj, unsigned char *arr, const int len);

	// var
	HRESULT m_hres;
	IWbemLocator	*m_pLoc;
	IWbemServices	*m_pSvc;
	IEnumWbemClassObject *m_pEnumerator_bios_sn;
	IEnumWbemClassObject *m_pEnumerator_mb_uuid;
	IWbemClassObject	 *m_pclsObj;
	ULONG m_uReturn;
};