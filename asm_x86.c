#include "main.h"
#include "elf32.h"
#include <stdio.h>
#include <stdlib.h>

void exec_varsize(T_NODE * n, int offset, T_BUFFER * buffer, void (*f1)(int offset, T_BUFFER * buffer), 
    void (*f2)(int offset, T_BUFFER * buffer), void (*f4)(int offset, T_BUFFER * buffer)) {
    switch(variable_size(n)) {
        case 1:
            f1(offset, buffer);
            break;
        case 2:
            f2(offset, buffer);
            break;
        case 4: 
            f4(offset, buffer);            
            break;
        default:
            error_elt(n->elt, "Unsupported variable size");
            break;
    }
}

void write_buffer_2(T_BUFFER * buffer, U8 a, U8 b) {
    buffer->buffer[buffer->length++] = a;
    buffer->buffer[buffer->length++] = b;
}

void write_buffer_3(T_BUFFER * buffer, U8 a, U8 b, U8 c) {
    buffer->buffer[buffer->length++] = a;
    buffer->buffer[buffer->length++] = b;
    buffer->buffer[buffer->length++] = c;
}

void write_buffer_4(T_BUFFER * buffer, U8 a, U8 b, U8 c, U8 d) {
    buffer->buffer[buffer->length++] = a;
    buffer->buffer[buffer->length++] = b;
    buffer->buffer[buffer->length++] = c;
    buffer->buffer[buffer->length++] = d;
}

void write_buffer_dword(T_BUFFER * buffer, U32 value) {
    memcpy( &(buffer->buffer[buffer->length]), &value , sizeof(value));
    buffer->length += 4;
}

// mov EAX, value
void asm_load_eax(U32 value, T_BUFFER * buffer) {
    printf("[ASM][%x] mov eax,  %d\n", buffer->length+START, value);
    buffer->buffer[buffer->length++] = 0xB8; 
    write_buffer_dword(buffer, value);
} 

// mov EBX, value
void asm_load_ebx(U32 value, T_BUFFER * buffer) {
    printf("[ASM][%x] mov ebx,  %d\n", buffer->length+START, value);
    buffer->buffer[buffer->length++] = 0xBB; 
    write_buffer_dword(buffer, value);
} 

// mov AX, value
void asm_load_ax(U16 value, T_BUFFER * buffer) {
    printf("[ASM][%x] mov ax,  %d\n", buffer->length+START, value);
    write_buffer_2(buffer, 0x66, 0xB8);
    memcpy( &(buffer->buffer[buffer->length]), &value , sizeof(value));
    buffer->length += 2;
} 

// mov AL, value
void asm_load_al(U8 value, T_BUFFER * buffer) {
    printf("[ASM][%x] mov ax,  %d\n", buffer->length+START, value);
    write_buffer_2(buffer, 0xB0, value);
} 


//sub esp, size
void asm_add_variable(T_NODE * up, T_BUFFER * buffer) {
    U32 size  = variable_size(up);
    printf("[ASM][%x] sub esp, %d\n", buffer->length+START, size);
    if ( (size >> 8) == 0 ) {
        write_buffer_3(buffer, 0x83, 0xEC, size);
    } else {
        write_buffer_2(buffer, 0x81, 0xEC);
        write_buffer_dword(buffer, size);
    }
}

// mov eax, DWORD PTR SS:[esp]
void asm_retrieve_variable_nested(int offset, T_BUFFER * buffer) {
    if ( (offset >> 8) == 0 ) {
        write_buffer_4(buffer, 0x8B, 0x44, 0x24, offset);
    } else {
        write_buffer_3(buffer, 0x8B, 0x84, 0x24);
        write_buffer_dword(buffer, offset);
    }
}

// mov ax, DWORD PTR SS:[esp]
void asm_retrieve_variable_ax(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov ax, WORD PTR SS:[esp + %d]\n", buffer->length+START, offset);
    buffer->buffer[buffer->length++] = 0x66;
    asm_retrieve_variable_nested(offset, buffer);
}

