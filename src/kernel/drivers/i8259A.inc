;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	i8259A.inc of the PiscisOS
;;	Procs to operate the 8259 interrupt controller
;;	
;;	23/01/2012
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


ResetIRQs:
	push eax
	stc
	
	mov al, 011h		; ICW1 to master 8259a
	out I8259A_M0, al
	call io_delay
	out I8259A_S0, al	; ICW1 to slave 8259a
	call io_delay
	
	mov al, 020h		; ICW2 to master 8259a  0x20 -> IRQ0
	out I8259A_M1, al
	call io_delay
	mov al, 028h		; ICW2 to slave 8259a	0x28 -> IRQ8
	out I8259A_S1, al		
	call io_delay
	
	mov al, 004h		; ICW3 to master 8259a	Slave 8259A	-> IRQ2
	out I8259A_M1, al
	call io_delay
	
	mov al, 002h		; ICW3 to slave 8259a	Slave 8259A	-> IRQ2
	out I8259A_S1, al
	call io_delay
	
	mov al, 001h		; ICW4 to master 8259a 8086 mode
	out I8259A_M1, al
	call io_delay
	out I8259A_S1, al	; ICW4 to slave 8259a 8086 mode
	call io_delay
	
	mov al, IRQ_MF_ALL	; Mask all IRQs of master 8259a
	out I8259A_M1, al
	call io_delay
	
	mov al, IRQ_MF_ALL	; Mask all IRQs of slave 8259a
	out I8259A_S1, al
	call io_delay
	
	clc
	pop eax
	ret

EnableAllIRQs:
	push eax
	stc
	
	mov al, IRQ_EF_ALL		     ; enable all irqs
	out I8259A_M1, al
	call io_delay

	mov al, IRQ_EF_ALL
	out I8259A_S1, al
	
	clc
	pop eax
	ret    

FlushAllIRQs:
	push eax
	push ecx
	
	mov ecx, 32
.flush_all_irqs:
	mov al, 20h		     		; ready for irqs
	out I8259A_M0, al
	out I8259A_S0, al
	loop .flush_all_irqs		; flush the queue
	
	pop ecx
	pop eax
	ret 

SendEOI2Master:
	push eax
	push edx
	
	mov al, 20h
	mov dx, I8259A_M0
	out dx, al
	
	pop edx
	pop eax
	ret
	
SendEOI2Slave:
	push eax
	push edx
	
	mov al, 20h
	mov dx, I8259A_S0
	out dx, al
	
	pop edx
	pop eax
	ret
	

	