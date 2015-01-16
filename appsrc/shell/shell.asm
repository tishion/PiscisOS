include ".\include\ascii.h"
include ".\include\process.h"

FILEITEM_NAME 			equ 00h
FILEITEM_EXTEND			equ 08h
FILEITEM_ATTR   		equ 0bh
FILEITEM_RESERVED 		equ 10h
FILEITEM_WRITETIME 		equ 16h
FILEITEM_WRITEDATE	 	equ 18h
FILEITEM_FIRSTCLUSTER	equ 1ah
FILEITEM_FILESIZE		equ 1ch
FULLFILENAME_LEN		equ 0bh

FATTR_RDONLY			equ 00000001b
FATTR_HIDDEN			equ	00000010b
FATTR_SYSTEM			equ	00000100b
FATTR_VOLLAB			equ	00001000b
FATTR_SUBDIR			equ	00010000b
FATTR_DEVICE			equ	00100000b
FATTR_ARCHIV			equ	01000000b
FATTR_NOTUSE			equ	10000000b

use32

	org 00h
APP_HEADER:
	db	'PISCISAPP000'	;;Signature
	dd	0				;;Reserverd
	dd	MAIN			;;Entry Point
	dd	APP_ARGS		;;Arguments Buffer
	dd	0				;;Reserved

APP_ARGS:
	times (256) db	0 

MAIN:
	call InitShell
	
	mov edi, copyright
	call PrintVersion
	
.loop_m:
	call PrintPromot
	
	call GetCommand
	
	call GetNameAndArgs
	
	; mov edi, newline
	; mov eax, 04h
	; int 50h
	
	; mov edi, cmdexedone
	; mov eax, 04h
	; int 50h
	
	; mov edi, cmdbuf
	; mov eax, 04h
	; int 50h
	
	; mov edi, newline
	; mov eax, 04h
	; int 50h
	
	; mov edi, cmdname
	; mov eax, 04h
	; int 50h
	
	; mov edi, commandname
	; mov eax, 04h
	; int 50h
	
	; mov edi, newline
	; mov eax, 04h
	; int 50h
	
	; mov edi, cmdargs
	; mov eax, 04h
	; int 50h
	
	; mov edi, commandargs
	; mov eax, 04h
	; int 50h
	
	call RunCommandAsBuildin
	jc .loop_m
	
	call RunCommandAsApp
	
	jmp .loop_m
	
MAIN_END:
	
InitShell:
	mov edi, pwd
	mov ecx, 64
	mov al, 0
	cld
	rep stosb
	
	mov [pwd+0], byte '\'		;rootdir '
	mov [pwd+1], byte 0
	
	ret	
	
;;show version and copyright info
PrintVersion:
	push eax
	push edi
	
	mov edi, copyright
	mov eax, 04h
	int 50h
	
	pop edi
	pop eax
	ret
	
PrintPromot:
	push eax
	push edi
	
	mov edi, strpromot_pwd
	mov esi, pwd
	call strcopy
	
	mov esi, strpromot_pwd
	call formatpath
	
	mov edi, strpromot_pwd
	add edi, ecx
	mov [edi+0], byte '>'
	mov [edi+1], byte 0
	
	mov edi, strpromot
	mov eax, 04h
	int 50h
	
	pop edi
	pop eax
	ret

GetCommand:
;;in	esi:buffer to save command
;;		ecx:buffer size in bytes
	push eax
	push ebx
	push ecx
	push edx
	push edi
	
	mov esi, cmdbuf
	mov ecx, 128
	
	push ecx
	push esi
	
	mov edi, esi
	mov al, 0
	cld
	rep stosb
	
	pop esi
	pop ecx
	
	mov edx, esi
	add ecx, esi
.get_key:
	mov eax, 01h
	int 50h
	
	cmp al, 0ffh
	je .get_key
	
	cmp al, ASC_CC_CR
	je	.get_cmd_ok
	
	cmp al, ASC_CC_BS
	je	.is_bs
	
	cmp esi, ecx
	jae .get_key
	jmp .is_char
	
