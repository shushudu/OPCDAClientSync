#pragma once

#include "atlbase.h"
#include <string>
#include <map>

/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#include "windows.h"
#include "ole2.h"

#pragma once

extern "C"{


    typedef DWORD OPCHANDLE;


    typedef 
        enum tagOPCDATASOURCE
    {	OPC_DS_CACHE	= 1,
        OPC_DS_DEVICE	= OPC_DS_CACHE + 1
    } 	OPCDATASOURCE;


    typedef enum tagOPCSERVERSTATE {} OPCSERVERSTATE;


    typedef enum tagOPCENUMSCOPE {} OPCENUMSCOPE;


    typedef struct tagOPCITEMSTATE
    {
        OPCHANDLE hClient;
        FILETIME ftTimeStamp;
        WORD wQuality;
        WORD wReserved;
        VARIANT vDataValue;
    } 	OPCITEMSTATE;


    typedef struct tagOPCSERVERSTATUS
    {
        FILETIME ftStartTime;
        FILETIME ftCurrentTime;
        FILETIME ftLastUpdateTime;
        OPCSERVERSTATE dwServerState;
        DWORD dwGroupCount;
        DWORD dwBandWidth;
        WORD wMajorVersion;
        WORD wMinorVersion;
        WORD wBuildNumber;
        WORD wReserved;
        /* [string] */ LPWSTR szVendorInfo;
    } 	OPCSERVERSTATUS;


    typedef struct tagOPCITEMDEF
    {
        /* [string] */ LPWSTR szAccessPath;
        /* [string] */ LPWSTR szItemID;
        BOOL bActive;
        OPCHANDLE hClient;
        DWORD dwBlobSize;
        /* [size_is] */ BYTE *pBlob;
        VARTYPE vtRequestedDataType;
        WORD wReserved;
    } 	OPCITEMDEF;


    typedef struct tagOPCITEMRESULT
    {
        OPCHANDLE hServer;
        VARTYPE vtCanonicalDataType;
        WORD wReserved;
        DWORD dwAccessRights;
        DWORD dwBlobSize;
        /* [size_is] */ BYTE *pBlob;
    } 	OPCITEMRESULT;


    EXTERN_C const IID IID_IOPCServer;


    MIDL_INTERFACE("39c13a4d-011e-11d0-9675-0020afd8adb3")
        IOPCServer : public IUnknown
    {
    public:
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

        virtual HRESULT STDMETHODCALLTYPE GetErrorString( 
            /* [in] */ HRESULT dwError,
            /* [in] */ LCID dwLocale,
            /* [string][out] */ LPWSTR *ppString) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetGroupByName( 
            /* [string][in] */ LPCWSTR szName,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPUNKNOWN *ppUnk) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetStatus( 
            /* [out] */ OPCSERVERSTATUS **ppServerStatus) = 0;

        virtual HRESULT STDMETHODCALLTYPE RemoveGroup( 
            /* [in] */ OPCHANDLE hServerGroup,
            /* [in] */ BOOL bForce) = 0;

        virtual HRESULT STDMETHODCALLTYPE CreateGroupEnumerator( 
            /* [in] */ OPCENUMSCOPE dwScope,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPUNKNOWN *ppUnk) = 0;

    };


    EXTERN_C const IID IID_IOPCGroupStateMgt;

    MIDL_INTERFACE("39c13a50-011e-11d0-9675-0020afd8adb3")
        IOPCGroupStateMgt : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            /* [out] */ DWORD *pUpdateRate,
            /* [out] */ BOOL *pActive,
            /* [string][out] */ LPWSTR *ppName,
            /* [out] */ LONG *pTimeBias,
            /* [out] */ FLOAT *pPercentDeadband,
            /* [out] */ DWORD *pLCID,
            /* [out] */ OPCHANDLE *phClientGroup,
            /* [out] */ OPCHANDLE *phServerGroup) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetState( 
            /* [in][unique] */ DWORD *pRequestedUpdateRate,
            /* [out] */ DWORD *pRevisedUpdateRate,
            /* [in][unique] */ BOOL *pActive,
            /* [in][unique] */ LONG *pTimeBias,
            /* [in][unique] */ FLOAT *pPercentDeadband,
            /* [in][unique] */ DWORD *pLCID,
            /* [in][unique] */ OPCHANDLE *phClientGroup) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetName( 
            /* [string][in] */ LPCWSTR szName) = 0;

        virtual HRESULT STDMETHODCALLTYPE CloneGroup( 
            /* [string][in] */ LPCWSTR szName,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPUNKNOWN *ppUnk) = 0;

    };


    EXTERN_C const IID IID_IOPCSyncIO;

    MIDL_INTERFACE("39c13a52-011e-11d0-9675-0020afd8adb3")
        IOPCSyncIO : public IUnknown
    {
    public:
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


    EXTERN_C const IID IID_IOPCItemMgt;

    MIDL_INTERFACE("39c13a54-011e-11d0-9675-0020afd8adb3")
        IOPCItemMgt : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddItems( 
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ OPCITEMDEF *pItemArray,
            /* [size_is][size_is][out] */ OPCITEMRESULT **ppAddResults,
            /* [size_is][size_is][out] */ HRESULT **ppErrors) = 0;

        virtual HRESULT STDMETHODCALLTYPE ValidateItems( 
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ OPCITEMDEF *pItemArray,
            /* [in] */ BOOL bBlobUpdate,
            /* [size_is][size_is][out] */ OPCITEMRESULT **ppValidationResults,
            /* [size_is][size_is][out] */ HRESULT **ppErrors) = 0;

        virtual HRESULT STDMETHODCALLTYPE RemoveItems( 
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ OPCHANDLE *phServer,
            /* [size_is][size_is][out] */ HRESULT **ppErrors) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetActiveState( 
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ OPCHANDLE *phServer,
            /* [in] */ BOOL bActive,
            /* [size_is][size_is][out] */ HRESULT **ppErrors) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetClientHandles( 
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ OPCHANDLE *phServer,
            /* [size_is][in] */ OPCHANDLE *phClient,
            /* [size_is][size_is][out] */ HRESULT **ppErrors) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetDatatypes( 
            /* [in] */ DWORD dwCount,
            /* [size_is][in] */ OPCHANDLE *phServer,
            /* [size_is][in] */ VARTYPE *pRequestedDatatypes,
            /* [size_is][size_is][out] */ HRESULT **ppErrors) = 0;

        virtual HRESULT STDMETHODCALLTYPE CreateEnumerator( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPUNKNOWN *ppUnk) = 0;

    };
}



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
