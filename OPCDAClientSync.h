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
    /* [string] */ LPWSTR szAccessPath;
    /* [string] */ LPWSTR szItemID;
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
        /* [string][in] */ LPCWSTR szName,
        /* [in] */ BOOL bActive,
        /* [in] */ DWORD dwRequestedUpdateRate,
        /* [in] */ OPCHANDLE hClientGroup,
        /* [in][unique] */ LONG *pTimeBias,
        /* [in][unique] */ FLOAT *pPercentDeadband,
        /* [in] */ DWORD dwLCID,
        /* [out] */ OPCHANDLE *phServerGroup,
        /* [out] */ DWORD *pRevisedUpdateRate,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ LPUNKNOWN *ppUnk) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetErrorString() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetGroupByName() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStatus() = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveGroup() = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateGroupEnumerator() = 0;
};

MIDL_INTERFACE("39c13a50-011e-11d0-9675-0020afd8adb3")
IOPCGroupStateMgt : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetState() = 0;
    virtual HRESULT STDMETHODCALLTYPE SetState() = 0;
    virtual HRESULT STDMETHODCALLTYPE SetName() = 0;
    virtual HRESULT STDMETHODCALLTYPE CloneGroup() = 0;
};

MIDL_INTERFACE("39c13a52-011e-11d0-9675-0020afd8adb3")
IOPCSyncIO : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Read( 
        /* [in] */ OPCDATASOURCE dwSource,
        /* [in] */ DWORD dwCount,
        /* [size_is][in] */ OPCHANDLE *phServer,
        /* [size_is][size_is][out] */ OPCITEMSTATE **ppItemValues,
        /* [size_is][size_is][out] */ HRESULT **ppErrors) = 0;

    virtual HRESULT STDMETHODCALLTYPE Write( 
        /* [in] */ DWORD dwCount,
        /* [size_is][in] */ OPCHANDLE *phServer,
        /* [size_is][in] */ VARIANT *pItemValues,
        /* [size_is][size_is][out] */ HRESULT **ppErrors) = 0;
};

MIDL_INTERFACE("39c13a54-011e-11d0-9675-0020afd8adb3")
IOPCItemMgt : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE AddItems( 
        /* [in] */ DWORD dwCount,
        /* [size_is][in] */ OPCITEMDEF *pItemArray,
        /* [size_is][size_is][out] */ OPCITEMRESULT **ppAddResults,
        /* [size_is][size_is][out] */ HRESULT **ppErrors) = 0;

    virtual HRESULT STDMETHODCALLTYPE ValidateItems() = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveItems() = 0;
    virtual HRESULT STDMETHODCALLTYPE SetActiveState() = 0;
    virtual HRESULT STDMETHODCALLTYPE SetClientHandles() = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDatatypes() = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateEnumerator() = 0;
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
