;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	ramdiskfs.inc of the PiscisOS
;;	driver of fat12 file system in ram 
;;	
;;	23/01/2012
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


RDFS_DISKIMG_BASE		dd	00h
RDFS_RAW_FAT_BASE		dd	00h
RDFS_NEW_FAT_BASE		dd	00h

RDFS_CLUSTER_SIZE		dd	00h
RDFS_SECT_SIZE			dd	00h

RDFS_ROOTDIR_BASE		dd	00h
RDFS_DATA_BASE			dd	00h


RdfsInit:
;; init rdfs parameters
;;in	eax:diskette image base
;; 		ebx:new fat base
	pusha
	stc
	mov [RDFS_NEW_FAT_BASE], ebx

	mov esi, eax
	mov [RDFS_DISKIMG_BASE], esi

	mov ecx, wReservedSect
	add ecx, esi
	xor ebx, ebx
	mov bx, [ecx]
	mov eax, ebx

	mov ecx, dwsHiddenSect
	add ecx, esi
	xor ebx, ebx
	mov ebx, [ecx]

	add eax, ebx			;start sect of fat	

	mov ecx, wSectSize
	add ecx, esi
	xor ebx, ebx
	mov bx, [ecx]
	mov [RDFS_SECT_SIZE], ebx

	xor edx, edx
	mul bx
	add eax, esi
	mov [RDFS_RAW_FAT_BASE], eax
				
	mov ecx, wSectPerFat		;number of start sect of fat
	add ecx, esi
	xor eax, eax
	mov ax, [ecx]

	mov ecx, bFatCount
	add ecx, esi
	xor ebx, ebx
	mov bl, [ecx]

	xor edx, edx
	mul bx
	mov ebx, [RDFS_SECT_SIZE]
	xor edx, edx
	mul bx
	mov ebx, [RDFS_RAW_FAT_BASE]
	add eax, ebx
	mov [RDFS_ROOTDIR_BASE], eax

	mov ecx, wRootDirSize
	add ecx, esi
	xor eax, eax
	mov ax, [ecx]
	mov ebx, DIRITEM_SIZE
	xor edx, edx
	mul bx
	mov ebx, [RDFS_SECT_SIZE]
	xor edx, edx
	div bx
	cmp dx, 0
	je .noextra
	inc eax
.noextra:
	mov ebx, [RDFS_SECT_SIZE]
	xor edx, edx
	mul bx
	mov ebx, [RDFS_ROOTDIR_BASE]
	add eax, ebx
	mov [RDFS_DATA_BASE], eax

	mov ecx, bClusterSize
	add ecx, esi
	xor ebx, ebx
	mov bl, [ecx]				;sect count per cluseter

	mov eax, [RDFS_SECT_SIZE]
	xor edx, edx
	mul bx
	mov [RDFS_CLUSTER_SIZE], eax

	call RdfsRebuildFat

	popa
	clc		
	ret 

RdfsRebuildFat:
;;rebuild fat
;;calculat the raw fat at RDFS_RAW_FAT_BASE 
;;and store dump fat to RDFS_NEW_FAT_BASE
	push eax
	push ebx
	push ecx
	push esi
	push edi
	
	mov esi, [RDFS_RAW_FAT_BASE]
	mov edi, [RDFS_NEW_FAT_BASE]
	mov ecx, edi
	add ecx, 3072*2

.loop_0:
	xor ax, ax
	xor bx, bx
	mov al, [esi+0]  ; byte 1
	mov bl, [esi+1]
	and bx, 0fh
	shl bx, 8
	add ax, bx
	mov [edi], ax
	add edi, 2

	xor ax, ax
	xor bx, bx
	mov bl, [esi+1]  ; byte 2
	mov al, [esi+2]
	shr bx, 4
	shl ax, 4
	add ax, bx
	mov [edi], ax
	add edi, 2

	add esi, 3

	cmp	edi, ecx
	jb .loop_0

	pop edi
	pop esi
	pop ecx
	pop ebx
	pop eax
	ret

