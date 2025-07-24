;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	Boot sector of the PiscisOS
;;	December 2011
;;
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include 'bootsect.h'

use16
    org BOOTSEC_OFFSET
	; BPB_start
    jmp short Main						;jmp to Main code
    nop									;nop
	bsOEMString		db "PiscisOS"		; OEM String,8 bytes ASCII code
    bsSectSize		dw 0x0200 		    ; Bytes per sector
    bsClusterSize	db 0x01				; Sectors per cluster
    bsReservedSect	dw 0x0001			; # of reserved sectors
    bsFatCount		db 0x02				; # of fat
    bsRootDirSize	dw 0x00e0 		    ; size of root directory
    bsTotalSect		dw 0x0b40		    ; total # of sectors if < 32 meg
    bsMediaType		db 0xf0				; Media Descriptor
    bsSectPerFat	dw 0x0009			; Sectors per FAT
    bsSectPerTrack	dw 0x0012			; Sectors per track
    bsHeadCount		dw 0x0002			; number of read-write heads
    bsHiddenSect	dd 0x00000000		; number of hidden sectors
    bsHugeSect		dd 0x00000000		; if bsTotalSect is 0 this value is the number of sectors
    bsBootDrv		db 0x00				; holds drive that the bs came from
					db 0x00				; not used for anything
    bsBootSign		db 0x29 		    ; boot signature 29h
    bsVolumeID		dd 0x0214FABE		; Disk volume ID also used for temp sector # / # sectors to load
    bsVolumeLabel	db 'PiscisOSVOL'	; Volume Label
    bsFSType		db 'FAT12    ' 	  	; File System type <- FAT 12 File system
	;BPB_end

;code_start
Main:
    mov ax, cs
	mov ds, ax						;set data segment register
    mov es, ax						;set extend segment register
	mov ss, ax						;set stack segment register
	
	mov bp, BOOTSEC_OFFSET			
	mov sp, bp 						;set top-pointer of stack[stack size[501h~7bffh]]
	
;======================================================================
;read disk infomation into stack
	movzx dx, byte [bsBootDrv]
	push dx							;bootDrv
	
	mov esi, [bsHiddenSect]
	add si, [bsReservedSect]
	push si							;nStartSectOfFat
	
	xor ax, ax
	mov al, [bsFatCount]
	mul byte [bsSectPerFat]
	add si, ax
	push si							;nStartSectOfRootdir
	
	mov ax, [bsRootDirSize]
	mov bx, DIRITEM_SIZE
	mul bx
	mov bx, [bsSectSize]
	div bx
	cmp dx, 0
	je .noextra
	inc ax
.noextra:
	push ax							;nSectCountOfRootdir
	
	add si, ax
	push si							;nStartSectOfData
	
	mov ax, [bsSectSize]
	movzx bx, byte [bsClusterSize]
	mul bx
	mov [wClusterBytes], ax
;======================================================================
	mov ax, 0002h
	int 10h
	
	call FindKernelImage			;find loader image save first cluster no into [wCurClusterNo]
	jc .error
	
	mov si, boot_ing
	call PrintStr
	
	call LoadFat					;load the FAT
	jc .error
	
	call LoadKernelImage			;Load the kernel image
	jc .error
	
	mov dx, 03f2h					;Kill floppy motor
	mov al, 0
	out dx, al
	
	mov si, boot_ok
	call PrintStr
	
	;jmp KERNEL_BASE:KERNEL_OFFSET
	push word KERNEL_BASE
	push word 00h
	retf
	
.error:
	mov si, error_boot
	call PrintStr
    jmp $
Main_end:
;=========================================================
;==========================================================================
;Print a string start from the current 
;position of the beam
PrintStr:
	push ax
	push bx
.next_char:
	lodsb
	cmp al, 0
	je .exit_p
	mov ah, 0eh
	xor bh, bh
	int 10h
	jmp .next_char
.exit_p:
	pop bx
	pop ax
	ret
