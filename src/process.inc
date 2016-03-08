;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;processes
set_gdt_process_tss_descriptor:		
	mov ecx, tss_block_process_base
	mov edi, 0
.next:
	mov [tss_process0_l+edi+0], word tss_unit_process_size			;SegmentLimit(0~15bit)
	mov [tss_process0_l+edi+2], cx									;SegmentBase(0~15bit)
	mov eax, ecx
	shr eax, 16
	mov [tss_process0_l+edi+4], al									;SegmentBase(16~23bit)
	mov [tss_process0_l+edi+7], ah									;SegmentBase(24~31bit)
	mov [tss_process0_l+edi+5], word 01010000b*256+11101001b		;SegmentAttribut(DPL=3,TYPE=9 386TSS)
	add ecx, tss_unit_process_size
	add edi, 8
	cmp edi, 8*(max_processes+3)
	jb .next

	clc
	ret
	
set_gdt_process_taskgate_descriptor:
	mov edi, 0
	mov dx, sel_tss_process0
.next:
	mov [tg_process0_l+edi+0], word 0
	mov [tg_process0_l+edi+2], dx
	mov [tg_process0_l+edi+4], word 11100101b*256			;SegmentAttribut(DPL=3,TYPE=9 task gate)
	mov [tg_process0_l+edi+6], word 0
	add dx, 8
	add edi, 8

	cmp edi, 8*(max_processes+3)
	jb .next
	
	clc
	ret
	
set_gdt_process_segment_descriptor:
;;code
	mov ecx, app_mem_base
	;mov ecx, 0
	mov edi, 0
.next_c:
	mov [user_code_l+edi+0], word 0								;SegmentLimit(0~15bit)
	;mov [user_code_l+edi+0], word 0ffffh						;SegmentLimit(0~15bit)
	mov [user_code_l+edi+2], cx									;SegmentBase(0~15bit)
	mov eax, ecx
	shr eax, 16
	mov [user_code_l+edi+4], al									;SegmentBase(16~23bit)
	mov [user_code_l+edi+7], ah									;SegmentBase(24~31bit)
	mov [user_code_l+edi+5], word 11010001b*256+11111010b		;Attributes[G=1,D=1,AVL=1]+SegmentLimit=01h(16~19bit)+Attributes[DPL=3,Type=A E\R]
	;mov [user_code_l+edi+5], word 11011111b*256+11111010b		;Attributes[G=1,D=1,AVL=1]+SegmentLimit=01h(16~19bit)+Attributes[DPL=3,Type=A E\R]
	add ecx, app_mem_size
	add edi, 8
	cmp edi, 8*(max_processes+3)
	jb .next_c

;data
	mov ecx, app_mem_base
	;mov ecx, 0
	mov edi, 0
.next_d:
	mov [user_data_l+edi+0], word 0								;SegmentLimit(0~15bit)
	;mov [user_data_l+edi+0], word 0ffffh						;SegmentLimit(0~15bit)
	mov [user_data_l+edi+2], cx									;SegmentBase(0~15bit)
	mov eax, ecx
	shr eax, 16
	mov [user_data_l+edi+4], al									;SegmentBase(16~23bit)
	mov [user_data_l+edi+7], ah									;SegmentBase(24~31bit)
	mov [user_data_l+edi+5], word 11010001b*256+11110010b		;SegmentAttributes [DPL=2;Type=2 R\W]
	;mov [user_data_l+edi+5], word 11011111b*256+11110010b		;SegmentAttributes [DPL=2;Type=2 R\W]
	add ecx, app_mem_size
	add edi, 8
	cmp edi, 8*(max_processes+3)
	jb .next_d

	clc
	ret

set_process_tss:
	mov [ts.eflags], dword 11202h
	mov [ts.ss0], sel_os_stack
	mov [ts.ss1], sel_ring1_stack
	mov [ts.ss2], sel_ring2_stack
	mov [ts.esp0], RING0_ESP_0
	mov [ts.esp1], RING1_ESP_1
	mov [ts.esp2], RING2_ESP_2
	mov eax, cr3
	mov [ts.cr3], eax
	mov [ts.eip], 0
	mov [ts.gs], sel_video_data
	mov [ts.esp], app_esp
	
	mov dx, 0
	mov edi, tss_block_process_base
.next:	
	mov cx, dx
	add cx, sel_user_code
	mov [ts.cs], cx
	mov cx, dx
	add cx, sel_user_data
	mov [ts.ss], cx
	mov [ts.ds], cx
	mov [ts.es], cx
	mov [ts.fs], cx
	push edi
	mov esi, tss_struct
	mov ecx, 120/4
	cld
	rep movsd
	pop edi
	add edi, tss_unit_process_size
	add dx, 8

	cmp dx, 8*(max_processes+3)
	jb .next
	
	clc
	ret
	