rdfsReadRootDir:
;;in	edi:dest buffer
;;
;;out	eax:byte count of the file, -1 read error
	push ecx
	mov ecx, [RDFS_DATA_BASE]
	sub ecx, [RDFS_ROOTDIR_BASE]
	mov eax, ecx
	mov esi, [RDFS_ROOTDIR_BASE]
	rep movsb
	pop ecx
	ret
	
	
rdfsGetNextFatNumber:
;;in	eax:current fat number
;;
;;out	eax:next fat number
	push edi
	mov edi, [RDFS_NEW_FAT_BASE]
	shl eax, 1
	add edi, eax
	movzx eax, word [edi]
	pop edi
	ret
	
rdfsFormateName:
;;in esi:file name to
;;   edi:dest buffer to save file name
	push eax
	push ecx
	push edx
	
	mov [edi+00h], dword 20202020h
	mov [edi+04h], dword 20202020h
	mov [edi+08h], dword 20202020h
	mov [edi+0ch], dword 0
	
	xor ecx, ecx
	mov edx, edi
.prefix:	
	cmp [esi], byte '\'			;is '\'
	jne .valid
	inc esi
	jmp .prefix
	
.valid:
	mov al, [esi]
	cmp al, 61h
	jb .notlow
	cmp al, 7ah
	ja .notlow
	sub al, 20h
.notlow:
	cmp ecx, 8
	jb .notfull
	mov ecx, 7
	mov al, 7eh
.notfull:
	mov edi, edx
	add edi, ecx
	mov [edi], al
	inc ecx
	inc esi
	cmp [esi], byte '\'
	je .done
	cmp [esi], byte 0
	je .done
	jmp .valid
.done:
	pop edx
	pop ecx
	pop eax
	ret

rdfsGetFatNumberInRootDir:
;;in	esi:file name
;;
;;out	eax:fat number or -1 not found
	push ecx
	push esi
	push edi
	
	mov edi, [RDFS_ROOTDIR_BASE]
.search_loop:
	push edi
	push esi
	mov ecx, FULLFILENAME_LEN
	m_memcmp
	pop esi
	pop edi
	je .search_ok			;ok find the file
	
	add edi, 20h
	cmp edi, [RDFS_DATA_BASE]
	jb .search_loop			;continue searching next file item
	mov eax, 0ffffffffh				;not found the file
	jmp .final_ret
.search_ok:
	movzx eax, word [edi+FILEITEM_FIRSTCLUSTER]
.final_ret:
	pop ecx
	pop edi
	pop esi
	ret


rdfsGetFatNumberInDir:
;;in	esi:name buffer
;;		eax:first fat number of dir
;;
;;out	eax:first fat nmuber of file, -1 not found
	push ebx
	push ecx
	push edx
	push esi
	push edi

	cmp eax, 0
	jne .in_next_fat
	call rdfsGetFatNumberInRootDir
	jmp .final_ret
	
.in_next_fat:
	mov ebx, eax
	
	sub eax, 2
	imul eax, [RDFS_CLUSTER_SIZE]	
	
	mov edx, [RDFS_DATA_BASE]
	add edx, eax
	mov edi, edx
	add edx, [RDFS_CLUSTER_SIZE]
.search_loop:
	push edi
	push esi
	mov ecx, FULLFILENAME_LEN
	m_memcmp
	pop esi
	pop edi
	je .search_ok			;ok find the file
	
	add edi, 20h
	cmp edi, edx
	jb .search_loop			;continue searching next file item
	
	mov eax, ebx
	call rdfsGetNextFatNumber
	cmp eax, 0ff0h
	jb .in_next_fat			;;not last fat continue to read

	mov eax, 0ffffffffh		;;-1 not found the fille
	jmp .final_ret
.search_ok:
	movzx eax, word [edi+FILEITEM_FIRSTCLUSTER]
.final_ret:
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	ret 


