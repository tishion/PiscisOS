;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	Kernel of the PiscisOS
;;	The only one main source file to be assembled
;;	
;;	23/01/2012
;;	Copyright (C) tishion
;;	E-Mail:tishion@163.com
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
include ".\include\sys32.h"
include ".\include\process.h"
include ".\include\ascii.h"
include ".\include\drivers\console.h"
include ".\include\drivers\i8259A.h"
include ".\include\drivers\keyboard.h"
include ".\include\drivers\ramdiskfs.h"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Kernel base an offset
KERNEL_BASE		 			equ 1000h
KERNEL_OFFSET				equ 0000h
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Address of some CONST
MEMORY_SIZE					equ	0f000h	;address of memory size of system[4Byte]	
sys_tic						equ	0f010h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
LOW_MEMORY_SAVE_BASE		equ	0290000h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Diskette bases
DISKIMG_BASE				equ	100000h
RAW_FAT_BASE				equ	100200h
NEW_FAT_BASE				equ	280000h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Cursor
row_start					equ	08000h	;address of number of current screen row
x_cursor					equ	08002h	;address of number of current column
y_cursor					equ	08004h	;address of number of current row
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			16 BIT CODE ENTRY AFTER BOOTSECTOR
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
use16
	org 00h
	
kernel_start:
	
	jmp start_of_code16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			16 BIT DATAS
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
macro title_ver_row	
{
	times 33 db 32
	db 'Piscis OS v1.0'
	times 33 db 32
}

macro title_sep_row
{
	times 80 db 205
}

initscreen:
	title_ver_row
	title_sep_row

str_debug			db	0dh, 0ah, 'Piscis_Debug_String', 0
str_not386			db	0dh, 0ah, 'CPU is unsupported. 386+ is requried.', 0
str_loaddisk   		db	0dh, 0ah, 'Loading floppy cache: 00 %', 8, 8, 8, 8, 0
str_memmovefailed	db	0dh, 0ah, 'Fatal - Floppy image move failed.', 0
str_badsect			db	0dh, 0ah, 'Fatal - Floppy damaged.', 0
str_memsizeunknow	db	0dh, 0ah, 'Fatal - Memory size unknown.', 0
str_notenoughmem	db	0dh, 0ah, 'Fatal - Memory not enough. At least 16M', 0
str_pros			db	'00', 8, 8, 0
str_okt				db	' ... OK', 0

char_backspace	    db	8, 0
char_newline		db	0dh, 0ah, 0

mem_range_count		dw 0
mem_range_buffer	db 20 dup 0
mem_size			dd	0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			16 BIT PROCS
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PrintStr:
	push ax
	push bx
	cld
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

PrintByte:  ;use al
	push bx
	mov ah, 0eh
	push ax
	
	xor bh, bh
	shr al, 4
	add al, 30h
	cmp al, 3ah
	jb .notalpha_h
	add al, 7
.notalpha_h:
	int 10h
	
	pop ax
	xor bh, bh
	and al, 0fh
	add al, 30h
	cmp al, 3ah
	jb .notalpha_l
	add al, 7
.notalpha_l:
	int 10h
	
	pop bx
	ret
	
PrintWord:  ;use ax
	push ax
	
	shr ax, 8
	call PrintByte
	
	pop ax
	and ax, 00ffh
	call PrintByte
	
	ret
	
PrintDWord:  ;use eax
	push eax
	
	shr eax, 16
	call PrintWord
	
	pop eax
	and eax, 0000ffffh
	call PrintWord
	
	ret

DrawInitScreen:
	pusha
	mov ax, 0b800h
	mov es, ax
	mov di, 0
	mov si, initscreen
	mov cx, 80*2
	mov ah, 00000111b
.loop_0:
	cld
	lodsb
	stosw
	loop .loop_0
	
	mov cx, 80*23
	mov ah, 00000111b
	mov al, 32
.loop_1:
	stosw
	loop .loop_1
	
	mov ah, 02h
	mov bh, 0
	mov dh, 2
	mov dl, 0
	int 10h
	
	popa
	ret

	
;;entry point of the 16 bit code.	
start_of_code16:
	;;Code of 16 bit
	mov ax, KERNEL_BASE
    mov es, ax
    mov ds, ax

    mov ax, KERNEL_STACK_BASE
    mov ss, ax
    mov sp, STACK_SIZE_16BIT

    xor ax, ax
    xor bx, bx
    xor cx, cx
    xor dx, dx
    xor si, si
    xor di, di
    xor bp, bp
	
	call DrawInitScreen
	
;;Test cup 386+ ?
testcpu:
    pushf
    pop ax
    mov dx, ax
    xor ax, 4000h
    push ax
    popf
    pushf
    pop ax
    and ax, 4000h
    and dx, 4000h
    cmp ax,dx
    jnz testcpu_ok
	
    mov si, str_not386
    call PrintStr
    jmp $
testcpu_ok:

resetregs:
;;Reset the 32 bit registers and stack
	mov ax, KERNEL_BASE
    mov es, ax
    mov ds, ax

    mov ax, KERNEL_STACK_BASE	; init stack segment base regester
    mov ss, ax
    mov sp, STACK_SIZE_16BIT	; int stack pointer regester-indicate the size of stack

    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    xor ebp, ebp
