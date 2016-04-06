### About PiscisOS
PiscisOS is a tiny operation system based x86 architecture. This system is developed with assembly language(every piece of the code). 
Features of PiscisOS:
* Run in x86 protect model
* Multi-task supported (Maximum 250 processes)
* Ram Disk file system (FAT12)
* System calls
* Application supported

PiscisOS is a very simple system, I write it just for learning some knowledge about x86 protect model. You know the file system used by PiscisOS is very very old. But this is not the key point, if you are interested in this project, you can replace the filesystem with whatever you want. :)

### Build PiscisOS
For one step building & burning image files, you cna clone the repository and just run the build.bat.
```
F:\Private Proj\PiscisOS>build.bat
====== Building bootsector... ======
flat assembler  version 1.71.51  (1048576 kilobytes memory)
3 passes, 512 bytes.

====== Building pkernel... ======
flat assembler  version 1.71.51  (1048576 kilobytes memory)
4 passes, 28980 bytes.

====== Building shell... ======
flat assembler  version 1.71.51  (1048576 kilobytes memory)
3 passes, 2759 bytes.

====== Building applications... ======
+Building F:\Private Proj\PiscisOS\src\apps\date\date.asm
flat assembler  version 1.71.51  (1048576 kilobytes memory)
2 passes, 408 bytes.
+Building F:\Private Proj\PiscisOS\src\apps\debug\debug.asm
flat assembler  version 1.71.51  (1048576 kilobytes memory)
2 passes, 309 bytes.
+Building F:\Private Proj\PiscisOS\src\apps\echo\echo.asm
flat assembler  version 1.71.51  (1048576 kilobytes memory)
2 passes, 303 bytes.
+Building F:\Private Proj\PiscisOS\src\apps\hello\hello.asm
flat assembler  version 1.71.51  (1048576 kilobytes memory)
2 passes, 317 bytes.
+Building F:\Private Proj\PiscisOS\src\apps\ls\ls.asm
flat assembler  version 1.71.51  (1048576 kilobytes memory)
3 passes, 11083 bytes.
+Building F:\Private Proj\PiscisOS\src\apps\time\time.asm
flat assembler  version 1.71.51  (1048576 kilobytes memory)
2 passes, 404 bytes.

====== Burning OS image... ======
+Creating image file with bootsector...
+Copying perkenel.bin to image file system...
+Copying shell to image file system...
+Create bin folder in image file system...
+Copying all applications to image file system...
Build and burn done successfully!
Output floppy image file: F:\Private Proj\PiscisOS\image\piscisos.img
```
Now you have got the image file and you can load it with virtual machine.


=================================
tishion 2012/06/08

Source code of the PiscisOS.

if you want to build in one step
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