rdfsGetFileFirstFatNumber:
;;in esi:file path
;;
;;out eax:byte count of the file, -1 read error
	push esi
	push edi
	
	cmp [esi+0], byte '\'
	jne .invalid_path
	cmp [esi+1], byte 0
	jne .not_root
	mov eax, 0
	jmp .final_ret
	
.invalid_path:
	mov eax, 0ffffffffh
	jmp .final_ret
	
.not_root:
	mov [.curname+00h], dword 0
	mov [.curname+04h], dword 0
	mov [.curname+08h], dword 0
	mov [.curname+0ch], dword 0

	mov eax, 0
.recur_dir:		
	mov edi, .curname
	call rdfsFormateName
	
	push esi
	mov esi, .curname
	call rdfsGetFatNumberInDir
	pop esi
	
	cmp eax, 0ffffffffh
	je .final_ret

	cmp [esi], byte 0
	je .final_ret
	jmp .recur_dir
	
.final_ret:
	pop edi
	pop esi
	ret
	
.curname	dd 0, 0, 0, 0
		times (16) db 0
		

rdfsReadFileByFat:
;;in	eax:first fat number of file
;;		edi:dest buffer
;;
;;out	eax:byte count of the file, -1 read error
	push ebx
	push ecx
	push edx
	push esi
	push edi

	xor ebx, ebx
.read_next_fat:
	push eax
	
	sub eax, 2
	imul eax, [RDFS_CLUSTER_SIZE]	
	
	mov edx, [RDFS_DATA_BASE]
	add edx, eax
	mov esi, edx
	
	mov ecx, [RDFS_CLUSTER_SIZE]
	m_memcopy
	
	inc ebx
	pop eax
	call rdfsGetNextFatNumber
	cmp eax, 0ff0h
	jb .read_next_fat			;;not last fat continue to read
	
	mov eax, ebx
	imul eax, [RDFS_CLUSTER_SIZE]
	
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	ret 
	
	
RdfsReadFile:
;;in esi:file path
;;	 edi:dest buffer
;;out eax:byte count of the file, -1 read error
	push esi
	push edi
	
	call rdfsGetFileFirstFatNumber
	cmp eax, 0ffffffffh
	je .final_ret
	
	cmp eax, 0
	je .read_root
	call rdfsReadFileByFat
	jmp .final_ret
.read_root:
	call rdfsReadRootDir
.final_ret:
	pop edi
	pop esi
	ret

	
; __RdfsLoadFile:
;in esi:file path
;	 edi:dest buffer
;out eax:byte count of the file, -1 read error
	; push esi
	; push edi
	
	; call rdfsGetFileFirstFatNumber
	; cmp eax, 0ffffffffh
	; je .final_ret
	
	; call rdfsReadFileByFat
; .final_ret:
	; pop edi
	; pop esi
	; ret

	
;'\bin\test\dira', 0
; RdfsLoadFile___:
;in esi:file path
;	 edi:dest buffer
;out eax:byte count of the file, -1 read error
	; push edx
	; mov edx, edi
	
	; mov [.curname+00h], dword 0
	; mov [.curname+04h], dword 0
	; mov [.curname+08h], dword 0
	; mov [.curname+0ch], dword 0
	
	; mov edi, .curname
	; call rdfsFormateName
	; push esi
	
	; mov esi, .curname
	; call rdfsGetFatNumberInRootDir
	; pop esi	
	
	; cmp eax, 0ffffffffh
	; je .final_ret
	
	; cmp [esi], byte 0
	; je .find_ok

; .recur_dir:
	; mov edi, .curname
	; call rdfsFormateName
	; push esi
	
	; mov esi, .curname
	; call rdfsGetFatNumberInDir
	; pop esi
	
	; cmp eax, 0ffffffffh
	; je .final_ret
	
	; cmp [esi], byte 0
	; je .find_ok
	; jmp .recur_dir
	
; .find_ok:
	; mov edi, edx
	; call RdfsReadFile
; .final_ret:
	; pop edx
	; ret
	
