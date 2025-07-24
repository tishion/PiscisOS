;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	sys32.inc of the PiscisOS
;;	
;;	
;;	23/01/2012
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; GDTs (Global Descriptor Table entries)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; GDT           STRUC         ; Segment descriptor structure
    ; LIMITL        DW      0     ; Lower 16 bits of segment limit
    ; BASEL         DW      0     ; Lower 16 bits of base address
    ; BASEM         DB      0     ; Middle 8 bits of base address
    ; LIMITH&ATTRIB DW      0     ; Attributes [G=1, D=1, 0, AVL=1] + upper 4 bits of segment limit + attributes [P=1, DPL, DT=1, TYPE]
    ; BASEH         DB      0     ; Upper 8 bits of base address
    ; GDT           ENDS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; GATE descriptor structure definition
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; GATE          STRUC          ; Gate descriptor structure type definition
    ; OFFSETL       DW      0      ; Lower 16 bits of 32-bit offset
    ; SELECTOR      DW      0      ; Segment selector
    ; DCOUNT        DB      0      ; 0000 + double-word count field
    ; GTYPE         DB      0      ; P, DPL, DT = 0, TYPE
    ; OFFSETH       DW      0      ; Upper 16 bits of 32-bit offset
    ; GATE          ENDS

;=======================================================
; stack_size_int            equ	1024
; stack_zize_sysc           equ 4096
; max_interrupts            equ 64
; max_processes             equ 250
; tss_block_interrupt_base  equ 02a0000h
; tss_block_syscall_base    equ 02a8000h
; tss_block_process_base    equ 0700000h
; tss_unit_size             equ 128
; tss_unit_process_size     equ (128+8192)
;========================================================
align 4
gdts:
        dw     gdte-gdts-1
        dd     gdts
        dw     0

os_code_l:				;;32bit kernel code segment
        dw     0xffff    ;SegmentLimit(0~15bit)
        dw     0x0000    ;SegmentBase(0~15bit)
        db     0x00      ;SegmentBase(15~32bit)
        dw     11011111b*256+10011010b;Attributes[G=1,D=1,AVL=1]+SegmentLimit=0ffh(16~19bit)+Attributes[DPL=0,Type=A E\R]
        db     0x00      ;SegmentBase(24~31bit)

os_data_l:				;;32bit kernel data segment
        dw     0xffff
        dw     0x0000
        db     0x00
        dw     11011111b*256+10010010b;SegmentAttributes [DPL=0;Type=2 R\W]
        db     0x00
		
os_stack_l:				;;32bit kernel stack segment  [50000h~6ffffh]	0x20 * 4k
        dw     0x0020
        dw     0x0000
        db     0x05
        dw     11010000b*256+10010010b;SegmentAttributes [DPL=0;Type=2 R\W]
        db     0x00

video_data_l:			;;video data segment	
        dw     0x08
        dw     0x8000
        db     0x0b
        dw     11010000b*256+11110010b;SegmentAttributes [DPL=3;Type=2 R\W]
        db     0x00
		
; int_code_l:				;;interrupt code segment
        ; dw     0xffff    ;SegmentLimit(0~15bit)
        ; dw     0x0000    ;SegmentBase(0~15bit)
        ; db     0x00      ;SegmentBase(15~32bit)
        ; dw     11011111b*256+10011010b;Attributes[G=1,D=1,AVL=1]+SegmentLimit=0ffh(16~19bit)+Attributes[DPL=0,Type=A E\R]
        ; db     0x00      ;SegmentBase(24~31bit)

; int_data_l:				;;intterupt data segment
        ; dw     0xffff
        ; dw     0x0000
        ; db     0x00
        ; dw     11011111b*256+10010010b;SegmentAttributes [DPL=0;Type=2 R\W]
        ; db     0x00
		
ring1_code_l:
        dw     0xffff    ;SegmentLimit(0~15bit)
        dw     0x0000    ;SegmentBase(0~15bit)
        db     0x00      ;SegmentBase(15~32bit)
        dw     11011111b*256+10111010b;Attributes[G=1,D=1,AVL=1]+SegmentLimit=0ffh(16~19bit)+Attributes[DPL=1,Type=A E\R]
        db     0x00      ;SegmentBase(24~31bit)

ring1_data_l:
        dw     0xffff
        dw     0x0000
        db     0x00
        dw     11011111b*256+10110010b;SegmentAttributes [DPL=1;Type=2 R\W]
        db     0x00		
		
ring1_stack_l:
        dw     0x0020
        dw     0x0000
        db     0x05
        dw     11010000b*256+10110010b;SegmentAttributes [DPL=1;Type=2 R\W]
        db     0x00
		
ring2_code_l:
        dw     0xffff    ;SegmentLimit(0~15bit)
        dw     0x0000    ;SegmentBase(0~15bit)
        db     0x00      ;SegmentBase(15~32bit)
        dw     11011111b*256+11011010b;Attributes[G=1,D=1,AVL=1]+SegmentLimit=0ffh(16~19bit)+Attributes[DPL=2,Type=A E\R]
        db     0x00      ;SegmentBase(24~31bit)

ring2_data_l:
        dw     0xffff
        dw     0x0000
        db     0x00
        dw     11011111b*256+11010010b;SegmentAttributes [DPL=2;Type=2 R\W]
        db     0x00

