=================================
tishion 2012/06/08

Source code of the PiscisOS.

if you want build in one batch file
just run build.bat


if you want to build some components, you can do as follows

build:

1. install Flat assembler, add the environment path

2. build the boot sector
  2.1. cd  X:\xxxxx\PiscisOS\src\boot
  2.2. run "fasm bootsect.asm", you will get bootsect.bin

3. build kernel
  3.1. cd  X:\xxxxx\PiscisOS\src\
  3.2. run "fasm pkernel.asm", then you will get pkernel.bin

4. make image file.....
