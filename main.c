#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    IF,
    ELSE,
    SUP,
    INF,
    RET,
    EQEQ,
    FOR,
    ADDADD,
    SUBSUB
} T_KEYWORD;

typedef struct T_CTXT {
    enum keyword type;
} T_CTXT;

typedef struct T_ELT {
    char * str;
    int len;
    struct T_ELT * next;
} T_ELT;

typedef struct T_NODE {
    struct T_NODE * desc;
    struct T_NODE * next;
    T_ELT * elt;
    enum keyword type;
    T_CTXT ctxt;
    struct T_NODE * asc;
    int offset;
} T_NODE;



#define U32 unsigned int
#define U16 unsigned short
#define U8 unsigned char

#define ELF_MAGIC 0x464C457F
//#define ELF_MAGIC 0
#define ELF_CLASS_32 1
#define ELF_CLASS_64 2
#define ELF_DATA_LE 1
#define ELF_DATA_BE 2
#define ELF_VERSION 1
#define ELF_OSABI_LINUX 0x03
#define ELF_ABI_VERSION 0
#define ELF_PAD_VALUE 0
#define ELF_TYPE_EXEC 0x02
#define ELF_MACHINE_X86 0x03
#define ELF_MACHINE_X86_64 0x3E
#define ELF_PHOFF_32 0x34

// Program header type
#define PT_NULL     0x00000000
#define PT_LOAD     0x00000001
#define PT_DYNAMIC  0x00000002
#define PT_INTERP   0x00000003
#define PT_NOTE     0x00000004
#define PT_SHLIB    0x00000005
#define PT_PHDR     0x00000006
#define PT_LOOS     0x60000000
#define PT_HIOS     0x6FFFFFFF
#define PT_LOPROC   0x70000000
#define PT_HIPROC   0x7FFFFFFF


// Set the VADDR
#define ELF_ENTRY_VADDR 0x08048000

#define MOV_EAX_0 (U32)0xB800000000
#define MOV_EBX_0 (U32)0xBB00000000
#define INT_0x80 0xCD80CD80

typedef struct T_ELF {
    U32 magic;
    U8 ei_class;
    U8 ei_data;
    U8 ei_version;
    U8 ei_osabi;
    U8 ei_abiversion;
    U8 ei_pad0;
    U8 ei_pad1;
    U8 ei_pad2;
    U8 ei_pad3;
    U8 ei_pad4;
    U8 ei_pad5;
    U8 ei_pad6;
    U16 e_type;
    U16 e_machine;
    U32 e_version;
    U32 e_entry;
    U32 e_phoff;
    U32 e_shoff;
    U32 e_flags;
    U16 e_ehsize;
    U16 e_phentsize;
    U16 e_phnum;
    U16 e_shentsize;
    U16 e_shnum;
    U16 e_shstrndx;
} T_ELF;

typedef struct T_ELF_PRG32_HDR {
    U32 p_type;
    U32 p_offset;
    U32 p_vaddr;
    U32 p_paddr;
    U32 p_filesz;
    U32 p_memsz;
    U32 p_flags;
    U32 p_align;
} T_ELF_PRG32_HDR;

T_ELF elf32 = { 
    ELF_MAGIC, 
    ELF_CLASS_32, 
    ELF_DATA_LE, 
    ELF_VERSION,
    ELF_OSABI_LINUX,
    ELF_ABI_VERSION,
    ELF_PAD_VALUE,
    ELF_PAD_VALUE,
    ELF_PAD_VALUE,
    ELF_PAD_VALUE,
    ELF_PAD_VALUE,
    ELF_PAD_VALUE,
    ELF_PAD_VALUE,
    ELF_TYPE_EXEC,
    ELF_MACHINE_X86,
    ELF_VERSION,
    ELF_ENTRY_VADDR+sizeof(T_ELF)+sizeof(T_ELF_PRG32_HDR),
    ELF_PHOFF_32,
    ELF_PAD_VALUE,
    ELF_PAD_VALUE,
    52,
    sizeof(T_ELF_PRG32_HDR),
    1,
    0x28,
    ELF_PAD_VALUE,
    ELF_PAD_VALUE
};


typedef struct T_BUFFER {
    U8 buffer[10000];
    int length;
    int main_offset;
    T_NODE * top;
    T_NODE * local_symbol[100];
    int local_symbol_count;
} T_BUFFER;


