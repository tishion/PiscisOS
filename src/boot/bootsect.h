;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	Boot sector of the PiscisOS
;;	December 2011
;;
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


TEMP_BUFFER_BASE	equ 0060h
FAT_BASE			equ 0600h

KERNEL_BASE 		equ 1000h
KERNEL_OFFSET		equ 0000h
BOOTSEC_OFFSET		equ 7c00h


DIRITEM_SIZE			equ 20h
FILEITEM_NAME 			equ 00h
FILEITEM_EXTEND			equ 08h
FILEITEM_ATTR   		equ 0bh
FILEITEM_RESERVED 		equ 10h
FILEITEM_WRITETIME 		equ 16h
FILEITEM_WRITEDATE	 	equ 18h
FILEITEM_FIRSTCLUSTER	equ 1ah
FILEITEM_FILESIZE		equ 1ch
FULLFILENAME_LEN		equ 0bh
;=====================================================================
;           |-----------------------|0000:0500
;           |       .........       |       .........
;           |       0000:7BEE       | [bp-0x12] FILE_BLOCK 3
;           |       0000:7BF0       | [bp-0x10] FILE_BLOCK 2
;           |       0000:7BF2       | [bp-0x0e] FILE_BLOCK 1
;           |       0000:7BF4       | [bp-0x0c] START_LOADER  = &FILE_BLOCK1
;           |       0000:7BF6       | [bp-0x0a] nStartSectOfData
;           |       0000:7BF8       | [bp-0x08] nSectCountOfRootdir
;           |       0000:7BFA       | [bp-0x06] nStartSectOfRootdir
;           |       0000:7BFC       | [bp-0x04] nStartSectOfFat
;           |       0000:7BFE       | [bp-0x02] BootDrv
;           |-----------------------|0000:7C00
;           |      FAT12��������	|
;           |-----------------------|0000:7E00
nStartSectOfData		equ (bp-0x0a)
nSectCountOfRootdir		equ (bp-0x08)
nStartSectOfRootdir		equ (bp-0x06)
nStartSectOfFat			equ (bp-0x04)
bootdrv					equ (bp-0x02)
;=====================================================================