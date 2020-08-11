# OPCDAClientSync
c++ class for easy interaction with OPC servers of the OPC DA 2.05a specification

## Description
* OPCDAClientSync written in C++
* No external dependencies, only the ATL library is used (included as standart component for Visual Studio)
* Supported both versions of client **X86** and **X64** for server **X86** and **X64** in any combination
* Supported only **sync read** and **sync write** operations from OPC DA 2.05a specification
* Low memory usage
* High productivity:
	* 59.9 seconds for 1 000 000 read operations for local OPC Server (Intel Core i5-7200U processor)
	* 53.3 seconds for 1 000 000 write operations for local OPC Server (Intel Core i5-7200U processor)
* Tested on 
	* Windows 10 (x64) with VS2019 build with x86 project.
	* Windows 10 (x64) with VS2019 build with x64 project.
* Simple usage:
	* Only two files for usage: `OPCDAClientSync.h` and `OPCDAClientSync.cpp`
	* All readed and writed values is `std::wstring`.
* Included Visual Studio 2019 solution `OPCDAClientSync.sln` and project `OPCDAClientSync.vcxproj`
	
## Usage
```cpp
#include "OPCDAClientSync.h"

void test(void)
{
	OPCDAClientSync opc (L"OPC.Server.ProgId.1.0");
	opc.WriteItem (L"opc.item.name", L"aaa");
	std::wstring aaa = opc.ReadItem (L"opc.item.name");
}

void main(void)
{
	OPCDAClientSync::Initialize();
	test();
	OPCDAClientSync::Uninitialize ();
}
```
## Tips
* `OPCDAClientSync::Initialize()` must be called in every thread, before using this class
* `OPCDAClientSync::Uninitialize()` must be called in every thread, after using this class

## Detail & ChangeLog
* Date: 2020-08-10
	* First release

