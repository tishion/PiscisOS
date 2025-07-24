;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	ramdiskfs.h of the PiscisOS
;;	driver of fat12 file system in ram 
;;	
;;	23/01/2012
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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

FATTR_RDONLY			equ 00000001b
FATTR_HIDDEN			equ	00000010b
FATTR_SYSTEM			equ	00000100b
FATTR_VOLLAB			equ	00001000b
FATTR_SUBDIR			equ	00010000b
FATTR_DEVICE			equ	00100000b
FATTR_ARCHIV			equ	01000000b
FATTR_NOTUSE			equ	10000000b

bsOEMString		equ 03h
wSectSize		equ 0bh
bClusterSize	equ 0dh
wReservedSect	equ 0eh
bFatCount		equ 10h
wRootDirSize	equ 11h
wTotalSect		equ 13h
bMediaType		equ 15h
wSectPerFat		equ 16h
wSectPerTrack	equ 18h
wHeadCount		equ 1ah
dwsHiddenSect	equ 1ch
dwHugeSect		equ 20h
bBootDrv		equ 24h
bBootSign		equ 26h
dwVolumeID		equ 27h
bsVolumeLabel	equ 2bh
bsFSType		equ 36h