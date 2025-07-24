# System Call Definitions

## sysc_gettick
**System Call ID:** `00h`

**Input:**
- `eax`: `00h`

**Output:**
- `eax`: tick count

---

## sysc_getkey
**System Call ID:** `01h`

**Input:**
- `eax`: `01h`

**Output:**
- `eax`: key ascii code

---

## sysc_screenandcursor
**System Call ID:** `02h`

**Input:**
- `eax`: `02h`
- `ebx`: Operation type
  - `0` = clear screen
  - `1` = setcursorpos
    - `ecx`: X|Y coordinates
  - `2` = getcursorpos

**Output:**
- For `ebx=2` (getcursorpos):
  - `ecx`: X|Y coordinates

---

## sysc_putchar
**System Call ID:** `03h`

**Input:**
- `eax`: `03h`
- `bl`: char ascii code

**Output:**
- `eax`: (return value)

---

## sysc_print
**System Call ID:** `04h`

**Input:**
- `eax`: `04h`
- `ebx`: Position specification
  - `bh` = row, `bl` = column
  - If `ebx = 0xffffffff`, print string at current cursor position
- `edi`: buffer of string to print

**Output:**
- `eax`: (return value)

---

## sysc_time
**System Call ID:** `05h`

**Input:**
- `eax`: `05h`

**Output:**
- `eax`: `|notuse|s|m|h|` (BCD format)

---

## sysc_date
**System Call ID:** `05h`

**Input:**
- `eax`: `05h`

**Output:**
- `eax`: `|dw|d|m|y|` (BCD format - day of week, day, month, year)

---

## sysc_createprocess
**System Call ID:** `10h`

**Input:**
- `eax`: `10h`
- `ebx`: arguments buffer
- `edi`: buffer of path

**Output:**
- `eax`: (return value)

---

## sysc_exitprocess
**System Call ID:** `11h`

**Input:**
- `eax`: `11h`

**Output:**
- (none)

---

## sysc_waitpid
**System Call ID:** `12h`

**Input:**
- `eax`: `12h`
- `ebx`: pid to be waited

**Output:**
- `eax`: If function succeeds, will not return 0 until the process exits. If failed, returns -1 immediately.

---

## sysc_rdfs
**System Call ID:** `20h`

**Input:**
- `eax`: `20h`
- `ebx`: Operation type
  - `0` = GetFileAttribute
    - `edi`: path
    - `ecx`: buffer to save file item
  - `1` = readfile
    - `edi`: path
    - `ecx`: buffer to save the file
  - `2` = writefile
    - `edi`: path
    - `ecx`: buffer to be saved into file

**Output:**
- For `ebx=0` (GetFileAttribute):
  - `eax`: -1 if not found
- For other operations:
  - `eax`: (return value)