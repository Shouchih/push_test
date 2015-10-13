#pragma once
#include "StdAfx.h"
#include "WmiInfo.h"


//----------------------------------------------------------
//--                         marco                        --
//----------------------------------------------------------

//#define DEBUG_OUTPUT

//----------------------------------------------------------
//--                        Initial                       --
//----------------------------------------------------------
// Return 1 : Error

bool WmiInfo::initial(void)
{
	if (initCom())
	{
#ifdef DEBUG_OUTPUT
		MessageBox(0, TEXT("Failed to initialize COM library."), TEXT("WMI Error"), MB_OK | MB_ICONERROR);
#endif
		return 1;
	}

	if (setComLevel())
	{
#ifdef DEBUG_OUTPUT
		MessageBox(0, TEXT("Failed to initialize security."), TEXT("WMI Error"), MB_OK | MB_ICONERROR);
#endif
		return 1;
	}

	if (initLoc())
	{
#ifdef DEBUG_OUTPUT
		MessageBox(0, TEXT("Failed to create IWbemLocator object."), TEXT("WMI Error"), MB_OK | MB_ICONERROR);
#endif
		return 1;
	}

	if (connWMI())
	{
#ifdef DEBUG_OUTPUT
		MessageBox(0, TEXT("Could not connect."), TEXT("WMI Error"), MB_OK | MB_ICONERROR);
#endif
		return 1;
	}
	else
	{
#ifdef DEBUG_OUTPUT
		MessageBox(0, TEXT("Connected to ROOT\\CIMV2 WMI namespace."), TEXT("WMI Error"), MB_OK | MB_ICONERROR);
#endif
	}

	if (setProxy())
	{
#ifdef DEBUG_OUTPUT
		MessageBox(0, TEXT("Could not set proxy blanket."), TEXT("WMI Error"), MB_OK | MB_ICONERROR);
#endif
		return 1;
	}

	if (queryCommand(bstr_t("SELECT * FROM Win32_BIOS"), &m_pEnumerator_bios_sn))
	{
#ifdef DEBUG_OUTPUT
		MessageBox(0, TEXT("Query for BIOS SerialNumber failed."), TEXT("WMI Error"), MB_OK | MB_ICONERROR);
#endif
		return 1;
	}

	if (queryCommand(bstr_t("SELECT * FROM Win32_ComputerSystemProduct"), &m_pEnumerator_mb_uuid))
	{
#ifdef DEBUG_OUTPUT
		MessageBox(0, TEXT("Query for MotherBoard UUID failed."), TEXT("WMI Error"), MB_OK | MB_ICONERROR);
#endif
		return 1;
	}

	return 0;
}

//----------------------------------------------------------
//--                    Initialize COM.                   --
//----------------------------------------------------------
// Return 1 : Error

bool WmiInfo::initCom(void)
{
    
	// m_hres =CoInitializeEx(0, COINIT_MULTITHREADED)  
	m_hres =  CoInitialize(0); 	
	return (FAILED(m_hres)) ? 1 : 0;
}

//----------------------------------------------------------
//--           Set general COM security levels            --
//----------------------------------------------------------
// Return 1 : Error

bool WmiInfo::setComLevel(void)
{
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------

    m_hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );

	// This function is called exactly once per process, either explicitly or implicitly.
	// RPC_E_TOO_LATE means CoInitializeSecurity has already been called.

	if (SUCCEEDED(m_hres) || RPC_E_TOO_LATE==m_hres)
	{
		return 0;
	}
	else
	{
        CoUninitialize();
        return 1;                    // Program has failed.	
	}
}

//----------------------------------------------------------
//--          Obtain the initial locator to WMI           --
//----------------------------------------------------------
// Return 1 : Error

bool WmiInfo::initLoc(void)
{
    m_hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &m_pLoc);
 
    if (FAILED(m_hres))
    {
        CoUninitialize();
        return 1;                 // Program has failed.
    }

	return 0;
}

//----------------------------------------------------------
//--   Conn2WMI through the IWbemLocator::ConnectServer   --
//----------------------------------------------------------
// Return 1 : Error

