# OPCDAClientSync

// simple usage sync opc da client

#include "OPCDAClientSync.h"

void main(void)

{

	OPCDAClientSync::Initialize();
  
	{
  
		OPCDAClientSync opc (L"OPC.Server.ProgId.1.0");

		opc.WriteItem (L"opc.item.name", L"aaa");
    
		std::wstring aaa = opc.ReadItem (L"CKT.STORAGE.cmd");
    
	}
  
	OPCDAClientSync::Uninitialize ();
}