// mov eax, DWORD PTR SS:[esp]
void asm_retrieve_variable_eax(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov eax, DWORD PTR SS:[esp + %d]\n", buffer->length+START, offset);
    asm_retrieve_variable_nested(offset, buffer);
}

// mov al, BYTE PTR SS:[esp]
void asm_retrieve_variable_al(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov al, BYTE PTR SS:[esp + %d]\n", buffer->length+START, offset);
    if ( (offset >> 8) == 0 ) {
        write_buffer_4(buffer, 0x8A, 0x44, 0x24, offset);
    } else {
        write_buffer_3(buffer, 0x8A, 0x84, 0x24);
        write_buffer_dword(buffer, offset);
    }
}

void asm_retrieve_variable(T_NODE * n, int offset, T_BUFFER * buffer) {
    exec_varsize(n, offset, buffer, asm_retrieve_variable_al, asm_retrieve_variable_ax, asm_retrieve_variable_eax);
}

// mov ebx, DWORD PTR SS:[esp]
void asm_retrieve_variable_ebx(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov ebx, DWORD PTR SS:[esp + %d]\n", buffer->length+START, offset);
    write_buffer_4(buffer, 0x8B, 0x5C, 0x24, offset);
}

// mov ebx, [offset]
void asm_retrieve_variable_ebx_mem(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov ebx, [%x]\n", buffer->length+START, offset);
    write_buffer_2(buffer, 0x8B, 0x1D);
    write_buffer_dword(buffer, offset);
}

//MEM
// mov eax, [offset]
void asm_retrieve_variable_nested_mem(int offset, T_BUFFER * buffer) {
    buffer->buffer[buffer->length++] = 0xA1;
    write_buffer_dword(buffer, offset);
}

// mov ax, [offset]
void asm_retrieve_variable_ax_mem(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov ax, [%x]\n", buffer->length+START, offset);
    buffer->buffer[buffer->length++] = 0x66;
    asm_retrieve_variable_nested_mem(offset, buffer);
}

// mov eax, [offset]
void asm_retrieve_variable_eax_mem(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov eax, [%x]\n", buffer->length+START, offset);
    asm_retrieve_variable_nested_mem(offset, buffer);
}

// mov al, [offset]
void asm_retrieve_variable_al_mem(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov al, [%x]\n", buffer->length+START, offset);
    buffer->buffer[buffer->length++] = 0xA0;
    write_buffer_dword(buffer, offset);
}

void asm_retrieve_variable_mem(T_NODE * n, int offset, T_BUFFER * buffer) {
    exec_varsize(n, offset, buffer, asm_retrieve_variable_al_mem, asm_retrieve_variable_ax_mem, asm_retrieve_variable_eax_mem);
}

void asm_add_variable_and_store_nested(int offset, T_BUFFER * buffer) {
    if ( (offset >> 8) == 0 ) 
        write_buffer_4(buffer, 0x01, 0x44, 0x24, offset);
    else {
        write_buffer_3(buffer, 0x01, 0x84, 0x24);
        write_buffer_dword(buffer, offset);
    }
}

//add WORD PTR SS:[esp], ax
void asm_add_variable_and_store_ax(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] add WORD PTR SS:[esp + %d], ax\n", buffer->length+START, offset);
    buffer->buffer[buffer->length++] = 0x66;
    asm_add_variable_and_store_nested(offset, buffer);
}

//add DWORD PTR SS:[esp], eax
void asm_add_variable_and_store_eax(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] add DWORD PTR SS:[esp + %d], eax\n", buffer->length+START, offset);
    asm_add_variable_and_store_nested(offset, buffer);
}