.is_bs:
	cmp esi, edx
	jbe .get_key
	dec esi
	mov byte [esi], 0
	jmp .show_char

.is_char:
	mov [esi], al
	inc esi
	
.show_char:
	mov bl, al
	mov eax, 03h
	int 50h

	jmp .get_key

.get_cmd_ok:
	pop edi
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret

GetNameAndArgs:
	push eax
	push ecx
	push esi
	push edi
	
	
	mov edi, commandname
	mov ecx, 64
	mov al, 0
	cld
	rep stosb
	
	mov edi, commandargs
	mov ecx, 64
	mov al, 0
	cld
	rep stosb
	
	mov esi, cmdbuf
.name_prefix:
	cmp [esi], byte ' '			;is ' '
	jne .copy_name
	inc esi
	jmp .name_prefix
	
.copy_name:	
	mov edi, commandname
	cmp [esi], byte 00h
	je .final_ret
	cld
.copy_name_char:
	lodsb
	cmp al, byte ' '
	je .args_prefix
	cmp al, byte 00h
	je .copy_pwd_as_args
	;je .copy_bin_as_args		;=========
	stosb
	jmp .copy_name_char
	
.args_prefix:	
	cmp [esi], byte ' '			;is ' '
	jne .copy_args
	inc esi
	jmp .args_prefix
	
; .copy_bin_as_args:			;=========
	; mov esi, defbinpath
	; mov edi, commandargs
	; cld
; .copy_bin_char:
	; lodsb
	; cmp al, byte 00h
	; je .final_ret
	; stosb
	; jmp .copy_bin_char
	
.copy_pwd_as_args:
	mov esi, pwd
	mov edi, commandargs
	cld
.copy_pwd_char:
	lodsb
	cmp al, byte 00h
	je .final_ret
	stosb
	jmp .copy_pwd_char
	
.copy_args:
	mov edi, commandargs
	cld
.copy_args_char:
	lodsb
	cmp al, byte 00h
	je .final_ret
	stosb
	jmp .copy_args_char
.final_ret:
	pop edi
	pop esi
	pop ecx
	pop eax
	ret

RunCommandAsBuildin:
;;out	cf:1 find buildin command and runned, 0 not find buildin command
	;;is buildin commands????????
	push eax
	push esi
	push edi
	
	; mov esi, commandname
	; mov edi, BuildinCmd_ls
	; call strcmp
	; test eax, eax
	; jz .call_ls
	
	mov esi, commandname
	mov edi, BuildinCmd_cd
	call strcmp
	test eax, eax
	jz .call_cd
	
	mov esi, commandname
	mov edi, BuildinCmd_pwd
	call strcmp
	test eax, eax
	jz .call_pwd
	
	mov esi, commandname
	mov edi, BuildinCmd_cls
	call strcmp
	test eax, eax
	jz .call_cls
	
	mov esi, commandname
	mov edi, BuildinCmd_exit
	call strcmp
	test eax, eax
	jz .call_exit
	
	clc
	jmp .final_ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	

; .call_ls:
	; call BuildinCmd_ls+8
	; jmp .process_done
.call_cd:
	call BuildinCmd_cd+8
	jmp .process_done
.call_pwd:
	call BuildinCmd_pwd+8
	jmp .process_done
.call_cls:
	call BuildinCmd_cls+8
	jmp .process_done
.call_exit:
	call BuildinCmd_exit+8
	jmp .process_done

.process_done:
	stc
.final_ret:
	pop edi
	pop esi
	pop eax
	ret

	
RunCommandAsApp:
	push eax
	push edi
	
	mov edi, newline
	mov eax, 04h
	int 50h
	
	mov edi, .apppath
	mov ecx, 128
	mov al, 0
	cld
	rep stosb
	
	cmp [commandname], byte 0
	je .final_ret
	
	xor ecx, ecx
	cmp [commandname], byte '\'
	je .no_def_path
	mov esi, defbinpath
	mov edi, .apppath
	call strcopy
