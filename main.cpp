#include "OPCDAClientSync.h"

void main(void)
{
	OPCDAClientSync::Initialize();
	{
		OPCDAClientSync opc (L"OPC.Server.ProgId.1.0");

		for (int i = 0; i < 1000; ++i)
		{
			opc.WriteItem (L"opc.item.name", L"aaa");
			//Sleep (5);
			std::wstring aaa = opc.ReadItem (L"CKT.STORAGE.cmd");
			//assert (L"aaa" == aaa);


			opc.WriteItem (L"opc.item.name", L"bbb");
			//Sleep (5);
			std::wstring bbb = opc.ReadItem (L"opc.item.name");
			//assert (L"bbb" == bbb);
		}
	}
	OPCDAClientSync::Uninitialize ();
}