resetregs_ok:

memsize:
	mov ebx, 0
	mov di, mem_range_buffer
.next_range:
	clc
	mov eax, 0e820h
	mov ecx, 20
	mov edx, 0534d4150h
	int 15h
	jc .fail
	
	inc word [mem_range_count]		; One AddressRangeMemory has been read done
	mov eax, dword [di+16]		 
	cmp eax, 1						; Type is 1 ?
	jne .is_last					; Not 1, but 2 or others, goto test is this one the last AddressRangeMemory ?
	mov eax, dword [di]				; Type is 1, BaseAddressLow
	mov esi, dword [di+8]			; LengthLow
	add eax, esi					; Add the LengthLow to the BaseAddressLow 	
	mov [mem_size], eax		; Save the memory size;
	
;;;;;;;;;;;;;;;;;;;;;;;;DEBUG_CODE;;;;;;;;;;;;;;;;;;;;;;;;
	; mov si, char_newline
	; call PrintStr
	
	; mov eax, [mem_size]
	; call PrintDWord
;;;;;;;;;;;;;;;;;;;;;;;;DEBUG_CODE;;;;;;;;;;;;;;;;;;;;;;;;
	
.is_last:
	test ebx, ebx					; Is the last AddressRangeMemory ?
	jz .memsize_ok					; Yes, get memory size done
	jne .next_range					; No, get next AddressRangeMemory
.fail:
    mov si, str_memsizeunknow
    call PrintStr
    jmp $
.memsize_ok:
	mov eax, [mem_size]
	cmp eax, 01000000h				;At least 16M 
	jae memsize_end
	mov si, str_notenoughmem
	call PrintStr
	jmp $
memsize_end:

stormemsize:
	push es
	xor ax, ax
	mov es, ax
	mov dword [es:MEMORY_SIZE], eax
	pop es
stormemsize_end:

;;Load diskette to memory. Each time read and move 18 sectors(One track or One cylinder).
loadfloppyimage:
    mov si, str_loaddisk
    call PrintStr
    mov ax, 0000h	    ; Reset drive
    mov dx, 0000h
    int 13h
    mov cx, 0001h	    ; Number of start cyl and sector
    mov dx, 0000h	    ; Number of start head and drive
    push word 80*2	    ; Count of tracks
.readmove_loop:
    pusha
    xor si, si			; Error count
.read18sectors:
    push word 0
    pop es
    mov bx, 0a000h	    ; es:bx -> data area
	mov al, 18;			; Number of sectors to read
    mov ah, 02h			; Read
    int 13h
    cmp ah, 0
    jz .read18sectorsOK
    add si, 1
    cmp si, 10
    jnz .read18sectors
    mov si, str_badsect
    call PrintStr
    jmp $
.read18sectorsOK:
    ; move -> 1mb
    mov si, .movedesc
    push word 1000h
    pop es
    mov cx, 256*18
    mov ah, 0x87
    int 15h

    cmp ah, 00h		     ; Move ok ?
    je .moveok
    mov dx, 03f2fh	     ; Floppy motor off
    mov al, 00h
    out dx, al
    mov si, str_memmovefailed
    call PrintStr
    jmp $
	
.moveok:
    mov eax, [.movedesc+18h+2]
    add eax, 512*18
    mov [.movedesc+18h+2], eax
    popa
    inc dh				; Head+1				
    cmp dh, 2			; Current head is number 1 ?
    jnz .headinc		; No, read the other track on current Cylinder
    mov dh, 0			; Yes, read next two track on next Cylinder(Cylinder+1)
    inc ch				; Cylinder+1;
    pusha		      	; Display prosentage
    push word KERNEL_BASE
    pop es
    xor eax, eax  ; 5
    mov al, ch
    shr eax, 2
    and eax, 1
    mov ebx, 5
    mul bx
    add al, 30h
    mov [str_pros+1], al
    xor eax, eax  ; 10
    mov al, ch
    shr eax, 3
    add al, 30h
    mov [str_pros], al
    mov si, str_pros
    call PrintStr
    popa
.headinc:
    pop ax
    dec ax
    push ax
    cmp ax, 0				; All read and move done?
    jnz .readmove_loop		; No, read and move next 18 sectors
	
	mov dx, 03f2h	      	; Floppy motor off
    mov al, 0
    out dx, al
	
    jmp .readmovedone		; Yes, all sectors read and move done
.movedesc:
    db 0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0	; reserved
	db 0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0	; reserved
    db 0xff,0xff							; segment length
	db 0x0,0xa0,0x00						; source address
	db 0x93									; access privalige
	db 0x0,0x0								; reserved
    db 0xff,0xff							; segment length
	db 0x00,0x00,0x10						; dest address
	db 0x93									; access privalige
	db 0x0,0x0								; reserved
    db 0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0	; reserved
    db 0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0	; reserved
    db 0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0	; reserved
    db 0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0	; reserved
.readmovedone:
    pop ax
    mov si, char_backspace
    call PrintStr

    mov si, str_okt
    call PrintStr