//add BYTE PTR SS:[esp], al
void asm_add_variable_and_store_al(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] add BYTE PTR SS:[esp + %d], al\n", buffer->length+START, offset);
    if ( (offset >> 8) == 0 ) {
        write_buffer_4(buffer, 0x00, 0x44, 0x24, offset);
    } else {
        write_buffer_3(buffer, 0x00, 0x84, 0x24);
        write_buffer_dword(buffer, offset);        
    }
}

void asm_add_variable_and_store(T_NODE * n, int offset, T_BUFFER * buffer) {
    exec_varsize(n, offset, buffer, asm_add_variable_and_store_al, asm_add_variable_and_store_ax, asm_add_variable_and_store_eax);
}


//sub DWORD PTR SS:[esp], eax
void asm_sub_variable_and_store_nested(int offset, T_BUFFER * buffer) {
    if ( (offset >> 8) == 0 ) {
        write_buffer_4(buffer, 0x29, 0x44, 0x24, offset);
    } else {
        write_buffer_3(buffer, 0x29, 0x84, 0x24);
        write_buffer_dword(buffer, offset); 
    }
}

//sub WORD PTR SS:[esp], eax
void asm_sub_variable_and_store_eax(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] sub DWORD PTR SS:[esp + %d], eax\n", buffer->length+START, offset);
    asm_sub_variable_and_store_nested(offset, buffer);
}

//sub WORD PTR SS:[esp], ax
void asm_sub_variable_and_store_ax(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] sub WORD PTR SS:[esp + %d], ax\n", buffer->length+START, offset);
    buffer->buffer[buffer->length++] = 0x66;
    asm_sub_variable_and_store_nested(offset, buffer);
}

//sub BYTE PTR SS:[esp], al
void asm_sub_variable_and_store_al(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] sub BYTE PTR SS:[esp + %d], al\n", buffer->length+START, offset);
    if ( (offset >> 8) == 0 ) {
        write_buffer_4(buffer, 0x28, 0x44, 0x24, offset);
    } else {
        write_buffer_3(buffer, 0x28, 0x84, 0x24);
        write_buffer_dword(buffer, offset); 
    }
}

void asm_sub_variable_and_store(T_NODE * n, int offset, T_BUFFER * buffer) {
    exec_varsize(n, offset, buffer, asm_sub_variable_and_store_al, asm_sub_variable_and_store_ax, asm_sub_variable_and_store_eax);
}

//mov eax, esp
void asm_mov_eax_esp(T_BUFFER * buffer) {
    printf("[ASM][%x] mov eax, esp\n", buffer->length+START);
    write_buffer_2(buffer, 0x89, 0xE0);
}


//sub eax, value
void asm_sub_eax_value(T_BUFFER * buffer, int value) {
    printf("[ASM][%x] sub eax, %d\n", buffer->length+START, value);    
    buffer->buffer[buffer->length++] = 0x2D;
    write_buffer_dword(buffer, value);
}

//add eax, value
void asm_add_eax_value(T_BUFFER * buffer, int value) {
    printf("[ASM][%x] add eax, %d\n", buffer->length+START, value);  
    buffer->buffer[buffer->length++] = 0x05;
    write_buffer_dword(buffer, value);
}

//mov WORD PTR SS:[esp], al
void asm_store_variable_al(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov BYTE PTR SS:[esp + %d], al\n", buffer->length+START, offset);
    if ( (offset >> 8) == 0 ) {
        write_buffer_4(buffer, 0x88, 0x44, 0x24, offset);
    } else {
        write_buffer_3(buffer, 0x88, 0x84, 0x24);
        write_buffer_dword(buffer, offset);
    }    
}

//mov DWORD PTR SS:[esp], ax
void asm_store_variable_ax(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov DWORD PTR SS:[esp + %d], ax\n", buffer->length+START, offset);
    if ( (offset >> 8) == 0 ) {
        write_buffer_4(buffer, 0x66, 0x89, 0x44, 0x24);
        buffer->buffer[buffer->length++] = offset;
    } else {
        write_buffer_4(buffer, 0x66, 0x89, 0x84, 0x24);
        write_buffer_dword(buffer, offset);       
    }
}