U8 prog[] = {
    0x89, 0xC3,                   // MOV EBX, EAX
    0xB8, 0x01, 0x00, 0x00, 0x00, // MOV EAX, 1
    0xCD, 0x80                    // INT 0x80
};

T_NODE dummy;

 T_ELT elt_int;
T_NODE anonymous_int;

T_ELF_PRG32_HDR elf32_prg_hdr = {
    PT_LOAD,
    0,
    ELF_ENTRY_VADDR,
    ELF_ENTRY_VADDR,
    sizeof(T_ELF)+sizeof(T_ELF_PRG32_HDR)+sizeof(prog),
    sizeof(T_ELF)+sizeof(T_ELF_PRG32_HDR)+sizeof(prog),
    5,
    0x1000
};

void error(char * msg) {
    puts(msg);
    exit(0);
}

void display_all_elt(T_ELT * current) {    
    do {
        for (int i = 0;i < current->len;i++) {
            putchar(current->str[i]);
        }
        putchar('\n');
        current = current->next;
    } while (current != NULL);
}

void display_elt(T_ELT * current) {
    if (current == NULL) return;
    for (int i = 0;i < current->len;i++) {
        putchar(current->str[i]);
    }
}

void display_elt_ctxt(char * msg, T_ELT * elt) {
    printf("%s [", msg);
    display_elt(elt);
    printf("]\n");
}