loadfloppyimage_end:

;;;;;;;;;;;;;;;;;;;;;;;;DEBUG_CODE;;;;;;;;;;;;;;;;;;;;;;;;
	; mov si, str_debug
	; call PrintStr
;;;;;;;;;;;;;;;;;;;;;;;;DEBUG_CODE;;;;;;;;;;;;;;;;;;;;;;;;	

	; mov si, char_newline
	; call PrintStr
	
	; mov ax, [cs:gdts-KERNEL_BASE*16]
	; call PrintWord
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			SWITCH TO PROTECTMODEL OF 32 BIT
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sel_os_code			equ	os_code_l - gdts + 0
sel_os_data			equ	os_data_l - gdts + 0
sel_os_stack		equ os_stack_l - gdts + 0
sel_video_data		equ video_data_l - gdts + 3

sel_int_stack		equ	int_stack_l - gdts +0
sel_sysc_stack		equ	sysc_stack_l - gdts + 0

sel_ring1_code		equ	ring1_code_l - gdts + 1
sel_ring1_data		equ	ring1_data_l - gdts + 1
sel_ring1_stack		equ	ring1_stack_l - gdts + 1

sel_ring2_code		equ	ring2_code_l - gdts + 2
sel_ring2_data		equ	ring2_data_l - gdts + 2
sel_ring2_stack		equ	ring2_stack_l - gdts + 2

sel_tss_int0		equ tss_interrupt0_l - gdts + 0

sel_tss_process0 	equ	tss_process0_l - gdts + 0
sel_tg_process0		equ	tg_process0_l - gdts + 0

sel_tss_sysc0		equ	tss_syscall0_l - gdts + 0 

sel_user_code		equ user_code_l	- gdts + 3
sel_user_data		equ user_data_l	- gdts + 3

;; Set CR0 register - Protect mode.	
	cli
	lgdt [cs:gdts-KERNEL_BASE*16]	; Load GDT
	
	in al, 92h				; Enable A20
	or al, 02h
	out 92h, al
	
	mov	eax, cr0
	or eax, 01h
	mov	cr0, eax

	jmp pword sel_os_code:pm32_entry
	
use32
kernel_32bit:

org (KERNEL_BASE*16 + (kernel_32bit - kernel_start))

macro align value 
{ 
	rb (value-1) - ($ + value-1) mod value 
}
boot_debug		db	'debug string.', 0dh, 0ah, 'debugstring...debugstring...debugstring...debugstring...debugstring...debugstring...debugstring...debugstring...debugstring...debugstring...debugstring...debugstring...', 0

boot_str_rcpuid				db	'Reading CPUIDs...', 0
boot_str_rpirqs				db	'Setting all IRQs...', 0

boot_str_setitss			db	'Setting interrupt TSS...', 0
boot_str_setitssdesc		db	'Setting GDT with interrupt TSS descriptors...', 0
boot_str_setitaskgate		db	'Setting IDT with interrupt task gate...', 0

boot_str_setrdfs			db	'Setting ramdisk file system...', 0

boot_str_setsysctss			db	'Setting syscall TSS...', 0
boot_str_setsysctssdesc		db	'Setting GDT with syscall TSS descriptors...', 0
boot_str_setsysctaskgate	db	'Setting IDT with syscall task gate...', 0

boot_str_setptss			db	'Setting process TSS...', 0
boot_str_setpcb				db	'Setting process control block...', 0
boot_str_setptssdesc		db	'Setting GDT with process TSS descriptors...', 0
boot_str_setptaskgate		db	'Setting GDT with process task gate...', 0
boot_str_setprocessseg		db	'Setting GDT with process segment descriptor...', 0

boot_str_settimer			db	'Setting timer(8259)...', 0
boot_str_setostask			db	'Setting system process...', 0
boot_str_enirqs				db	'Enabling all IRQs...', 0

boot_str_initshell			db	'Initializing PShell...', 0

boot_str_presenter			db	'All settings have done! Press ENTER key to start!', 0

shell_path					db	'\shell', 0

promot_row		db	0
promot_ok		db	'[  OK  ]', 0
promot_failed	db	'[FAILED]', 0

cpuid_0		dd 0, 0, 0, 0
cpuid_1		dd 0, 0, 0, 0
cpuid_2		dd 0, 0, 0, 0
cpuid_3		dd 0, 0, 0, 0

byte2Hstr:
;;	Convert byte data to hex string
;;	al:[in] byte to convert
;;	ds-edi:[in] dest buffer to save hex chars
	push edi
	push eax
	
	push eax
	shr al, 4
	add al, 30h
	cmp al, 3ah
	jb .notalpha_h
	add al, 7
.notalpha_h:
	mov [edi], al
	
	pop eax
	and al, 0fh
	add al, 30h
	cmp al, 3ah
	jb .notalpha_l
	add al, 7
.notalpha_l:
	mov [edi+1], al

	pop eax
	pop edi
	ret

word2Hstr:
	push edi
	push eax
	
	push eax
	shr ax, 8
	call byte2Hstr
	
	pop eax
	add edi, 2
	call byte2Hstr
	
	pop eax
	pop edi
	ret
	