ring2_stack_l:
        dw     0x0020
        dw     0x0000
        db     0x05
        dw     11010000b*256+11010010b;SegmentAttributes [DPL=2;Type=2 R\W]
        db     0x00

int_stack_l:		;;interrupt stack segment  [2c0000h~2fffffh]	0x40 * 4k
        dw     0x0040
        dw     0x0000
        db     0x2c
        dw     11010000b*256+10010010b;SegmentAttributes [DPL=0;Type=2 R\W]
        db     0x00
		
sysc_stack_l:	;;interrupt stack segment  [300000h~3fffffh]	0x100 * 4k
        dw     0x0100
        dw     0x0000
        db     0x30
        dw     11010000b*256+10010010b;SegmentAttributes [DPL=0;Type=2 R\W]
        db     0x00

tss_interrupt0_l:		;Descriptors of TSSs of interrupts
		times (max_interrupts+5) dd 0, 0

tss_process0_l:			;Descriptors of TSSs of process code
		times (max_processes+5)	dd 0, 0

tg_process0_l:			;Descriptors of Task Gate of process code
		times (max_processes+5)	dd 0, 0

tss_syscall0_l:
		times (max_processes+5)	dd 0, 0

user_code_l:			;Descriptors of LDT of process code
		times (max_processes+5) dd 0, 0

user_data_l:			;Descriptors of LDT of process code
		times (max_processes+5) dd 0, 0

gdte:

		times (16)	db	0	;;Seprator
		
align 4
idts:
		dw	idte-$-1
		dd	idts+8
		dw	0
idt_l:
		times max_interrupts+5 dd 0, 0
idte:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;interrupts
set_interrupt_tss:
	mov [ts.eflags], dword 11002h
	mov [ts.ss0], sel_os_stack
	mov [ts.ss1], sel_ring1_stack
	mov [ts.ss2], sel_ring2_stack
	mov [ts.esp0], RING0_ESP_0
	mov [ts.esp1], RING1_ESP_1
	mov [ts.esp2], RING2_ESP_2

	mov eax, cr3
	mov [ts.cr3], eax
	mov [ts.cs], sel_os_code
	mov [ts.ss], sel_int_stack
	mov [ts.ds], sel_os_data
	mov [ts.es], sel_os_data
	mov [ts.fs], sel_os_data
	mov [ts.gs], sel_video_data

	mov eax, i_entry
	mov [ts.esp], stack_size_int
	
	mov edi, tss_block_interrupt_base
.next:
	push edi
	mov ebx, [eax]
	mov [ts.eip], ebx
	mov esi, tss_struct
	mov ecx, 120/4
	cld
	rep movsd
	pop edi

	add edi, tss_unit_size
	add [ts.esp], stack_size_int
	add eax, 4

	cmp eax, i_entry+4*max_interrupts
	jb .next
	
	clc
	ret
	
set_gdt_interrupt_tss_descriptor:
	mov ecx,  tss_block_interrupt_base
	mov edi, 0
.next:
	mov [tss_interrupt0_l+edi+0], word tss_unit_size
	mov [tss_interrupt0_l+edi+2], cx
	mov eax, ecx
	shr eax, 16
	mov [tss_interrupt0_l+edi+4], al
	mov [tss_interrupt0_l+edi+7], ah
	mov	[tss_interrupt0_l+edi+5], word 01010000b*256+11101001b;SegmentAttribut(DPL=3,TYPE=9 386TSS)
	add ecx, tss_unit_size
	add edi, 8
	cmp edi, 8*max_interrupts
	jb .next
	
	clc
	ret
	
set_idt_interrupt_taskgate_descriptor:
	mov edi, 0
	mov dx, sel_tss_int0
.next:
	mov [idt_l+edi+0], word 0
	mov [idt_l+edi+2], dx
	mov [idt_l+edi+4], word 10000101b*256			;SegmentAttribut(DPL=0,TYPE=9 task gate)
	mov [idt_l+edi+6], word 0
	add dx, 8
	add edi, 8

	cmp edi, 8*max_interrupts
	jb .next
	
	clc
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;syscalls
set_syscall_tss:
	mov [ts.eflags], dword 11002h
	mov [ts.ss0], sel_os_stack
	mov [ts.ss1], sel_ring1_stack
	mov [ts.ss2], sel_ring2_stack
	mov [ts.esp0], RING0_ESP_0
	mov [ts.esp1], RING1_ESP_1
	mov [ts.esp2], RING2_ESP_2

	mov eax, cr3
	mov [ts.cr3], eax
	mov [ts.cs], sel_os_code
	mov [ts.ss], sel_sysc_stack
	mov [ts.ds], sel_os_data
	mov [ts.es], sel_os_data
	mov [ts.fs], sel_os_data
	mov [ts.gs], sel_video_data
	mov [ts.esp], stack_size_sysc
	
	mov edi, tss_block_syscall_base
.next:
	push edi
	mov ebx, ih50_syscall
	mov [ts.eip], ebx
	mov esi, tss_struct
	mov ecx, 120/4
	cld
	rep movsd
	pop edi
	
	add [ts.esp], stack_size_sysc
	add edi, tss_unit_size

	cmp edi, tss_block_syscall_base+8*(max_processes+3)
	jb .next
	
	clc
	ret
	
set_gdt_syscall_tss_descriptor:
	mov ecx,  tss_block_syscall_base
	mov edi, 0
