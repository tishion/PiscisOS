PiscisOS
=====
a tiny multi-task operating system based x86 architecture, run in protect mode

Build in one step and Run
=======

if you want to build in one step just run:
 
> build.bat

and also you can add paremeter -run to launch it with bochs (you need to install bochs first)

> build.bat -run

enjoy this tiny OS


Build components
========
if you want to build some components, you can do as follows

1.  install Flat assembler, add the environment path

2.  build the boot sector
> cd X:\xxxxx\PiscisOS\src\boot
>
> fasm bootsect.asm

  you will get X:\xxxxx\PiscisOS\out\bootsect.bin

3. build kernel
> cd X:\xxxxx\PiscisOS\src\
>
> fasm pkernel.asm
    
  then you will get X:\xxxxx\PiscisOS\out\pkernel.bin
  
4. build shell
> cd X:\xxxxx\PiscisOS\src\shell
>
> fasm shell.asm
  
  then you will get X:\xxxxx\PiscisOS\out\shell.bin
  
4. make image file.....