set_process_control_block:
	mov eax, 5000h
	mov [pid_init], eax
	
	mov edi, PCB_TABLE_BASE
	mov eax, app_mem_base
.next:
	mov [edi+PID_OFFSET], dword 0ffffffffh			;;pid
	mov [edi+PPID_OFFSET], dword 0ffffffffh			;;ppid
	mov [edi+MLOC_OFFSET], eax						;;mlocation
	mov [edi+SYSC_OFFSET], dword 0ffffffffh			;;syscall tss dsc
	mov [edi+TICK_OFFSET], dword 0					;;tickcount
	mov [edi+STAT_OFFSET], byte PS_READY			;;status
	mov [edi+PCBS_OFFSET], byte PCBS_FREE			;;pcb status
	mov [edi+WPID_OFFSET], dword 0ffffffffh			;;wait pid
	
	add edi, PCB_SIZE
	add eax, app_mem_size
	
	cmp edi, PCB_TABLE_BASE+PCB_SIZE*max_processes
	jb .next
	
	clc
	ret

	
reset_process_tss:
;;in	eax:pcb number	
	push eax
	push ecx
	push edx
	push edi
	push esi
	
	mov [ts.eflags], dword 11202h
	mov [ts.ss0], sel_os_stack
	mov [ts.ss1], sel_ring1_stack
	mov [ts.ss2], sel_ring2_stack
	mov [ts.esp0], RING0_ESP_0
	mov [ts.esp1], RING1_ESP_1
	mov [ts.esp2], RING2_ESP_2
	push eax
	mov eax, cr3
	mov [ts.cr3], eax
	pop eax
	mov [ts.eip], 0
	mov [ts.gs], sel_video_data
	mov [ts.esp], app_esp
	
	mov edx, eax
	imul edx, 8
	mov cx, dx
	add cx, sel_user_code
	mov [ts.cs], cx
	mov cx, dx
	add cx, sel_user_data
	mov [ts.ss], cx
	mov [ts.ds], cx
	mov [ts.es], cx
	mov [ts.fs], cx

	mov edi, eax
	imul edi, tss_unit_process_size
	add edi, tss_block_process_base
	mov esi, tss_struct
	mov ecx, 120/4
	cld
	rep movsd

	pop edi
	pop esi
	pop edx
	pop ecx
	pop eax
	ret
	
find_free_pcb:
;;out eax:free pcb number, -1 not found
	push edi
	
	cmp dword [pcb_total_count], max_processes
	jae .max_proc
	
	mov eax, 0
.find_next:
	inc eax
	cmp eax, max_processes
	je .max_proc
	mov edi, eax
	imul edi, PCB_SIZE
	add edi, PCB_TABLE_BASE
	
	cmp byte [edi+PCBS_OFFSET], PCBS_FREE
	je .final_ret
	jmp .find_next

.max_proc:
	mov eax, 0ffffffffh
.final_ret:
	pop edi
	ret

check_set_app_parameters:
;;in 	eax:pcb number
;;		edi:app memory location
;;   	esi:argments buffer
;;out 	eax:0 ok, -1 fail
	push ebx
	push ecx
	push edx
	push esi
	push edi
	
	cmp [edi+0], dword 'PISC'
	jne .check_fail
	cmp [edi+4], dword 'ISAP'
	jne .check_fail	
	cmp [edi+8], dword 'P000'
	jne .check_fail
	
	mov ebx, tss_unit_process_size
	xor edx, edx
	mul ebx
	add eax, tss_block_process_base
	mov edx, eax
	mov eax, [edi+APPH_START]
	mov [edx+ts.eip-tss_struct], eax
	
	;;copy arguments
	cmp esi, edi
	je .noargs
	mov ebx, [edi+APPH_ARGS]
	add edi, ebx
	push ecx
	call strcopy
	pop ecx
	
.noargs:
	mov eax, 0
	jmp .final_ret
.check_fail:	
	mov eax, 0ffffffffh
.final_ret:
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	ret
	
create_process:
;;in	esi:file name
;;		eax:argments buffer £¨PA£©
;;out	eax:pid
	push ebx
	push edx
	push esi
	push edi
	
	mov edi, eax
	
	call find_free_pcb
	cmp eax, 0ffffffffh							;;max process count	
	je .er_max_count
	mov ebx, eax
	
	mov edx, ebx
	imul edx, PCB_SIZE
	add edx, PCB_TABLE_BASE
	
	push edi
	mov edi, [edx+MLOC_OFFSET]
	call RdfsReadFile
	pop edi
	cmp eax, 0ffffffffh							;;file not found
	je .er_not_found_file
	
	mov esi, edi
	mov eax, ebx
	mov edi, [edx+MLOC_OFFSET]
	call check_set_app_parameters	
	cmp eax, 0ffffffffh							;;file format is invlad
	je .er_invalid_format
	
