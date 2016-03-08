;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	i8259A.inc of the PiscisOS
;;	Procs to operate the 8259 interrupt controller
;;	
;;	23/01/2012
;;	Copyright (C) tishion
;;	E-Mail:tishion@163.com
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CrtfnDisableCursor:
	push eax
	push edx
	
	mov dx, CRT_CTRL_ADDR_REG
	mov al, CRT_CTRL_DATA_CURBEGIN
	out dx, al
	
	mov al, 00100000b
	mov dx, CRT_CRTL_DATA_REG
	out dx, al
	
	pop edx
	pop eax
	
	ret
	
CrtfnEnableCursor:
	push eax
	push edx
	
	mov dx, CRT_CTRL_ADDR_REG
	mov al, CRT_CTRL_DATA_CURBEGIN
	out dx, al
	
	mov al, 0dh
	and al, 11011111b
	mov dx, CRT_CRTL_DATA_REG
	out dx, al
	
	pop edx
	pop eax
	
	ret

CrtfnSetStartRow:
;;	Set start row of visible
;;	ax=number of start row
	push eax
	push ebx
	push ecx
	push edx
	
	mov bx, COLUMN_COUNT
	mul bx
	mov cx, ax
	
	mov dx, CRT_CTRL_ADDR_REG
	mov al, CRT_CTRL_DATA_STARTADDR_H
	out dx, al
	
	mov dx, CRT_CRTL_DATA_REG
	mov al, ch
	out dx, al
	
	mov dx, CRT_CTRL_ADDR_REG
	mov al, CRT_CTRL_DATA_STARTADDR_L
	out dx, al
	
	mov dx, CRT_CRTL_DATA_REG
	mov al, cl
	out dx, al
	
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret

CrtfnSetCursorPos:
;;	Set cursor position
;;	ax=Y:row
;;	cx=X:column
	push eax
	push ebx
	push ecx
	push edx

	mov bx, 80
	mul bx
	add cx, ax
	
	mov dx, CRT_CTRL_ADDR_REG
	mov al, CRT_CTRL_DATA_CURSORPOS_H
	out dx, al
	
	mov dx, CRT_CRTL_DATA_REG
	mov al, ch
	out dx, al
	
	mov dx, CRT_CTRL_ADDR_REG
	mov al, CRT_CTRL_DATA_CURSORPOS_L
	out dx, al
	
	mov dx, CRT_CRTL_DATA_REG
	mov al, cl
	out dx, al
	
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret


	