.next:
	mov [tss_syscall0_l+edi+0], word tss_unit_size
	mov [tss_syscall0_l+edi+2], cx
	mov eax, ecx
	shr eax, 16
	mov [tss_syscall0_l+edi+4], al
	mov [tss_syscall0_l+edi+7], ah
	mov	[tss_syscall0_l+edi+5], word 01010000b*256+11101001b;SegmentAttribut(DPL=3,TYPE=9 386TSS)
	add ecx, tss_unit_size
	add edi, 8
	cmp edi, 8*max_interrupts
	jb .next
	
	clc
	ret
	
set_idt_syscall_taskgate_descriptor:
	mov edi, 8*50h
	mov dx, sel_tss_sysc0

	mov [idt_l+edi+0], word 0
	mov [idt_l+edi+2], dx
	mov [idt_l+edi+4], word 11100101b*256			;SegmentAttribut(DPL=0,TYPE=9 task gate)
	mov [idt_l+edi+6], word 0
	
	clc
	ret


			times (16)	db	0	;;Seprator
			
align 4
tss_struct:
	ts.back		dw 0,0d
	ts.esp0		dd 0
	ts.ss0		dw 0,0
	ts.esp1		dd 0
	ts.ss1		dw 0,0
	ts.esp2		dd 0
	ts.ss2		dw 0,0
	ts.cr3		dd 0
	ts.eip		dd 0
	ts.eflags	dd 0
	ts.eax		dd 0
	ts.ecx		dd 0
	ts.edx		dd 0
	ts.ebx		dd 0
	ts.esp		dd 0
	ts.ebp		dd 0
	ts.esi		dd 0
	ts.edi		dd 0
	ts.es		dw 0,0
	ts.cs		dw 0,0
	ts.ss		dw 0,0
	ts.ds		dw 0,0
	ts.fs		dw 0,0
	ts.gs		dw 0,0
	ts.ldt		dw 0,0
	ts.trap		dw 0
	ts.io		dw 0

		times (16)	db	0	;;Seprator

align 4
i_entry:
;;	Intel defined intterupts(00h~1fh)
	dd gi00, gi01, gi02, gi03, gi04, gi05, gi06, gi07
	dd gi08, gi09, gi0a, gi0b, gi0c, gi0d, gi0e, gi0f
	dd gi10, gi11, gi12, gi13, gi14, gi15, gi16, gi17
	dd gi18, gi19, gi1a, gi1b, gi1c, gi1d, gi1e, gi1f
;;	Irqs(20h~2fh)
	dd irq00, irq01, irq02, irq03, irq04, irq05, irq06, irq07
	dd irq08, irq09, irq0a, irq0b, irq0c, irq0d, irq0e, irq0f
;;	custom interrupts(30h~0ffh)	
	dd ih30, ih31, ih32, ih33, ih34, ih35, ih36, ih37
	dd ih38, ih39, ih3a, ih3b, ih3c, ih3d, ih3e, ih3f
	dd ih40, ih41, ih42, ih43, ih44, ih45, ih46, ih47
	dd ih48, ih49, ih4a, ih4b, ih4c, ih4d, ih4e, ih4f
	dd ih50, ih51, ih52, ih53, ih54, ih55, ih56, ih57
	dd ih58, ih59, ih5a, ih5b, ih5c, ih5d, ih5e, ih5f
	dd ih60, ih61, ih62, ih63, ih64, ih65, ih66, ih67
	dd ih68, ih69, ih6a, ih6b, ih6c, ih6d, ih6e, ih6f
	dd ih70, ih71, ih72, ih73, ih74, ih75, ih76, ih77
	dd ih78, ih79, ih7a, ih7b, ih7c, ih7d, ih7e, ih7f
	dd ih80, ih81, ih82, ih83, ih84, ih85, ih86, ih87
	dd ih88, ih89, ih8a, ih8b, ih8c, ih8d, ih8e, ih8f
	dd ih90, ih91, ih92, ih93, ih94, ih95, ih96, ih97
	dd ih98, ih99, ih9a, ih9b, ih9c, ih9d, ih9e, ih9f
	dd iha0, iha1, iha2, iha3, iha4, iha5, iha6, iha7
	dd iha8, iha9, ihaa, ihab, ihac, ihad, ihae, ihaf
	dd ihb0, ihb1, ihb2, ihb3, ihb4, ihb5, ihb6, ihb7
	dd ihb8, ihb9, ihba, ihbb, ihbc, ihbd, ihbe, ihbf
	dd ihc0, ihc1, ihc2, ihc3, ihc4, ihc5, ihc6, ihc7
	dd ihc8, ihc9, ihca, ihcb, ihcc, ihcd, ihce, ihcf
	dd ihd0, ihd1, ihd2, ihd3, ihd4, ihd5, ihd6, ihd7
	dd ihd8, ihd9, ihda, ihdb, ihdc, ihdd, ihde, ihdf
	dd ihe0, ihe1, ihe2, ihe3, ihe4, ihe5, ihe6, ihe7
	dd ihe8, ihe9, ihea, iheb, ihec, ihed, ihee, ihef

		times (16)	db	0	;;Seprator
		