.no_def_path:
	mov edi, .apppath
	add edi, ecx
	mov esi, commandname
	call strcopy
	
	mov esi, .apppath
	call formatpath
	
	mov edi, .apppath
	mov ebx, commandargs
	mov eax, 10h
	int 50h

	cmp eax, CPER_MAX_COUNT
	je .er_max_count
	cmp eax, CPER_NOT_FOUND_FILE
	je .er_not_found_file
	cmp eax, CPER_INVALID_FORMAT
	je .er_invalid_format
	
	mov ebx, eax
	mov eax, 12h
	int 50h
	
	jmp .process_done
	
.er_max_count:
	mov edi, estr_maxcount
	jmp .show_err
.er_not_found_file:
	mov edi, estr_notfoundfille
	jmp .show_err
.er_invalid_format:	
	mov edi, estr_invalidforma
	
.show_err:
	mov eax, 04h
	int 50h
	
.process_done:
	mov edi, newline
	mov eax, 04h
	int 50h
	
.final_ret:
	pop edi
	pop eax
	ret
	
.apppath:	times (128) db 0
			times (64) db 0

; BuildinCmd_ls:
; .cmd_name:
	; db	'l', 's', 0, 0, 0, 0, 0, 0
; .start:
	; push edi
	; push eax
	
	; mov edi, newline
	; mov eax, 04h
	; int 50h
	
	; mov edi, buildinstr
	; mov eax, 04h
	; int 50h
	
	; mov edi, .cmd_name
	; mov eax, 04h
	; int 50h
	
	; mov edi, newline
	; mov eax, 04h
	; int 50h
	
	; pop eax
	; pop edi
	; ret
	
BuildinCmd_cd:
.cmd_name:
	db	'c', 'd', 0, 0, 0, 0, 0, 0
.start:
	push edi
	push eax

	mov edi, newline
	mov eax, 04h
	int 50h
	
	; mov edi, buildinstr
	; mov eax, 04h
	; int 50h
	
	; mov edi, .cmd_name
	; mov eax, 04h
	; int 50h
	
	mov edi, .dirpath
	mov ecx, 128
	mov al, 0
	cld
	rep stosb
	
	cmp [commandargs], byte 0
	je .final_ret
	
	xor ecx, ecx
	cmp [commandargs], byte '\'
	je .no_pwd_path
	mov esi, pwd
	mov edi, .dirpath
	call strcopy
.no_pwd_path:
	mov edi, .dirpath
	add edi, ecx
	mov esi, commandargs
	call strcopy
	
	mov esi, .dirpath
	call formatpath
	
	mov ebx, 0
	mov ecx, .fitembuf
	mov edi, .dirpath
	mov eax, 20h
	int 50h
	
	cmp eax, 0ffffffffh
	je .not_found
	mov edi, .fitembuf
	mov bl, [edi+FILEITEM_ATTR]
	and bl, FATTR_SUBDIR
	jz .not_subdir

	mov edi, pwd
	mov ecx, 64
	mov al, 0
	cld
	rep stosb
	
	mov esi, .dirpath
	mov edi, pwd
	call strcopy
	cmp ecx, 1
	je .final_ret
	
	mov [pwd+ecx+0], byte '\'
	mov [pwd+ecx+1], byte 0

	jmp .final_ret

.not_found:
	mov edi, .notfoundstr
	mov eax, 04h
	int 50h
	jmp .showpath
	
.not_subdir:
	mov edi, .notsubdirstr
	mov eax, 04h
	int 50h
	jmp .showpath
	
.showpath:
	mov edi, .dirpath
	mov eax, 04h
	int 50h
	
.final_ret:
	mov edi, newline
	mov eax, 04h
	int 50h
	
	pop eax
	pop edi
	ret

.dirpath:	times (128) db 0
.fitembuf:	times (32) db 0
			times (16) db 0
			
.notfoundstr 	db	'Directory not found:', 0
.notsubdirstr	db	'Path is not directory:', 0
			times (16) db 0
			
