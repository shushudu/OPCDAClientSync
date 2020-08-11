#pragma once
#include "OPCDAClientSync.h"
#include <exception>

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

	ATL::CComPtr<IOPCServer> pIOPCServer;
	HRESULT result = pIOPCServer.CoCreateInstance(server_prog_id);
	if (FAILED (result))
	{
		throw std::exception ("CoCreateInstance IOPCServer failed");
	}

	CComBSTR group_name = L"group";
	FLOAT group_deadband = 0.0;
	OPCHANDLE group_handle = 0;
	DWORD group_revised_updaterate = 0;

	ATL::CComPtr<IOPCGroupStateMgt>	pIOPCGroupStateMgt;
	result = pIOPCServer->AddGroup (group_name, FALSE, 1000, 0, 0, &group_deadband, 0, &group_handle, &group_revised_updaterate, __uuidof(IOPCGroupStateMgt), ( LPUNKNOWN * ) &pIOPCGroupStateMgt);
	if (FAILED (result))
	{
		throw std::exception ("AddGroup failed");
	}

	ATL::CComPtr<IOPCSyncIO> pIOPCSyncIO;
	result = pIOPCGroupStateMgt.QueryInterface (&pIOPCSyncIO);
	if (FAILED (result))
	{
		throw std::exception ("QueryInterface IID_IOPCSyncIO failed");
	}


	ATL::CComPtr<IOPCItemMgt> pIOPCItemMgt;
	result = pIOPCGroupStateMgt.QueryInterface (&pIOPCItemMgt);
	if (FAILED (result))
	{
		throw std::exception ("QueryInterface IID_IOPCItemMgt failed");
	}

	m_pIOPCSyncIO.Attach(pIOPCSyncIO.Detach());
	m_pIOPCItemMgt.Attach(pIOPCItemMgt.Detach());
	m_pIOPCServer.Attach(pIOPCServer.Detach());

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