align 4
syscall_table:
	dd sysc_gettick			;;0
	dd sysc_getkey			;;1
	dd sysc_screenandcursor	;;2
	dd sysc_putchar			;;3
	dd sysc_print			;;4
	dd sysc_time			;;5
	dd sysc_date			;;6
	dd 0					;;7
	dd 0					;;8
	dd 0					;;9
	dd 0					;;a
	dd 0					;;b
	dd 0					;;c
	dd 0					;;d
	dd 0					;;e
	dd 0					;;f
	dd sysc_createprocess	;;10
	dd sysc_exitprocess		;;11
	dd sysc_waitpid			;;12
	dd 0					;;13
	dd 0					;;14
	dd 0					;;15
	dd 0					;;16
	dd 0					;;17
	dd 0					;;18
	dd 0					;;19
	dd 0					;;1a
	dd 0					;;1b
	dd 0					;;1c
	dd 0					;;1d
	dd 0					;;1e
	dd 0					;;1f
	dd sysc_rdfs			;;20
	dd 0					;;21
	dd 0					;;22
	dd 0					;;23
	dd 0					;;24
	dd 0					;;25
		times (16)	db	0	;;Seprator

e_db_int	db	-1
e_dd_err	dd	-1
e_dd_pid	dd	-1
e_dd_eip	dd	-1


e_dd_ientry	dd	-1
	
exception_message:
			db	0dh, 0ah
			db	'Kernel-Exception: INT:0x'
emsg.int	db	'00' 			
			db	' ERR:0x'
emsg.err	db	'00000000'
			db	' PID:0x'
emsg.pid	db	'00000000'
			db	' EIP:0x'
emsg.eip	db	'00000000'
			db	0
			
show_exception_message:
	push edi
	push eax
	
	mov al, [e_db_int]
	mov edi, emsg.int
	call byte2HexStr
	
	mov eax, [e_dd_err]
	mov edi, emsg.err
	call dword2HexStr
	
	mov edi, [pcb_current_base]
	mov eax, [edi+00h]
	mov edi, emsg.pid
	call dword2HexStr
	
	mov edi, [pcb_current_no]
	imul edi, tss_unit_process_size
	mov eax, [edi+tss_block_process_base+ts.eip-tss_struct]
	mov edi, emsg.eip
	call dword2HexStr
	
	mov esi, exception_message
	call Kfn_PrintString
	
	pop eax
	pop edi
	ret
	
gihandler:
	mov edx, [pcb_current_base]
	mov [edx+STAT_OFFSET], byte PS_ZOMBIE
	
	xor edx, edx
	mov dl, [e_db_int]
	imul edx, 8
	mov [tss_interrupt0_l+edx+05h], word 01010000b*256+11101001b
	
	
	xor edx, edx
	mov dl, [e_db_int]
	imul edx, tss_unit_size
	add edx, tss_block_interrupt_base
	mov esi, [e_dd_ientry]
	mov [edx+ts.eip-tss_struct], esi
	mov [edi+ts.eflags-tss_struct], dword 11002h
	
	jmp change_task
	
gi00:
	mov byte [e_db_int], 00h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi00
	
	call show_exception_message
	
	jmp gihandler
	
gi01:
	mov byte [e_db_int], 01h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi01
	
	call show_exception_message
	
	jmp gihandler
	
gi02:
	mov byte [e_db_int], 02h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi02
	
	call show_exception_message
	
	jmp gihandler
	
gi03:
	mov byte [e_db_int], 03h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi03
	
	call show_exception_message
	
	jmp gihandler
	
gi04:
	mov byte [e_db_int], 04h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi04
	
	call show_exception_message
	
	jmp gihandler
	
gi05:
	mov byte [e_db_int], 05h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi05
	
	call show_exception_message
	
	jmp gihandler
	
gi06:
	mov byte [e_db_int], 06h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi06
	
	call show_exception_message
	
	jmp gihandler
	
gi07:
	mov byte [e_db_int], 07h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi07
	
	call show_exception_message
	
	jmp gihandler
	
gi08:
	mov byte [e_db_int], 08h
	pop eax
	mov [e_dd_err], eax
	mov dword [e_dd_ientry], gi08
	
	call show_exception_message
	
	jmp gihandler
	
gi09:
	mov byte [e_db_int], 09h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi09
	
	call show_exception_message
	
	jmp gihandler
	
gi0a:
	mov byte [e_db_int], 0ah
	pop eax
	mov [e_dd_err], eax
	mov dword [e_dd_ientry], gi0a
	
	call show_exception_message
	
	jmp gihandler
	
gi0b:
	mov byte [e_db_int], 0bh
	pop eax
	mov [e_dd_err], eax
	mov dword [e_dd_ientry], gi0b
	
	call show_exception_message
	
	jmp gihandler
	
gi0c:
	mov byte [e_db_int], 0ch
	pop eax
	mov [e_dd_err], eax
	mov dword [e_dd_ientry], gi0c
	
	call show_exception_message
	
	jmp gihandler
	
gi0d:
	mov byte [e_db_int], 0dh
	pop eax
	mov [e_dd_err], eax
	mov dword [e_dd_ientry], gi0d
	
	call show_exception_message
	
	jmp gihandler
	
gi0e:
	mov byte [e_db_int], 0eh
	pop eax
	mov [e_dd_err], eax
	mov dword [e_dd_ientry], gi0e
	pop eax
	mov [e_dd_pid], eax
	call show_exception_message
	
	jmp gihandler
	
gi0f:
	mov byte [e_db_int], 0fh
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi0f
	
	call show_exception_message
	
	jmp gihandler
	
gi10:
	mov byte [e_db_int], 10h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi10
	
	call show_exception_message
	
	jmp gihandler
	