dword2Hstr:
	push edi
	push eax
	
	push eax
	shr eax, 16
	call word2Hstr
	
	pop eax
	add edi, 4
	call word2Hstr
	
	pop eax
	pop edi
	ret

boot_promot:
	pusha
	xor eax, eax
	mov al, [promot_row]
	mov ecx, 160			
	mul ecx				; x*80*2
	mov edi, eax
	
	cld
	mov ah, 0fh			; Black background, White forground, and Hilght
.next_char:
	lodsb
	cmp al, 0
	je .exit_p
	mov [gs:edi], ax
	add edi, 2
	jmp .next_char
.exit_p:
	popa
	ret

boot_promot_status:
	pusha 
	jc	.failed
.ok:
	mov esi, promot_ok
	push 0a00h				; Black background, Green forground, and Hilght
	jmp .show_string
.failed:
	mov esi, promot_failed
	push 0c00h				; Black background, Red forground, and Hilght
.show_string:
	xor eax, eax
	mov al, [promot_row]
	mov ecx, 160			
	mul ecx				; x*80*2
	add eax, 50*2
	mov edi, eax
	
	cld
	pop eax				
.next_char:
	lodsb
	cmp al, 0
	je .exit_p
	mov [gs:edi], ax
	add edi, 2
	jmp .next_char
.exit_p:
	mov al, [promot_row]
	inc al
	mov [promot_row], al
	popa
	ret
	
read_cpuid:
	stc
	pushfd						; get current flags
	pop eax
	mov ecx, eax
	xor	eax, 00200000h			; attempt to toggle ID bit
	push eax
	popfd
	pushfd						; get new EFLAGS
	pop eax
	push ecx					; restore original flags
	popfd
	and	eax, 00200000h			; if we couldn't toggle ID,
	and	ecx, 00200000h			; then this is i486
	cmp	eax, ecx
	jz .exit_p					; It's not Pentium or later. ret
	mov edi, cpuid_0			; It's Pentium use CPUID instruction read again
	mov esi, 0
.cpuid_new_read:
	mov	eax, esi
	cpuid	
	mov	[edi+00h], eax
	mov	[edi+04h], ebx
	mov	[edi+08h], ecx
	mov	[edi+0ch], edx
	add edi, 4*4
	cmp esi, 3
	jge .exit_p
	cmp	esi, [cpuid_0]
	jge .exit_p
	inc esi
	jmp .cpuid_new_read
.exit_p:
	clc
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			32 BIT CODE
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 4

pm32_entry:	
;; Set ds, es, fs, ss, esp gs
	mov ax, sel_os_data		;;0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov ax, sel_os_stack
	mov ss, ax
	mov esp, STACK_SIZE_32BIT
	mov ax, sel_video_data
	mov gs, ax

;;Clear screen
	push es	
	mov ax, gs
	mov es, ax
	xor edi, edi
	mov ecx, 80*25*8
	mov ax, 0720h
	cld
	rep stosw
	pop es

;;Set paging tables
;;Set Pageing Directory Entrys
SetPDEs:
	xor edx, edx
	mov eax, dword [ds:MEMORY_SIZE]
	mov ebx, 400000h
	div ebx
	mov ecx, eax
	test edx, edx
	jz .no_remainder
	inc ecx
.no_remainder:
	push ecx
	mov ax, sel_os_data
	mov es, ax
	mov edi, PDE_OFFSET
	mov eax, PTE_OFFSET+7
    cld
.loop_0:
    stosd
    add eax, 4096			; One PDE has 1024 PTEs(4Byte) 	
    loop .loop_0
SetPDEs_ok:

;;Set paging table entry 4 kb paging
SetPTEs:
    mov ax, sel_os_data
    mov es, ax		
	pop eax
	mov ebx, 1024
	mul ebx
	mov ecx, eax			;Count of PTEs =1024*count of PDEs
    mov edi, PTE_OFFSET
	mov eax, 0+7			; For 0 M		
    cld
.loop_0:
    stosd
    add eax, 4096
    loop .loop_0
SetPTEs_ok:

EnablePaging:
	mov eax, PDE_OFFSET+8+16	; Page directory and enable caches
    mov cr3, eax
	
	mov eax, cr0
	or eax, 80000000h
	mov cr0, eax
	jmp short EnablePaging_ok
EnablePaging_ok:

;;Save & clear 0h-0efffh
	mov   esi, 0x0000
	mov   edi, LOW_MEMORY_SAVE_BASE
	mov   ecx, 0f000h / 4
	cld
	rep   movsd
	xor   eax, eax
	mov   edi, 0
	mov   ecx, 0f000h / 4
	cld
	rep   stosd
	
;;debugcode
	; mov di, 0
	; mov cx, 20
	; mov al, 'a'
; debug_loop:
	; mov ah, 07h
	; mov [gs:di], ax
	; inc al
	; add di, 2
	; loop debug_loop
	
	; mov di, ((80*4)+0)*2
	; mov cx, 20
	; mov al, 'A'
; debug_loop2:
	; mov ah, 0ch
	; mov [gs:di], ax
	; inc al
	; add di, 2
	; loop debug_loop2
	
