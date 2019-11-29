#include "main.h"

enum DN {
    D0,
    D1,
    D2,
    D3, 
    D4,
    D5,
    D6,
    D7
} T_DN;

enum AN {
    A0,
    A1,
    A2,
    A3, 
    A4,
    A5,
    A6,
    A7
} T_AN;


U8 dn2byte(enum DN dn) {
    return dn;
}

U8 qbit(char a, char b, char c, char d) {
    return d | c << 1 | b << 2 | a << 3; 
}

U8 qbit_and(U8 qb1, U8 qb2) {
    return qb2 | qb1 << 4;
}

#define M_ADDRESS qbit(0, 0, 1, 0)
#define M_IMMEDIATE qbit(0, 1, 1, 1)
#define M_DATA_REGISTER qbit(0, 0, 0, 0)
#define M_ADDRESS_REGISTER qbit(0, 0, 0, 1)
#define M_ADDRESS_DISP qbit(0, 1, 0, 1)
#define M_DISP_PC qbit(0, 1, 1, 1)

#define XN_IMMEDIATE qbit(0, 1, 0, 0)
#define XN_ABSOLUTE_L qbit(0, 0, 0, 1)
#define XN_DISP_PC qbit(0, 0, 1, 0)

#define OPS_B qbit(0, 0, 0, 1)
#define OPS_W qbit(0, 0, 1, 1)
#define OPS_L qbit(0, 0, 1, 0)

#define OPS_2_B qbit(0, 0, 0, 0)
#define OPS_2_W qbit(0, 0, 0, 1)
#define OPS_2_L qbit(0, 0, 1, 0)

#define D_RM2D 0
#define D_RM2M 1

#define M68K_ADD qbit(1, 1, 0, 1)
#define M68K_SUB qbit(1, 0, 0, 1)

#define COND_GEQ qbit(1, 1, 0, 0)
#define COND_EQ qbit(0, 1, 1, 1)
#define COND_LEQ qbit(1, 1, 1, 1)
#define COND_NEQ qbit(0, 1, 1, 0)


U8 ops(U8 size) {
    (size == 4) ? OPS_L : (size == 2) ? OPS_W : OPS_B;
}

U8 ops2(U8 size) {
    (size == 4) ? OPS_2_L : (size == 2) ? OPS_2_W : OPS_2_B;
}

int size2char(U8 size) {
    if (size == 4) 
        return 'l';
    else if (size == 2) 
            return 'w';
         else
            return 'b';
}

void write_u32(T_BUFFER * buffer, U32 value) {
    buffer->buffer[buffer->length++] = (value & 0xFF000000) >> 24;
    buffer->buffer[buffer->length++] = (value & 0x00FF0000) >> 16;
    buffer->buffer[buffer->length++] = (value & 0x0000FF00) >> 8;
    buffer->buffer[buffer->length++] = (value & 0x000000FF) >> 0;
}

void write_u16(T_BUFFER * buffer, U16 value) {
    buffer->buffer[buffer->length++] = (value & 0xFF00) >> 8;
    buffer->buffer[buffer->length++] = (value & 0xFF);    
}

void write_value(T_BUFFER * buffer, U32 value, U8 size) {
    if (size == 4) 
        write_u32(buffer, value);
    else 
        write_u16(buffer, value);    
}

void moveq(T_BUFFER * buffer, U8 value, enum DN dn) {
    printf("[ASM][%x] moveq #%d, d%d\n", buffer->length, value, dn);
    buffer->buffer[buffer->length++] = dn2byte(dn) << 1 | qbit(0, 1, 1, 1)  << 4;
    buffer->buffer[buffer->length++] = value;
}

//RTS
void rts(T_BUFFER * buffer) {
    printf("[ASM][%x] rts\n", buffer->length);
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    U8 q3 = qbit(0, 1, 1, 1);
    U8 q4 = qbit(0, 1, 0, 1);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = qbit_and(q3, q4);
}