gi11:
	mov byte [e_db_int], 11h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi11
	
	call show_exception_message
	
	jmp gihandler
	
gi12:
	mov byte [e_db_int], 12h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi12
	
	call show_exception_message
	
	jmp gihandler
	
gi13:
	mov byte [e_db_int], 13h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi13
	
	call show_exception_message
	
	jmp gihandler
	
gi14:
	mov byte [e_db_int], 14h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi14
	
	call show_exception_message
	
	jmp gihandler
	
gi15:
	mov byte [e_db_int], 15h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi15
	
	call show_exception_message
	
	jmp gihandler
	
gi16:
	mov byte [e_db_int], 16h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi16
	
	call show_exception_message
	
	jmp gihandler
	
gi17:
	mov byte [e_db_int], 17h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi17
	
	call show_exception_message
	
	jmp gihandler
	
gi18:
	mov byte [e_db_int], 18h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi18
	
	call show_exception_message
	
	jmp gihandler
	
gi19:
	mov byte [e_db_int], 19h
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi19
	
	call show_exception_message
	
	jmp gihandler
	
gi1a:
	mov byte [e_db_int], 1ah
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi1a
	
	call show_exception_message
	
	jmp gihandler
	
gi1b:
	mov byte [e_db_int], 1bh
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi1b
	
	call show_exception_message
	
	jmp gihandler
	
gi1c:
	mov byte [e_db_int], 1ch
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi1c
	
	call show_exception_message
	
	jmp gihandler
	
gi1d:
	mov byte [e_db_int], 1dh
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi1d
	
	call show_exception_message
	
	jmp gihandler
	
gi1e:
	mov byte [e_db_int], 1eh
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi1e
	
	call show_exception_message
	
	jmp gihandler
	
gi1f:
	mov byte [e_db_int], 1fh
	mov [e_dd_err], -1
	mov dword [e_dd_ientry], gi1f
	
	call show_exception_message
	
	jmp gihandler
	
send_master_eoi:
	mov al, 20h
	mov dx, 20h
	out dx, al
	ret
	
send_slave_eoi:
	mov al, 20h
	mov dx, 0a0h
	out dx, al
	ret
	
	
reset_current_task:
	mov edi, [pcb_current_no]
    imul edi, 8
    mov [tss_process0_l+edi+5], word 01010000b*256+11101001b
	
	ret

return_current_task:
	mov ebx, [pcb_current_no]
	shl bx, 3
	add bx, sel_tg_process0
	mov [.jmp_sel], bx

.jmp_opcode	db 0eah
.jmp_offset	dd 0
.jmp_sel	dw sel_tg_process0
	
	ret
	
