;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	Application for PiscisOS: Helloworld
;;	Assembled by Flat Assembler
;;	
;;	23/04/2012
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

use32

	org 00h
APP_HEADER:
	db	'PISCISAPP000'	;;Signature
	dd	0				;;Reserverd
	dd	APP_START		;;Entry Point
	dd	APP_ARGS		;;Arguments Buffer
	dd	0				;;Reserved

APP_ARGS:
	times (256) db	0 

APP_START:
	mov edi, str_nt
	mov eax, 04h
	int 50h
	
	mov eax, 05h
	int 50h

	mov edi, str_hh
	call Bcd2Asc
	
	shr eax, 8
	mov edi, str_mm
	call Bcd2Asc
	
	shr eax, 8
	mov edi, str_ss
	call Bcd2Asc
	
	mov edi, str_hh
	mov eax, 04h
	int 50h
	
	mov eax, 11h
	int 50h
	
Bcd2Asc:
;;in	al:bcd number
;;		edi:destbuffer to store string
	push ebx
	
	mov bl, al
	and bl, 0f0h
	shr bl, 4
	add bl, '0'
	mov [edi], bl
	
	mov bl, al
	and bl, 0fh
	add bl, '0'
	mov [edi+1], bl
	
	pop ebx
	ret
	
str_nt	db 'Now time ', 0
str_hh	db	'00', ':'
str_mm	db	'00', ':'
str_ss	db	'00', 0