//mov DWORD PTR SS:[esp], eax
void asm_store_variable_eax(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov DWORD PTR SS:[esp + %d], eax\n", buffer->length+START, offset);
    if ( (offset >> 8) == 0 ) {
        write_buffer_4(buffer, 0x89, 0x44, 0x24, offset);
    } else {
        write_buffer_3(buffer, 0x89, 0x84, 0x24);
        write_buffer_dword(buffer, offset);  
    }
}

void asm_store_variable(T_NODE * n, int offset, T_BUFFER * buffer) {
    exec_varsize(n, offset, buffer, asm_store_variable_al, asm_store_variable_ax, asm_store_variable_eax);
}

//MEM
//mov [offset], al
void asm_store_variable_al_mem(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov [%x], al\n", buffer->length+START, offset);
    buffer->buffer[buffer->length++] = 0xA2;
    write_buffer_dword(buffer, offset);
}

//mov [offset], ax
void asm_store_variable_ax_mem(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov [%x], ax\n", buffer->length+START, offset);
    buffer->buffer[buffer->length++] = 0x66;
    buffer->buffer[buffer->length++] = 0xA3;
    write_buffer_dword(buffer, offset);
}

//mov [offset], eax
void asm_store_variable_eax_mem(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov [%x], eax\n", buffer->length+START, offset);
    buffer->buffer[buffer->length++] = 0xA3;
    write_buffer_dword(buffer, offset);
}

void asm_store_variable_mem(T_NODE * n, int offset, T_BUFFER * buffer) {
    exec_varsize(n, offset, buffer, asm_store_variable_al_mem, asm_store_variable_ax_mem, asm_store_variable_eax_mem);
}

//add esp, size
void asm_remove_variable(T_BUFFER * buffer, U32 size) {
    if (size == 0) return;
    printf("[ASM][%x] add esp, %d\n", buffer->length+START, size); 
    if ( (size >> 8) == 0 ) { 
        write_buffer_3(buffer, 0x83, 0xC4, size);
    } else {
        write_buffer_2(buffer, 0x81, 0xC4);
        write_buffer_dword(buffer, size);
    }
}

//ret
void asm_ret(T_BUFFER * buffer) {
    printf("[ASM][%x] ret\n", buffer->length+START);
    buffer->buffer[buffer->length++] = 0xC3;
}

//call [addr]
void asm_call(T_BUFFER * buffer, U32 addr) {
    addr = addr - 5;
    printf("[ASM][%x] call %x\n",buffer->length+START, addr);
    buffer->buffer[buffer->length++] = 0xE8; 
    write_buffer_dword(buffer, addr);
}

//imul eax, value
void asm_imul_eax_value(T_BUFFER * buffer, U32 value) {
    printf("[ASM][%x] imul eax, %d\n", buffer->length+START, value);
    write_buffer_2(buffer, 0x69, 0xC0);    
    write_buffer_dword(buffer, value);
}

//mov ebx, eax
void asm_mov_ebx_eax(T_BUFFER * buffer) {
    printf("[ASM][%x] mov ebx, eax\n", buffer->length+START);
    write_buffer_2(buffer, 0x89, 0xC3);    
}

//add ebx, eax
void asm_add_ebx_eax(T_BUFFER * buffer) {
    printf("[ASM][%x] add ebx, eax\n", buffer->length+START);
    write_buffer_2(buffer, 0x01, 0xC3);    
}

//cmp eax, ebx
void asm_cmp_eax_ebx(T_BUFFER * buffer) {
    printf("[ASM][%x] cmp eax, ebx\n", buffer->length+START);
    write_buffer_2(buffer, 0x39, 0xD8);          
}

