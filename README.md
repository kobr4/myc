# MYCC open source C compiler
The goal of this project is the build a minimalistic C compiler that do not requires a PhD to understand and modify. 

## Features
* Targetting x86 32bit instruction set on the Linux platform
* Highly incomplete currently
* Should be retargetable in the futrue

## Algorithm
Very simple stuff

1. Tokenizing pass : transform character and expressions into tokens
2. AST (sort of) pass : creates a tree starting from the top of the source file
3. Code generation pass : browse through tokens and generate the corresponding machine code

One main function processes one program step (which is a variable notion depending of the expression in encounter) and returns the last token processed :
{
  T_NODE * step(T_NODE * up, int stack_offset, T_BUFFER * buffer)
}

Then one function processes many step until the end of the line is reached
{
  T_NODE * line(T_NODE * up, int stack_offset, T_BUFFER * buffer)
}

Another function iterate many blocks until the end of block is reached marked by: { ... }
{
  T_NODE * block(T_NODE * up, int stack_offset, T_BUFFER * buffer)
}
