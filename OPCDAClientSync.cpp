#pragma once
#include "OPCDAClientSync.h"
#include <exception>
#include <rpc.h>

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

//MIDL_DEFINE_GUID(IID, IID_CATID_OPCDAServer10,0x63D5F430,0xCFE4,0x11d1,0xB2,0xC8,0x00,0x60,0x08,0x3B,0xA1,0xFB);
//MIDL_DEFINE_GUID(IID, IID_CATID_OPCDAServer20,0x63D5F432,0xCFE4,0x11d1,0xB2,0xC8,0x00,0x60,0x08,0x3B,0xA1,0xFB);
//MIDL_DEFINE_GUID(IID, IID_CATID_OPCDAServer30,0xCC603642,0x66D7,0x48f1,0xB6,0x9A,0xB6,0x25,0xE7,0x36,0x52,0xD7);
MIDL_DEFINE_GUID(IID, IID_IOPCServer,0x39c13a4d,0x011e,0x11d0,0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3);
MIDL_DEFINE_GUID(IID, IID_IOPCGroupStateMgt,0x39c13a50,0x011e,0x11d0,0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3);
MIDL_DEFINE_GUID(IID, IID_IOPCSyncIO,0x39c13a52,0x011e,0x11d0,0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3);
MIDL_DEFINE_GUID(IID, IID_IOPCItemMgt,0x39c13a54,0x011e,0x11d0,0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3);
#undef MIDL_DEFINE_GUID

void OPCDAClientSync::Initialize ()
{
	HRESULT	result = CoInitialize(NULL);
	if (FAILED(result))
	{
		throw std::exception("CoInitialize failed");
	}

	result = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(result))
	{
		throw std::exception("CoInitializeSecurity failed");
	}
}

void OPCDAClientSync::Uninitialize ()
{
	CoUninitialize();
}

OPCDAClientSync::OPCDAClientSync (const std::wstring & serverProgID)
{
	CComBSTR server_prog_id = serverProgID.c_str();

	CLSID clsid;
	HRESULT result = CLSIDFromProgID (server_prog_id, &clsid);
	if (FAILED (result))
	{
		throw std::exception ("CLSIDFromProgID failed");
	}

	ATL::CComPtr<IClassFactory> iClassFactory;
	result = CoGetClassObject (clsid, CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, ( void ** ) &iClassFactory);
	if (FAILED (result))
	{
		throw std::exception ("CoGetClassObject failed");
	}

	ATL::CComPtr<IUnknown> iUnknown;
	result = iClassFactory->CreateInstance (NULL, IID_IUnknown, ( void ** ) &iUnknown);
	if (FAILED (result))
	{
		throw std::exception ("CreateInstance failed");
	}

	ATL::CComPtr<IOPCServer> iOpcServer;
	result = iUnknown->QueryInterface (IID_IOPCServer, ( void ** ) &iOpcServer);
	if (FAILED (result))
	{
		throw std::exception ("QueryInterface IID_IOPCServer failed");
	}


	CComBSTR group_name = L"group";
	FLOAT group_deadband = 0.0;
	OPCHANDLE group_handle = 0;
	DWORD group_revised_updaterate = 0;


	ATL::CComPtr<IOPCGroupStateMgt>	iStateManagement;
	result = iOpcServer->AddGroup (group_name, FALSE, 1000, 0, 0, &group_deadband, 0, &group_handle, &group_revised_updaterate, IID_IOPCGroupStateMgt, ( LPUNKNOWN * ) &iStateManagement);
	if (FAILED (result))
	{
		throw std::exception ("AddGroup failed");
	}

	ATL::CComPtr<IOPCSyncIO>		iSychIO;
	result = iStateManagement->QueryInterface (IID_IOPCSyncIO, ( void ** ) &iSychIO);
	if (FAILED (result))
	{
		throw std::exception ("QueryInterface IID_IOPCSyncIO failed");
	}


	ATL::CComPtr<IOPCItemMgt>		iItemManagement;
	result = iStateManagement->QueryInterface (IID_IOPCItemMgt, ( void ** ) &iItemManagement);
	if (FAILED (result))
	{
		throw std::exception ("QueryInterface IID_IOPCItemMgt failed");
	}

	m_pIOPCSyncIO.Attach(iSychIO.Detach());
	m_pIOPCItemMgt.Attach(iItemManagement.Detach());
	//m_pIOPCGroupStateMgt.Attach(iStateManagement.Detach());
	m_pIOPCServer.Attach(iOpcServer.Detach());

	m_server_handles.clear ();
};

void OPCDAClientSync::AddItem (const std::wstring & name)
{
	CComBSTR item_name = name.c_str();
	static OPCHANDLE next_item_client_handle = 1;

	OPCITEMDEF def = { 0 };
	def.szItemID = item_name;
	def.hClient = next_item_client_handle++;

	OPCITEMRESULT * addresult = nullptr;
	HRESULT * hresult = nullptr;

	HRESULT result = m_pIOPCItemMgt->AddItems (1, &def, &addresult, &hresult);
	if (FAILED (result))
	{
		throw std::exception ("AddItems failed");
	}

	m_server_handles.emplace (name, addresult->hServer);

	CoTaskMemFree (addresult);
	CoTaskMemFree (hresult);
};

std::wstring OPCDAClientSync::ReadItem (const std::wstring & name)
{
	if (!m_server_handles.count (name))
	{
		AddItem (name);
	}

	OPCHANDLE serverHandles = m_server_handles.at(name);
	OPCITEMSTATE * itemState;
	HRESULT * itemResult;

	HRESULT result = m_pIOPCSyncIO->Read (OPC_DS_DEVICE, 1, &serverHandles, &itemState, &itemResult);
	if (FAILED (result))
	{
		throw std::exception ("Read failed");
	}

	result = VariantChangeType (&itemState->vDataValue, &itemState->vDataValue, 0, VT_BSTR);
	if (FAILED (result))
	{
		throw std::exception ("VariantChangeType failed");
	}

	std::wstring ret = itemState->vDataValue.bstrVal;


	CoTaskMemFree (itemResult);
	VariantClear (&itemState->vDataValue);
	CoTaskMemFree (itemState);

	return ret;
};

void OPCDAClientSync::WriteItem (const std::wstring & name, const std::wstring & val)
{
	if (!m_server_handles.count (name))
	{
		AddItem (name);
	}

	OPCHANDLE serverHandles = m_server_handles.at(name);
	CComVariant value = val.c_str();
	HRESULT * itemResult;

	HRESULT result = m_pIOPCSyncIO->Write(1, &serverHandles, &value, &itemResult);
	if (FAILED(result)){
		throw std::exception("Write failed");
	}

	CoTaskMemFree (itemResult);
}
