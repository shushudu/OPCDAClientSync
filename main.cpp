#include "OPCDAClientSync.h"
#include <cassert>

void main(void)
{
	OPCDAClientSync::Initialize();
	{
		OPCDAClientSync opc (L"Graybox.Simulator.1");

		for (int i = 0; i < 100; ++i)
		{
			opc.WriteItem (L"storage.string.reg01", L"aaa");
			Sleep (5);
			std::wstring aaa = opc.ReadItem (L"storage.string.reg01");
			assert (L"aaa" == aaa);


			opc.WriteItem (L"storage.string.reg01", L"bbb");
			Sleep (5);
			std::wstring bbb = opc.ReadItem (L"storage.string.reg01");
			assert (L"bbb" == bbb);
		}
	}
	OPCDAClientSync::Uninitialize ();
}