BuildinCmd_pwd:
.cmd_name:
	db	'p', 'w', 'd', 0, 0, 0, 0, 0
.start:
	push edi
	push eax

	; mov edi, newline
	; mov eax, 04h
	; int 50h
	
	; mov edi, buildinstr
	; mov eax, 04h
	; int 50h
	
	; mov edi, .cmd_name
	; mov eax, 04h
	; int 50h
	
	mov edi, newline
	mov eax, 04h
	int 50h
	
	mov edi, .curpath
	mov ecx, 64
	mov al, 0
	cld
	rep stosb
	
	mov edi, .curpath
	mov esi, pwd
	call strcopy
	
	mov esi, .curpath
	call formatpath
	
	mov edi, .curpath
	mov eax, 04h
	int 50h
	
	mov edi, newline
	mov eax, 04h
	int 50h
	
	pop eax
	pop edi
	ret

.curpath:	times 64 	db 0


BuildinCmd_cls:
.cmd_name:
	db	'c', 'l', 's', 0, 0, 0, 0, 0
.start:
	push edi
	push eax

	; mov edi, newline
	; mov eax, 04h
	; int 50h
	
	; mov edi, buildinstr
	; mov eax, 04h
	; int 50h
	
	; mov edi, .cmd_name
	; mov eax, 04h
	; int 50h
	
	; mov edi, newline
	; mov eax, 04h
	; int 50h
	
	; mov edi, newline
	; mov eax, 04h
	; int 50h
	mov ebx, 00h
	mov eax, 02h
	int 50h
	
	mov ebx, 01h
	mov ecx, 00h
	mov eax, 02h
	int 50h
	
	pop eax
	pop edi
	ret

BuildinCmd_exit:
.cmd_name:
	db	'e', 'x', 'i', 't', 0, 0, 0, 0
.start:
	push edi
	push eax

	mov edi, newline
	mov eax, 04h
	int 50h
	
	mov edi, buildinstr
	mov eax, 04h
	int 50h
	
	mov edi, .cmd_name
	mov eax, 04h
	int 50h
	
	mov edi, newline
	mov eax, 04h
	int 50h
	
	pop eax
	pop edi
	ret
	
	
formatpath:	
;;in	esi:path buffer
;;
;;out	ecx:strlen after deleted '\'s
	call strlen
.loop_1:
	cmp ecx, 2
	jb .final_ret
	cmp [esi+ecx-1], byte '\'
	jne .final_ret
	dec ecx
	mov [esi+ecx], byte 0	
	jmp .loop_1
.final_ret:
	ret
	
;;\bin\..\.\.\bin\..
parsepath:
;;in	esi:path
;;		edi:dest buffer to save short path
.next_name:	
	xor edx, edx
	cld
.next_char:
	lodsb
	cmp al, '\'
	je .name_done
	mov [.curname+edx], al
	inc edx
	jmp .next_char
.name_done:
	mov [.curname+edx], byte 0
	
	
	ret

.curname: times 12 db 0
		  times 16 db 0

include "..\include\string.inc"
	
;db	'********************************************************************************'
copyright	db	'************************* PShell Ver 1.0 for Piscis OS *************************'
			db	'*************** Copyright (C) 2012 Tishion. All Rights Reserved. ***************', 0dh, 0ah, 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;debug string
newline:	db	0dh, 0ah, 0
cmdexedone:	db	'CommandLine:', 0
cmdname:	db	'CmdName:', 0
cmdargs:	db	'CmdArgs:', 0
buildinstr:	db	'Buildin Command:', 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


estr_maxcount			db	'Max running app count in system!', 0
estr_notfoundfille		db	'App not found!', 0
estr_invalidforma		db	'App format invalid!', 0


defbinpath	db	'\bin\', 0


cmdbuf:	times 128 db 0

strpromot	db	'Piscis:'
strpromot_pwd:	times 80 	db 0

pwd: 			times 64 	db 0
commandname:	times 64	db 0
commandargs:	times 64 	db 0