.create_ok:
	cli
	inc dword [pid_init]
	mov eax, [pid_init]
	mov [edx+PID_OFFSET], eax
	mov [edx+PPID_OFFSET], dword 0
	mov [edi+SYSC_OFFSET], dword 0ffffffffh
	mov [edx+TICK_OFFSET], dword 0
	mov [edx+STAT_OFFSET], byte PS_READY
	mov [edx+PCBS_OFFSET], byte PCBS_USED
	mov [edx+WPID_OFFSET], dword 0ffffffffh
	inc dword [pcb_total_count]
	sti
	jmp .final_ret
	
.er_max_count:
	mov eax, CPER_MAX_COUNT
	jmp .final_ret
.er_not_found_file:
	mov eax, CPER_NOT_FOUND_FILE
	jmp .final_ret
.er_invalid_format:	
	mov eax, CPER_INVALID_FORMAT
	jmp .final_ret
	
.final_ret:
	pop edi
	pop esi
	pop edx
	pop ebx
	ret

pid_to_pcb_number:
;;in	eax:pid
;;out	eax:pcb number	,0ffffffffh not found
	push ebx
	push edi
	
	mov ebx, eax
	
	mov eax, 0
.find_next:
	inc eax
	cmp eax, max_processes
	je .not_found
	mov edi, eax
	imul edi, PCB_SIZE
	add edi, PCB_TABLE_BASE
	
	cmp [edi+PCBS_OFFSET], byte PCBS_FREE
	je .find_next
	cmp [edi+PID_OFFSET], ebx
	jne .find_next
	jmp .final_ret
.not_found:
	mov eax, 0ffffffffh
.final_ret:
	
	pop edi
	pop ebx
	ret
	
wait_process_id:
;;in	eax:pid to be waited
;;out	eax:if the function success it will not return 0 untill the process exit, if failed, return -1 right now
	push ebx
	push edi
	
	mov ebx, eax
	call pid_to_pcb_number
	cmp eax, 0ffffffffh						;;not found the process to be waited
	je .final_ret
	
	mov edi, eax
	imul edi, PCB_SIZE
	add edi, PCB_TABLE_BASE
	cmp [edi+STAT_OFFSET], byte PS_TERMINATED	;;already terminated
	je .wait_ok
	
	cli
	mov edi, [pcb_current_base]
	mov [edi+STAT_OFFSET], byte PS_WAIT
	mov [edi+WPID_OFFSET], ebx
	call change_task
	sti
.wait_ok:
	mov eax, 0
.final_ret:
	pop edi
	pop ebx
	ret
	
notify_waiting_process:
;;in	eax:pid are waited
	push eax
	push ebx
	push edi
	
	mov ebx, eax
	
	mov eax, 0
.find_next:
	inc eax
	cmp eax, max_processes
	je .final_ret
	mov edi, eax
	imul edi, PCB_SIZE
	add edi, PCB_TABLE_BASE
	
	cmp [edi+PCBS_OFFSET], byte PCBS_FREE
	je .find_next
	cmp [edi+STAT_OFFSET], byte PS_WAIT
	jne .find_next
	cmp [edi+WPID_OFFSET], ebx
	jne .find_next
	mov [edi+STAT_OFFSET], byte PS_READY
	cmp [edi+WPID_OFFSET], dword 0ffffffffh
	jmp .find_next

.final_ret:
	
	pop edi
	pop ebx
	pop eax
	ret
	
terminate_process:
;;in	eax:pcb number
	cli
	
	push eax
	push edi
	
	call reset_process_tss
	
	mov edi, eax
	imul edi, PCB_SIZE
	add edi, PCB_TABLE_BASE
	
	mov eax, [edi+PID_OFFSET]
	call notify_waiting_process
	
	mov eax, [edi+SYSC_OFFSET]
	call complet_syscall
	
	mov [edi+PID_OFFSET], dword 0ffffffffh
	mov [edi+PPID_OFFSET], dword 0ffffffffh
	mov [edi+SYSC_OFFSET], dword 0ffffffffh
	mov [edi+TICK_OFFSET], dword 0
	mov [edi+STAT_OFFSET], byte PS_TERMINATED
	mov [edi+PCBS_OFFSET], byte PCBS_FREE
	mov [edi+WPID_OFFSET], dword 0ffffffffh
	dec dword [pcb_total_count]
	
	mov edi, (max_processes+1)*8
	mov [tss_process0_l+edi+5], word 01010000b*256+11101001b
	
	mov ax, sel_tss_process0	;set tss
	add ax, (max_processes+1)*8
	ltr ax
	
	call change_task
	
	pop edi
	pop eax
	sti
	ret 

	
	
	
	
	
	
