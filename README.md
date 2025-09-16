# PM/0 Virtual Machine (C)

A stack-based **P-machine (PM/0)** virtual machine written in C11.  
Loads `OP L M` triples from a text file into a 500-cell array; stack grows downward.

## Build
gcc -std=c11 -Wall -Wextra -O2 -o vm vm.c

## Run
./vm program.txt

**SYS ops:**  
- `9 0 1` → print top of stack  
- `9 0 2` → read integer  
- `9 0 3` → halt
