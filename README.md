# MYCC open source C compiler
The goal of this project is to build a minimalistic C compiler that do not requires a PhD to understand and modify. 

## Features
* Targetting x86 32bit instruction set on the Linux platform
* Highly incomplete currently
* Should be retargetable in the future

## Minimal X86 32 bit architecture knowledge

| Registers     | Width | Usage                                | Compiler use
|---------------|-------|--------------------------------------|--------------------
| EAX           |  32   | Generic / Can be divided into 16 bits / 8 bits registers  | Store the last value processed
| EBX           |  32   | Generic                              | Used to store the second hand of comparison
| ECX           |  32   | Generic                              | Not used
| EDX           |  32   | Generic                              | Not used
| EIP           |  32   | Pointer to the current instruction   | Changed by using "CALL" instruction to run a function
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
