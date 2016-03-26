#ifndef _VFASMCORE_H_
#define _VFASMCORE_H_

#define EXTERN_IMPORT extern "C" _declspec(dllimport)

EXTERN_IMPORT __int32 RunFasmCore(
	const char* lpInPutFile, 
	const char* lpOutPutFile,
	const char* lpSymbolsFile,
	__int32 dwMemorySize,
	__int16 wPassessLimit);

#endif