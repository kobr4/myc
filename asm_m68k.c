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
#define M_ADDRESS_POST_INC qbit(0, 0, 1, 1)
#define M_IMMEDIATE qbit(0, 1, 1, 1)
#define M_DATA_REGISTER qbit(0, 0, 0, 0)
#define M_ADDRESS_REGISTER qbit(0, 0, 0, 1)
#define M_ADDRESS_DISP qbit(0, 1, 0, 1)
#define M_DISP_PC qbit(0, 1, 1, 1)
#define M_ABS_LS qbit(0, 1 , 1, 1)

#define XN_IMMEDIATE qbit(0, 1, 0, 0)
#define XN_ABSOLUTE_L qbit(0, 0, 0, 1)
#define XN_ABSOLUTE_S qbit(0, 0, 0, 0)
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
#define COND_HI qbit(0, 0, 1, 0)
#define COND_CC qbit(0, 1, 0, 0)
#define COND_CS qbit(0, 1, 0, 1)
#define COND_LS qbit(0, 0, 1, 1)
#define COND_VC qbit(1, 0, 0, 0)
#define COND_VS qbit(1, 0, 0, 1)
#define COND_PL qbit(1, 0, 1, 0)
#define COND_MI qbit(1, 0, 1, 1)
#define COND_LT qbit(1, 1, 0, 1)
#define COND_GT qbit(1, 1, 1, 0)

#define MAX_LABEL 10
#define MAX_LABEL_LENGTH 20

typedef struct T_ASM_CTXT {
    char labels[MAX_LABEL][MAX_LABEL_LENGTH];
    U32 label_offset[MAX_LABEL];
    int label_count;
} T_ASM_CTXT;


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

void write_u16_ptr(U8 * ptr, U16 value) {
    *ptr = (value & 0xFF00) >> 8;
    ptr++;
    *ptr = (value & 0xFF);   
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
    printf("[ASM][%x] add d%d, (a%d + #%d)\n", buffer->length, dn, an, offset);
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
    U16 instr = qbit(1, 1, 0, 0) << 12 | dn2 << 9 | qbit(0, 1, 1, 1) << 6 | M_DATA_REGISTER << 3 | dn1;
    write_u16(buffer, instr);
}

//MULU
void mulu_reg(T_BUFFER * buffer, enum DN dn, enum DN dn2) {
    U16 instr = qbit(1, 1, 0, 0) << 12 | dn2 << 9 | qbit(0, 0, 1, 1) << 6 | M_DATA_REGISTER << 3 | dn;
    write_u16(buffer, instr);
}

//DIVS
void divs_reg(T_BUFFER * buffer, enum DN dn1, enum DN dn2) {
    U16 instr = qbit(1, 0, 0, 0) << 12 | dn2 << 9 | qbit(0, 1, 1, 1) << 6 | M_DATA_REGISTER << 3 | dn1;
    write_u16(buffer, instr);
}

