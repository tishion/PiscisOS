APPMEM_BASE					equ	0a00000h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;PCB
PCB_SIZE					equ	64		;PCB unit size 32 byte
PCB_TABLE_BASE				equ	1000h	;PCB table base

pcb_total_count				equ	4ff8h	;address of total count of processes in current system
pcb_current_no				equ 4ff0h	;address of number of current running process 
pcb_current_base			equ	4ff4h	;address of base of current running process, =PCB_TABLE_BASE+pcb_current_no*PCB_SIZE

PID_OFFSET					equ	00h
PPID_OFFSET					equ	04h
MLOC_OFFSET					equ	10h
SYSC_OFFSET					equ	14h
TICK_OFFSET					equ	20h
STAT_OFFSET					equ	30h
PCBS_OFFSET					equ 31h
WPID_OFFSET					equ	34h

PCBS_FREE					equ	00h
PCBS_USED					equ	01h

;;Process status consts
PS_TERMINATED				equ	00h
PS_READY					equ	01h
PS_RUNNING					equ	02h
PS_WAIT						equ	03h
PS_ZOMBIE					equ	04h
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

max_processes				equ	250
tss_unit_process_size		equ	(128+8192)

tss_block_process_base		equ	0800000h
app_mem_base				equ	0900000h		;;first process is os task, application process is beging at 0a00000h

app_mem_size				equ	0100000h

app_esp						equ	00ffff0h

app_arg_size				equ	256

pid_init					equ	0f008h


APPH_SIG					equ	00h
APPH_START					equ	10h
APPH_ARGS					equ 14h

CPER_MAX_COUNT					equ 0ffffffffh
CPER_NOT_FOUND_FILE				equ	0fffffffeh
CPER_INVALID_FORMAT				equ	0fffffffdh

