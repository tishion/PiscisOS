;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	i8259A.inc of the PiscisOS
;;	Procs to operate the 8259 interrupt controller
;;	
;;	23/01/2012
;;	Copyright (C) tishion
;;	https://github.com/tishion/PiscisOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


key_map:
	db KEY_NULL, KEY_ESC, '1234567890-=', KEY_BS
	db KEY_TAB, 'qwertyuiop[]', KEY_ENTER, KEY_CTRL
	db 'asdfghjkl;',27h,60h, KEY_SHIFT_L, '\zxcvbnm,./'
	db KEY_SHIFT_R, KEY_PRINTSCREEN, KEY_ALT, KEY_SPACE, KEY_CAPS
	db KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10
	db KEY_NUM, KEY_SCROLL
	
shift_key_map:
	db KEY_NULL, KEY_ESC, '!@#$%^&*()_+', KEY_BS
	db KEY_TAB, 'QWERTYUIOP{}', KEY_ENTER, KEY_CTRL
	db 'ASDFGHJKL:"~', KEY_SHIFT_L, '|ZXCVBNM<>?'
	db KEY_SHIFT_R, KEY_PRINTSCREEN, KEY_ALT, KEY_SPACE, KEY_CAPS
	db KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10
	db KEY_NUM, KEY_SCROLL

ctrl_key_map:
	db KEY_NULL, KEY_ESC, '1234567890-=', KEY_BS
	db KEY_TAB, 'qwertyuiop[]', KEY_ENTER, KEY_CTRL
	db 'asdfghjkl;',27h,60h, KEY_SHIFT_L, '\zxcvbnm,./'
	db KEY_SHIFT_R, KEY_PRINTSCREEN, KEY_ALT, KEY_SPACE, KEY_CAPS
	db KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10
	db KEY_NUM, KEY_SCROLL
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Set keboard LEDs
;;		bit 0=1	SCRLK ON
;		bit 1=1	NUMLK ON
;		bit 2=1	CAPLK ON
KbChangeLeds:
	push ax
	push bx
	
	mov al, 0edh
	out 60h, al
	call io_delay
	
	in al, 60h
	call io_delay
	
	xor al, 000000111b
	mov bl, al
	
	mov al, 0edh
	out 60h, al
	call io_delay
	
	mov al, bl
	out 60h, al
	call io_delay
	
	pop bx
	pop ax
	ret
	

; KB_IN_BYTES			equ	32			;; size of keyboard input buffer     
; MAP_COLS			equ	03h			;; Number of columns in keymap     
; NR_SCAN_CODES		equ	80h			;; Number of scan codes (rows in keymap)     
        
; FLAG_BREAK			equ	0080h      ;; Break Code               
; FLAG_EXT			equ	0100h      ;; Normal function keys         
; FLAG_SHIFT_L		equ	0200h      ;; Shift key                
; FLAG_SHIFT_R		equ	0400h      ;; Shift key                
; FLAG_CTRL_L			equ	0800h      ;; Control key              
; FLAG_CTRL_R			equ	1000h      ;; Control key              
; FLAG_ALT_L			equ	2000h      ;; Alternate key            
; FLAG_ALT_R			equ	4000h      ;; Alternate key            
; FLAG_PAD			equ	8000h      ;; keys in num pad          
        
; MASK_RAW			equ	01FFh      ;; raw key value = code passed to tty & MASK_RAW    
									;the value can be found either in the keymap column 0    
									;or in the list below     
        
; Special keys    
; ESC					equ	(01h + FLAG_EXT)   ;; Esc          
; TAB					equ	(02h + FLAG_EXT)   ;; Tab          
; ENTER				equ	(03h + FLAG_EXT)   ;; Enter        
; BACKSPACE			equ	(04h + FLAG_EXT)   ;; BackSpace        
        
; GUI_L				equ	(05h + FLAG_EXT)   ;; L GUI        
; GUI_R				equ	(06h + FLAG_EXT)   ;; R GUI        
; APPS				equ	(07h + FLAG_EXT)   ;; APPS     
        
; Shift, Ctrl, Alt    
; SHIFT_L				equ	(08h + FLAG_EXT)   ;; L Shift      
; SHIFT_R				equ	(09h + FLAG_EXT)   ;; R Shift      
; CTRL_L				equ	(0Ah + FLAG_EXT)   ;; L Ctrl       
; CTRL_R				equ	(0Bh + FLAG_EXT)   ;; R Ctrl       
; ALT_L				equ	(0Ch + FLAG_EXT)   ;; L Alt        
; ALT_R				equ	(0Dh + FLAG_EXT)   ;; R Alt        
        
