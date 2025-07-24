;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	misc.inc of the PiscisOS
;;	String and Memory functions
;;	
;;	23/01/2012
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

macro m_memcopy
{
	cld
	rep movsb
}

macro m_memcmp
{
	cld
	repe cmpsb
}
	

memcopy:
;;esi: source buffer
;;edi: dest buffer
;;ecx: byte count to copy
;;note: the es and da must be the same
	push edi
	push esi
	push ecx
	
	cld
	rep movsb
	
	pop ecx
	pop esi
	pop edi
	
	ret


memcmp:
;;esi:mem 1
;;edi:mem 2
;;ecx:length to compare
;;return eax:0 equ, 0ffffff mem1 is less than mem2, 1 mem1 is greater than mem2
;;note: the es and da must be the same
	push edi
	push esi
	push ecx
	
	cld
	repe cmpsb
	je .ret_e
	ja .ret_a
	jb .ret_b
.ret_a:
	mov eax, 1
	jmp .ret
.ret_b:
	mov eax, 0ffffffffh
	jmp .ret
.ret_e:
	mov eax, 0
.ret:	
	pop ecx
	pop esi
	pop edi
	ret
	
strcopy:
;;in	esi:buffer of source
;;		edi:buffer fs destination
;;out	eax:count of char copyed, not include '\0'
	push esi
	push edi
	
	xor ecx, ecx
	cmp [esi], byte 0
	je .final_ret
	cld
.next_char:
	lodsb
	cmp al, 0
	je .copy_done
	mov [edi], al
	inc edi
	inc ecx
	jmp .next_char
.copy_done:
	mov [edi], al
	mov eax, ecx
.final_ret:
	pop edi
	pop esi
	ret	
	
strlen:
;;in	esi:buffer
;;out	ecx:length without '\0'
	push esi
	
	xor ecx, ecx
	cld
.next_char:
	lodsb
	cmp al, 0
	je .ret
	inc ecx
	jmp .next_char
	
.ret:
	pop esi
	ret
	
	
strcmp:
;;in	esi:buffer of source
;;		edi:buffer os destination
;;out	eax:
	push edi
	push esi
	push ecx
	push eax

	call strlen
	mov ebx, eax
	
	push esi
	mov esi, edi
	call strlen
	pop esi
	
	cmp ebx, eax
	je .cmp_chars
	ja .ret_a
	jb .ret_b

.cmp_chars:
	mov ecx, eax
.loop_1:
	cld
	repe cmpsb
	je .ret_e
	ja .ret_a
	jb .ret_b
.ret_a:
	mov eax, 1
	jmp .ret
.ret_b:
	mov eax, 0ffffffffh
	jmp .ret
.ret_e:
	mov eax, 0
.ret:	
	pop ecx
	pop esi
	pop edi
	ret