irq00:
	; mov esi, [sys_tic]
	; shl esi, 1
	; add esi, 80*20
	; shl esi, 1
	; mov al, '+'
	; mov ah, 0ch
	; mov [gs:esi], ax
	
	inc dword [sys_tic]	;; system total tickcount 
	
	;get current process info
	mov edi, [pcb_current_no]
	imul edi, 8
	mov [tss_process0_l+edi+5], word 01010000b*256+11101001b	;corrected (: restet the Descriptor of B flag

	; mov edi, [pcb_current_base]
	; cmp [edi+STAT_OFFSET], byte PS_TERMINATED
	; je .nextproc
	; cmp [edi+STAT_OFFSET], byte PS_ZOMBIE
	; je .nextproc
	; cmp [edi+STAT_OFFSET], byte PS_WAIT
	; je .nextproc
	mov edi, [pcb_current_base]
	cmp [edi+STAT_OFFSET], byte PS_RUNNING
	jne .nextproc
	
	mov [edi+STAT_OFFSET], byte PS_READY		 	;set process status RUNNING

	;set next process to run
.nextproc:
	mov eax, [pcb_current_no]
	inc eax
	mov bx, max_processes
	xor edx, edx
	div bx
	mov eax, edx								;;next pcb number
	mov [pcb_current_no], eax
	
	mov bx, PCB_SIZE
	xor edx, edx
	mul bx
	mov edi, eax				
	add edi, PCB_TABLE_BASE		
	mov [pcb_current_base], edi					;;next pcb base
	
	cmp [edi+PCBS_OFFSET], byte PCBS_FREE
	je .nextproc
	
	;IF NOT THEN JMP .nextproc
	; cmp [edi+STAT_OFFSET], byte PS_TERMINATED		;terminated process
	; je .nextproc
	; cmp [edi+STAT_OFFSET], byte PS_ZOMBIE			;zombie process
	; je .nextproc
	; cmp [edi+STAT_OFFSET], byte PS_WAIT				;Sleep or Wait
	; je .nextproc
	mov edi, [pcb_current_base]
	cmp [edi+STAT_OFFSET], byte PS_READY
	jne .nextproc
	
	mov [edi+STAT_OFFSET], byte PS_RUNNING		 	;set process status RUNNING
	
	mov eax, [pcb_current_no]
	shl ax, 3
	add ax, tss_process0_l-gdts+0			;tss_process0_l+no*8-gdts+0
	mov [.jmp_sel], ax

	; mov esi, [sys_tic]
	; shl esi, 1
	; sub esi, 1
	; add esi, 80*20
	; shl esi, 1
	; mov al, '-'
	; mov ah, 0ch
	; mov [gs:esi], ax
	
	call send_master_eoi
	
.jmp_opcode		db 0eah
.jmp_offset		dd 0
.jmp_sel		dw sel_tg_process0
	
	jmp irq00
	
	
change_task:
	dec dword [sys_tic]
	int 20h
	ret
	
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;keyboard interrupt handler
irq01:
	call reset_current_task
	
	xor eax, eax
	mov dx, 60h
	in al, dx
	
	mov [.k_scode], al
	
	cmp al, SC_EX
	jne .noex
	
.noex:

	cmp al, SC_CAPS
	jne .nocapsdown
	mov [kbled_stat_change], byte 1 
	mov [kbled_num_change], byte 0
	jmp .nochar
.nocapsdown:

	cmp al, SC_NUM
	jne .nosnumdown
	mov [kbled_stat_change], byte 1 
	mov [kbled_num_change], byte 2
	jmp .nochar
.nosnumdown:

	cmp al, SC_SCROLL
	jne .noscrolldown
	mov [kbled_stat_change], byte 1 
	mov [kbled_num_change], byte 3
	jmp .nochar
.noscrolldown:
	

	cmp al, SC_CTRL		; CTRL down
	jne .noctrldown
	mov [.k_ctrl], byte 1
	jmp .nochar
.noctrldown:
	
	cmp al, SC_CTRL+80h		;CTRL up
	jne .noctrlup
	mov [.k_ctrl], byte 0
	jmp .nochar
.noctrlup:

	cmp al, SC_ALT			;ALT down
	jne .noaltdown
	mov [.k_alt], byte 1
	jmp .nochar
.noaltdown:

	cmp al, SC_ALT+80h			;ALT up
	jne .noaltup
	mov [.k_alt], byte 0
	jmp .nochar
.noaltup:

	cmp al, SC_SHIFT_L			;SHIFT-L down
	jne .noshiftldown
	mov [.k_shift], byte 1
	jmp .nochar
.noshiftldown:
	cmp al, SC_SHIFT_R			;SHIFT-R down
	jne .noshiftrdown
	mov [.k_shift], byte 1
	jmp .nochar
.noshiftrdown:

	cmp al, SC_SHIFT_L+80h			;SHIFT-L up
	jne .noshiftlup
	mov [.k_shift], byte 0
	jmp .nochar
.noshiftlup:
	cmp al, SC_SHIFT_R+80h			;SHIFT-R up
	jne .noshiftrup
	mov [.k_shift], byte 0
	jmp .nochar
.noshiftrup:

	cmp al, byte 80h
	jb	.ischar
	jmp .nochar
	
.ischar:
	mov ecx, eax
	add ecx, key_map
	mov bl, [ecx]
	
	cmp [.k_shift], 1
	jne	.noshift
	mov ecx, eax
	add ecx, shift_key_map
	mov bl, [ecx]
.noshift:
	
	cmp bl, 0
	je .nochar
	
	; mov al, bl
	; mov ah, 0ch
	; movzx esi, byte [kasbuf_count]
	; imul esi, 2
	; add esi, 80*13*2
	; mov [gs:esi], ax
	
	cmp [kasbuf_count], byte kasbuf_len-1
	jae .kbuffull
	
	movzx edi, byte [kasbuf_tail]
	add edi, kasbuf_base
	mov [edi], bl
	
	movzx ax, byte [kasbuf_tail]
	inc ax
	mov bh, kasbuf_len
	idiv bh
	mov [kasbuf_tail], ah
	
	inc byte [kasbuf_count]
	
.nochar:
.kbuffull:
	call send_master_eoi
	
	mov ebx, [pcb_current_no]
	shl bx, 3
	add bx, sel_tg_process0
	mov [.jmp_sel], bx

.jmp_opcode	db 0eah
.jmp_offset	dd 0
.jmp_sel	dw sel_tg_process0

	jmp irq01

.k_scode	db	0
.k_shift	db	0
.k_ctrl		db	0
.k_alt		db	0
	
	
ih50_syscall:
	cli
	
	mov edi, [pcb_current_no]				;;reset current process tss descriptor
    imul edi, 8
    mov [tss_process0_l+edi+5], word 01010000b*256+11101001b

	mov eax, [cur_sysctssdesc]				;;set current syscall tss descriptor USEING
	mov [sysctssdesc_status+eax], byte 1
	push eax								;;save the number of current syscall tss descriptor

	mov edi, [pcb_current_base]
	mov [edi+SYSC_OFFSET], eax				;; set syscall flag
	
	mov eax, 0								;;find a free syscall tss descriptor 
.find_next_sysctssdesc:
	cmp [sysctssdesc_status+eax], byte 0
	je .find_ok
	inc eax
	cmp eax, max_processes+3
	jb .find_next_sysctssdesc
	jmp $									;;not find ! FATAL ERROR !
.find_ok:
	mov [cur_sysctssdesc], eax				;;set for next INT 50h
	
	mov edx, [cur_sysctssdesc]				;;set INT 50h point the syscall tss descriptor found
	imul edx, 8
	add edx, sel_tss_sysc0
	
	mov edi, 8*50h
	mov [idt_l+edi+0], word 0
	mov [idt_l+edi+2], dx
	mov [idt_l+edi+4], word 11100101b*256			;SegmentAttribut(DPL=0,TYPE=9 task gate)
	mov [idt_l+edi+6], word 0
	
	mov ebx, [pcb_current_no]
	;;F_ck! How can I make such a foolish mistake ???? 						
	;imul ecx, 8						
	imul ebx, 8
	add ebx, tss_process0_l
	
	mov ecx, [pcb_current_no]
	imul ecx, 8
	mov eax, [ebx]
	mov [proctssdesc_copy+ecx], eax					;;save the current process tss descriptor
	mov eax, [ebx+4]
	mov [proctssdesc_copy+ecx+4], eax
	
	mov ecx, [esp]
	imul ecx, 8
	mov eax, [tss_syscall0_l+ecx]					;;replace the current process tss descriptor 
	mov [ebx], eax									;;with current syscall tss descriptor
	mov eax, [tss_syscall0_l+ecx+4]
	mov [ebx+4], eax
	
	mov esi, [pcb_current_no]						;;save eax, ebx, ecx
	imul esi, tss_unit_process_size					;;for they are parameters
	add esi, tss_block_process_base					;;
													;;
	mov eax, [esi+ts.eax-tss_struct]				;;

    mov ebx, [esi+ts.ebx-tss_struct]				;;
    mov ecx, [esi+ts.ecx-tss_struct]				;;
	
	pusha											;;push all parameters into stack
													;;
	mov edi, [esi+ts.eax-tss_struct]				;;eax->edi	
    mov eax, [esi+ts.ebx-tss_struct]				;;ebx->eax
    mov ebx, [esi+ts.ecx-tss_struct]				;;ecx->ebx
    mov ecx, [esi+ts.edx-tss_struct]				;;edx->ecx
    mov edx, [esi+ts.esi-tss_struct]				;;esi->edx
    mov esi, [esi+ts.edi-tss_struct]				;;edi->esi
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;
	sti												;;syscall task set ok, enable interrupt
	push eax										;;for eax will be used in any syscall so save it  
	and edi, 0ffh									;;make sure the number of syscall max 255
	call dword [syscall_table+edi*4]				;;call syscall proc
	pop eax											;;restort eax
	cli												;;syscall task end, disable interrupt and restore process task
	;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	popa											;;restore all parameters
	
	mov esi, [pcb_current_no]						;;return syscall result
	imul esi, tss_unit_process_size
	add esi, tss_block_process_base
	
	mov [esi+ts.eax-tss_struct], eax
    mov [esi+ts.ebx-tss_struct], ebx
    mov [esi+ts.ecx-tss_struct], ecx
	
	mov ebx, [pcb_current_no]						;;restore task tss descriptor
	imul ebx, 8
	add ebx, tss_process0_l
	
	mov ecx, [pcb_current_no]
	imul ecx, 8
	mov eax, [proctssdesc_copy+ecx]
	mov [ebx], eax
	;;F_ck! How can I make such a foolish mistake ???? 
	;mov ecx, [.proctssdesc_copy+ecx+4]				
	mov eax, [proctssdesc_copy+ecx+4]	
	mov [ebx+4], eax
	
	pop eax											;;current number of syscall tss descriptor
	mov [sysctssdesc_status+eax], byte 0			;;set free
	
	mov edi, eax									;;restore current syscall tss descriptor
	imul edi, 8
	mov [tss_syscall0_l+edi+5], word 01010000b *256 +11101001b
	
	mov edi, [pcb_current_base]						;;clear syscall flag
	mov [edi+SYSC_OFFSET], dword 0

	mov ebx, [pcb_current_no]						;;jmp current task
	shl bx, 3
	add bx, sel_tg_process0
	mov [.jmp_sel], bx

.jmp_opcode	db 0eah
.jmp_offset	dd 0
.jmp_sel	dw sel_tg_process0

	jmp ih50_syscall

cur_sysctssdesc	dd	0, 0
sysctssdesc_status	:	times (max_processes+3+5)	db	0
proctssdesc_copy	:	times (max_processes+3+5)	dd	0, 0


complet_syscall:
;;in	eax:syscall descriptor number
	push eax
	push ebx
	push ecx
	push edi
	
	cmp eax, 0ffffffffh
	je .final_ret
	
	mov [sysctssdesc_status+eax], byte 0

	mov edi, eax									;;restore current syscall tss descriptor
	imul edi, 8
	mov [tss_syscall0_l+edi+5], word 01010000b*256+11101001b

    mov edi, eax
    imul edi, tss_unit_size
	mov [tss_block_syscall_base+edi+ts.eflags-tss_struct], dword 0x11002
    mov [tss_block_syscall_base+edi+ts.eip-tss_struct], dword ih50_syscall
   
    mov ebx, eax
    imul ebx, stack_size_sysc
    add ebx, stack_size_sysc
    mov [tss_block_syscall_base+edi+ts.esp-tss_struct], ebx
	
	mov ebx, [pcb_current_no]						;;restore task tss descriptor
	imul ebx, 8
	add ebx, tss_process0_l
	
	mov ecx, [pcb_current_no]
	imul ecx, 8
	mov eax, [proctssdesc_copy+ecx]
	mov [ebx], eax		
	mov eax, [proctssdesc_copy+ecx+4]	
	mov [ebx+4], eax

.final_ret:
	pop edi
	pop ecx
	pop ebx
	pop eax

	ret
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Unused irq handler
irq_handler:
	push edi
	call send_master_eoi
	
	pop edi
	cmp edi, 07h
	jbe .irq_done
	call send_slave_eoi
.irq_done:
	ret
	
irq02:
	call reset_current_task
	
	mov edi, 02h
	call irq_handler
	
	call return_current_task
	
	jmp irq02
	
irq03:
	call reset_current_task
	
	mov edi, 03h
	call irq_handler
	
	call return_current_task
	
	jmp irq03
	
irq04:
	call reset_current_task
	
	mov edi, 04h
	call irq_handler
	
	call return_current_task
	
	jmp irq04
	
irq05:
	call reset_current_task
	
	mov edi, 05h
	call irq_handler
	
	call return_current_task
	
	jmp irq05
	
irq06:
	call reset_current_task
	
	mov edi, 06h
	call irq_handler
	
	call return_current_task
	
	jmp irq06
	
			
irq07:
	call reset_current_task
	
	mov edi, 07h
	call irq_handler
	
	call return_current_task
	
	jmp irq07
	
irq08:
	call reset_current_task
	
	mov edi, 08h
	call irq_handler
	
	call return_current_task
	
	jmp irq08
	
irq09:
	call reset_current_task
	
	mov edi, 09h
	call irq_handler
	
	call return_current_task
	
	jmp irq09
	
irq0a:
	call reset_current_task
	
	mov edi, 0ah
	call irq_handler
	
	call return_current_task
	
	jmp irq0a
	
irq0b:
	call reset_current_task
	
	mov edi, 0bh
	call irq_handler
	
	call return_current_task
	
	jmp irq0b
	
irq0c:
	call reset_current_task
	
	mov edi, 0ch
	call irq_handler
	
	call return_current_task
	
	jmp irq0c
	
irq0d:
	call reset_current_task
	
	mov edi, 0dh
	call irq_handler
	
	call return_current_task
	
	jmp irq0d
	
irq0e:
	call reset_current_task
	
	mov edi, 0eh
	call irq_handler
	
	call return_current_task
	
	jmp irq0e
	
irq0f:
	call reset_current_task
	
	mov edi, 0fh
	call irq_handler
	
	call return_current_task
	
	jmp irq0f

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Unused interrupt handler
undefine_int_handler:
	cli
	pop eax
	sti
	jmp $

ih30:
	push 30h
	jmp undefine_int_handler
ih31:
	push 31h
	jmp undefine_int_handler
ih32:
	push 32h
	jmp undefine_int_handler
ih33:
	push 33h
	jmp undefine_int_handler
ih34:
	push 34h
	jmp undefine_int_handler
ih35:
	push 35h
	jmp undefine_int_handler
ih36:
	push 36h
	jmp undefine_int_handler
ih37:
	push 37h
	jmp undefine_int_handler
ih38:
	push 38h
	jmp undefine_int_handler
ih39:
	push 39h
	jmp undefine_int_handler
ih3a:
	push 3ah
	jmp undefine_int_handler
ih3b:
	push 3bh
	jmp undefine_int_handler
ih3c:
	push 3ch
	jmp undefine_int_handler
ih3d:
	push 3dh
	jmp undefine_int_handler
ih3e:
	push 3eh
	jmp undefine_int_handler
ih3f:
	push 3fh
	jmp undefine_int_handler

ih40:
ih41:
ih42:
ih43:
ih44:
ih45:
ih46:
ih47:
ih48:
ih49:
ih4a:
ih4b:
ih4c:
ih4d:
ih4e:
ih4f:

ih50:
	jmp ih50_syscall
	
	
ih51:
ih52:
ih53:
ih54:
ih55:
ih56:
ih57:
ih58:
ih59:
ih5a:
ih5b:
ih5c:
ih5d:
ih5e:
ih5f:

ih60:
ih61:
ih62:
ih63:
ih64:
ih65:
ih66:
ih67:
ih68:
ih69:
ih6a:
ih6b:
ih6c:
ih6d:
ih6e:
ih6f:

ih70:
ih71:
ih72:
ih73:
ih74:
ih75:
ih76:
ih77:
ih78:
ih79:
ih7a:
ih7b:
ih7c:
ih7d:
ih7e:
ih7f:

ih80:
ih81:
ih82:
ih83:
ih84:
ih85:
ih86:
ih87:
ih88:
ih89:
ih8a:
ih8b:
ih8c:
ih8d:
ih8e:
ih8f:

ih90:
ih91:
ih92:
ih93:
ih94:
ih95:
ih96:
ih97:
ih98:
ih99:
ih9a:
ih9b:
ih9c:
ih9d:
ih9e:
ih9f:

iha0:
iha1:
iha2:
iha3:
iha4:
iha5:
iha6:
iha7:
iha8:
iha9:
ihaa:
ihab:
ihac:
ihad:
ihae:
ihaf:

ihb0:
ihb1:
ihb2:
ihb3:
ihb4:
ihb5:
ihb6:
ihb7:
ihb8:
ihb9:
ihba:
ihbb:
ihbc:
ihbd:
ihbe:
ihbf:

ihc0:
ihc1:
ihc2:
ihc3:
ihc4:
ihc5:
ihc6:
ihc7:
ihc8:
ihc9:
ihca:
ihcb:
ihcc:
ihcd:
ihce:
ihcf:

ihd0:
ihd1:
ihd2:
ihd3:
ihd4:
ihd5:
ihd6:
ihd7:
ihd8:
ihd9:
ihda:
ihdb:
ihdc:
ihdd:
ihde:
ihdf:

ihe0:
ihe1:
ihe2:
ihe3:
ihe4:
ihe5:
ihe6:
ihe7:
ihe8:
ihe9:
ihea:
iheb:
ihec:
ihed:
ihee:
ihef:
