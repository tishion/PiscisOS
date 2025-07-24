# Memory Mapping Tables

---
## Booting Stage

| Start Address | End Address | Size | Description | Details |
|---------------|-------------|------|-------------|---------|
| 00000000 | 00000500 | 1.25K | BIOS low [Reserved] | |
| 00000501 | 00007BFF | 30K | Boot Sector Stack | |
| 00007C00 | 00007DFF | 512B | Boot Sector [Reserved] | |
| 00007E00 | 00007FFF | 512B | Free | |
| 00008000 | 00008FFF | 4K | Free | |
| 00009000 | 0000EFFF | 24K | | |
| 0000F000 | 0000FFFF | 4K | System information | [Boot System Info Details](#boot-system-info) |

### <a id="boot-system-info"></a>Boot System Information Area (0000F000 - 0000FFFF)

| Address | Data Type | Description |
|---------|-----------|-------------|
| 0000F000 | word | System Total Memory size |
| 0000F100 | dword | |

---
## Runtime (32 bit Protect Mode)

| Start Address | End Address | Size | Description | Details |
|---------------|-------------|------|-------------|---------|
| 00000000 | 00000FFF | 4K | Free | |
| 00001000 | 00004F3FF | 16K | Process Control Blocks | [PCB Details](#pcb-structure) |
| 00004F40 | 00004FEF | 176B | Free | |
| 00004FF0 | 00004FEF | 16B | PCB Management Area | [PCB Management Details](#pcb-management) |
| 00003000 | 00003FFF | 4K | Free | |
| 00004000 | 00004FFF | 4K | Free | |
| 00005000 | 00005FFF | 4K | Free | |
| 00006000 | 00006FFF | 4K | Free | |
| 00007000 | 00007FFF | 4K | Free | |
| 00008000 | 00008FFF | 4K | Free | [Display Control Details](#display-control) |
| 00009000 | 0000EFFF | 24K | Display String temp buffer | |
| 0000F000 | 0000FFFF | 4K | System informations | [Runtime System Info Details](#runtime-system-info) |
| 0000F100 | 0000F1FF | 256B | Keyboard Ascii Code Buffer | [Keyboard Buffer Details](#keyboard-buffer) |
| 00010000 | 0004FFFF | 256K | Kernel Bin | |
| 00050000 | 0006FFFF | 128K | Stack | [Stack Details](#stack-area) |
| 00070000 | 00090000 | 128K | Free | |
| 00090000 | 0009FFFF | 64K | Reserved | |
| 000A0000 | 000AFFFF | 64K | Screen access area [Reserved] | |
| 000B0000 | 000FFFFF | 320K | Bios [Reserved] | |
| 00100000 | 0027FFFF | 1.5M | diskette image | |
| 00280000 | 0028FFFF | 64K | diskette fat | |
| 00290000 | 0029FFFF | 64K | Low Memory | |
| 002A0000 | 002A7FFF | 32K | TSS block of interrupts | |
| 002A8000 | 002AFFFF | 32K | TSS block of system calls - 256 entries | |
| 002B0000 | 002B0FFF | 4K | Page Directory Table | |
| 002B1000 | 002BFFFF | 60K | Free | |
| 002C0000 | 002FFFFF | 256K | Stacks of interrupts[per stack 1024 bytes, Max 256 stacks] | |
| 00300000 | 003FFFFF | 1M | Stacks of syscalls[per stack 4096 bytes, Max 256 stacks] | |
| 00400000 | 007FFFFF | 4M | Page Table Entry 4M reserved for MAX 4G Memory | |
| 00800000 | 009FFFFF | 2M | TSS block of processes [(128 bytes tss data + 8192 bytes IO map)*250 processes] | |
| 00A00000 | 0FA00000 | 240M | Application mem, One process use 1M byte memory (1M*250) | [Application Memory Details](#application-memory) |


### <a id="pcb-structure"></a>PCB Structure (00001000 - 00004F3FF)

Each PCB entry follows this structure:

| Offset | Address | Data Type | Description |
|--------|---------|-----------|-------------|
| +0x00 | 00001000 | dword | PID(process identifier) |
| +0x04 | 00001004 | dword | parent PID |
| +0x08 | 00001008 | dword | reserved |
| +0x0C | 0000100c | dword | reserved |
| +0x10 | 00001010 | dword | base memory address of the process image |
| +0x14 | 00001014 | dword | index of syscall TSS descriptor this process is using |
| +0x18 | 00001018 | dword | reserved |
| +0x1C | 0000101c | dword | reserved |
| +0x20 | 00001020 | dword | Tick count |
| +0x24 | 00001024 | dword | reserved |
| +0x28 | 00001028 | dword | reserved |
| +0x2C | 0000102c | dword | reserved |
| +0x30 | 00001030 | byte | process status |
| +0x31 | 00001031 | byte | PCB Status |
| +0x32 | 00001032 | word | reserved |
| +0x34 | 00001034 | dword | PID of the process which is waiting for this process |
| +0x38 | 00001038 | dword | reserved |
| +0x3C | 0000103c | dword | reserved |

### <a id="pcb-management"></a>PCB Management Area (00004FF0 - 00004FFF)

| Address | Data Type | Description |
|---------|-----------|-------------|
| 00004FF0 | dword | index number of current running process |
| 00004FF4 | dword | base of PCB of current running process, =PCB_TABLE_BASE+pcb_current_no*PCB_SIZE |
| 00004FF8 | dword | total count of processes in current system |

### <a id="display-control"></a>Display Control Area (00008000 - 00008FFF)

| Address | Data Type | Description |
|---------|-----------|-------------|
| 00008000 | word | row_start - Number of start row in current screen |
| 00008002 | word | x_cursor - Col of Cursor |
| 00008004 | word | y_cursor - Row of Cursor |

### <a id="runtime-system-info"></a>Runtime System Information Area (0000F000 - 0000FFFF)

| Address | Data Type | Description |
|---------|-----------|-------------|
| 0000F000 | dword | System Total Memory size |
| 0000F008 | dword | Pid Init |
| 0000F010 | dword | System Tick count |
| 0000F020 | byte | keyboard LEDS status change 0:no change 1:changed |
| 0000F021 | byte | keyboard LEDS changed num : 0 = capsl 1 = numl 2 = scrl |

### <a id="keyboard-buffer"></a>Keyboard Buffer (0000F100 - 0000F1FF)

| Address | Data Type | Description |
|---------|-----------|-------------|
| 0000F100-0000F17F | | Queue buffer 128 bytes |
| 0000F190 | byte | Queue buffer head |
| 0000F1A0 | byte | Queue buffer tail |
| 0000F1B0 | byte | Queue data count |

### <a id="stack-area"></a>Stack Area (00050000 - 0006FFFF)

| Address | Description |
|---------|-------------|
| 0005FFF0 | 16 bit Kernel stack top |
| 0006C000 | 32 bit Kernel stack top |
| 0006D000 | esp0=6D000-50000 |
| 0006E000 | esp1=6E000-50000 |
| 0006F000 | esp2=6F000-50000 |

### <a id="application-memory"></a>Application Memory Layout (00A00000 - 0FA00000)

Each process uses 1M byte memory with the following segments:

| Segment | Address Range | Size | Description |
|---------|---------------|------|-------------|
| Code Segment | 0-100000 | 1M | Executable code |
| Data Segment | 0-100000 | 1M | Data storage |
| Stack Segment | 0-100000 | 1M | Stack space, esp = 0FFFF0 |

**Total:** 250 processes × 1M = 250M memory space