;;Set Row start and cursor position
	call CrtfnDisableCursor

	mov ax, 0
	mov [row_start], ax
	call CrtfnSetStartRow
	
	mov ax, 0
	mov [y_cursor], ax
	mov cx, 0
	mov [x_cursor], cx
	call CrtfnSetCursorPos

;;Redirect all IRQs to INTs 020h~02fh	
	mov esi, boot_str_rpirqs
	call boot_promot
	call ResetIRQs
	call boot_promot_status
	
;;Set interrupts
	mov esi, boot_str_setitss
	call boot_promot
	call set_interrupt_tss
	call boot_promot_status
	
	mov esi, boot_str_setitssdesc
	call boot_promot
	call set_gdt_interrupt_tss_descriptor
	call boot_promot_status
	
	mov esi, boot_str_setitaskgate
	call boot_promot
	call set_idt_interrupt_taskgate_descriptor
	call boot_promot_status
	
	lidt [cs:idts]
	
;;Set ramdisk file system
	mov esi, boot_str_setrdfs
	call boot_promot
	mov eax, DISKIMG_BASE
	mov ebx, NEW_FAT_BASE
	call RdfsInit
	call boot_promot_status 
	
;;Set syscalls
	mov esi, boot_str_setsysctss
	call boot_promot
	call set_syscall_tss
	call boot_promot_status
	
	mov esi, boot_str_setsysctssdesc
	call boot_promot
	call set_gdt_syscall_tss_descriptor
	call boot_promot_status
	
	mov esi, boot_str_setsysctaskgate
	call boot_promot
	call set_idt_syscall_taskgate_descriptor
	call boot_promot_status
	
;;Set timer to 1/100 s
	mov esi, boot_str_settimer
	call boot_promot
	mov al, 34h		   ; Set model 2, 16 bit counter
	out 43h, al
	mov al, 9bh		   ; [msb:lsb]=[2e9bh]=[11931] lsb=9bh   
	out 40h, al
	mov al, 2eh		   ; msb=2eh
	out 40h, al
	
	clc
	call boot_promot_status

;;Read CPUID
	mov esi,boot_str_rcpuid
	call boot_promot
	call read_cpuid
	call boot_promot_status

;;Set processes
	mov esi, boot_str_setptss
	call boot_promot
	call set_process_tss
	call boot_promot_status
	
	mov esi, boot_str_setpcb
	call boot_promot
	call set_process_control_block
	call boot_promot_status
	
	mov esi, boot_str_setptssdesc
	call boot_promot
	call set_gdt_process_tss_descriptor
	call boot_promot_status
	
	mov esi, boot_str_setptaskgate
	call boot_promot
	call set_gdt_process_taskgate_descriptor
	call boot_promot_status
	
	mov esi, boot_str_setprocessseg
	call boot_promot
	call set_gdt_process_segment_descriptor
	call boot_promot_status

	
;;;;;;;;;;;;;;;;Debug;;;;;;;;;;;;;;;;;;;;;;
	;mov [400000h], byte 0h
	
	;jmp $
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
;;Set OS task
	mov esi, boot_str_setostask
	call boot_promot
	
	mov ax, sel_tss_process0	;set tss to save the temp task
	add ax, (max_processes+1)*8
	ltr ax
	
	mov [ts.eflags], dword 0x11202 ; sti and resume
	mov [ts.ss0], sel_os_stack
	mov [ts.ss1], sel_ring1_stack
	mov [ts.ss2], sel_ring2_stack
	mov [ts.esp0], RING0_ESP_0
	mov [ts.esp1], RING1_ESP_1
	mov [ts.esp2], RING2_ESP_2

	mov eax, cr3
	mov [ts.cr3], eax
	mov [ts.eip], mainosloop
	mov [ts.esp], STACK_SIZE_32BIT
	mov [ts.cs], sel_os_code
	mov [ts.ss], sel_os_stack
	mov [ts.ds], sel_os_data
	mov [ts.es], sel_os_data
	mov [ts.fs], sel_os_data
	mov [ts.gs], sel_video_data

	mov esi, tss_struct
	mov edi, 0
	imul edi, tss_unit_process_size
	add edi, tss_block_process_base
	mov ecx, 120/4
	cld
	rep movsd
	
	mov edi, 0
	imul edi, PCB_SIZE
	add edi, PCB_TABLE_BASE
	
	mov [edi+PID_OFFSET], dword 0			;;pid
	mov	[edi+PPID_OFFSET], dword 0			;;ppid
	mov [edi+MLOC_OFFSET], dword 0			;;mlocation
	mov [edi+TICK_OFFSET], dword 0			;;tickcount
	mov [edi+STAT_OFFSET], byte PS_READY	;;status
	mov [edi+PCBS_OFFSET], byte PCBS_USED	;;pcb status
	
	mov [pcb_current_base], edi
	mov [pcb_current_no], dword 0
	mov [pcb_total_count], dword 1
		
	clc
	call boot_promot_status
	