; Lock keys    
; CAPS_LOCK			equ	(0Eh + FLAG_EXT)   ;; Caps Lock        
; NUM_LOCK			equ	(0Fh + FLAG_EXT)   ;; Number Lock      
; SCROLL_LOCK			equ	(10h + FLAG_EXT)   ;; Scroll Lock      
        
; Function keys    
; F1					equ	(11h + FLAG_EXT)   ;; F1           
; F2					equ	(12h + FLAG_EXT)   ;; F2           
; F3					equ	(13h + FLAG_EXT)   ;; F3           
; F4					equ	(14h + FLAG_EXT)   ;; F4           
; F5					equ	(15h + FLAG_EXT)   ;; F5           
; F6					equ	(16h + FLAG_EXT)   ;; F6           
; F7					equ	(17h + FLAG_EXT)   ;; F7           
; F8					equ	(18h + FLAG_EXT)   ;; F8           
; F9					equ	(19h + FLAG_EXT)   ;; F9           
; F10					equ	(1Ah + FLAG_EXT)   ;; F10          
; F11					equ	(1Bh + FLAG_EXT)   ;; F11          
; F12					equ	(1Ch + FLAG_EXT)   ;; F12          
        
; Control Pad    
; PRINTSCREEN			equ	(1Dh + FLAG_EXT)   ;; Print Screen     
; PAUSEBREAK			equ	(1Eh + FLAG_EXT)   ;; Pause/Break      
; INSERT				equ	(1Fh + FLAG_EXT)   ;; Insert       
; DELETE				equ	(20h + FLAG_EXT)   ;; Delete       
; HOME				equ	(21h + FLAG_EXT)   ;; Home         
; END					equ	(22h + FLAG_EXT)   ;; End          
; PAGEUP				equ	(23h + FLAG_EXT)   ;; Page Up      
; PAGEDOWN			equ	(24h + FLAG_EXT)   ;; Page Down        
; UP					equ	(25h + FLAG_EXT)   ;; Up           
; DOWN				equ	(26h + FLAG_EXT)   ;; Down         
; LEFT				equ	(27h + FLAG_EXT)   ;; Left         
; RIGHT				equ	(28h + FLAG_EXT)   ;; Right        
        
; ACPI keys    
; POWER				equ	(29h + FLAG_EXT)   ;; Power        
; SLEEP				equ	(2Ah + FLAG_EXT)   ;; Sleep        
; WAKE				equ	(2Bh + FLAG_EXT)   ;; Wake Up      
        
; Num Pad    
; PAD_SLASH			equ	(2Ch + FLAG_EXT)   ;; /            
; PAD_STAR			equ	(2Dh + FLAG_EXT)   ;; *            
; PAD_MINUS			equ	(2Eh + FLAG_EXT)   ;; -            
; PAD_PLUS			equ	(2Fh + FLAG_EXT)   ;; +            
; PAD_ENTER			equ	(30h + FLAG_EXT)   ;; Enter        
; PAD_DOT				equ	(31h + FLAG_EXT)   ;; .            
; PAD_0				equ	(32h + FLAG_EXT)   ;; 0            
; PAD_1				equ	(33h + FLAG_EXT)   ;; 1            
; PAD_2				equ	(34h + FLAG_EXT)   ;; 2            
; PAD_3				equ	(35h + FLAG_EXT)   ;; 3            
; PAD_4				equ (36h + FLAG_EXT)   ;; 4            
; PAD_5				equ	(37h + FLAG_EXT)   ;; 5            
; PAD_6				equ	(38h + FLAG_EXT)   ;; 6            
; PAD_7				equ	(39h + FLAG_EXT)   ;; 7            
; PAD_8				equ	(3Ah + FLAG_EXT)   ;; 8            
; PAD_9				equ	(3Bh + FLAG_EXT)   ;; 9            
; PAD_UP				equ	PAD_8           ;; Up           
; PAD_DOWN			equ	PAD_2           ;; Down         
; PAD_LEFT			equ	PAD_4           ;; Left         
; PAD_RIGHT			equ	PAD_6           ;; Right        
; PAD_HOME			equ	PAD_7           ;; Home         
; PAD_END				equ	PAD_1           ;; End          
; PAD_PAGEUP			equ	PAD_9           ;; Page Up      
; PAD_PAGEDOWN		equ	PAD_3           ;; Page Down        
; PAD_INS				equ	PAD_0           ;; Ins          
; PAD_MID				equ	PAD_5           ;; Middle key       
; PAD_DEL				equ	PAD_DOT         ;; Del         
   
  


