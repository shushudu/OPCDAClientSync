#pragma once

#include "atlbase.h"
#include <string>
#include <map>

typedef DWORD OPCHANDLE;

enum class OPCDATASOURCE
{	OPC_DS_CACHE	= 1,
    OPC_DS_DEVICE	= OPC_DS_CACHE + 1
};

struct OPCITEMSTATE
{
    OPCHANDLE hClient;
    FILETIME ftTimeStamp;
    WORD wQuality;
    WORD wReserved;
    VARIANT vDataValue;
};

struct OPCITEMDEF
{
    LPWSTR szAccessPath;
    LPWSTR szItemID;
    BOOL bActive;
    OPCHANDLE hClient;
    DWORD dwBlobSize;
    /* [size_is] */ BYTE *pBlob;
    VARTYPE vtRequestedDataType;
    WORD wReserved;
};

struct OPCITEMRESULT
{
    OPCHANDLE hServer;
    VARTYPE vtCanonicalDataType;
    WORD wReserved;
    DWORD dwAccessRights;
    DWORD dwBlobSize;
    /* [size_is] */ BYTE *pBlob;
};

MIDL_INTERFACE("39c13a4d-011e-11d0-9675-0020afd8adb3")
IOPCServer : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE AddGroup( 
        LPCWSTR szName,
        BOOL bActive,
        DWORD dwRequestedUpdateRate,
        OPCHANDLE hClientGroup,
        /* [in][unique] */ LONG *pTimeBias,
        /* [in][unique] */ FLOAT *pPercentDeadband,
        DWORD dwLCID,
        /* [out] */ OPCHANDLE *phServerGroup,
        /* [out] */ DWORD *pRevisedUpdateRate,
        REFIID riid,
        /* [iid_is][out] */ LPUNKNOWN *ppUnk) = 0;
};

MIDL_INTERFACE("39c13a50-011e-11d0-9675-0020afd8adb3")
IOPCGroupStateMgt : IUnknown {};

MIDL_INTERFACE("39c13a52-011e-11d0-9675-0020afd8adb3")
IOPCSyncIO : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Read( 
        OPCDATASOURCE dwSource,
        DWORD dwCount,
        /* [size_is][in] */ OPCHANDLE *phServer,
        /* [size_is][out] */ OPCITEMSTATE **ppItemValues,
        /* [size_is][out] */ HRESULT **ppErrors) = 0;

    virtual HRESULT STDMETHODCALLTYPE Write( 
        DWORD dwCount,
        /* [size_is][in] */ OPCHANDLE *phServer,
        /* [size_is][in] */ VARIANT *pItemValues,
        /* [size_is][out] */ HRESULT **ppErrors) = 0;
};

MIDL_INTERFACE("39c13a54-011e-11d0-9675-0020afd8adb3")
IOPCItemMgt : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE AddItems( 
        /* [in] */ DWORD dwCount,
        /* [size_is][in] */ OPCITEMDEF *pItemArray,
        /* [size_is][out] */ OPCITEMRESULT **ppAddResults,
        /* [size_is][out] */ HRESULT **ppErrors) = 0;
};

class OPCDAClientSync
{
	ATL::CComPtr<IOPCSyncIO> m_pIOPCSyncIO;
	ATL::CComPtr<IOPCItemMgt> m_pIOPCItemMgt;
	ATL::CComPtr<IOPCServer> m_pIOPCServer;
	std::map<std::wstring, DWORD> m_server_handles;

	void AddItem (const std::wstring & name);

public:

	static void Initialize ();
	static void Uninitialize ();

	OPCDAClientSync (const std::wstring & serverProgID);
	std::wstring ReadItem (const std::wstring & name);
	void WriteItem (const std::wstring & name, const std::wstring & val);
};
