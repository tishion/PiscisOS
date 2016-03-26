; Core Dynamic library for Visual Fasm
; Tishion 11-13-2012
format PE GUI 4.0 DLL
entry DllMain

;include '..\..\INCLUDE\win32a.inc'
include 'win32a.inc'

section '.text' code readable executable

	proc DllMain hinstDLL,fdwReason,lpvReserved	
	._return:
		mov	eax,TRUE
		ret
	endp

	proc RunFasmCore lpInPutFile, lpOutPutFile, lpSymbolsFile, dwMemorySize, dwPassesLimit
		mov	[con_handle],STD_OUTPUT_HANDLE
		mov	esi, _logo
		call display_string
			
		mov	[input_file], 0
		mov	[output_file], 0
		mov	[symbols_file], 0
		mov	[memory_setting], 0
		mov	[passes_limit], 100
		
		cmp [lpInPutFile], NULL
		je ._error
		cmp [lpOutPutFile], NULL
		je ._error
		
		push esi
		
		mov esi, [lpInPutFile]
		mov	[input_file], esi
		mov esi, [lpOutPutFile]
		mov	[output_file], esi
		mov esi, [lpSymbolsFile]
		mov	[symbols_file], esi
		mov esi, [dwMemorySize]
		mov	[memory_setting], esi
		mov esi, [dwPassesLimit]
		mov	[passes_limit], si
		
		pop esi
		
		call [GetTickCount]
		mov	[start_time], eax

		call init_memory
		call preprocessor
		call parser
		call assembler
		call formatter
		
		call [GetTickCount]
		mov [end_time], eax
		
		call display_user_messages
		movzx eax, [current_pass]
		inc	eax
		call display_number
		mov	esi, _passes_suffix
		call display_string
		
		mov eax, [end_time]
		sub	eax, [start_time]
		xor	edx, edx
		mov	ebx, 100
		div	ebx
		or eax, eax
		jz .display_bytes_count
		xor	edx, edx
		mov	ebx, 10
		div	ebx
		push edx
		call display_number
		mov	dl, '.'
		call display_character
		pop	eax
		call display_number
		mov	esi, _seconds_suffix
		call display_string
		
	.display_bytes_count:
		mov	eax, [written_size]
		call display_number
		mov	esi, _bytes_suffix
		call display_string
		xor	al, al
		
		call free_memory

		jmp ._ok
	
	._error:
		mov eax, 0
		jmp ._return
	._ok:
		mov eax, 1
	._return:
		ret
	endp

include 'vfcinterface.inc'

include '..\errors.inc'
include '..\symbdump.inc'
include '..\preproce.inc'
include '..\parser.inc'
include '..\exprpars.inc'
include '..\assemble.inc'
include '..\exprcalc.inc'
include '..\formats.inc'
include '..\x86_64.inc'
include '..\avx.inc'

include '..\tables.inc'
include '..\messages.inc'

section '.data' data readable writeable

include '..\version.inc'

_logo db 'flat assembler  version ',VERSION_STRING,0

_memory_prefix db '  (',0
_memory_suffix db ' kilobytes memory)',0Dh,0Ah,0
_passes_suffix db ' passes, ',0
_seconds_suffix db ' seconds, ',0
_bytes_suffix db ' bytes.',0Dh,0Ah,0

align 4

include '..\variable.inc'

con_handle dd ?
memory_setting dd ?
start_time dd ?
end_time dd ?
bytes_count dd ?
displayed_count dd ?
character db ?
last_displayed rb 2

params rb 1000h
options rb 1000h
buffer rb 4000h

stack 10000h

section '.idata' import data readable writeable
	dd 0,0,0,rva kernel_name,rva kernel_table
	dd 0,0,0,0,0
	
	kernel_table:
		ExitProcess dd rva _ExitProcess
		CreateFile dd rva _CreateFileA
		ReadFile dd rva _ReadFile
		WriteFile dd rva _WriteFile
		CloseHandle dd rva _CloseHandle
		SetFilePointer dd rva _SetFilePointer
		GetCommandLine dd rva _GetCommandLineA
		GetEnvironmentVariable dd rva _GetEnvironmentVariable
		GetStdHandle dd rva _GetStdHandle
		VirtualAlloc dd rva _VirtualAlloc
		VirtualFree dd rva _VirtualFree
		GetTickCount dd rva _GetTickCount
		GetSystemTime dd rva _GetSystemTime
		GlobalMemoryStatus dd rva _GlobalMemoryStatus
		dd 0
		
	kernel_name db 'KERNEL32.DLL',0
		
		_ExitProcess dw 0
		db 'ExitProcess',0
		_CreateFileA dw 0
		db 'CreateFileA',0
		_ReadFile dw 0
		db 'ReadFile',0
		_WriteFile dw 0
		db 'WriteFile',0
		_CloseHandle dw 0
		db 'CloseHandle',0
		_SetFilePointer dw 0
		db 'SetFilePointer',0
		_GetCommandLineA dw 0
		db 'GetCommandLineA',0
		_GetEnvironmentVariable dw 0
		db 'GetEnvironmentVariableA',0
		_GetStdHandle dw 0
		db 'GetStdHandle',0
		_VirtualAlloc dw 0
		db 'VirtualAlloc',0
		_VirtualFree dw 0
		db 'VirtualFree',0
		_GetTickCount dw 0
		db 'GetTickCount',0
		_GetSystemTime dw 0
		db 'GetSystemTime',0
		_GlobalMemoryStatus dw 0
		db 'GlobalMemoryStatus',0
			
section '.edata' export data readable

	export 'VFASMCORE.DLL',\
		RunFasmCore,'RunFasmCore'
		
section '.rsrc' resource data readable

  ; resource directory
  directory RT_VERSION,versions

  ; resource subdirectories
  resource versions,\
	   1,LANG_NEUTRAL,version

  versioninfo version,VOS__WINDOWS32,VFT_APP,VFT2_UNKNOWN,LANG_ENGLISH+SUBLANG_DEFAULT,0,\
	      'FileDescription','Core Module for Visual Flat Assembler.',\
	      'LegalCopyright','Tishion (C) 2013 All rights reserved.',\
	      'FileVersion','1.0',\
	      'ProductVersion','1.0',\
	      'OriginalFilename','vfasm.dll'


section '.reloc' fixups data readable discardable