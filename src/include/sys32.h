;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Stack 
KERNEL_STACK_BASE			equ 05000h	;[50000:0000]

STACK_SIZE_16BIT			equ	05f000h - KERNEL_STACK_BASE*16
STACK_SIZE_32BIT			equ	06c000h - KERNEL_STACK_BASE*16

RING0_ESP_0					equ	06d000h - KERNEL_STACK_BASE*16
RING1_ESP_1					equ	06e000h - KERNEL_STACK_BASE*16
RING2_ESP_2					equ	06f000h - KERNEL_STACK_BASE*16

stack_size_int				equ	1024
stack_size_sysc				equ 4096
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Pagging table
PDE_OFFSET					equ 02b0000h
PTE_OFFSET					equ	0400000h
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;TSS
max_interrupts				equ	250	
tss_block_interrupt_base	equ	02a0000h
tss_block_syscall_base		equ	02a8000h
tss_unit_size				equ	128

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Keyboard Queue buffer 
kasbuf_len					equ 40h
kasbuf_base					equ 0f100h
kasbuf_head					equ	0f190h
kasbuf_tail					equ	0f1a0h
kasbuf_count				equ	0f1b0h

kbled_stat_change			equ	0f020h
kbled_num_change			equ	0f021h