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
#define M_ADRESS_REGISTER qbit(0, 0, 0, 1)
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

U8 ops(U8 size) {
    (size == 4) ? OPS_L : (size == 2) ? OPS_W : OPS_B;
}

U8 ops2(U8 size) {
    (size == 4) ? OPS_2_L : (size == 2) ? OPS_2_W : OPS_2_B;
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
    printf("[ASM] moveq #%d, d%d\n", value, dn);
    buffer->buffer[buffer->length++] = dn2byte(dn) << 1 | qbit(0, 1, 1, 1)  << 4;
    buffer->buffer[buffer->length++] = value;
}

//RTS
void rts(T_BUFFER * buffer) {
    printf("[ASM] rts\n");
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    U8 q3 = qbit(0, 1, 1, 1);
    U8 q4 = qbit(0, 1, 0, 1);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = qbit_and(q3, q4);
}

//JSR
void jsr(T_BUFFER * buffer, U32 address) {
    printf("[ASM] jsr $%x\n", address);
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = 1 << 7 | 0 << 6 | M_IMMEDIATE << 3 | XN_ABSOLUTE_L;
    write_u32(buffer, address);
}

//JMP
void jmp_abs(T_BUFFER * buffer, U32 address) {
    printf("[ASM] jmp $%x\n", address);
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = 1 << 7 | 1 << 6 | M_IMMEDIATE << 3 | XN_ABSOLUTE_L;
    write_u32(buffer, address);
}

void jmp_disp(T_BUFFER * buffer, U16 offset) {
    printf("[ASM] jmp $%x\n", offset);
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = 1 << 7 | 1 << 6 | M_DISP_PC << 3 | XN_DISP_PC;
    write_u16(buffer, offset);
}

//MOVE
void move_imm(T_BUFFER * buffer, enum DN dn, U32 value, U8 size) {
    printf("[ASM] move #%d, d%d\n", value, dn);
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | dn << 9 | M_DATA_REGISTER << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
}

//MOVE REG -> (ADDR) + OFFSET
void move_reg_addr_disp(T_BUFFER * buffer, enum DN dn, enum AN an, U16 offset, U8 size) {
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | an << 9 | M_ADDRESS_DISP << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
    write_u16(buffer, offset);
}

//MOVE (ADDR) + OFFSET -> REG
void move_addr_disp_reg(T_BUFFER * buffer, enum AN an, enum DN dn, U16 offset, U8 size) {
    U16 instr = qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | dn << 9 | M_DATA_REGISTER << 6 | M_ADDRESS_DISP << 3 | an;
    write_u16(buffer, instr);
    write_u16(buffer, offset);
}

//ADDI
void add_imm(T_BUFFER * buffer, enum DN dn, U32 value, U8 size) {
    U16 instr = qbit(0, 0, 0, 0) << 12 | qbit(0, 1, 1, 0) << 8 | ops2(size) << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
}

void add_imm_addr(T_BUFFER * buffer, enum AN an, U32 value, U8 size) {
    U16 instr = qbit(0, 0, 0, 0) << 12 | qbit(0, 1, 1, 0) << 8 | ops2(size) << 6 | M_ADRESS_REGISTER << 3 | an;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
}

//SUBI
void sub_imm(T_BUFFER * buffer, enum DN dn, U32 value, U8 size) {
    U16 instr = qbit(0, 0, 0, 0) << 12 | qbit(0, 1, 0, 0) << 8 | ops2(size) << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
}

void sub_imm_addr(T_BUFFER * buffer, enum AN an, U32 value, U8 size) {
    U16 instr = qbit(0, 0, 0, 0) << 12 | qbit(0, 1, 0, 0) << 8 | ops2(size) << 6 | M_ADRESS_REGISTER << 3 | an;
    write_u16(buffer, instr);
    write_value(buffer, value, size);
}

//TST
void tst(T_BUFFER * buffer, enum DN dn, U8 size) {
    U16 instr = qbit(0, 1, 0, 0) << 12 | qbit(1, 0, 1, 0) << 8 | ops2(size) << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
}


int asm_line(T_BUFFER * buffer, char * line) {
    char * cline = line;

    if (strncmp(cline, "move", 4) == 0) {
        cline += 4;
    }
    
    return 0;
}


//sub esp, size
void asm_add_variable(T_NODE * up, T_BUFFER * buffer) {
    U32 size  = variable_size(up);
    sub_imm_addr(buffer, A7, size, 4);
}

//add esp, size
void asm_remove_variable(T_BUFFER * buffer, U32 size) {
    if (size == 0) return;
    add_imm_addr(buffer, A7, size, 4);
}

void asm_store_variable(T_NODE * n, int offset, T_BUFFER * buffer) {
    move_reg_addr_disp(buffer, D0, A7, offset, variable_size(n));
}

void asm_retrieve_variable(T_NODE * n, int offset, T_BUFFER * buffer) {
    move_addr_disp_reg(buffer, A7, D0, offset, variable_size(n));
}