;;Set test task a
	; mov [ts.eflags], dword 0x11202 ; sti and resume
	; mov [ts.ss0], sel_os_stack
	; mov [ts.ss1], sel_ring1_stack
	; mov [ts.ss2], sel_ring2_stack
	; mov [ts.esp0], RING0_ESP_0
	; mov [ts.esp1], RING1_ESP_1
	; mov [ts.esp2], RING2_ESP_2

	; mov eax, cr3
	; mov [ts.cr3], eax

	; mov [ts.eip], 0
	; mov [ts.esp], STACK_SIZE_32BIT
	
	; mov ax, sel_user_code
	; add ax, 8
	; mov [ts.cs], ax
	
	; mov ax, sel_user_data
	; add ax, 8
	; mov [ts.ss], ax
	; mov [ts.ds], ax
	; mov [ts.es], ax
	; mov [ts.fs], ax
	; mov [ts.gs], sel_video_data

	; mov esi, tss_struct
	; mov edi, 1
	; imul edi, tss_unit_process_size
	; add edi, tss_block_process_base
	; mov ecx, 120/4
	; cld
	; rep movsd
	
	
	
	; mov ebx, 1
	; mov edi, 1
	; imul edi, PCB_SIZE
	; add edi, PCB_TABLE_BASE
	
	; mov [edi+PID_OFFSET], dword 1000		;;pid
	; mov	[edi+PPID_OFFSET], dword 0			;;ppid
	; mov eax, app_mem_size
	; imul eax, ebx
	; add eax, app_mem_base
	; mov [edi+MLOC_OFFSET], eax				;;mlocation
	; mov [edi+TICK_OFFSET], dword 0			;;tickcount
	; mov [edi+STAT_OFFSET], byte PS_READY	;;status
	; mov [edi+PCBS_OFFSET], byte PCBS_USED	;;pcb status
	
	; mov [pcb_current_base], dword PCB_TABLE_BASE
	; mov [pcb_current_no], dword 0
	; mov [pcb_total_count], dword 2
	mov esi, boot_str_initshell
	call boot_promot
	mov esi, shell_path
	mov eax, 0
	call create_process
	clc
	call boot_promot_status
	
;;Set test task b
	; mov [ts.eflags], dword 0x11202 ; sti and resume
	; mov [ts.ss0], sel_os_stack
	; mov [ts.ss1], sel_ring1_stack
	; mov [ts.ss2], sel_ring2_stack
	; mov [ts.esp0], RING0_ESP_0
	; mov [ts.esp1], RING1_ESP_1
	; mov [ts.esp2], RING2_ESP_2

	; mov eax, cr3
	; mov [ts.cr3], eax
	; mov [ts.eip], testproc_b
	; mov [ts.esp], STACK_SIZE_32BIT
	; mov [ts.cs], sel_os_code
	; mov [ts.ss], sel_os_stack
	; mov [ts.ds], sel_os_data
	; mov [ts.es], sel_os_data
	; mov [ts.fs], sel_os_data
	; mov [ts.gs], sel_video_data

	; mov esi, tss_struct
	; mov edi, 2
	; imul edi, tss_unit_process_size
	; add edi, tss_block_process_base
	; mov ecx, 120/4
	; cld
	; rep movsd
	
	; mov edi, 2
	; imul edi, PCB_SIZE
	; add edi, PCB_TABLE_BASE
	
	; mov [edi+PID_OFFSET], dword 1001		;;pid
	; mov [edi+PPID_OFFSET], dword 0			;;ppid
	; mov [edi+MLOC_OFFSET], dword 0			;;mlocation
	; mov [edi+TICK_OFFSET], dword 0			;;tickcount
	; mov [edi+STAT_OFFSET], byte PS_READY	;;status
	
	; mov [pcb_current_base], dword PCB_TABLE_BASE
	; mov [pcb_current_no], dword 0
	; mov [pcb_total_count], dword 3

;;Test function area
	; mov ax, 22
	; mov [y_cursor], ax
	; mov cx, 0
	; mov [x_cursor], cx
	; call CrtfnSetCursorPos
	
	; mov esi, .shelln
	; call rdfsFindInRootDir
	; push eax
	
	; mov esi,  shell_path
	; mov edi, 0a00000h
	; call RdfsLoadFile
	
	; mov edi, .fat_num
	; call word2Hstr
	
	; mov esi, .fatstr	
	; call Kfn_PrintString
	
	; pop eax
	; push eax
	; mov edi, 0a00000h
	; call RdfsReadFile
	
	; jmp test_ok
; .shelln		db	'SHELL   BIN', 0
; .path		db	'\readme', 0
; .dirn		db	'BIN        ', 0
; .fn			db	'EYES    RAW', 0
; .fatstr		db	'FAT=0x'
; .fat_num 	db	'0000', 0
; test_ok:
	
;;Wait for ENTER key
	add [promot_row], byte 3
	mov esi, boot_str_presenter
	call boot_promot
.waitkey:
	in al, 64h	
	test al, 01
	jz .waitkey
	in al, 60h
	cmp al, 9ch
	jne .waitkey
	
;;Clear screen
	push es	
	mov ax, gs
	mov es, ax
	xor edi, edi
	mov ecx, 80*25*8
	mov ax, 0720h
	cld
	rep stosw
	pop es	

