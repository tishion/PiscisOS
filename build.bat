@echo off
rem Build batch file for Piscos OS
rem Author: Tishion (tishion#163.com)
rem 2016-03-26 11:58:38

rem change work directory to build folder
set PROJ_ROOT=%~dp0
set FASM_PATH=%PROJ_ROOT%tools\Flat Assembler\FASM.EXE
set SRC_ROOT=%PROJ_ROOT%src\
set SRC_APPS=%SRC_ROOT%apps\
set OUT_ROOT=%PROJ_ROOT%out\
set OUT_APPS=%OUT_ROOT%\bin\

cd /d %PROJ_ROOT%

rem create output folders
if not exist "%OUT_ROOT%" mkdir "%OUT_ROOT%"
if not exist "%OUT_APPS%" mkdir "%OUT_APPS%"

rem build bootsector file
echo ====== Building bootsector... ====== 
call "%FASM_PATH%" "%SRC_ROOT%bootsector\bootsect.asm" -s "%OUT_ROOT%bootsector.sym" "%OUT_ROOT%bootsector"
if not %errorlevel% == 0 (
	goto _l_end
)
echo.

rem build kernel file
echo ====== Building pkernel... ====== 
call "%FASM_PATH%" "%SRC_ROOT%kernel\pkernel.asm" -s "%OUT_ROOT%pkernel.sym" "%OUT_ROOT%pkernel.bin"
if not %errorlevel% == 0 (
	goto _l_end
)
echo.

rem build shell file
echo ====== Building shell... ====== 
call "%FASM_PATH%" "%SRC_ROOT%shell\shell.asm" -s "%OUT_ROOT%shell.sym" "%OUT_ROOT%shell"
if not %errorlevel% == 0 (
	goto _l_end
)
echo.

rem build apps
echo ====== Building applications... ======
for /R "%SRC_APPS%" %%i in (*.asm) do (
	echo +Building %%i
	call "%FASM_PATH%" "%%i" -s "%OUT_APPS%%%~ni.sym" "%OUT_APPS%%%~ni"
)
echo.

echo ====== Building OS image... ======

:_l_end