; .curname	dd 0, 0, 0, 0
		; times (16) db 0
		
		
rdfsGetFileItemInRootDir:
;;in	esi:file name
;;		edi:buffer to save fileitem
;;
;;out	eax:fat number or -1 not found
	push ecx
	push esi
	push edi
	
	mov ecx, edi
	
	mov edi, [RDFS_ROOTDIR_BASE]
.search_loop:
	push edi
	push esi
	push ecx
	mov ecx, FULLFILENAME_LEN
	m_memcmp
	pop ecx
	pop esi
	pop edi
	je .search_ok			;ok find the file
	
	add edi, 20h
	cmp edi, [RDFS_DATA_BASE]
	jb .search_loop			;continue searching next file item
	mov eax, 0ffffffffh				;not found the file
	jmp .final_ret
.search_ok:
	movzx eax, word [edi+FILEITEM_FIRSTCLUSTER]
	mov esi, edi
	mov edi, ecx
	mov ecx, 20h
	m_memcopy
.final_ret:
	pop ecx
	pop edi
	pop esi
	ret


rdfsGetFileItemInDir:
;;in	eax:first fat number of dir
;;		esi:file name
;;		edi:buffer to save file item
;;
;;out	eax:first fat nmuber of file, -1 not found
	push ebx
	push ecx
	push edx
	push esi
	push edi
	
	cmp eax, 0
	jne .not_root
	call rdfsGetFileItemInRootDir
	jmp .final_ret
	
.not_root:
	mov ecx, edi
.in_next_fat:	
	mov ebx, eax
	
	sub eax, 2
	imul eax, [RDFS_CLUSTER_SIZE]	
	
	mov edx, [RDFS_DATA_BASE]
	add edx, eax
	mov edi, edx
	add edx, [RDFS_CLUSTER_SIZE]
.search_loop:
	push edi
	push esi
	push ecx
	mov ecx, FULLFILENAME_LEN
	m_memcmp
	pop ecx
	pop esi
	pop edi
	je .search_ok			;ok find the file
	
	add edi, 20h
	cmp edi, edx
	jb .search_loop			;continue searching next file item
	
	mov eax, ebx
	call rdfsGetNextFatNumber
	cmp eax, 0ff0h
	jb .in_next_fat			;;not last fat continue to read

	mov eax, 0ffffffffh		;;-1 not found the fille
	jmp .final_ret
.search_ok:
	movzx eax, word [edi+FILEITEM_FIRSTCLUSTER]	
	mov esi, edi
	mov edi, ecx
	mov ecx, 20h
	m_memcopy
.final_ret:
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	ret 
	
		
RdfsGetFileItem:
;;in	esi:file path
;;		edi:dest buffer to save file item
;;out	eax:first fat nmuber of file, -1 not found
	push edx
	
	cmp [esi+0], byte '\'
	jne .invalid_path
	cmp [esi+1], byte 0
	jne .not_root
	mov eax, 0
	mov [edi+FILEITEM_ATTR], byte 10h
	jmp .final_ret
	
.invalid_path:
	mov eax, 0ffffffffh
	jmp .final_ret

.not_root:
	mov [.curname+00h], dword 0
	mov [.curname+04h], dword 0
	mov [.curname+08h], dword 0
	mov [.curname+0ch], dword 0
	
	mov edx, edi
	mov eax, 0
.recur_dir:
	mov edi, .curname
	call rdfsFormateName
	
	push esi
	mov esi, .curname
	mov edi, .fitembuf
	call rdfsGetFileItemInDir
	pop esi
	
	cmp eax, 0ffffffffh
	je .final_ret
	
	cmp [esi], byte 0
	je .find_ok
	jmp .recur_dir
	
.find_ok:
	mov esi, .fitembuf
	mov edi, edx
	mov ecx, 20h
	m_memcopy
.final_ret:
	pop edx
	ret
	
.curname	dd 0, 0, 0, 0
.fitembuf:	times (32) db 0
			times (16) db 0

		
		
		
		
		
		
		
