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

#define XN_IMMEDIATE qbit(0, 1, 0, 0)
#define XN_ABSOLUTE_L qbit(0, 0, 0, 1)

#define OPS_B qbit(0, 0, 0, 1)
#define OPS_W qbit(0, 0, 1, 1)
#define OPS_L qbit(0, 0, 1, 0)

#define OPS_2_B qbit(0, 0, 0, 0)
#define OPS_2_W qbit(0, 0, 0, 1)
#define OPS_2_L qbit(0, 0, 1, 0)

void write_u32(T_BUFFER * buffer, U32 value) {
    buffer->buffer[buffer->length++] = (value & 0xFF000000) >> 24;
    buffer->buffer[buffer->length++] = (value & 0x00FF0000) >> 16;
    buffer->buffer[buffer->length++] = (value & 0x0000FF00) >> 8;
    buffer->buffer[buffer->length++] = (value & 0x000000FF) >> 0;
}

void moveq(T_BUFFER * buffer, U8 value, enum DN dn) {
    printf("[ASM] moveq #%d, d%d", value, dn);
    buffer->buffer[buffer->length++] = dn2byte(dn) << 1 | qbit(0, 1, 1, 1)  << 4;
    buffer->buffer[buffer->length++] = value;
}

void rts(T_BUFFER * buffer) {
    printf("[ASM] rts\n");
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    U8 q3 = qbit(0, 1, 1, 1);
    U8 q4 = qbit(0, 1, 0, 1);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = qbit_and(q3, q4);
}

void jsr(T_BUFFER * buffer, U32 address) {
    printf("[ASM] jsr $%x\n", address);
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0);
    buffer->buffer[buffer->length++] = qbit_and(q1, q2);
    buffer->buffer[buffer->length++] = 1 << 7 | 0 << 6 | M_IMMEDIATE << 3 | XN_ABSOLUTE_L;
    write_u32(buffer, address);
}

void move_imm_u32(T_BUFFER * buffer, enum DN dn, U32 value) {
    printf("[ASM] move #%d, d%d\n", value, dn);
    U16 instr = qbit(0, 0, 0, 0) << 14 | OPS_L << 12 | dn << 9 | M_DATA_REGISTER << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    buffer->buffer[buffer->length++] = (instr & 0xFF00) >> 8;
    buffer->buffer[buffer->length++] = 0xFF;
    write_u32(buffer, value); 
}

void move_imm_u8(T_BUFFER * buffer, enum DN dn, U8 value) {
    U16 instr = qbit(0, 0, 0, 0) << 14 | OPS_B << 12 | dn << 9 | M_DATA_REGISTER << 6 | M_IMMEDIATE << 3 | XN_IMMEDIATE;
    buffer->buffer[buffer->length++] = (instr & 0xFF00) >> 8;
    buffer->buffer[buffer->length++] = (instr & 0xFF);
    buffer->buffer[buffer->length++] = value;
}

void add_imm_u32(T_BUFFER * buffer, enum DN dn, U32 value) {
    U16 instr = qbit(0, 0, 0, 0) << 12 | qbit(0, 1, 1, 0) << 8 | OPS_2_L << 6 | M_DATA_REGISTER << 3 | dn;
    buffer->buffer[buffer->length++] = (instr & 0xFF00) >> 8;
    buffer->buffer[buffer->length++] = (instr & 0xFF);
    write_u32(buffer, value);
}