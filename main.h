#ifndef MAIN

enum keyword {
    VOID,
    INT,
    SHORT,
    CHAR,
    PAR_C,
    PAR_O,
    ACC_C,
    ACC_O,
    PTR,
    ADD,
    SUB,
    END,
    EXPR,
    COM,
    EQ,
    NEQ,
    IF,
    ELSE,
    SUP,
    INF,
    RET,
    EQEQ,
    FOR,
    ADDADD,
    SUBSUB,
    WHILE,
    DO,
    AND,
    BR_O,
    BR_C,
    CUSTOM_T,
    DIV,
    STR,
    CCHAR,
    ASM
} T_KEYWORD;

typedef struct T_CTXT {
    enum keyword type;
} T_CTXT;

typedef struct T_ELT {
    int line;
    char * str;
    int len;
    struct T_ELT * next;
} T_ELT;

typedef struct T_NODE {
    struct T_NODE * desc;
    struct T_NODE * next;
    struct T_NODE * prev;
    T_ELT * elt;
    enum keyword type;
    struct T_NODE * asc;
    int offset;
} T_NODE;



#define U32 unsigned int
#define U16 unsigned short
#define U8 unsigned char



typedef struct T_BUFFER {
    U8 buffer[10000];
    int length;
    int main_offset;
    T_NODE * top;
    T_NODE * local_symbol[100];
    int local_symbol_count;
    T_NODE * global_symbol[100];
    int global_symbol_count;    
} T_BUFFER;

void error(char * msg) {
    puts(msg);
    exit(-1);
}

int variable_size(T_NODE * up);
void error_elt(T_ELT * elt, char * msg);

#define MAIN
#endif