//JSR
void jsr(T_BUFFER * buffer, U32 address) {
    printf("[ASM][%x] jsr $%x\n", buffer->length, address);
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = 1 << 7 | 0 << 6 | M_IMMEDIATE << 3 | XN_ABSOLUTE_L;
    write_u32(buffer, address);
}

//JSR PC with DISP
void jsr_disp(T_BUFFER * buffer, short offset) {
    printf("[ASM][%x] jsr (PC + $%x)\n", buffer->length, offset);
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = 1 << 7 | 0 << 6 | M_DISP_PC << 3 | XN_DISP_PC;
    write_u16(buffer, offset);
}

//JMP
void jmp_abs(T_BUFFER * buffer, U32 address) {
    printf("[ASM][%x] jmp $%x\n", buffer->length, address);
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = 1 << 7 | 1 << 6 | M_IMMEDIATE << 3 | XN_ABSOLUTE_L;
    write_u32(buffer, address);
}

U8 * jmp_disp(T_BUFFER * buffer, U16 offset) {
    printf("[ASM][%x] jmp (PC + $%x)\n", buffer->length, offset);
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = 1 << 7 | 1 << 6 | M_DISP_PC << 3 | XN_DISP_PC;
    write_u16(buffer, offset);
    return &(buffer->buffer[buffer->length - 2]); 
}

//MOVE
void move_imm(T_BUFFER * buffer, enum DN dn, U32 value, U8 size) {
    printf("[ASM][%x] move.%c #%d, d%d\n", buffer->length, size2char(size), value, dn);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | dn << 9 | M_DATA_REGISTER << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
}

//MOVE REG -> (ADDR) + OFFSET
void move_reg_addr_disp(T_BUFFER * buffer, enum DN dn, enum AN an, U16 offset, U8 size) {
    printf("[ASM][%x] move.%c (a%d + %d), d%d\n", buffer->length, size2char(size), an, offset, dn);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | an << 9 | M_ADDRESS_DISP << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
    write_u16(buffer, offset);
}

//MOVE REG -> REG
void move_reg_reg(T_BUFFER * buffer, enum DN dn1, enum DN dn2, U8 size) {
    printf("[ASM][%x] move.%c d%d, d%d\n", buffer->length, size2char(size), dn1, dn2);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | dn2 << 9 | M_DATA_REGISTER << 6 | M_DATA_REGISTER << 3 | dn1;
    write_u16(buffer, instr);
}

//MOVE REG -> ADDR
void move_reg_addr(T_BUFFER * buffer, enum DN dn, enum AN an, U8 size) {
    printf("[ASM][%x] move.%c d%d, a%d\n", buffer->length, size2char(size), dn, an);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | an << 9 | M_ADDRESS_REGISTER << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
}

//MOVE ADDR -> REG
void move_addr_reg(T_BUFFER * buffer, enum DN dn, enum AN an, U8 size) {
    printf("[ASM][%x] move.%c a%d, d%d\n", buffer->length, size2char(size), an, dn);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | dn << 9 | M_DATA_REGISTER << 6 | M_ADDRESS_REGISTER << 3 | an;
    write_u16(buffer, instr);
}

//MOVE (ADDR) + OFFSET -> REG
void move_addr_disp_reg(T_BUFFER * buffer, enum AN an, enum DN dn, U16 offset, U8 size) {
    printf("[ASM][%x] move.%c (a%d + %d), d%d\n", buffer->length, size2char(size), an, offset, dn);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | dn << 9 | M_DATA_REGISTER << 6 | M_ADDRESS_DISP << 3 | an;
    write_u16(buffer, instr);
    write_u16(buffer, offset);
}

//MOVEA
void movea_imm(T_BUFFER * buffer, enum AN an, U32 value) {
    printf("[ASM][%x] movea.l #%d, a%d\n", buffer->length, value, an);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(4) << 12 | an << 9 | qbit(0, 0, 0, 1) << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    write_u16(buffer, instr);
    write_u32(buffer, value);
}