;;Reset cursor postion
	call CrtfnEnableCursor
	
	mov ax, 0
	mov [row_start], ax
	call CrtfnSetStartRow
	
	mov ax, 0
	mov [y_cursor], ax
	mov cx, 0
	mov [x_cursor], cx
	call CrtfnSetCursorPos
	
;;Enalbe all IRQs
	call EnableAllIRQs
	call FlushAllIRQs

	sti
	jmp $
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			Main OS Loop
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;main os task loop (idle task)
mainosloop:
	call oslfn_checkbled
	jmp mainosloop
	
	jmp $
	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			32 BIT INCLUDE CODE
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
include ".\misc.inc"
include ".\sys32.inc"
include ".\process.inc"
include ".\drivers\console.inc"
include ".\drivers\i8259A.inc"
include ".\drivers\keyboard.inc"
include ".\drivers\ramdiskfs.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			KERNEL FUNCTIONS
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

oslfn_checkbled:
	cmp [kbled_stat_change], byte 0
	je .notchange
	
	call KbChangeLeds
	
	mov [kbled_stat_change], byte 0
.notchange:
	ret

	
io_delay:
	jmp .id_ret
.id_ret:
	ret

Kfn_WriteVideoBuffer:
;;	write source buffer to the vide buffer at specified offset
;;	ds:esi	= source buffer
;;	edi		= dest offset in video buffer
;;	ecx		= count of word to write 
	push esi
	push edi
	push ecx	
	
	push es
	mov ax, gs
	mov es, ax
	cld
	rep movsw
	
	pop es
	pop ecx
	pop edi
	pop esi
	
	ret
	
Kfn_WriteCharToCursor:
;;	write char to current position of cursor
;;	al	=	char
;;	ah	=	attribute
	push ebx
	push edi
	push ecx
	push edx
	
	mov ecx, eax
	cmp al, 20h
	jb	.is_cr

	mov ax, [y_cursor]
	mov bx, 80
	mul bx
	mov di, ax
	
	mov ax, [x_cursor]
	add di, ax
	mov ax, di
	inc ax
	xor edx, edx
	mov bx, 80
	div bx
	mov word [x_cursor], dx
	mov word [y_cursor], ax
	
	mov ax, [row_start]
	mov bx, 80
	mul bx
	add di, ax
	shl di, 1
	
	mov eax, ecx
	mov [gs:di], ax
	
	jmp .move_cursor
	
.is_cr:
	cmp al, ASC_CC_CR		;if al=return
	jne .is_lf
	mov word [x_cursor], 0
	jmp .move_cursor
	
.is_lf:
	cmp al, ASC_CC_LF		;if al=linefeed
	jne .is_bs
	inc word [y_cursor]
	jmp .move_cursor	
	
.is_bs:
	cmp al, ASC_CC_BS
	jne .invalidchar
	mov ax, [x_cursor]
	add ax, 79
	xor edx, edx
	mov bx, 80
	div bx
	mov word [x_cursor], dx
	dec word [y_cursor]
	add word [y_cursor], ax
	
	mov ax, [y_cursor]
	mov bx, 80
	mul bx
	mov di, ax
	
	mov ax, [x_cursor]
	add di, ax
	
	mov ax, [row_start]
	mov bx, 80
	mul bx
	add di, ax
	shl di, 1
	
	mov ax, 0720h
	mov [gs:di], ax
	jmp .move_cursor
	
.invalidchar:
	mov cl, 20h
	
	mov ax, [y_cursor]
	mov bx, 80
	mul bx
	mov di, ax
	
	mov ax, [x_cursor]
	add di, ax
	mov di, ax
	inc ax
	xor edx, edx
	mov bx, 80
	div bx
	mov word [x_cursor], dx
	add word [y_cursor], ax
	
	mov ax, [row_start]
	mov bx, 80
	mul bx
	add di, ax
	shl di, 1
	
	mov eax, ecx
	mov [gs:di], ax
	
	jmp .move_cursor

.move_cursor:	
	cmp [y_cursor], word 25
	jne .noturnpage
	cmp [x_cursor], word 0
	jne .noturnpage
	
	inc word [row_start]
	mov ax, [row_start]
	call CrtfnSetStartRow
	mov [y_cursor], word 24	
.noturnpage:
	mov ax, [row_start]
	add ax, [y_cursor]
	mov cx, [x_cursor]
	call CrtfnSetCursorPos
	
	pop edx
	pop ecx
	pop edi
	pop ebx
	ret
	
Kfn_PrintString:
;;	Print string at the cursor position
;;	ds:esi	= string buffer
	push eax
	push esi
	cld
.next_char:
	lodsb
	cmp al, 0
	je .exit_p
	mov ah, 07h
	call Kfn_WriteCharToCursor
	jmp .next_char
.exit_p:
	pop esi
	pop eax
	ret
	
delay_ms:     ; delay in 1/1000 sec
	push eax
	push ecx

	mov ecx, esi

	imul ecx, 33941
	shr ecx, 9

	in al, 61h
	and al, 10h
	mov ah, al
	cld