//DIVU
void divu_reg(T_BUFFER * buffer, enum DN dn, enum DN dn2) {
    U16 instr = qbit(1, 0, 0, 0) << 12 | dn2 << 9 | qbit(0, 0, 1, 1) << 6 | M_DATA_REGISTER << 3 | dn;
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




#define ASM_ERROR error("Error while parsing ASM statement.");

U8 parse_operand(T_BUFFER * buffer, char * input, U8 mn, U8 size) {
    printf("OPERAND: %s\n", input);
    int offset = 0;
    int res = 0;
    int reg = 0;
    res = sscanf(input, "%d(a%d)", &offset, &reg);
    if (res == 2) {
        write_u16(buffer, offset);
        return mn ? M_ADDRESS_DISP << 3 | reg : reg << 3 | M_ADDRESS_DISP;
    }

    res = sscanf(input, "(a%d)", &reg);
    if (res == 1)
        return mn ? M_ADDRESS << 3 | reg : reg << 3 | M_ADDRESS;

    res = sscanf(input, "(a%d)+", &reg);
    if (res == 1)
        return mn ? M_ADDRESS_POST_INC << 3 | reg : reg << 3 | M_ADDRESS_POST_INC;

    res = sscanf(input, "#%x", &offset);
    if (res == 1){
        switch(size) {
            case 1 : write_u16(buffer, offset); break;
            case 2 : write_u16(buffer, offset); break;
            case 4 : write_u32(buffer, offset); break;
        }        
        return mn ? M_IMMEDIATE << 3 | XN_IMMEDIATE : XN_IMMEDIATE << 3 | M_IMMEDIATE;
    }

    res = sscanf(input, "(%x).w", &offset);
    if (res == 1) {
        write_u16(buffer, offset);
        return mn ? M_ABS_LS << 3 | XN_ABSOLUTE_S : XN_ABSOLUTE_S << 3 | M_ABS_LS;
    }

    res = sscanf(input, "(%x).l", &offset);
    if (res == 1) {
        write_u32(buffer, offset);
        return mn ? M_ABS_LS << 3 | XN_ABSOLUTE_L : XN_ABSOLUTE_L << 3 | M_ABS_LS;
    }

    res = sscanf(input, "a%d", &reg);
    if (res == 1)
        return mn ? M_ADDRESS_REGISTER << 3 | reg : reg << 3 | M_ADDRESS_REGISTER;

    res = sscanf(input, "d%d", &reg);
    if (res == 1)
        return mn ? M_DATA_REGISTER << 3 | reg : reg << 3 | M_DATA_REGISTER;
    
    ASM_ERROR;
}


void token(char * line, char * token, int max) {
    for(int i = 0; i < max;i++) {
        if (line[i] == '\n' || line[i] == 0 || line[i] == ' ' || line[i] == ',') {
            token[i] = 0;
            return;
        }
        token[i]= line[i];
    }
}


void parse_jsr(T_BUFFER * buffer, char * line) {
    U8 q1 = qbit(0, 1, 0, 0);
    U8 q2 = qbit(1, 1, 1, 0); 
    char op[10];
    token(line, op, 10);
    U8 * instr_ptr = &buffer->buffer[buffer->length];
    buffer->length += 2;
    U8 mxn = parse_operand(buffer, op, 1, 4);
    if (mxn == 0) ASM_ERROR;
    write_u16_ptr(instr_ptr, qbit_and(q1, q2) << 8 | 1 << 7 | 0 << 6 |  mxn);
}

char * trim(char * input) {
    if (*input == 0) return input;
    while(*input == ' ' || *input == '\t' || *input == '\n') input++;
    return input;
}

char * parse_size(char * line, U8 * size) {
    if (*line != '.') ASM_ERROR;
    line++;

    if (*line == 'b') *size = 1;
        else if (*line == 'w') *size = 2;
            else if (*line == 'l') *size = 4;
                else ASM_ERROR;
    return line++;
}

void parse_dual_token(char * line, char * op1, char * op2) {
    token(line, op1, 15); 
    while(*line != ',') line++;
    line = trim(++line);
    token(line, op2, 15);
} 

U16 build_move(U8 size, U8 mxn, U8 mxn2) {
    return qbit(0, 0, 0, 0) << 14 | ops(size) << 12 | mxn2 << 6 | mxn;
}

U16 build_cmp(U8 size, U8 mxn, U8 mxn2) {
    return qbit(1, 0, 1, 1) << 12 | (mxn2 >> 3) << 9 | 0 << 8 | ops2(size) << 6 | mxn;
}

U16 build_adda(U8 size, U8 mxn, U8 mxn2) {
    U8 s;
    if (size == 2) s = 0;
    else if (size == 4) s = 1;
        else ASM_ERROR;
    return qbit(1, 1, 0, 1) << 12 | (mxn2 >> 3) << 9 | s << 8 | 1 << 7 | 1 << 6 | mxn;
}

U16 build_suba(U8 size, U8 mxn, U8 mxn2) {
    U8 s;
    if (size == 2) s = 0;
    else if (size == 4) s = 1;
        else ASM_ERROR;
    return qbit(1, 0, 0, 1) << 12 | (mxn2 >> 3) << 9 | s << 8 | 1 << 7 | 1 << 6 | mxn;
}

U16 build_lea(U8 size, U8 mxn, U8 mxn2) {
    return qbit(0, 1, 0, 0) << 12 | (mxn2 >> 3) << 9 | qbit(0, 1, 1, 1) << 6 | mxn;
}

U16 build_divu(U8 size, U8 mxn, U8 mxn2) {
    return qbit(1, 0, 0, 0) << 12 | (mxn2 >> 3) << 9 | qbit(0, 0, 1, 1) << 6 | mxn;
}

U16 build_divs(U8 size, U8 mxn, U8 mxn2) {
    return qbit(1, 0, 0, 0) << 12 | (mxn2 >> 3) << 9 | qbit(0, 1, 1, 1) << 6 | mxn;
}

U16 build_mulu(U8 size, U8 mxn, U8 mxn2) {
    return qbit(1, 1, 0, 0) << 12 | (mxn2 >> 3) << 9 | qbit(0, 0, 1, 1) << 6 | mxn;
}

U16 build_muls(U8 size, U8 mxn, U8 mxn2) {
    return qbit(1, 1, 0, 0) << 12 | (mxn2 >> 3) << 9 | qbit(0, 1, 1, 1) << 6 | mxn;
}

U16 build_cmpi(U8 size, U8 mxn, U8 mxn2) {
    return qbit(0, 0, 0, 0) << 12 | qbit(1, 1, 0, 0) << 8 | ops2(size) << 6 | mxn2 >> 3 | ((mxn2 & qbit(0, 1, 1, 1)) << 3);
}

U16 build_andi(U8 size, U8 mxn, U8 mxn2) {
    return qbit(0, 0, 0, 0) << 12 | qbit(0, 0, 1, 0) << 8 | ops2(size) << 6 | mxn2 >> 3 | ((mxn2 & qbit(0, 1, 1, 1)) << 3);
}

U16 build_addi(U8 size, U8 mxn, U8 mxn2) {
    return qbit(0, 0, 0, 0) << 12 | qbit(0, 1, 1, 0) << 8 | ops2(size) << 6 | mxn2 >> 3 | ((mxn2 & qbit(0, 1, 1, 1)) << 3);
}

U16 build_subi(U8 size, U8 mxn, U8 mxn2) {
    return qbit(0, 0, 0, 0) << 12 | qbit(0, 1, 0, 0) << 8 | ops2(size) << 6 | mxn2 >> 3 | ((mxn2 & qbit(0, 1, 1, 1)) << 3);
}

U16 build_eori(U8 size, U8 mxn, U8 mxn2) {
    return qbit(0, 0, 0, 0) << 12 | qbit(1, 0, 1, 0) << 8 | ops2(size) << 6 | mxn2 >> 3 | ((mxn2 & qbit(0, 1, 1, 1)) << 3);
}

U16 build_ori(U8 size, U8 mxn, U8 mxn2) {
    return qbit(0, 0, 0, 0) << 12 | qbit(0, 0, 0, 0) << 8 | ops2(size) << 6 | mxn2 >> 3 | ((mxn2 & qbit(0, 1, 1, 1)) << 3);
}

U16 build_add(U8 size, U8 dn, U8 direction, U8 mxn) {
    return qbit(1, 1, 0, 1) << 12 |  dn << 9 |  direction << 8 |  ops2(size) << 6 | mxn;
}

U16 build_and(U8 size, U8 dn, U8 direction, U8 mxn) {
    return qbit(1, 1, 0, 0) << 12 |  dn << 9 |  direction << 8 |  ops2(size) << 6 | mxn;
}

U16 build_or(U8 size, U8 dn, U8 direction, U8 mxn) {
    return qbit(1, 0, 0, 0) << 12 |  dn << 9 |  direction << 8 |  ops2(size) << 6 | mxn;
}

U16 build_sub(U8 size, U8 dn, U8 direction, U8 mxn) {
    return qbit(1, 0, 0, 1) << 12 |  dn << 9 |  direction << 8 |  ops2(size) << 6 | mxn;
}

U16 build_bcc_cond(U8 size, U8 mxn, U8 condition) {
   if (size == 1) error("ASM Bcc byte displacement not supported.");
   if (size == 4) error("ASM Bcc long displacement not supported.");
   return qbit(0, 1, 1, 0) << 12 | condition << 8 | (size == 2 ? 0x0 : 0xFF);
}

U16 build_beq(U8 size, U8 mxn) {
    return build_bcc_cond(size, mxn, COND_EQ); 
}

U16 build_bge(U8 size, U8 mxn) {
    return build_bcc_cond(size, mxn, COND_GEQ); 
}

U16 build_ble(U8 size, U8 mxn) {
    return build_bcc_cond(size, mxn, COND_LEQ); 
}

U16 build_bne(U8 size, U8 mxn) {
    return build_bcc_cond(size, mxn, COND_NEQ); 
}

U16 build_bcc(U8 size, U8 mxn) {
    return build_bcc_cond(size, mxn, COND_CC); 
}

U16 build_bcs(U8 size, U8 mxn) {
    return build_bcc_cond(size, mxn, COND_CS); 
}

U16 build_bvc(U8 size, U8 mxn) {
    return build_bcc_cond(size, mxn, COND_VC); 
}

U16 build_bvs(U8 size, U8 mxn) {
    return build_bcc_cond(size, mxn, COND_VS); 
}

U16 build_bhi(U8 size, U8 mxn) {
    return build_bcc_cond(size, mxn, COND_HI); 
}


U16 build_tst(U8 size, U8 mxn) {
    return qbit(0, 1, 0, 0) << 12 | qbit(1, 0, 1, 0) << 8 | ops2(size) << 6 | mxn;
}


int retrieve_label(T_ASM_CTXT * ctxt, char * input) {
    for (int i = 0;ctxt != NULL && i < ctxt->label_count;i++) {
        if (strcmp(ctxt->labels[i], input) == 0) {
            return i;
        }
    }
    return -1;
}

void parse_single_op(T_BUFFER * buffer, char * line, U16(*build)(U8, U8)) {
    char op[15];
    U8 size;
    U8 mxn;
    line = parse_size(line, &size);
    line = trim(++line);
    token(line, op, 15);
    U8 * instr_ptr = &buffer->buffer[buffer->length];
    buffer->length += 2;
    mxn = parse_operand(buffer, op, 1, size);
    U16 instr = build(size, mxn);
    write_u16_ptr(instr_ptr, instr);
}

void parse_branch_op(T_ASM_CTXT * ctxt, T_BUFFER * buffer, char * line, U16(*build)(U8, U8)) {
    char op[20];
    U8 size;
    U8 mxn;
    line = parse_size(line, &size);
    line = trim(++line);
    token(line, op, 20);
    U8 * instr_ptr = &buffer->buffer[buffer->length];
    buffer->length += 2;
    int i;
    if ((i = retrieve_label(ctxt, op)) != -1) {
        mxn = M_DISP_PC << 3 | XN_DISP_PC;
        write_u16(buffer, ctxt->label_offset[i] - buffer->length );
    } else {
        mxn = parse_operand(buffer, op, 1, size);
    }
    
    U16 instr = build(size, mxn);
    write_u16_ptr(instr_ptr, instr);
}

void parse_dual_op(T_BUFFER * buffer, char * line, U16(*build)(U8, U8, U8) ) {
    char op1[15], op2[15];
    U8 size;
 
    line = parse_size(line, &size);
    line = trim(++line);    
    parse_dual_token(line, op1, op2);      
   
    U8 * instr_ptr = &buffer->buffer[buffer->length];
    buffer->length += 2;    
    U8 mxn = parse_operand(buffer, op1, 1, size);
    U8 mxn2 = parse_operand(buffer, op2, 0, size);
    
    U16 instr = build(size, mxn, mxn2);
    write_u16_ptr(instr_ptr, instr);
}

void parse_dual_op_direction(T_BUFFER * buffer, char * line, U16(*build)(U8, U8, U8, U8)) {
    char op1[15], op2[15];
    U8 size;    
    U8 direction = D_RM2D;
    line = parse_size(line, &size);
    line = trim(++line);          
    parse_dual_token(line, op1, op2);

    U8 * instr_ptr = &buffer->buffer[buffer->length];
    buffer->length += 2;    

    U32 dn;
    U8 mxn;
    int res = sscanf(op2, "d%d", &dn);
    if (res == 0) {
        res = sscanf(op1, "d%d", &dn);
        if (res == 0) ASM_ERROR;
        mxn = parse_operand(buffer, op2, 1, size);
        direction = D_RM2M;
    } else {
        mxn = parse_operand(buffer, op1, 1, size);
    }
    U16 instr = build(size, dn, direction, mxn);
    
    write_u16_ptr(instr_ptr, instr);
}

char * skip_line(char * input) {
    while (*input == '\n' || *input == ' ') 
        input++;
        
    return input;
}

void add_label(T_ASM_CTXT * ctxt, char * label, U32 offset) {
    int i;
    for (i = 0;label[i] != 0 && label[i] != '\n'; i++) ctxt->labels[ctxt->label_count][i] = label[i];
    ctxt->label_offset[ctxt->label_count] = offset;
    ctxt->labels[ctxt->label_count++][i] = 0;
    
}

int asm_line(T_ASM_CTXT * ctxt, T_BUFFER * buffer, char * line) {
    char * cline = skip_line(line);
    
    char s_label[20];
    char c;
    if (ctxt != NULL && sscanf(cline, "%s %[:]%*s", s_label, &c) == 2) {
        printf("[ASM] Adding label: %s\n", s_label);
        add_label(ctxt, s_label, buffer->length);
        return 0;
    }
    
    printf("[ASM][%x] %s\n",buffer->length, cline);
    if (strncmp(cline, "movea", 5) == 0) {
        cline += 5;
        parse_dual_op(buffer, cline, build_move);
    } else if (strncmp(cline, "move", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_move);
    } else if (strncmp(cline,"jsr ",4) == 0) {
        cline += 4;
        parse_jsr(buffer, cline);
    } else if (strncmp(cline, "andi", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_andi);
    } else if (strncmp(cline, "and", 3) == 0) {
        cline += 3;
        parse_dual_op_direction(buffer, cline, build_and);
    } else if (strncmp(cline, "addi", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_addi);
    } else if (strncmp(cline, "adda", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_adda);
    } else if (strncmp(cline, "divu", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_divu);
    } else if (strncmp(cline, "divs", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_divs);
    } else if (strncmp(cline, "mulu", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_mulu);
    } else if (strncmp(cline, "muls", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_muls);
    } else if (strncmp(cline, "add", 3) == 0) {
        cline += 3;
        parse_dual_op_direction(buffer, cline, build_add);
    } else if (strncmp(cline, "lea", 3) == 0) {
        cline += 3;
        parse_dual_op(buffer, cline, build_lea);
    } else if (strncmp(cline, "cmpi", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_cmpi);
    } else if (strncmp(cline, "cmp", 3) == 0) {
        cline += 3;
        parse_dual_op(buffer, cline, build_cmp);
    } else if (strncmp(cline, "subi", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_subi);
    } else if (strncmp(cline, "suba", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_suba);
    } else if (strncmp(cline, "sub", 3) == 0) {
        cline += 3;
        parse_dual_op_direction(buffer, cline, build_sub);
    } else if (strncmp(cline, "ori", 3) == 0) {
        cline += 3;
        parse_dual_op(buffer, cline, build_ori);
    } else if (strncmp(cline, "or", 2) == 0) {
        cline += 2;
        parse_dual_op_direction(buffer, cline, build_or);
    } else if (strncmp(cline, "eori", 4) == 0) {
        cline += 4;
        parse_dual_op(buffer, cline, build_eori);
    } else if (strncmp(cline, "ble", 3) == 0) {
        cline += 3;
        parse_branch_op(ctxt, buffer, cline, build_ble);
    } else if (strncmp(cline, "beq", 3) == 0) {
        cline += 3;
        parse_branch_op(ctxt, buffer, cline, build_beq);
    } else if (strncmp(cline, "bne", 3) == 0) {
        cline += 3;
        parse_branch_op(ctxt, buffer, cline, build_bne);
    } else if (strncmp(cline, "bge", 3) == 0) {
        cline += 3;
        parse_branch_op(ctxt, buffer, cline, build_bge);
    } else if (strncmp(cline, "bvs", 3) == 0) {
        cline += 3;
        parse_branch_op(ctxt, buffer, cline, build_bvs);
    } else if (strncmp(cline, "bcc", 3) == 0) {
        cline += 3;
        parse_branch_op(ctxt, buffer, cline, build_bcc);
    } else if (strncmp(cline, "bcs", 3) == 0) {
        cline += 3;
        parse_branch_op(ctxt, buffer, cline, build_bcs);
    } else if (strncmp(cline, "bvc", 3) == 0) {
        cline += 3;
        parse_branch_op(ctxt, buffer, cline, build_bvc);
    } else if (strncmp(cline, "bhi", 3) == 0) {
        cline += 3;
        parse_branch_op(ctxt, buffer, cline, build_bhi);
    } else if (strncmp(cline, "tst", 3) == 0) {
        cline += 3;
        parse_single_op(buffer, cline, build_tst);
    } else error("Invalid inline ASM.");
    
    return 0;
}

char * skip_to_endline(char * input) {
    while (*input != '\n' && *input != 0) input++;
    input++;
    return input;
}

int asm_block(T_BUFFER * buffer, char * block) {
    T_ASM_CTXT ctxt;
    ctxt.label_count = 0;
    block = trim(block);
    while(*block != 0) {
        trim(block);
        asm_line(&ctxt, buffer, block);
        block = skip_to_endline(block);
        block = trim(block);
    }
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
    printf("[ASM][%x] move.%c (a%d), d%d\n", buffer->length, size2char(size), an, D0);
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