//ADD
void add_reg_addr_disp(T_BUFFER * buffer, enum DN dn, enum AN an, U8 size, int offset) {
    printf("[ASM][%x] add_reg_addr_disp\n", buffer->length);
    U16 instr = M68K_ADD << 12 | dn << 9 | D_RM2M << 8 | ops2(size) << 6 | M_ADDRESS_DISP << 3 | an;
    write_u16(buffer, instr);
    write_u16(buffer, offset);
}

//SUB
void sub_reg_addr_disp(T_BUFFER * buffer, enum DN dn, enum AN an, U8 size, int offset) {
    printf("[ASM][%x] sub_reg_addr_disp\n", buffer->length);
    U16 instr = M68K_SUB << 12 | dn << 9 | D_RM2M << 8 | ops2(size) << 6 | M_ADDRESS_DISP << 3 | an;
    write_u16(buffer, instr);
    write_u16(buffer, offset);
}


//ADDI
void add_imm(T_BUFFER * buffer, enum DN dn, U32 value, U8 size) {
    printf("[ASM][%x] addi.%c d%d, %d\n", buffer->length, size2char(size), dn, value);
    U16 instr = qbit(0, 0, 0, 0) << 12 | qbit(0, 1, 1, 0) << 8 | ops2(size) << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
}


//ADDA
void add_imm_addr(T_BUFFER * buffer, enum AN an, U32 value, U8 size) {
    printf("[ASM][%x] adda.%c %d, a%d\n", buffer->length, size2char(size), value, an);
    U16 instr = qbit(1, 1, 0, 1) << 12 | an << 9 | 1 << 8 | qbit(0, 0, 1, 1) << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
}

void add_imm_addr_disp(T_BUFFER * buffer, enum AN an, U32 value, U8 size, U16 offset) {
    printf("[ASM][%x] add_imm_addr_disp\n", buffer->length);
    U16 instr = qbit(0, 0, 0, 0) << 12 | qbit(0, 1, 1, 0) << 8 | ops2(size) << 6 | M_ADDRESS_DISP << 3 | an;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
    write_u16(buffer, offset);
}

//ADDA
void adda_reg(T_BUFFER * buffer, enum DN dn, enum AN an) {
    printf("[ASM][%x] adda.l d%d, a%d\n", buffer->length, dn, an);
    U16 instr = M68K_ADD << 12 | an << 9 | 1 << 8 | qbit(0, 0, 1, 1) << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
}

//SUBI
void sub_imm(T_BUFFER * buffer, enum DN dn, U32 value, U8 size) {
    printf("[ASM][%x] sub_imm\n", buffer->length);
    U16 instr = qbit(0, 0, 0, 0) << 12 | qbit(0, 1, 0, 0) << 8 | ops2(size) << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
}

//SUBA
void sub_imm_addr(T_BUFFER * buffer, enum AN an, U32 value, U8 size) {
    printf("[ASM][%x] suba.%c %d, a%d\n", buffer->length, size2char(size), value, an);
    U16 instr = qbit(1, 0, 0, 1) << 12 | an << 9 | 1 << 8 | qbit(0, 0, 1, 1) << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
}

//TST
void tst(T_BUFFER * buffer, enum DN dn, U8 size) {
    printf("[ASM][%x] tst.%c d%d\n", buffer->length, size2char(size), dn);
    U16 instr = qbit(0, 1, 0, 0) << 12 | qbit(1, 0, 1, 0) << 8 | ops2(size) << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
}

//CMP
void cmp(T_BUFFER * buffer, enum DN dn1, enum DN dn2, U8 size) {
    U16 instr = qbit(1, 0, 1, 1) << 12 | dn1 << 9 | 0 << 8 | ops2(size) << 6 | M_DATA_REGISTER << 3 | dn2;
    write_u16(buffer, instr);
}