; u32 Key_Map[NR_SCAN_CODES * MAP_COLS] = {    

	
;key_map | key_shift_map | E0 XX |  	scan-code
; ====================================================================    
; 0,			0,     		0,			; 0x00 - none    
; ESC,			ESC,        0,			; 0x01 - ESC     
; '1',			'!',        0,			; 0x02 - '1'     
; '2',			'@',        0,			; 0x03 - '2'     
; '3',			'#',        0,			; 0x04 - '3'     
; '4',			'$',        0,			; 0x05 - '4'     
; '5',			'%',        0,			; 0x06 - '5'     
; '6',			'^',        0,			; 0x07 - '6'     
; '7',			'&',        0,			; 0x08 - '7'     
; '8',			'*',        0,			; 0x09 - '8'     
; '9',			'(',        0,			; 0x0A - '9'     
; '0',			')',        0,			; 0x0B - '0'     
; '-',			'_',        0,			; 0x0C - '-'     
; '=',			'+',        0,			; 0x0D - '='     
; BACKSPACE,	BACKSPACE,  0,			; 0x0E - BS      
; TAB,			TAB,        0,			; 0x0F - TAB     
; 'q',			'Q',        0,			; 0x10 - 'q'     
; 'w',			'W',        0,			; 0x11 - 'w'     
; 'e',			'E',        0,			; 0x12 - 'e'     
; 'r',			'R',        0,			; 0x13 - 'r'     
; 't',			'T',        0,			; 0x14 - 't'     
; 'y',			'Y',        0,			; 0x15 - 'y'     
; 'u',			'U',        0,			; 0x16 - 'u'     
; 'i',			'I',        0,			; 0x17 - 'i'     
; 'o',			'O',        0,			; 0x18 - 'o'     
; 'p',			'P',        0,			; 0x19 - 'p'     
; '[',			'{',        0,			; 0x1A - '['     
; ']',			'}',        0,			; 0x1B - ']'     
; ENTER,		ENTER,      PAD_ENTER,	; 0x1C - CR/LF   
; CTRL_L,		CTRL_L,     CTRL_R,		; 0x1D - l. Ctrl 
; 'a',			'A',        0,			; 0x1E - 'a'     
; 's',			'S',        0,			; 0x1F - 's'     
; 'd',			'D',        0,			; 0x20 - 'd'     
; 'f',			'F',        0,			; 0x21 - 'f'     
; 'g',			'G',        0,			; 0x22 - 'g'     
; 'h',			'H',        0,			; 0x23 - 'h'     
; 'j',			'J',        0,			; 0x24 - 'j'     
; 'k',			'K',        0,			; 0x25 - 'k'     
; 'l',			'L',        0,			; 0x26 - 'l'     
; ';',			':',        0,			; 0x27 - ';'     
; '/'',			'"',        0,			; 0x28 - '/''    
; '`',			'~',        0,			; 0x29 - '`'     
; SHIFT_L,		SHIFT_L,    0,			; 0x2A - l. SHIFT
; '//',			'|',        0,			; 0x2B - '/'     
; 'z',			'Z',        0,			; 0x2C - 'z'     
; 'x',			'X',        0,			; 0x2D - 'x'     
; 'c',			'C',        0,			; 0x2E - 'c'     
; 'v',			'V',        0,			; 0x2F - 'v'     
; 'b',			'B',        0,			; 0x30 - 'b'     
; 'n',			'N',        0,			; 0x31 - 'n'     
; 'm',			'M',        0,			; 0x32 - 'm'     
; ',',			'<',        0,			; 0x33 - ','     
; '.',			'>',        0,			; 0x34 - '.'     
; '/',			'?',        PAD_SLASH,	; 0x35 - '/'     
; SHIFT_R,		SHIFT_R,    0,			; 0x36 - r. SHIFT
; '*',			'*',        0,			; 0x37 - '*'     
; ALT_L,		ALT_L,      ALT_R,		; 0x38 - ALT     
; ' ',			' ',        0,			; 0x39 - ' '     
; CAPS_LOCK,	CAPS_LOCK,  0,			; 0x3A - CapsLock
; F1,     		F1,			0,			; 0x3B - F1      
; F2,     		F2,			0,			; 0x3C - F2      
; F3,     		F3,			0,			; 0x3D - F3      
; F4,     		F4,			0,			; 0x3E - F4      
; F5,     		F5,			0,			; 0x3F - F5      
; F6,     		F6,			0,			; 0x40 - F6      
; F7,     		F7,			0,			; 0x41 - F7      
; F8,     		F8,			0,			; 0x42 - F8      
; F9,     		F9,			0,			; 0x43 - F9      
; F10,			F10,        0,			; 0x44 - F10     
; NUM_LOCK,		NUM_LOCK,   0,			; 0x45 - NumLock 
; SCROLL_LOCK,	SCROLL_LOCK,0,			; 0x46 - ScrLock 
; PAD_HOME,		'7',        HOME,		; 0x47 - Home    
; PAD_UP,		'8',        UP,			; 0x48 - CurUp   
; PAD_PAGEUP,	'9',        PAGEUP,		; 0x49 - PgUp    
; PAD_MINUS,	'-',        0,			; 0x4A - '-'     
; PAD_LEFT,		'4',        LEFT,		; 0x4B - Left    
; PAD_MID,		'5',        0,			; 0x4C - MID     
; PAD_RIGHT,	'6',        RIGHT,		; 0x4D - Right   
; PAD_PLUS,		'+',        0,			; 0x4E - '+'     
; PAD_END,		'1',        END,		; 0x4F - End     
; PAD_DOWN,		'2',        DOWN,		; 0x50 - Down    
; PAD_PAGEDOWN,	'3',       PAGEDOW		; 0x51 - PgDown  
; PAD_INS,		'0',        INSERT,		; 0x52 - Insert  
; PAD_DOT,		'.',        DELETE,		; 0x53 - Delete  
; 0,			0,			0,			; 0x54 - Enter   
; 0,			0,			0,			; 0x55 - ???     
; 0,			0,			0,			; 0x56 - ???     
; F11,			F11,		0,			; 0x57 - F11     
; F12,			F12,		0,			; 0x58 - F12  
; 0,			0,			0,			; 0x59 - ???     
; 0,			0,			0,			; 0x5A - ???    	
; 0,			0,			GUI_L,		; 0x5B - ???     
; 0,			0,			GUI_R,		; 0x5C - ???     
; 0,			0,			APPS,		; 0x5D - ???     
; 0,			0,			0,			; 0x5E - ???     
; 0,			0,			0,			; 0x5F - ???     
; 0,			0,			0,			; 0x60 - ???     
; 0,			0,			0,			; 0x61 - ???     
; 0,			0,			0,			; 0x62 - ???     
; 0,			0,			0,			; 0x63 - ???     
; 0,			0,			0,			; 0x64 - ???     
; 0,			0,			0,			; 0x65 - ???     
; 0,			0,			0,			; 0x66 - ???     
; 0,			0,			0,			; 0x67 - ???     
; 0,			0,			0,			; 0x68 - ???     
; 0,			0,			0,			; 0x69 - ???     
; 0,			0,			0,			; 0x6A - ???     
; 0,			0,			0,			; 0x6B - ???     
; 0,			0,			0,			; 0x6C - ???     
; 0,			0,			0,			; 0x6D - ???     
; 0,			0,			0,			; 0x6E - ???     
; 0,			0,			0,			; 0x6F - ???     
; 0,			0,			0,			; 0x70 - ???     
; 0,			0,			0,			; 0x71 - ???     
; 0,			0,			0,			; 0x72 - ???     
; 0,			0,			0,			; 0x73 - ???     
; 0,			0,			0,			; 0x74 - ???     
; 0,			0,			0,			; 0x75 - ???     
; 0,			0,			0,			; 0x76 - ???     
; 0,			0,			0,			; 0x77 - ???     
; 0,			0,			0,			; 0x78 - ???     
; 0,			0,			0,			; 0x78 - ???     
; 0,			0,			0,			; 0x7A - ???     
; 0,			0,			0,			; 0x7B - ???     
; 0,			0,			0,			; 0x7C - ???     
; 0,			0,			0,			; 0x7D - ???     
; 0,			0,			0,			; 0x7E - ???     
; 0,			0,			0			; 0x7F - ???     