bool WmiInfo::connWMI(void)
{
    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    m_hres = m_pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (for example, Kerberos)
         0,                       // Context object 
         &m_pSvc                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(m_hres))
    {
        m_pLoc->Release();     
        CoUninitialize();
        return 1;                // Program has failed.
    }

	return 0;
}

//----------------------------------------------------------
//--           Set security levels on the proxy           --
//----------------------------------------------------------
// Return 1 : Error

bool WmiInfo::setProxy(void)
{
    m_hres = CoSetProxyBlanket(
       m_pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(m_hres))
    {
        m_pSvc->Release();
        m_pLoc->Release();     
        CoUninitialize();
        return 1;               // Program has failed.
    }

	return 0;
}

//----------------------------------------------------------
//--   Use the IWbemServices ptr to make requests of WMI  --
//----------------------------------------------------------
// Return 1 : Error

bool WmiInfo::queryCommand(const bstr_t cmd, IEnumWbemClassObject **obj)
{
	m_hres = m_pSvc->ExecQuery(
        bstr_t("WQL"), 
		cmd,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        obj);
    
    if (FAILED(m_hres))
    {
        m_pSvc->Release();
        m_pLoc->Release();
        CoUninitialize();
        return 1;               // Program has failed.
    }

	return 0;
}

//----------------------------------------------------------
//--                     Get WMI Data                     --
//----------------------------------------------------------

void WmiInfo::getWmiData(const LPCWSTR cmd, IEnumWbemClassObject *obj, unsigned char *arr, const int len)
{
    while (obj)
    {
        HRESULT hr = obj->Next(WBEM_INFINITE, 1, &m_pclsObj, &m_uReturn);

        if (0 == m_uReturn)
        {
            break;
        }

        VARIANT vtProp;
        hr = m_pclsObj->Get(cmd, 0, &vtProp, 0, 0);
		char *pchStr = _com_util::ConvertBSTRToString(vtProp.bstrVal);

		// copy data
		memcpy(arr, pchStr, strlen(pchStr));
		memset(arr+len-1, '\0', sizeof(unsigned char));

  		delete pchStr;
        VariantClear(&vtProp);
        m_pclsObj->Release();
    }
}

//----------------------------------------------------------
//--                Get BIOS SerialNumber                 --
//----------------------------------------------------------

void WmiInfo::getBIOS_SN(wmi_sys_info &infoDst)
{
	getWmiData(L"SerialNumber", m_pEnumerator_bios_sn, infoDst.bios_sn, sizeof(infoDst.bios_sn));
}

//----------------------------------------------------------
//--                Get MotherBoard UUID                  --
//----------------------------------------------------------

void WmiInfo::getMB_UUID(wmi_sys_info &infoDst)
{
	getWmiData(L"UUID", m_pEnumerator_mb_uuid, infoDst.mb_uuid, sizeof(infoDst.mb_uuid));
}

//----------------------------------------------------------
//--                       Cleanup                        --
//----------------------------------------------------------

void WmiInfo::release(void)
{
    m_pSvc->Release();
    m_pLoc->Release();
    m_pEnumerator_bios_sn->Release();
	m_pEnumerator_mb_uuid->Release();
    CoUninitialize();
}

//** ---------------------------------
//		Static Get WMI Information (Enc)
//** ---------------------------------
// Return 1: Error
bool WmiInfo::getWmiInfoEnc(wmi_sys_info &wmiDst)
{	
	WmiInfo *wmiConn = new WmiInfo();

	// connect to WMI
	if (wmiConn->initial())
	{
#if defined(DEBUG_OUTPUT)
#if defined(MSG_BOX)
		MessageBox(0, TEXT("Can Not Initial WMI."), TEXT("WMI Error"), MB_OK | MB_ICONERROR);
#else
		cout << "Can Not Initial WMI.\n";
#endif
#endif
		delete wmiConn;
		return 1;
	}

	// get WMI info
	wmiConn->getBIOS_SN(wmiDst);
	wmiConn->getMB_UUID(wmiDst);

	// release WMI Conn
	wmiConn->release();
	delete wmiConn;

	return 0;
}