//MULS
void muls(T_BUFFER * buffer, enum DN dn, U16 value) {
    U16 instr = qbit(1, 1, 0, 0) << 12 | dn << 9 | qbit(0, 1, 1, 1) << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    write_u16(buffer, instr);
    write_u16(buffer, value);
}

//MULU
void mulu(T_BUFFER * buffer, enum DN dn, U16 value) {
    U16 instr = qbit(1, 1, 0, 0) << 12 | dn << 9 | qbit(0, 0, 1, 1) << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    write_u16(buffer, instr);
    write_u16(buffer, value);
}

//MULS
void muls_reg(T_BUFFER * buffer, enum DN dn1, enum DN dn2) {
    U16 instr = qbit(1, 1, 0, 0) << 12 | dn1 << 9 | qbit(0, 1, 1, 1) << 6 | M_DATA_REGISTER << 3 | dn2;
    write_u16(buffer, instr);
}

//MULU
void mulu_reg(T_BUFFER * buffer, enum DN dn, enum DN dn2) {
    U16 instr = qbit(1, 1, 0, 0) << 12 | dn << 9 | qbit(0, 0, 1, 1) << 6 | M_DATA_REGISTER << 3 | dn2;
    write_u16(buffer, instr);
}

//DIVS
void divs_reg(T_BUFFER * buffer, enum DN dn1, enum DN dn2) {
    U16 instr = qbit(1, 0, 0, 0) << 12 | dn1 << 9 | qbit(0, 1, 1, 1) << 6 | M_DATA_REGISTER << 3 | dn2;
    write_u16(buffer, instr);
}

//DIVU
void divu_reg(T_BUFFER * buffer, enum DN dn, enum DN dn2) {
    U16 instr = qbit(1, 0, 0, 0) << 12 | dn << 9 | qbit(0, 0, 1, 1) << 6 | M_DATA_REGISTER << 3 | dn2;
    write_u16(buffer, instr);
}

//MULS
void muls_addr_disp(T_BUFFER * buffer, enum DN dn, enum AN an, U16 offset) {
    U16 instr = qbit(1, 1, 0, 0) << 12 | dn << 9 | qbit(0, 1, 1, 1) << 6 | M_ADDRESS_DISP << 3 | an;
    write_u16(buffer, instr);
    write_u16(buffer, offset);
}

//MULU
void mulu_addr_disp(T_BUFFER * buffer, enum DN dn, enum AN an, U16 offset) {
    U16 instr = qbit(1, 1, 0, 0) << 12 | dn << 9 | qbit(0, 0, 1, 1) << 6 | M_ADDRESS_DISP << 3 | an;
    write_u16(buffer, instr);
    write_u16(buffer, offset);
}

//DIVS
void divs_imm(T_BUFFER * buffer, enum DN dn, U16 value) {
    U16 instr = qbit(1, 0, 0, 0) << 12 | dn << 9 | qbit(0, 1, 1, 1) << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    write_u16(buffer, instr);
    write_u16(buffer, value);
}

//DIVU
void divu_imm(T_BUFFER * buffer, enum DN dn, U16 value) {
    U16 instr = qbit(1, 0, 0, 0) << 12 | dn << 9 | qbit(0, 0, 1, 1) << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    write_u16(buffer, instr);
    write_u16(buffer, value);
}

//LEA
void lea_disp_pc(T_BUFFER * buffer, enum AN an, short offset) {
    printf("[ASM][%x] lea (PC + #%d), a%d\n", buffer->length, offset, an);    
    U16 instr = qbit(0, 1, 0, 0) << 12 | an << 9 | qbit(0, 1, 1, 1) << 6 | M_DISP_PC << 3  | XN_DISP_PC;
    write_u16(buffer, instr);
    write_u16(buffer, offset);
}

//BCC
U8 * bcc(T_BUFFER * buffer, U8 q_cond, U16 offset) {
    printf("[ASM][%x] Bcc\n", buffer->length);
    buffer->buffer[buffer->length++] = qbit(0, 1, 1, 0) << 4 | q_cond;
    buffer->buffer[buffer->length++] = 0;
    write_u16(buffer, offset);
    return &(buffer->buffer[buffer->length - 2]);  
}

