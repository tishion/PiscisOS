;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	Application for PiscisOS: Helloworld
;;	Assembled by Flat Assembler
;;	
;;	23/04/2012
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DIRITEM_SIZE			equ 20h
FILEITEM_NAME 			equ 00h
FILEITEM_EXTEND			equ 08h
FILEITEM_ATTR   		equ 0bh

FATTR_RDONLY			equ 00000001b
FATTR_HIDDEN			equ	00000010b
FATTR_SYSTEM			equ	00000100b
FATTR_VOLLAB			equ	00001000b
FILEITEM_RESERVED 		equ 10h
FILEITEM_WRITETIME 		equ 16h
FILEITEM_WRITEDATE	 	equ 18h
FILEITEM_FIRSTCLUSTER	equ 1ah
FILEITEM_FILESIZE		equ 1ch
FULLFILENAME_LEN		equ 0bh
FATTR_SUBDIR			equ	00010000b
FATTR_DEVICE			equ	00100000b
FATTR_ARCHIV			equ	01000000b
FATTR_NOTUSE			equ	10000000b

use32
	org 00h
APP_HEADER:
	db	'PISCISAPP000'	;;00h	Signature
	dd	0				;;0ch	Reserverd		
	dd	APP_START		;;10h	Entry Point
	dd	APP_ARGS		;;14h	Arguments Buffer
	dd	0				;;18h	Reserved

APP_ARGS:
	times (256) db	0 
	
newline:		db	0dh, 0ah, 0	
notfoundstr 	db	'Directory not found:', 0
notsubdirstr	db	'Path is not directory:', 0
openerrstr		db	'Open directory error:', 0

APP_START:
	; mov edi, APP_ARGS
	; mov eax, 04h
	; int 50h
	
	; mov edi, newline
	; mov eax, 04h
	; int 50h
	
	mov esi, APP_ARGS
	call formatpath
	
	mov ebx, 00h
	mov edi, APP_ARGS
	mov ecx, .fitembuf
	mov eax, 20h
	int 50h
	
	cmp eax, 0ffffffffh
	je .not_found
	mov edi, .fitembuf
	mov bl, [edi+FILEITEM_ATTR]
	and bl, FATTR_SUBDIR
	jz .not_subdir
	
	mov ebx, 01h
	mov edi, APP_ARGS
	mov ecx, filebuf
	mov eax, 20h
	int 50h
	
	cmp eax, 0ffffffffh
	je .open_error
	
	mov esi, filebuf
.next_fitem:
	cmp [esi], byte 00h
	je .end
	
	cmp [esi], byte 05h
	je .skip_item
	
	cmp [esi], byte 0e5h
	je .skip_item
	
	mov [.filename+0], dword 20202020h
	mov [.filename+4], dword 20202020h
	mov [.filename+8], dword 00002020h
	
	mov eax, [esi+0]
	mov [.filename+0], eax
	mov eax, [esi+4]
	mov [.filename+4], eax
	
	mov bl, [esi+FILEITEM_ATTR]
	and bl, FATTR_SUBDIR
	jz .show_itemname
	
	push esi
	mov esi, .filename
	mov bl, 20h
	call strchr
	mov [esi+eax], byte '\'
	pop esi
	
.show_itemname:
	mov edi, .filename
	mov eax, 04h
	int 50h
.skip_item:
	add esi, DIRITEM_SIZE
	jmp .next_fitem
	
.not_found:
	mov edi, notfoundstr
	mov eax, 04h
	int 50h
	jmp .showpath
	
.not_subdir:
	mov edi, notsubdirstr
	mov eax, 04h
	int 50h
	jmp .showpath
	
.open_error:
	mov edi, openerrstr
	mov eax, 04h
	int 50h
	jmp .showpath
	
.showpath:
	mov edi, APP_ARGS
	mov eax, 04h
	int 50h
	
	
.end:	
	mov eax, 11h
	int 50h
	
	
.filename: times 12 db 20h
.fitembuf: times 32 db	0

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

include "..\include\string.inc"

;;dir data buffer 10k
filebuf:	times (1024*10+32)	db	0

	