PrintStr_end:
;==========================================================================
;==========================================================================
;(+)[es:bx = buffer to receive the data]
;(+)[ax = number of the start logical sector]
;(+)[cl = count of the sectors to read]
;(-)[cf = 0:failed 1:success]
ERROR_COUNT equ 5
LoadnSector:
	pusha
	push bx
	push cx
	
	mov bx, [bsSectPerTrack]
	div bl
	inc ah
	mov cl, ah				;cl = sector number
	
	xor ah, ah
	mov bx, [bsHeadCount]
	div bl
	mov ch, al				;ch = cylinder number
	mov dh, ah				;dh = head number
	
	mov dl, [bsBootDrv]		;dl = driver number
	
	pop bx
	mov al, bl				;al = sector count
	
	pop bx
	mov si, ERROR_COUNT

.error_loop:	
	mov ah, 02h
	int 13h
	jnc .final_ret
	dec si
	jnz .error_loop
	stc
	
.final_ret:
	popa
	ret
LoadnSector_end:
;==========================================================================


;==========================================================================
;(-)[cf = 0:success 1:fail]
FindKernelImage:
	push es
	pusha	
	
	mov ax, TEMP_BUFFER_BASE
	mov es, ax
	
	mov dx, [nSectCountOfRootdir]
	mov ax,	[nStartSectOfRootdir]
.readrootdir_loop:
	xor bx, bx
	mov cx, 01h
	call LoadnSector
	jnc .readrootdir_ok
	jmp .final_ret 
	
.readrootdir_ok:
	xor di, di
	mov si, kernelfilename

.search_loop:
	push di
	push si
	mov cx, FULLFILENAME_LEN
	cld
	repe cmpsb
	pop si
	pop di
	jz .search_end			;ok find the image file
	
	add di, 20h
	sub byte [es:di], 0	
	jnz .search_loop		;continue searching next file item
	inc ax
	dec dx
	jnz .readrootdir_loop
	stc						;not find the image file
	jmp .final_ret
.search_end:
	clc
	mov ax, [es:di+FILEITEM_FIRSTCLUSTER]
	mov [wCurClusterNo], ax

.final_ret:
	popa
	pop es
	ret 
FindKernelImage_end:
;==========================================================================

;==========================================================================
;Loade FAT.Store the fat into [0600:0000h] = 06000h.
;==========================================================================
LoadFat:
	push es
	pusha
	
	mov ax, FAT_BASE
	mov es, ax
	xor bx, bx
	mov cx, [bsSectPerFat]
	mov ax, [nStartSectOfFat]
	call LoadnSector
	
	popa
	pop es
	ret 
LoadFat_end:
;==========================================================================

;==========================================================================
;(+)[ax = current cluster no]
;(-)[ax = next cluster no]
NextClusterNumber:
	push es
	pusha 
	
	mov ax, FAT_BASE
	mov es, ax
	
	xor dx, dx
	mov ax, [wCurClusterNo]
	mov bx, 03h
	mul bx					;dx-ax:�˻��������Ϊ������ı�����
	mov bx, 02h
	div bx					;ax:��  dx:����
	
	mov si, ax 
	mov ax, [es:si]
	test dx, dx
	jz .even
.odd:
	shr ax, 04h
.even:
	and ax, 0fffh
	
	mov [wCurClusterNo], ax

	popa
	pop es
	ret 
NextClusterNumber_end:
;==========================================================================

;==========================================================================
;Load the loader image to the [0100:0000h] = 01000h
;==========================================================================
LoadKernelImage:
	push es
	pusha	
	
	mov ax, KERNEL_BASE
	mov es, ax
	mov bx, KERNEL_OFFSET
	mov dx, bx
.readimg_loop:
	mov ax, [wCurClusterNo]
	sub ax, 02h
	add ax, [nStartSectOfData]
	mov bx, dx
	mov cl, [bsClusterSize]
	call LoadnSector
	jc .final_ret
	
	mov ah, 0eh
	mov al, '.'
	mov bl, 0fh
	int 10h
	
	call NextClusterNumber
	
	cmp [wCurClusterNo], 0ff8h
	jae .final_ret
	
	add dx, [wClusterBytes]
	jmp .readimg_loop
.final_ret:		
	popa
	pop es
	ret 
LoadKernelImage_end:
;==========================================================================
;code_end

;data_start:
wCurClusterNo	dw	00h
wClusterBytes 	dw	00h

boot_ing db 'Booting', 0

boot_ok db 0dh, 0ah, 'Starting!', 0

error_boot db 'Invalid disk!', 0

kernelfilename db 'PKERNEL BIN'

;data_end:
bootsecsig:
     times BOOTSEC_OFFSET+200h-02h-$ db 00h
     db 55h, 0aah