int asm_line(T_BUFFER * buffer, char * line) {
    char * cline = line;

    if (strncmp(cline, "move", 4) == 0) {
        cline += 4;
    }
    
    return 0;
}

//Generic functions
void asm_load_u32(U32 value, T_BUFFER * buffer) {
    printf("asm_load_u32\n");
    move_imm(buffer, D0, value, 4);
}

void asm_add_variable(T_NODE * up, T_BUFFER * buffer) {
    U32 size  = variable_size(up);
    sub_imm_addr(buffer, A7, size, 4);
}

void asm_remove_variable(T_BUFFER * buffer, U32 size) {
    if (size == 0) return;
    printf("asm_remove_variable\n");
    add_imm_addr(buffer, A7, size, 4);
}

void asm_store_variable(T_NODE * n, int offset, T_BUFFER * buffer) {
    printf("asm_store_variable\n");
    move_reg_addr_disp(buffer, D0, A7, offset, variable_size(n));
}

void asm_store_variable_u32(int offset, T_BUFFER * buffer) {
    printf("asm_store_variable_u32\n");
    move_reg_addr_disp(buffer, D0, A7, offset, 4);
}

void asm_retrieve_variable(T_NODE * n, int offset, T_BUFFER * buffer) {
    printf("asm_retrieve_variable\n");
    move_addr_disp_reg(buffer, A7, D0, offset, variable_size(n));
}

void asm_retrieve_variable_u32(int offset, T_BUFFER * buffer) {
    printf("asm_retrieve_variable_u32\n");
    move_addr_disp_reg(buffer, A7, D0, offset, 4);
}

void asm_call(T_BUFFER * buffer, int offset) {
    offset = offset - 2;   
    jsr_disp(buffer, offset);
}

void write_setup(T_BUFFER * buffer, int main_offset) {
    jsr_disp(buffer, main_offset);
    rts(buffer);
}

void asm_add_variable_and_store(T_NODE * n, int offset, T_BUFFER * buffer) {
    add_reg_addr_disp(buffer, D0, A7, variable_size(n), offset);
}

void asm_sub_variable_and_store(T_NODE * n, int offset, T_BUFFER * buffer) {
    sub_reg_addr_disp(buffer, D0, A7, variable_size(n), offset);
}

void asm_retrieve_variable_indirect_vs(int size, T_BUFFER * buffer) {
    //printf("[ASM] asm_retrieve_variable_indirect_vs\n");
    printf("[ASM][%x] move.%c (a%d), d%d\n", buffer->length, size2char(size), D0, A0);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | D0 << 9 | M_DATA_REGISTER << 6 | M_ADDRESS << 3 | A0;
    write_u16(buffer, instr);
}

void asm_retrieve_variable_indirect_vs_an(int size, T_BUFFER * buffer, enum AN an) {
    //printf("[ASM] asm_retrieve_variable_indirect_vs\n");
    printf("[ASM][%x] move.%c (a%d), d%d\n", buffer->length, size2char(size), D0, A0);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | D0 << 9 | M_DATA_REGISTER << 6 | M_ADDRESS << 3 | an;
    write_u16(buffer, instr);
}

void asm_store_variable_indirect_vs(int size, T_BUFFER * buffer) {
    printf("[ASM][%x] move.%c d%d, (a%d)\n", buffer->length, size2char(size), A0, D0);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | A0 << 9 | M_ADDRESS << 6 | M_DATA_REGISTER << 3 | D0;
    write_u16(buffer, instr);    
}

void asm_update_jump_length(U8 * ptr, T_BUFFER * buffer, U16 start_offset) {
    start_offset = buffer->length - start_offset - 2;
    printf("Updating jump offset: %d\n", start_offset);
    *ptr = (start_offset & 0xFF00) >> 8;
    ptr++;
    *ptr = (start_offset & 0xFF);   
}