char * read_file(char * filename, int * size) {
    
    FILE * file = fopen(filename, "r");
    fseek(file, 0L, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char * fout = (char *)malloc(sizeof(char)**size);
    fread(fout, sizeof(char), *size, file);
    return fout;
}

enum keyword type(T_ELT * elt) {
    enum keyword result = EXPR;
    if (elt->len == 4 && strncmp(elt->str, "void", elt->len) == 0) result = VOID;
    else if (elt->len == 3 && strncmp(elt->str, "int", elt->len) == 0) result = INT;
    else if (elt->len == 4 && strncmp(elt->str, "char", elt->len) == 0) result = CHAR;
    else if (elt->len == 5 && strncmp(elt->str, "short", elt->len) == 0) result = SHORT;
    else if (elt->len == 2 && strncmp(elt->str, "if", elt->len) == 0) result = IF;
    else if (elt->len == 4 && strncmp(elt->str, "else", elt->len) == 0) result = ELSE;
    else if (elt->len == 6 && strncmp(elt->str, "return", elt->len) == 0) result = RET;
    else if (elt->len == 2 && strncmp(elt->str, "==", elt->len) == 0) result = EQEQ;
    else if (elt->len == 3 && strncmp(elt->str, "for", elt->len) == 0) result = FOR;
    else if (elt->len == 2 && strncmp(elt->str, "++", elt->len) == 0) result = ADDADD;
    else if (elt->len == 2 && strncmp(elt->str, "--", elt->len) == 0) result = SUBSUB;
    else if (strncmp(elt->str, "(", elt->len) == 0) result = PAR_O;
    else if (strncmp(elt->str, ")", elt->len) == 0) result = PAR_C;
    else if (strncmp(elt->str, "{", elt->len) == 0) result = ACC_O;
    else if (strncmp(elt->str, "}", elt->len) == 0) result = ACC_C;
    else if (strncmp(elt->str, "*", elt->len) == 0) result = PTR;
    else if (strncmp(elt->str, "+", elt->len) == 0) result = ADD;
    else if (strncmp(elt->str, "-", elt->len) == 0) result = SUB;
    else if (strncmp(elt->str, ";", elt->len) == 0) result = END;
    else if (strncmp(elt->str, ",", elt->len) == 0) result = COM;
    else if (strncmp(elt->str, "=", elt->len) == 0) result = EQ;
    else if (strncmp(elt->str, ">", elt->len) == 0) result = SUP;
    else if (strncmp(elt->str, "<", elt->len) == 0) result = INF;
    return result;
} 

int is_number(T_ELT * elt) {
    if (elt == NULL) return 0;
     if (elt->len > 0 && elt->str[0] >= '0' && elt->str[0] <= '9') 
        return 1;
     else return 0;
}

int is_main(T_ELT * elt) {
    if (strncmp(elt->str, "main", elt->len) == 0)
        return 1;
    return 0;
}

T_NODE * successor(T_NODE * node) {
    if (node == NULL) return NULL;
    else if (node->next != NULL) return NULL;
    else return node->desc;
}


T_ELT * add_token(T_ELT * current, char * p, int len) {
    T_ELT * new_c = (T_ELT *)malloc(sizeof(T_ELT));
    new_c->str = p;
    new_c->len = len;
    new_c->next = NULL;
    if (current != NULL) {
        current->next = new_c;
    }
    return new_c;
}

T_NODE * add_desc_node(T_NODE * father, T_ELT * elt) {
    T_NODE * new_n = (T_NODE *)malloc(sizeof(T_NODE));
    new_n->desc = NULL;
    new_n->next = NULL;
    new_n->elt = elt;
    new_n->ctxt.type = END;
    if (elt != NULL)
        new_n->type = type(elt);
    else new_n->type = END;
    new_n->asc = father;
    if (father != NULL) {
        if (father->desc == NULL)
            father->desc = new_n;
        else {
            T_NODE * node = father->desc;
            while (node->next != NULL) {
                node = node->next;
            }
            node->next = new_n;           
        }
    }    
    return new_n;
}

T_NODE * add_next_node(T_NODE * node, T_ELT * elt, T_CTXT ctxt) {
    T_NODE * new_n = (T_NODE *)malloc(sizeof(T_NODE));
    new_n->desc = NULL;
    new_n->next = NULL;
    new_n->elt = elt;
    new_n->ctxt = ctxt;
    new_n->type = type(elt);
    new_n->asc = node->asc;

    while (node->next != NULL) {
        node = node->next;
    }
    node->next = new_n;

    return new_n;
}

T_ELT * tokenize(char * input,int size) {
    T_ELT * head = NULL;
    T_ELT * current = NULL;

    int current_exp = -1;
    for(int c = 0;c < size; c++) {
        if (input[c] == ' ' || input[c] == '\n') {
            input[c] = 0;
        }

        char elt = input[c];
        switch (elt) {
            case 0:
                if (current_exp != -1) {
                    current = add_token(current, &input[current_exp], c-current_exp);
                    current_exp = -1;
                }
                break; 
            case '{':
            case '}':
            case ')':
            case '(':
            case ',':
            case '*':
            case ';':
                if (current_exp != -1) {
                    current = add_token(current, &input[current_exp], c-current_exp);
                    current_exp = -1;
                }            
                current = add_token(current, &input[c], 1);
                break;
            case '-':
            case '+':
            case '=':
                if (current_exp == -1) {
                    current_exp = c;
                } else if (input[c-1] == elt)  {
                    current = add_token(current, &input[current_exp], c-current_exp + 1);
                    current_exp = -1;                     
                } else {
                    current = add_token(current, &input[current_exp], c-current_exp);
                    current_exp = c;                     
                }
                break;
            default:
                if (current_exp == -1) {
                    current_exp = c;
                } else if (input[c-1] == '=' || input[c-1] == '+' || input[c-1] == '-')  {
                    current = add_token(current, &input[current_exp], c-current_exp);
                    current_exp = c;                     
                }
                break;
        }
    
        if (head == NULL) {
            head = current;
        }
    }

    return head;
}

void create_node_expr(T_NODE * up, T_ELT * current, T_CTXT ctxt) {
    T_NODE * current_n = NULL;
    
    if (current == NULL) 
        return;

    display_elt(current);
    putchar(' ');

    if (up == NULL)
        return;

    enum keyword t_current = type(current);
    switch (t_current) {
        case VOID:
        case INT:
        case CHAR:
        case SHORT:
            ctxt.type = t_current;
            create_node_expr(up, current->next, ctxt);
            break;
        case PAR_C:
        case ACC_C:
            ctxt.type = t_current;
            current_n = add_next_node(up->asc, current, ctxt);
            create_node_expr(current_n->asc, current->next, ctxt);
            break;
        case END:
        case EQ:
        case ADD:
        case SUB:
        case INF:
        case SUP:
        case EQEQ:
        case RET:
        case COM:
        case FOR:
        case ADDADD:
        case SUBSUB:
            ctxt.type = t_current;
            current_n = add_next_node(up, current, ctxt);
            create_node_expr(current_n, current->next, ctxt);      
            break;
        case PAR_O:
        case ACC_O:
            ctxt.type = t_current;
            current_n = add_desc_node(up, current);
            current_n = add_desc_node(current_n, NULL);
            create_node_expr(current_n, current->next, ctxt);
            break;
        case IF:
        case ELSE:
        case EXPR:
            current_n = add_next_node(up, current, ctxt);
            create_node_expr(current_n, current->next, ctxt);
            break;
        default:
            create_node_expr(up, current->next, ctxt);
            break;
    }
}

int is_type(enum keyword k) {
    switch(k) {
        case VOID:
        case INT:
        case CHAR:
        case SHORT:
            return 1;
            break;
        default :
            return 0;
            break;        
    }
}

int is_end(enum keyword k) {
    switch(k) {
        case COM:
            return 1;
            break;
        default:
            return 0;
            break;
    }
}

int is_matching(T_NODE * nodeA, T_NODE * nodeB) {
    if ( nodeA == NULL || nodeA->elt == NULL) return 0;

   int minlen = (nodeA->elt->len > nodeB->elt->len)? nodeA->elt->len: nodeB->elt->len;

    if (strncmp(nodeA->elt->str, nodeB->elt->str, minlen) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int is_procedure_body(T_NODE * up) {
    
    if ((up->type == EXPR) && is_type(up->ctxt.type)
        && (up->desc != NULL) && (up->desc->type == PAR_O)) {
        return 1;
    }

    return 0;
}

int is_procedure_call(T_NODE * up) {
    if ((up->type == EXPR) && !is_type(up->ctxt.type)
        && up->desc != NULL &&(up->desc->type == PAR_O)) {
        return 1;
    }

    return 0;
}

int is_variable_decl(T_NODE * up) {
    if ((up->type == EXPR) && is_type(up->ctxt.type)) {
        return 1;
    }
    return 0;
}

int variable_size(T_NODE * up) {
    if (is_number(up->elt)) {
        return 4;
    }

    switch (up->ctxt.type) {
        case INT: 
            return 4;
            break;
        case SHORT: 
            return 2;
            break;            
        case CHAR:
            return 1;
            break;
        default:
            display_elt_ctxt("Error on variable size: ", up->elt);    
            printf("TYPE: %d %d\n", up->ctxt.type, INT);      
            return 4;
            //exit(0);
            //return 0;

    }
}

// mov EAX, value
void asm_load_eax(U32 value, T_BUFFER * buffer) {
    printf("[ASM][%x] mov eax,  %d\n", buffer->length, value);
    buffer->buffer[buffer->length++] = 0xB8; 
    memcpy( &(buffer->buffer[buffer->length]), &value , sizeof(value));
    buffer->length += 4;
} 

//sub esp, size
void asm_add_variable(T_NODE * up, T_BUFFER * buffer) {
    U8 size  = variable_size(up);
    printf("[ASM][%x] sub esp, %d\n", buffer->length, size);
    buffer->buffer[buffer->length++] = 0x83; 
    buffer->buffer[buffer->length++] = 0xEC;
    buffer->buffer[buffer->length++] = size;
}

// mov eax, DWORD PTR SS:[esp]
void asm_retrieve_variable(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] mov eax, DWORD PTR SS:[esp + %d]\n", buffer->length, offset);
    buffer->buffer[buffer->length++] = 0x8B;
    buffer->buffer[buffer->length++] = 0x44; 
    buffer->buffer[buffer->length++] = 0x24;
    buffer->buffer[buffer->length++] = offset; 
}

//add DWORD PTR SS:[esp], eax
void asm_add_variable_and_store(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] add DWORD PTR SS:[esp + %d], eax\n", buffer->length, offset);
    buffer->buffer[buffer->length++] = 0x01;
    buffer->buffer[buffer->length++] = 0x44;
    buffer->buffer[buffer->length++] = 0x24;
    buffer->buffer[buffer->length++] = offset;
}

//sub DWORD PTR SS:[esp], eax
void asm_sub_variable_and_store(int offset, T_BUFFER * buffer) {
    printf("[ASM][%x] sub DWORD PTR SS:[esp + %d], eax\n", buffer->length, offset);
    buffer->buffer[buffer->length++] = 0x29;
    buffer->buffer[buffer->length++] = 0x44;
    buffer->buffer[buffer->length++] = 0x24;
    buffer->buffer[buffer->length++] = offset;
}

//mov DWORD PTR SS:[esp], eax
void asm_store_variable(int offset, T_BUFFER * buffer) {
    
    printf("[ASM][%x] mov DWORD PTR SS:[esp + %d], eax\n", buffer->length, offset);
    buffer->buffer[buffer->length++] = 0x89;
    buffer->buffer[buffer->length++] = 0x44; 
    buffer->buffer[buffer->length++] = 0x24;
    buffer->buffer[buffer->length++] = offset;
}

//add esp, size
void asm_remove_variable(T_BUFFER * buffer, U8 size) {
    printf("[ASM][%x] add esp, %d\n", buffer->length, size);
    buffer->buffer[buffer->length++] = 0x83; 
    buffer->buffer[buffer->length++] = 0xC4;
    buffer->buffer[buffer->length++] = size;    
}

//ret
void asm_ret(T_BUFFER * buffer) {
    printf("[ASM][%x] ret\n", buffer->length);
    buffer->buffer[buffer->length++] = 0xC3;
}

//call [addr]
void asm_call(T_BUFFER * buffer, U32 addr) {
    addr = addr - 5;
    printf("[ASM][%x] call %x\n",buffer->length, addr);
    buffer->buffer[buffer->length++] = 0xE8; 
    memcpy( &(buffer->buffer[buffer->length]), &addr , sizeof(addr));
    buffer->length += 4;
}

//mov ebx, eax
void asm_mov_ebx_eax(T_BUFFER * buffer) {
    printf("[ASM][%x] mov ebx, eax\n", buffer->length);
    buffer->buffer[buffer->length++] = 0x89; 
    buffer->buffer[buffer->length++] = 0xC3;     
}

//cmp eax, ebx
void asm_cmp_eax_ebx(T_BUFFER * buffer) {
    printf("[ASM][%x] cmp eax, ebx\n", buffer->length);
    buffer->buffer[buffer->length++] = 0x39; 
    buffer->buffer[buffer->length++] = 0xD8;        
}

//jg addr
void asm_jump_greater(T_BUFFER * buffer, U32 addr) {
    printf("[ASM][%x] jg %x\n",buffer->length, addr);
    buffer->buffer[buffer->length++] = 0x0F;
    buffer->buffer[buffer->length++] = 0x8F;
    memcpy( &(buffer->buffer[buffer->length]), &addr , sizeof(addr));
    buffer->length += 4;
}

//jge addr
void asm_jump_greater_eq(T_BUFFER * buffer, U32 addr) {
    printf("[ASM][%x] jg %x\n",buffer->length, addr);
    buffer->buffer[buffer->length++] = 0x0F;
    buffer->buffer[buffer->length++] = 0x8D;
    memcpy( &(buffer->buffer[buffer->length]), &addr , sizeof(addr));
    buffer->length += 4;
}


//jl addr
void asm_jump_less(T_BUFFER * buffer, int addr) {
    printf("[ASM][%x] jl %x\n",buffer->length, addr);
    buffer->buffer[buffer->length++] = 0x0F;
    buffer->buffer[buffer->length++] = 0x8C;
    memcpy( &(buffer->buffer[buffer->length]), &addr , sizeof(addr));
    buffer->length += 4;
}

//jle addr
void asm_jump_less_eq(T_BUFFER * buffer, int addr) {
    printf("[ASM][%x] jl %x\n",buffer->length, addr);
    buffer->buffer[buffer->length++] = 0x0F;
    buffer->buffer[buffer->length++] = 0x8E;
    memcpy( &(buffer->buffer[buffer->length]), &addr , sizeof(addr));
    buffer->length += 4;
}

//je addr
void asm_jump_equal(T_BUFFER * buffer, U32 addr) {
    printf("[ASM][%x] je %x\n",buffer->length, addr);
    buffer->buffer[buffer->length++] = 0x0F;
    buffer->buffer[buffer->length++] = 0x84;
    memcpy( &(buffer->buffer[buffer->length]), &addr , sizeof(addr));
    buffer->length += 4;
}

//jne addr
void asm_jump_not_equal(T_BUFFER * buffer, U32 addr) {
    printf("[ASM][%x] jne %x\n",buffer->length, addr);
    buffer->buffer[buffer->length++] = 0x0F;
    buffer->buffer[buffer->length++] = 0x85;
    memcpy( &(buffer->buffer[buffer->length]), &addr , sizeof(addr));
    buffer->length += 4;
}


//jmp addr
void asm_jump(T_BUFFER * buffer, int addr) {
    printf("[ASM][%x] jmp %x\n",buffer->length, addr);
    buffer->buffer[buffer->length++] = 0xE9;
    memcpy( &(buffer->buffer[buffer->length]), &addr , sizeof(addr));
    buffer->length += 4;
}

int get_variable_offset(T_NODE * target, T_BUFFER * buffer) {  
    int offset = 0;
    for (int i = 0; (i < buffer->local_symbol_count) && (!is_matching(buffer->local_symbol[buffer->local_symbol_count - i - 1], target));i++) {        
        offset += variable_size(buffer->local_symbol[buffer->local_symbol_count - i - 1]);
    }
    return offset;
}

int get_variable_dynamic_offset(int target, T_BUFFER * buffer) {
    int offset = 0;
    for (int i = buffer->local_symbol_count - 1; i > target ;i--) {
        offset += variable_size(buffer->local_symbol[i]);
    }
    return offset;
}

int get_variable_dynamic_offset_from_symbol(T_NODE * target, T_BUFFER * buffer) {
    for (int i = 0; i < buffer->local_symbol_count;i++) {        
        if (buffer->local_symbol[i] != NULL && is_matching(buffer->local_symbol[i], target)) {
            return i;
        }
    }

    return -1;
}

int get_all_variable_offset(T_BUFFER * buffer) {
    int offset = 0;
    for (int i = 0;i < buffer->local_symbol_count && buffer->local_symbol[buffer->local_symbol_count - i - 1]->elt != NULL;i++) {

        if (buffer->local_symbol[buffer->local_symbol_count - i - 1]->elt != NULL)
            offset += variable_size(buffer->local_symbol[buffer->local_symbol_count - i - 1]);
    }
    return offset;
}

T_NODE * skip(T_NODE * up) {
    if (up == NULL || up->type == END)
        return up;
    else return skip(up->next);
}

int proc_lookup(T_NODE * up, T_NODE * target) {
    if (up == NULL) {
        return -1;
    }

    if (is_procedure_body(up) && is_matching(up, target)) {
        return up->offset;
    } else {
        return proc_lookup(up->next, target);
    }
}
T_NODE * step(T_NODE * up, int stack_offset, T_BUFFER * buffer);

int add_local_symbol(T_NODE * up, T_BUFFER * buffer, char is_anon) {
    asm_add_variable(up, buffer);
    if (!is_anon)
        buffer->local_symbol[buffer->local_symbol_count++] = up;
    else
        buffer->local_symbol[buffer->local_symbol_count++] = &anonymous_int;

    return buffer->local_symbol_count - 1;
}

T_NODE * one(T_NODE * up, int stack_offset, T_BUFFER * buffer) {
    
    return step(up, stack_offset, buffer);;
}

T_NODE * line(T_NODE * up, int stack_offset, T_BUFFER * buffer) {
    while (up != NULL && up->type != END && up->type != COM) {
        up = one(up, stack_offset, buffer);
        if (up != NULL && up->type != END) 
            up = up->next;
    }
    return up;
}

T_NODE * block(T_NODE * up, int stack_offset, T_BUFFER * buffer) {
    while (up != NULL) {
        up = line(up, stack_offset, buffer);
        if (up != NULL)
            up = up->next;
    }
    return up;
}

void stack_parameters(T_NODE * up, T_BUFFER * buffer) {
    if (up->elt == NULL || up->type == COM)
        return  stack_parameters(up->next, buffer);

    display_elt_ctxt("STACK parameter: ", up->elt);  

    if (up->elt != NULL) {
        int doffset = add_local_symbol(up, buffer, 1);
        
        one(up, doffset, buffer);
        asm_store_variable(get_variable_dynamic_offset(doffset, buffer), buffer);
        if (up->next != NULL)
            line(up->next, doffset, buffer);
        
        //dodgy !
        //offset = variable_size(up);
        while(up != NULL && up->type != PAR_C && up->type != COM) up = up->next;
    }
    
    if (up != NULL) {
        stack_parameters(up, buffer);
    } 
}

void parameter_scan(T_NODE * up, T_BUFFER * buffer) {
    if (up == NULL)
        return;
    else if (is_variable_decl(up)) {
        buffer->local_symbol[buffer->local_symbol_count++] = up;
    }
    parameter_scan(up->next, buffer);
}



T_NODE * step(T_NODE * up, int stack_offset, T_BUFFER * buffer) {
    display_elt_ctxt("STEP: ", up->elt);    

    if (up == NULL) error("INVALID STEP");

    if (is_procedure_body(up)) {
        display_elt_ctxt("Body declaration: ", up->elt);

        buffer->local_symbol_count = 0;

        if (is_main(up->elt)) {
            buffer->main_offset = buffer->length;
        }
        up->offset = buffer->length;

        if (up->desc != NULL) {
            parameter_scan(up->desc->desc, buffer);
        }

        printf("LOCAL SYMBOL IN SIGNATURE: %d\n", buffer->local_symbol_count);
        buffer->local_symbol[buffer->local_symbol_count++] = &dummy; // Symbol for return adress of the call

        return block(up->desc->next->next->desc, -1, buffer);
    } else if (is_variable_decl(up)) {
        display_elt_ctxt("Variable declaration: ", up->elt);

        int doffset = add_local_symbol(up, buffer, 0);  

        return line(up->next, doffset, buffer);
    } else if (is_number(up->elt)) {
        
        T_ELT * elt = up->elt; 
        char value[32];
        for (int i = 0;i < elt->len;i++) {
            value[i] = elt->str[i]; 
        }

        value[elt->len] = 0;

        int v = atoi(value);
    
        asm_load_eax(v, buffer);
        return up;

     } else if(is_procedure_call(up)){
        display_elt_ctxt("Procedure call: ", up->elt);

        int offset = proc_lookup(buffer->top, up);
        if (offset == -1) error("No matching proc declaration.");

        if (up->desc != NULL && up->desc->type == PAR_O)
            stack_parameters(up->desc->desc, buffer);
        else 
            error("Invalid procedure call");

        asm_call(buffer, offset - buffer->length);
        
        return up;
    } else if (up->type == EXPR) {
        display_elt_ctxt("Variable assign: ", up->elt);
        
        T_NODE * last = up;
        asm_retrieve_variable(get_variable_offset(up, buffer), buffer);
        if (stack_offset == -1) {
            int doffset = get_variable_dynamic_offset_from_symbol(up, buffer);
            
            last = line(up->next, doffset, buffer);
        } 
        return last;
    } else if (up->type == SUP || up->type == INF || up->type == EQEQ) {
        
        puts("STORING TO EBX FOR COMP");
        asm_mov_ebx_eax(buffer);
        T_NODE * end = line(up->next, -1, buffer);
        asm_cmp_eax_ebx(buffer);
        if (up-> type == INF)
            asm_jump_less_eq(buffer, 0);
        else if (up->type == SUP)
            asm_jump_greater_eq(buffer, 0);
        else asm_jump_not_equal(buffer, 0); 
        return end;   

    } else if (up->type == RET) {

        int doffset = add_local_symbol(up, buffer, 1);
        T_NODE * last = one(up->next, doffset, buffer);
        asm_store_variable(get_variable_dynamic_offset(doffset, buffer), buffer);
        if (last != NULL && last->next != NULL)
            last = line(last->next, doffset, buffer);

        asm_retrieve_variable( get_variable_dynamic_offset(doffset, buffer), buffer);
        asm_remove_variable(buffer, get_all_variable_offset(buffer));
        asm_ret(buffer);      
        return last;

    } else if (up->type == IF){

        puts("IF STATEMENT");
        U8 * jgptr = NULL;
        U8 * jmptr = NULL; 
        U32 offset = 0;
        U32 offset2 = 0;
        if (up->desc != NULL && up->desc->desc != NULL && up->desc->desc->next != NULL) {
            block(up->desc->desc->next, -1, buffer);
            jgptr = &(buffer->buffer[buffer->length - 4]);
            offset = buffer->length;
        } else error("Error on \"if\" statement. condition clause");


        puts("IF FIRST BODY");
        int local_sym_count = buffer->local_symbol_count;
        if (up->desc != NULL && up->desc->next != NULL && up->desc->next->next != NULL && up->desc->next->next->desc != NULL && up->desc->next->next->desc->next != NULL) {
            block(up->desc->next->next->desc->next, -1, buffer);
            asm_jump(buffer, 0);
            jmptr = &buffer->buffer[buffer->length - 4];
            offset2 = buffer->length;
        } else error("Error on \"if\" statement. first condition body");
        buffer->local_symbol_count = local_sym_count;

        offset = buffer->length - offset;
        memcpy( jgptr, &offset, sizeof(U32) );

        puts("IF SECOND BODY");
        
        if (up->next != NULL && up->next->type == ELSE) 
            if (up->next->desc != NULL && up->next->desc->desc != NULL && up->next->desc->desc->next != NULL) {
                block(up->next->desc->desc->next, -1, buffer);
            } else error("Error on \"if\" statement. second condition body");

        buffer->local_symbol_count  = local_sym_count;
        offset2 = buffer->length - offset2;
        memcpy( jmptr, &offset2, sizeof(U32) );
        
        if (up->next != NULL && up->next->type == ELSE)
            up = up->next;

        return up;

    } else if (up->type == FOR) {
        int s_count = buffer->local_symbol_count;

        T_NODE * last = line(up->desc->desc->next, -1, buffer);
        U32 offset = buffer->length;
        last = line(last->next, -1, buffer);
        U32 offset3 = buffer->length;
        U8 * jcondptr = &(buffer->buffer[buffer->length - 4]);
        block(up->desc->next->next->desc, -1, buffer);
        line(last->next, -1, buffer);
        asm_jump(buffer, 0);
        U8 * jcondptr2 = &(buffer->buffer[buffer->length - 4]);
        
        int offset2 =  offset - buffer->length;
        memcpy( jcondptr2, &offset2, sizeof(U32) );
        offset3 = buffer->length - offset3;
        memcpy( jcondptr, &offset3, sizeof(U32) );

        
        asm_remove_variable(buffer, get_variable_dynamic_offset((s_count == 0) ? s_count : s_count - 1, buffer));
        buffer->local_symbol_count = s_count;
        return up;
        
    } else if (up->type == EQ) {

        T_NODE * last = one(up->next, stack_offset, buffer);
        asm_store_variable(get_variable_dynamic_offset(stack_offset, buffer), buffer);
        return last;
        
    } else if (up->type == ADD) {
        T_NODE * last = one(up->next, stack_offset, buffer);
        asm_add_variable_and_store(get_variable_dynamic_offset(stack_offset, buffer), buffer);
 
        return last;

    } else if (up->type == SUB) {
        T_NODE * last = one(up->next, stack_offset, buffer);
        asm_sub_variable_and_store(get_variable_dynamic_offset(stack_offset, buffer), buffer);
 
        return last;
    } else if (up->type == ADDADD) {
        asm_load_eax(1, buffer);
        asm_add_variable_and_store(get_variable_dynamic_offset(stack_offset, buffer), buffer);
        return up;
    } else if (up->type == SUBSUB) {
        asm_load_eax(1, buffer);
        asm_sub_variable_and_store(get_variable_dynamic_offset(stack_offset, buffer), buffer);
        return up;
    }

    
    return up;
}

void display_node(T_NODE * node, int spacing) {
    if (node == NULL) 
        return;

    for (int i = 0;i < spacing;i++) putchar('-');
    display_elt(node->elt);

    if (node->ctxt.type == VOID) {
        printf("[VOID]");
    }
    
    if (node->ctxt.type == INT) {
        printf("[INT]");
    }

    if (node->ctxt.type == CHAR) {
        printf("[CHAR]");
    }
    
    putchar('\n');
    display_node(node->desc, spacing+2);
    display_node(node->next, spacing);
}

void write_output(char * filename,  T_BUFFER * buffer) {
    FILE * f = fopen(filename, "wb");

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

void main(int c, char** argv) {
    dummy.ctxt.type = INT;
    dummy.elt = NULL;

    
    elt_int.next = NULL;
    elt_int.str = "__ANONYMOUS";
    elt_int.len = strlen(elt_int.str);
    anonymous_int.elt = &elt_int;
    anonymous_int.ctxt.type = INT;

    printf("Reading file %s\n",argv[1]);
    int size;
    char * filedata = read_file(argv[1], &size);
    T_ELT * elt = tokenize(filedata, size);
    display_all_elt(elt);
    T_NODE * up = add_desc_node(NULL, NULL);
    T_CTXT ctxt;
    ctxt.type = END;
    printf("CREATING AST\n");
    create_node_expr(up, elt, ctxt);
    display_node(up,0);
    
    T_BUFFER * buffer = (T_BUFFER *)malloc(sizeof(T_BUFFER));
    buffer->length = 0;
    buffer->top = up;
    buffer->local_symbol_count = 0;
    
    while(up != NULL) {
        one(up, -1, buffer);
        up = up->next; 
    };
    write_output("out", buffer);
    free(buffer);
    free(filedata);
}