//jg addr
U8 * asm_jump_greater(T_BUFFER * buffer, U32 addr) {
    printf("[ASM][%x] jg %x\n",buffer->length+START, addr);
    write_buffer_2(buffer, 0x0F, 0x8F); 
    write_buffer_dword(buffer, addr);
    return &(buffer->buffer[buffer->length - 4]);    
}

//jge addr
U8 * asm_jump_greater_eq(T_BUFFER * buffer, U32 addr) {
    printf("[ASM][%x] jg %x\n",buffer->length+START, addr);
    write_buffer_2(buffer, 0x0F, 0x8D);
    write_buffer_dword(buffer, addr);
    return &(buffer->buffer[buffer->length - 4]);
}


//jl addr
U8 * asm_jump_less(T_BUFFER * buffer, int addr) {
    printf("[ASM][%x] jl %x\n",buffer->length+START, addr);
    write_buffer_2(buffer, 0x0F, 0x8C);
    write_buffer_dword(buffer, addr);
    return &(buffer->buffer[buffer->length - 4]);    
}

//jle addr
U8 * asm_jump_less_eq(T_BUFFER * buffer, int addr) {
    printf("[ASM][%x] jl %x\n",buffer->length+START, addr);
    write_buffer_2(buffer, 0x0F, 0x8E);
    write_buffer_dword(buffer, addr);
    return &(buffer->buffer[buffer->length - 4]);
}

//je addr
U8 * asm_jump_equal(T_BUFFER * buffer, U32 addr) {
    printf("[ASM][%x] je %x\n",buffer->length+START, addr);
    write_buffer_2(buffer, 0x0F, 0x84);
    write_buffer_dword(buffer, addr);
    return &(buffer->buffer[buffer->length - 4]);    
}

//jne addr
U8 * asm_jump_not_equal(T_BUFFER * buffer, U32 addr) {
    printf("[ASM][%x] jne %x\n",buffer->length+START, addr);
    write_buffer_2(buffer, 0x0F, 0x85);
    write_buffer_dword(buffer, addr);
    return &(buffer->buffer[buffer->length - 4]);
}


//jmp addr
U8 * asm_jump(T_BUFFER * buffer, int addr) {
    printf("[ASM][%x] jmp %x\n",buffer->length+START, addr);
    buffer->buffer[buffer->length++] = 0xE9;
    write_buffer_dword(buffer, addr);
    return &(buffer->buffer[buffer->length - 4]);
}

//test eax
void asm_test_eax(T_BUFFER * buffer) {
    printf("[ASM][%x] test eax, eax\n", buffer->length+START);
    write_buffer_2(buffer, 0x85, 0xC0);    
}

//mov al, [ebx]
void asm_mov_al_ebx_addr(T_BUFFER * buffer) {
    printf("[ASM][%x] mov al, (ebx)\n", buffer->length+START);
    write_buffer_2(buffer, 0x8A, 0x03);
}

//mov ax, [ebx]
void asm_mov_ax_ebx_addr(T_BUFFER * buffer) {
    printf("[ASM][%x] mov ax, (ebx)\n", buffer->length+START);
    write_buffer_3(buffer, 0x66, 0x8B, 0x03);
}

//mov eax, [ebx]
void asm_mov_eax_ebx_addr(T_BUFFER * buffer) {
    printf("[ASM][%x] mov eax, (ebx)\n", buffer->length+START);
    write_buffer_2(buffer, 0x8B, 0x03);
}

//mov eax, ebx
void asm_mov_eax_ebx(T_BUFFER * buffer) {
    //0x89, 0xD8
    printf("[ASM][%x] mov eax, ebx\n", buffer->length+START);
    write_buffer_2(buffer, 0x89, 0xD8);
}

void asm_retrieve_variable_indirect_vs(int varsize, T_BUFFER * buffer) {
    switch(varsize) {
        case 1:
            asm_mov_al_ebx_addr(buffer);
            break;
        case 2:
            asm_mov_ax_ebx_addr(buffer);
            break;
        case 4: 
            asm_mov_eax_ebx_addr(buffer);
            break;
        default:
            error("Unsupported variable size");
            break;
    }
}

