@echo off
rem Build batch file for Piscis OS
rem Author: Tishion (tishion#163.com)
rem 2016-03-26 11:58:38

set RUN=%~1%
echo %RUN%

rem change work directory to build folder
set PROJ_ROOT=%~dp0
set SRC_ROOT=%PROJ_ROOT%\src
set SRC_APPS=%SRC_ROOT%\apps
set BUILD_DIR=%PROJ_ROOT%\build
set OUT_ROOT=%BUILD_DIR%\out
set IMG_ROOT=%BUILD_DIR%\image
set IMG_NAME=piscisos.img
set IMG_PATH=%IMG_ROOT%\%IMG_NAME%
set BOCHS_SCRIPT=%IMG_ROOT%\bochsrc.bxrc
set OUT_APPS=%OUT_ROOT%\app
set MTOOL_ROOT=%PROJ_ROOT%\tools\mtools

cd /d %PROJ_ROOT%

rem create output folders
if not exist "%OUT_ROOT%" mkdir "%OUT_ROOT%"
if not exist "%OUT_APPS%" mkdir "%OUT_APPS%"
if not exist "%IMG_ROOT%" mkdir "%IMG_ROOT%"

rem build bootsector file
echo ====== Building bootsector... ====== 
call fasm "%SRC_ROOT%\boot\bootsect.asm" -s "%OUT_ROOT%\bootsector.sym" "%OUT_ROOT%bootsector"
if not %errorlevel% == 0 (
	goto _l_end
)
echo.

rem build kernel file
echo ====== Building pkernel... ====== 
call fasm "%SRC_ROOT%\kernel\pkernel.asm" -s "%OUT_ROOT%\pkernel.sym" "%OUT_ROOT%pkernel.bin"
if not %errorlevel% == 0 (
	goto _l_end
)
echo.

rem build shell file
echo ====== Building shell... ====== 
call fasm "%SRC_ROOT%\shell\shell.asm" -s "%OUT_ROOT%\shell.sym" "%OUT_ROOT%shell"
if not %errorlevel% == 0 (
	goto _l_end
)
echo.

rem build apps
echo ====== Building applications... ======
for /R "%SRC_APPS%" %%i in (*.asm) do (
	echo +Building %%i
	call fasm "%%i" -s "%OUT_APPS%\%%~ni.sym" "%OUT_APPS%\%%~ni"
)
echo.

echo ====== Burning OS image... ======

echo +Creating image file with bootsector...
"%MTOOL_ROOT%\mformat.exe" -f 1440 -v PiscisOSVOL -B "%OUT_ROOT%\bootsector" -C -i "%IMG_PATH%" ::
if not %errorlevel% == 0 (
	goto _l_end
)	

echo +Copying perkenel.bin to image file system...
"%MTOOL_ROOT%\mcopy.exe" -i "%IMG_PATH%" "%OUT_ROOT%\pkernel.bin" ::
if not %errorlevel% == 0 (
	goto _l_end
)

echo +Copying shell to image file system...
"%MTOOL_ROOT%\mcopy.exe" -i "%IMG_PATH%" "%OUT_ROOT%\shell" ::
if not %errorlevel% == 0 (
	goto _l_end
)

echo +Creating bin folder in image file system...
"%MTOOL_ROOT%\mmd.exe"   -i "%IMG_PATH%" ::bin
if not %errorlevel% == 0 (
	goto _l_end
)

echo +Copying all applications to image file system...
"%MTOOL_ROOT%\mcopy.exe" -i "%IMG_PATH%" "%OUT_ROOT%\app\*" ::bin
if not %errorlevel% == 0 (
	goto _l_end
)

echo Build and burn done successfully!
echo Output floppy image file: %IMG_PATH%

echo +Creating bochs script...
echo floppya: type=1_44, 1_44="%IMG_PATH%", status=inserted, write_protected=1 > %BOCHS_SCRIPT%
echo Done!

if "%RUN%" == "-run" (
	%BOCHS_SCRIPT%
)

:_l_end












