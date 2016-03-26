
; example of simplified Windows programming using complex macro features

format PE GUI 4.0

include 'F:\Flat Assembler\INCLUDE\win32ax.inc' ; you can simply switch between win32ax, win32wx, win64ax and win64wx here

.code
  start:
	invoke	MessageBox, HWND_DESKTOP, invoke GetCommandLine, "Current CommanLine is:" ,MB_OK or MB_TOPMOST
	invoke	ExitProcess,0
	
.end start

section '.rsrc' resource data readable

  ; resource directory
  directory RT_ICON,icons,\
	    RT_GROUP_ICON,group_icons,\
	    RT_VERSION,versions

  ; resource subdirectories
  resource icons,\
	   1,LANG_NEUTRAL,icon_data

  resource group_icons,\
	   17,LANG_NEUTRAL,main_icon

  resource versions,\
	   1,LANG_NEUTRAL,version


  icon main_icon,icon_data,'program.ico'

  versioninfo version,VOS__WINDOWS32,VFT_APP,VFT2_UNKNOWN,LANG_ENGLISH+SUBLANG_DEFAULT,0,\
	      'FileDescription','A tool for command line test.',\
	      'LegalCopyright','Tishion (C) All rights reserved.',\
	      'FileVersion','1.0',\
	      'ProductVersion','1.0',\
	      'OriginalFilename','Program.exe'