//mov [ebx], al
void asm_mov_ebx_addr_al(T_BUFFER * buffer) {
    printf("[ASM][%x] mov (ebx), al\n", buffer->length+START);
    write_buffer_2(buffer, 0x88, 0x03);
}

//mov [ebx], ax
void asm_mov_ebx_addr_ax(T_BUFFER * buffer) {
    printf("[ASM][%x] mov (ebx), ax\n", buffer->length+START);
    write_buffer_3(buffer, 0x66, 0x89, 0x03);
}

//mov [ebx], eax
void asm_mov_ebx_addr_eax(T_BUFFER * buffer) {
    printf("[ASM][%x] mov (ebx), eax\n", buffer->length+START);
    write_buffer_2(buffer, 0x89, 0x03);
}

//mov eax, [eax]
void asm_mov_eax_eax_addr(T_BUFFER * buffer) {
    printf("[ASM][%x] mov eax, (eax)\n", buffer->length+START);
    write_buffer_2(buffer, 0x8B, 0x00);    
}

void asm_store_variable_indirect_vs(int varsize, T_BUFFER * buffer) {
    switch(varsize) {
        case 1:
            asm_mov_ebx_addr_al(buffer);
            break;
        case 2:
            asm_mov_ebx_addr_ax(buffer);
            break;
        case 4: 
            asm_mov_ebx_addr_eax(buffer);
            break;
        default:
            error("Unsupported variable size");
            break;
    }
}

//idiv ebx
void asm_idiv_ebx(T_BUFFER * buffer) {
    printf("[ASM][%x] idiv ebx\n", buffer->length+START);    
    write_buffer_2(buffer, 0xF7, 0xFB);
}

//imul ebx
void asm_imul_ebx(T_BUFFER * buffer) {
    printf("[ASM][%x] imul ebx\n", buffer->length+START);    
    write_buffer_2(buffer, 0xF7, 0xEB);
}


//xor ebx, ebx
void asm_xor_ebx_ebx(T_BUFFER * buffer) {
    printf("[ASM][%x] xor ebx, ebx\n", buffer->length+START);
    write_buffer_2(buffer, 0x31, 0xdb);
}

void asm_store_variable_indirect(T_NODE * n, T_BUFFER * buffer) {
    asm_store_variable_indirect_vs(variable_size(n), buffer);
}

void asm_update_jump_length(U8 * ptr, T_BUFFER * buffer, U32 start_offset) {
    start_offset = buffer->length - start_offset;
    memcpy( ptr, &start_offset, sizeof(U32) );
}

void write_elf32(char * filename,  T_BUFFER * buffer) {
    FILE * f = fopen(filename, "wb");
    if (f == NULL) error("Unable to create destination file.");

    //ELF_ENTRY_VADDR+sizeof(T_ELF)+sizeof(T_ELF_PRG32_HDR),
    elf32.e_entry = ELF_ENTRY_VADDR + sizeof(T_ELF) + sizeof(T_ELF_PRG32_HDR)+ buffer->length;

    printf("ADDR: %x\n", elf32.e_entry);
    fwrite(&elf32, sizeof(T_ELF), 1, f);

    elf32_prg_hdr.p_filesz = elf32_prg_hdr.p_filesz + buffer->length;
    elf32_prg_hdr.p_memsz = elf32_prg_hdr.p_memsz + buffer->length;

    fwrite(&elf32_prg_hdr, sizeof(T_ELF_PRG32_HDR), 1, f);

    asm_call(buffer, buffer->main_offset - buffer->length);

    fwrite(buffer->buffer, buffer->length, 1, f);

    
    fwrite(&prog, sizeof(prog), 1, f);
    fclose(f);
}