.loop_0:	
	in al, 61h
	and al, 10h
	cmp al, ah
	jz .loop_0

	mov ah, al
	loop .loop_0

	pop ecx
	pop eax

	ret
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;			SYSTEM CALL FUNCTIONS
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

align 4
sysc_gettick:
	cli
	mov eax, [sys_tic]
	sti
	mov [esp+36], eax
	ret
	
sysc_getkey:
	cmp byte [kasbuf_count], 0
	je .kbufempty
	
	movzx edi, byte [kasbuf_head]
	add edi, kasbuf_base
	mov cl, [edi]
	
	movzx ax, byte [kasbuf_head]
	inc ax
	mov bh, kasbuf_len
	idiv bh
	mov [kasbuf_head], ah
	
	dec byte [kasbuf_count]
	mov al, cl
	jmp .ret
.kbufempty:
	mov eax, 0ffffffffh
.ret:
	sti
	mov [esp+36], eax
	ret
	
	
sysc_screenandcursor:
	cli
	cmp eax, 0
	je .clearsc
	
	cmp eax, 1
	je .setcursor
	
	cmp eax, 2
	je .getcursor
	
.clearsc:
	push es
	push eax
	push ecx
	mov ax, gs
	mov es, ax
	xor edi, edi
	mov ecx, 80*25*8
	mov ax, 0720h
	cld
	rep stosw
	pop ecx
	pop eax
	pop es
	sti
	ret
.setcursor:	
	push ebx
	mov [y_cursor], bx
	shr ebx, 16
	mov [x_cursor], bx
	pop ebx
	call CrtfnSetCursorPos
	sti
	ret
	
.getcursor:
	xor ebx, ebx
	mov bx, [x_cursor]
	shl ebx, 16
	mov bx, [y_cursor] 
	sti
	ret

	
sysc_putchar:
	mov ah, 07h
	cli
	call Kfn_WriteCharToCursor
	sti
	ret
	
sysc_print:
	push edi
	mov edi, [pcb_current_base]
	add esi, [edi+MLOC_OFFSET]
	pop edi
	cld
.next_char:
	lodsb
	cmp al, 0
	je .print_done
	mov ah, 07h
	cli
	call Kfn_WriteCharToCursor
	sti
	jmp .next_char
.print_done:
	ret

align 4
sysc_time:
	cli
.test_status:
	mov al, 0ah
	out 70h, al
	call io_delay
	in al, 71h
	test al, 1000000b
	jnz .test_status
	
	mov al, 0	      
	out 70h, al
	in al, 71h		; seconds
	movzx ecx, al	
	shl ecx, 16
	
	mov al, 02	      
	out 70h, al
	in al, 71h		; minutes
	movzx edx, al	
	shl edx, 8
	
	add ecx, edx
	
	mov al, 04
	out 70h, al
	in al, 71h		; hours
	movzx edx, al
	
	add ecx, edx
	
	sti
	mov [esp+36], ecx	; ecx:|notuse|s|m|h|   BCD
	ret

align 4
sysc_date:
	cli
	
.test_status:
	mov al, 0ah
	out 70h, al
	call io_delay
	in al, 71h
	test al, 1000000b
	jnz .test_status
	
	mov al, 6
	out	70h, al
	call io_delay
	in	al, 71h		; day of week
	mov	ch, al
	
	mov al, 7
	out 70h, al
	call io_delay
	in al, 71h		; date
	mov	cl, al
	
	shl	ecx, 16
	
	mov	al, 8
	out	70h, al
	call io_delay
	in	al, 71h		; month
	mov	ch, al
	
	mov	al, 9
	out	70h, al
	call io_delay
	in	al, 71h		; year
	mov	cl, al
	
	sti
	mov	[esp+36], ecx	; ecx:|dw|d|m|y|
	ret

align 4
sysc_createprocess:
	cli
	mov edi, [pcb_current_base]
	add esi, [edi+MLOC_OFFSET]
	add eax, [edi+MLOC_OFFSET]
	call create_process
	sti
	mov [esp+36], eax
	ret
	
	
align 4
sysc_exitprocess:
	mov eax, [pcb_current_no]
	call terminate_process
	ret
	
align 4
sysc_waitpid:
	cli
	call wait_process_id
	sti
	mov [esp+36], eax
	ret
	

FSOP_FATTR	equ	00h
FSOP_FREAD	equ	01h
FSOP_FWRITE	equ	02h

align 4
sysc_rdfs:
	cli
	cmp eax, FSOP_FATTR
	je .fattr
	
	cmp eax, FSOP_FREAD
	je .fread
	
	cmp eax, FSOP_FWRITE
	je .fwrite
	
.fattr:
	mov edi, [pcb_current_base]	
	add ebx, [edi+MLOC_OFFSET]
	add esi, [edi+MLOC_OFFSET]
	mov edi, ebx
	call RdfsGetFileItem
	mov [esp+36], eax
	sti
	ret

.fread:
	mov edi, [pcb_current_base]	
	add ebx, [edi+MLOC_OFFSET]
	add esi, [edi+MLOC_OFFSET]
	mov edi, ebx
	call RdfsReadFile
	mov [esp+36], eax
	sti
	ret

.fwrite:
	mov [esp+36], eax
	sti
	ret



















	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	