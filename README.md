# MYCC open source C compiler
The goal of this project is to build a minimalistic C compiler that does not require a PhD to understand and modify. 

## Features
* Targetting x86 32bit instruction set on the Linux platform
* Targetting m68k instruction set on Amiga OS
* Procedures, pointers, arrays, global variables, local variables, char, short, long (32bit)  

## Usage

### Build

For X86 architecture
```bash
  gcc -D X86 main.c -o main
```

For Motorola 68000 architecture
```bash
  gcc -D M68K main.c -o main
```


### Compile code

```bash
  ./main tests/fibo.c
```

### Run unit test suite

Requires the test library cmocka
```bash
  ./run-mocka.sh
```

### Run compilation test suite

These tests are samples programs compiled for x86 which output are compared to the expected result. 
```bash
  ./run-tests.sh
```

## Missing features

* No operator priority : expression are evaluated from left to right
* No data structure and enum
* No pre-processor
* No floating point support
* No standard library support
* "return" have to be explicit even for procedures returning void

## Amiga code generation

MYCC can output Motorola 68000 code for Amiga OS. It generates an executable in the "HUNK" file format.
In addition, the compilers support inline ASM.

Here is an "Hello World" example for Amiga OS.
```C
char dos_name[] = "dos.library";
char hello[] = "Hello World !";

void * open_library(char * lib_name) {
    void * lib_ptr;
    asm {
        move.l ($4).w, a6
        move.l lib_name, a1
        move.l #0, d0
        jsr -552(a6)
        move.l d0, lib_ptr      
    }
    return lib_ptr;
}

void close_library(void * lib_ptr) {
    asm {
        move.l ($4).w, a6
        move.l lib_ptr, a1
        jsr -414(a6)        
    }
    return;
}

void dos_put_str(char * msg, void * dos_lib_ptr) {
    asm {
        move.l dos_lib_ptr, a6
        move.l msg, d1
        jsr -948(a6)        
    }
    return;
}

int main(int argc, char **argv) {
    void * dos_lib_ptr = open_library(dos_name);
    dos_put_str(hello, dos_lib_ptr);
    close_library(dos_lib_ptr);
    return 0;
}
```
Code generation is particular because without MMU, the code to run can be placed anywhere in memory by the operating system. The good practice is to make code location independent by using position relative memory location.
Motorola 68000 support an addressing mode called "PC + displacement", which allow to address memory location relative to the program counter with a caveat though, that the displacement is a signed 16 bit value, which means that it can only target addresses in the range -32kb, + 32kb. 
This limit can be override by declaring a relocation segment in the executable file, the relocation process is a mean to tell the program loader to add the absolute adress of the program to the offset set at designated places in the executable after having loaded it in memory. 

A little explanation of the sample code: to be able to output text in a shell in AmigaOS, we have to call PutStr() from the "dos.library". First we need to open the library by using the "open_library" system call.

### Motorola 68000 architecture

TODO

### Assembly syntax

TODO

## ELF 32bit executable format

The executable format genereted by the compiler is "ELF 32bit" which is the standard executable format for 
Linux 32bit systems, it can also be ran on Linux 64bit system.

All ELF files contain a header which indicates various informations on the file content. One important information is the "entry" field which is the memory address where execution will start.

After the header, an ELF file contains a list of the section of different types.
To be straight on point with code generation, we will have to insert a "program section", containing a section header followed by the program binary code.
The header section contains a field "sh_addr" that indicated the adress where the first byte of the program binary code will we placed in memory.

The entry point field in the file header will point to an adress somewhere in the program code where the main() C function starts.

## Minimal X86 32 bit architecture knowledge

| Registers     | Width | Usage                                | Compiler use
|---------------|-------|--------------------------------------|--------------------
| EAX           |  32   | Generic / Can be divided into 16 bits / 8 bits registers  | Store the last value processed
| EBX           |  32   | Generic                              | Used to store the second hand of comparison
| ECX           |  32   | Generic                              | Not used
| EDX           |  32   | Generic                              | Not used
| EIP           |  32   | Pointer to the current instruction   | Changed by using "CALL" instruction to run a function or jumps to reach another instruction block
| ESP           |  32   | Pointer to the top of the stack      | Stack stores local variables and function arguments

## Minimal ABI knowledge

* Function call parameters are stored at the top of the stack
* Function returned value is stored in the EAX register
* Local variables are stored in the stack, access is stack pointer relative
* Global variables are stored in memory at an absolute adress

## Algorithm
Very simple stuff

1. Tokenizing pass : transform characters and expressions into tokens
2. AST (sort of) pass : creates a tree starting from the top of the source file
3. Code generation pass : browses through tokens and generate the corresponding machine code

One main function processes one program step (which is a variable notion depending of the expression it encounters) and returns the last token processed :
```C
  T_NODE * step(T_NODE * up, int stack_offset, T_BUFFER * buffer)
```

Then one function processes many step until the end of the line is reached
```C
  T_NODE * line(T_NODE * up, int stack_offset, T_BUFFER * buffer)
```

Another function iterates many lines until the end of block is reached marked by: { ... }
```C
  T_NODE * block(T_NODE * up, int stack_offset, T_BUFFER * buffer)
```

## Development process

1. Write tests, lot of tests !
2. Refer to x86 documentation, opcodes conversions in particular:
    https://defuse.ca/online-x86-assembler.htm#disassembly
3. With GDB, place breakpoint using the compiler output which gives address for each program instruction
4. Profit !

## Useful docs
Linux asm system call conventions in x86 32bit architecture:
https://www.informatik.htw-dresden.de/~beck/ASM/syscall_list.html

ELF file format documentation:
http://www.skyfree.org/linux/references/ELF_Format.pdf

The famous "Dragon book":
https://fr.wikipedia.org/wiki/Dragon_book

