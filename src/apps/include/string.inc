;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	Application for PiscisOS: Helloworld
;;	Assembled by Flat Assembler
;;	
;;	23/04/2012
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;functions about string operation
strcopy:
;;in	esi:buffer of source
;;		edi:buffer of destination
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

	call strlen
	mov ebx, ecx
	
	push esi
	mov esi, edi
	call strlen
	pop esi
	
	cmp ebx, ecx
	je .cmp_chars
	ja .ret_a
	jb .ret_b

.cmp_chars:
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
	
strchr:
;;in	esi:buffer of source
;;		bl:char to find
;;out	eax:index of the char first
	push esi
	push ebx
	xor eax, eax
.next_char:	
	cmp [esi+eax], byte 0
	je .not_found
	cmp [esi+eax], bl
	je .final_ret
	inc eax
	jmp .next_char
.not_found:
	mov eax, 0ffffffffh
.final_ret:
	pop ebx
	pop esi
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	