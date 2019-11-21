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
    CCHAR
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
    //T_CTXT ctxt;
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
    T_NODE * global_symbol[100];
    int global_symbol_count;    
} T_BUFFER;


U8 prog[] = {
    0x89, 0xC3,                   // MOV EBX, EAX
    0xB8, 0x01, 0x00, 0x00, 0x00, // MOV EAX, 1
    0xCD, 0x80                    // INT 0x80
};

T_NODE dummy;

T_ELT elt_int;
T_NODE anonymous_int;
T_NODE anonymous_short;
T_NODE anonymous_char;
int START = ELF_ENTRY_VADDR + sizeof(T_ELF) + sizeof(T_ELF_PRG32_HDR);

T_ELF_PRG32_HDR elf32_prg_hdr = {
    PT_LOAD,
    0,
    ELF_ENTRY_VADDR,
    ELF_ENTRY_VADDR,
    sizeof(T_ELF)+sizeof(T_ELF_PRG32_HDR)+sizeof(prog),
    sizeof(T_ELF)+sizeof(T_ELF_PRG32_HDR)+sizeof(prog),
    0x4     
    | 0x1   //EXECUTE 
    | 0x2   //WRITE
    ,0x1000
};

void error(char * msg) {
    puts(msg);
    exit(-1);
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

void error_elt(T_ELT * elt, char * msg) {
    printf("Error at line %d near ", elt->line);
    display_elt(elt);
    printf(". ");
    error(msg);
}

void display_elt_ctxt(char * msg, T_ELT * elt) {
    printf("%s [", msg);
    display_elt(elt);
    printf("]\n");
}

char * read_file(char * filename, int * size) {
    
    FILE * file = fopen(filename, "r");
    if (file == NULL) error("Unable to open file");

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
    else if (elt->len == 2 && strncmp(elt->str, "!=", elt->len) == 0) result = NEQ;
    else if (elt->len == 3 && strncmp(elt->str, "for", elt->len) == 0) result = FOR;
    else if (elt->len == 2 && strncmp(elt->str, "++", elt->len) == 0) result = ADDADD;
    else if (elt->len == 2 && strncmp(elt->str, "--", elt->len) == 0) result = SUBSUB;
    else if (elt->len == 2 && strncmp(elt->str, "do", elt->len) == 0) result = DO;
    else if (elt->len == 5 && strncmp(elt->str, "while", elt->len) == 0) result = WHILE;
    else if (strncmp(elt->str, "(", elt->len) == 0) result = PAR_O;
    else if (strncmp(elt->str, ")", elt->len) == 0) result = PAR_C;
    else if (strncmp(elt->str, "{", elt->len) == 0) result = ACC_O;
    else if (strncmp(elt->str, "}", elt->len) == 0) result = ACC_C;
    else if (strncmp(elt->str, "[", elt->len) == 0) result = BR_O;
    else if (strncmp(elt->str, "]", elt->len) == 0) result = BR_C;    
    else if (strncmp(elt->str, "*", elt->len) == 0) result = PTR;
    else if (strncmp(elt->str, "/", elt->len) == 0) result = DIV;
    else if (strncmp(elt->str, "+", elt->len) == 0) result = ADD;
    else if (strncmp(elt->str, "-", elt->len) == 0) result = SUB;
    else if (strncmp(elt->str, ";", elt->len) == 0) result = END;
    else if (strncmp(elt->str, ",", elt->len) == 0) result = COM;
    else if (strncmp(elt->str, "=", elt->len) == 0) result = EQ;
    else if (strncmp(elt->str, ">", elt->len) == 0) result = SUP;
    else if (strncmp(elt->str, "<", elt->len) == 0) result = INF;
    else if (strncmp(elt->str, "&", elt->len) == 0) result = AND;
    else if (elt->len > 0 && elt->str[0]=='"') result = STR;
    else if (elt->len > 0 && elt->str[0]=='\'') result = CCHAR;
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

#define NEXT 1
#define DESC 2

T_NODE * get_token(T_NODE * up, char successor) {
    switch (successor) {
        case NEXT:
            if (up->next == NULL) error_elt(up->elt, "");
            else return up->next;
            break;
        case DESC:
            if (up->desc == NULL) error_elt(up->elt, "");
            else return up->desc;
            break;        
    }
    return NULL;
}

T_NODE * get_token_2(T_NODE * up, char a, char b) {
    return get_token(get_token(up, a), b);
}

T_NODE * get_token_3(T_NODE * up, char a, char b, char c) {
    return get_token(get_token(get_token(up, a), b), c);
}

T_NODE * get_token_4(T_NODE * up, char a, char b, char c, char d) {
    return get_token(get_token(get_token(get_token(up, a), b), c), d);
}

T_NODE * get_token_5(T_NODE * up, char a, char b, char c, char d, char e) {
    return get_token(get_token_4(up, a, b, c, d), e);
}


T_NODE * get_token_null(T_NODE * up, char successor) {
    if (up == NULL) return NULL;
    switch (successor) {
        case NEXT:
            if (up->next == NULL)   return NULL;
            else return up->next;
            break;
        case DESC:
            if (up->desc == NULL) return NULL;
            else return up->desc;
            break;        
    }
    return NULL;
}

T_NODE * get_token_3_null(T_NODE * up, char a, char b, char c) {
    return get_token_null(get_token_null(get_token_null(up, a), b), c);
}

T_NODE * get_token_4_null(T_NODE * up, char a, char b, char c, char d) {
    return get_token_null(get_token_null(get_token_null(get_token_null(up, a), b), c), d);
}

T_NODE * get_token_5_null(T_NODE * up, char a, char b, char c, char d, char e) {
    return get_token_null(get_token_4_null(up, a, b, c, d), e);
}



T_ELT * add_token(T_ELT * current, char * p, int len, int line) {
    T_ELT * new_c = (T_ELT *)malloc(sizeof(T_ELT));
    new_c->str = p;
    new_c->len = len;
    new_c->next = NULL;
    new_c->line = line;
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
    //new_n->ctxt.type = END;
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

T_NODE * add_next_node(T_NODE * node, T_ELT * elt, T_NODE * prev) {
    T_NODE * new_n = (T_NODE *)malloc(sizeof(T_NODE));
    new_n->desc = NULL;
    new_n->next = NULL;
    new_n->elt = elt;
    new_n->prev = prev;
    //new_n->ctxt = ctxt;
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
    int line = 0;
    int current_exp = -1;
    for(int c = 0;c < size; c++) {
        if (input[c] == '\n') line++;

        if (input[c] == ' ' || input[c] == '\n') {
            input[c] = 0;
        }

        char elt = input[c];
        switch (elt) {
            case 0:
                if (current_exp != -1) {
                    current = add_token(current, &input[current_exp], c-current_exp, line);
                    current_exp = -1;
                }
                break; 
            case '{':
            case '}':
            case ')':
            case '(':
            case '[':
            case ']':
            case ',':            
            case ';':
                if (current_exp != -1) {
                    current = add_token(current, &input[current_exp], c-current_exp, line);
                    current_exp = -1;
                }            
                current = add_token(current, &input[c], 1, line);
                break;
            case '*':
            
                if (c > 1 && input[c-1] == '/') {                    
                    while (c < size && !(input[c] == '/' && input[c-1] == '*')) {
                        if (input[c] != '\n') line++;
                        c++;
                    }
                    current_exp = -1;
                } else {

                    if (current_exp == -1) {
                        current_exp = c;
                    } else {
                        current = add_token(current, &input[current_exp], c-current_exp, line);
                        current_exp = c;                     
                    }
                }
                break;
            case '-':
            case '+':            
            case '=':
            case '&':
                if (current_exp == -1) {
                    current_exp = c;
                } else if (input[c-1] == elt)  {
                    current = add_token(current, &input[current_exp], c-current_exp + 1, line);
                    current_exp = -1;                     
                } else {
                    current = add_token(current, &input[current_exp], c-current_exp, line);
                    current_exp = c;                     
                }
                break;
            case '!':
                if (current_exp != -1) {
                    current = add_token(current, &input[current_exp], c-current_exp, line);
                }  
               
                current_exp = c;
                if (input[c+1] == '=') {
                    current = add_token(current, &input[current_exp], 2, line);
                    c++;
                    current_exp = -1;
                }
                break;
            case '/':
                if (current_exp == -1) {
                    current_exp = c;
                } else if (input[c-1] == elt)  {
                    while(c < size && input[c] != '\n') c++;
                    line++;  
                    current_exp = -1;             
                } else {
                    current = add_token(current, &input[current_exp], c-current_exp, line);
                    current_exp = c;                     
                }
                break;
            case '"':
                if (current_exp != -1) {
                    current = add_token(current, &input[current_exp], c-current_exp, line);
                }  
                current_exp = c;
                c++;
                while(c < size && input[c] != '"') c++;
                if (input[c] == '"') c++;
                current = add_token(current, &input[current_exp], c-current_exp, line);
                
                current_exp = -1;              
                break;
            case '\'':
                if (current_exp != -1) {
                    current = add_token(current, &input[current_exp], c-current_exp, line);
                }  
                current_exp = c;
                c++;
                while(c < size && input[c] != '\'') c++;
                if (input[c] == '\'') c++;
                current = add_token(current, &input[current_exp], c-current_exp, line);
                
                current_exp = -1;              
                break;                
            default:
                if (current_exp == -1) {
                    current_exp = c;
                } else if (input[c-1] == '=' || input[c-1] == '+' || input[c-1] == '-' || input[c-1] == '&' || input[c-1] == '*')  {
                    current = add_token(current, &input[current_exp], c-current_exp, line);
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

void create_node_expr(T_NODE * up, T_ELT * current) {
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
            //ctxt.type = t_current;
            //current_n = add_desc_node(up, current);
            current_n = add_next_node(up, current, up);
            create_node_expr(current_n, current->next);
            break;
        case PAR_C:
        case ACC_C:
        case BR_C:
            //ctxt.type = t_current;
            current_n = add_next_node(up->asc, current, up);
            create_node_expr(current_n->asc, current->next);
            break;
        case END:
        case EQ:
        case NEQ:
        case PTR:
        case DIV:
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
        case WHILE:
        case DO:
        case AND:
        case STR:
        case CCHAR:
            //ctxt.type = t_current;
            current_n = add_next_node(up, current, up);
            create_node_expr(current_n, current->next);      
            break;
        case PAR_O:
        case ACC_O:
        case BR_O:
            //ctxt.type = t_current;
            current_n = add_desc_node(up, current);
            current_n = add_desc_node(current_n, NULL);
            create_node_expr(current_n, current->next);
            break;
        case IF:
        case ELSE:
        case EXPR:
            current_n = add_next_node(up, current, up);
            create_node_expr(current_n, current->next);
            break;
        default:
            create_node_expr(up, current->next);
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

int type_size(enum keyword k) {
    switch(k) {
        case INT: return 4;
        case SHORT: return 2;
        case CHAR: return 1;
        case PTR: return 4;
        default : error("Invalid type for type_size()");
        break;
    }
    return -1;
}

int is_pointer_access(T_NODE * n) {
    if (n->prev != NULL && n->prev->type == PTR && !is_type(n->prev->prev->type) && (n->prev->prev->type != EXPR)) return 1;
    else return 0;
}

int is_array_access(T_NODE * n) {
    if (n->desc != NULL && n->desc->type == BR_O) return 1;
    else return 0;
}

int is_matching(T_NODE * nodeA, T_NODE * nodeB) {
    if ( nodeA == NULL || nodeA->elt == NULL) return 0;
    if ( nodeB == NULL || nodeB->elt == NULL) return 0;

   int minlen = (nodeA->elt->len > nodeB->elt->len)? nodeA->elt->len: nodeB->elt->len;

    if (strncmp(nodeA->elt->str, nodeB->elt->str, minlen) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int is_procedure_body(T_NODE * up) {
    
    if ((up->type == EXPR) && (up->prev != NULL) && is_type(up->prev->type)
        && (up->desc != NULL) && (up->desc->type == PAR_O)) {
        return 1;
    }

    return 0;
}

int is_procedure_call(T_NODE * up) {
    if ((up->type == EXPR) && (up->prev == NULL || !is_type(up->prev->type))
        && up->desc != NULL &&(up->desc->type == PAR_O)) {
        return 1;
    }

    return 0;
}

int resolve(T_NODE * node, int left, char do_next) {
    if (node == NULL) return left;

    T_NODE * next = (do_next == 1) ? node->next : NULL; 

    if (node->elt == NULL) return resolve(next, left, do_next);
    if (node->type == ADD) {
        return left + resolve(next, 0, do_next);
    } 
    if (node->type == SUB) {
        return left - resolve(next, 0, do_next);
    }
    if (node->type == PTR) {
        return left * resolve(next, 0, do_next);
    }
    if (is_number(node->elt)) {
        T_ELT * elt = node->elt; 
        char value[32];
        for (int i = 0;i < elt->len;i++) {
            value[i] = elt->str[i]; 
        }

        value[elt->len] = 0;

        int v = atoi(value);
        
        if (left != 0) error_elt(elt, "Syntax error");

        return resolve(next, v, do_next);
    }

    if (node->type == CCHAR) {
        
        return resolve(next, node->elt->str[1], do_next);
    }
    return resolve(next, left, do_next);
}

int variable_size(T_NODE * up) {
    if (up->prev == NULL) {
        if (up->type == BR_O && is_type(up->asc->prev->type)) {

            int size = resolve(up->desc, 0, 1) * type_size(up->asc->prev->type);

            printf("ARRAY size = %d\n", size);
            return size;
        } 
        else display_elt_ctxt("Error on variable size: ", up->elt);
    }

    if (is_array_access(up)) {
        
        display_elt_ctxt("is_array_access", up->elt);
        if (is_type(up->prev->type))
            return 4;

        return type_size(up->prev->type);;
    }

    switch (up->prev->type) {
        case INT: 
            return 4;
            break;
        case SHORT: 
            return 2;
            break;            
        case CHAR:
            return 1;
            break;
        case PTR:
            return 4;
            break;
        default:
            if (is_number(up->elt)) {
                return 4;
            }
            display_elt_ctxt("Error on variable size: ", up->elt);    
            printf("TYPE: %d %d\n", up->asc->type, INT);      
            exit(0);
            return 0;

    }
}

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

int get_variable_dynamic_offset_from_symbol(T_NODE * target, T_BUFFER * buffer) {
    for (int i = 0; i < buffer->local_symbol_count;i++) {        
        if (buffer->local_symbol[i] != NULL && is_matching(buffer->local_symbol[i], target)) {
            return i;
        }
    }

    return -1;
}

int get_variable_offset(T_NODE * target, T_BUFFER * buffer) {  
    if (get_variable_dynamic_offset_from_symbol(target, buffer) == -1) error_elt(target->elt, "No matching symbol found");
    
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

int get_offset_from_global_symbol(T_NODE * target, T_BUFFER * buffer) {
    for (int i = 0; i < buffer->global_symbol_count;i++) {        
        if (buffer->global_symbol[i] != NULL && is_matching(buffer->global_symbol[i], target)) {
            return i;
        }
    }

    return -1;
}

int is_global(T_NODE * target, T_BUFFER * buffer) {
    if (get_variable_dynamic_offset_from_symbol(target, buffer) != -1) {
        return 0;
    }
    return 1;
}


T_NODE * variable_decl_lookup(T_NODE * target, T_BUFFER * buffer) {
    for (int i = buffer->local_symbol_count - 1; i >= 0; i--) {
        
        if (is_matching(target, buffer->local_symbol[i])) {
            
            return buffer->local_symbol[i];
        }
    }
    for (int i = buffer->global_symbol_count - 1; i >= 0; i--) {
        if (is_matching(target, buffer->global_symbol[i])) {
            return buffer->global_symbol[i];
        }
    }
    
    error_elt(target->elt, "No matching declaration");
    return NULL;
}

int get_all_variable_offset(T_BUFFER * buffer) {
    int offset = 0;
    for (int i = 0;i < buffer->local_symbol_count && buffer->local_symbol[buffer->local_symbol_count - i - 1]->elt != NULL;i++) {

        if (buffer->local_symbol[buffer->local_symbol_count - i - 1]->elt != NULL)
            offset += variable_size(buffer->local_symbol[buffer->local_symbol_count - i - 1]);
    }
    return offset;
}

int has_type_decl(T_NODE * up) {
    if ( (up->prev != NULL && is_type(up->prev->type)) 
        || ((up->prev != NULL) && (up->prev->type == PTR) && (up->prev->prev != NULL) && is_type(up->prev->prev->type) ))
        return 1;
    else
        return 0;
}

int is_variable_decl(T_NODE * up, T_BUFFER * buffer) {
    if ((up->type == EXPR) && has_type_decl(up) && !is_number(up->elt) && get_variable_dynamic_offset_from_symbol(up, buffer) == - 1) {
        return 1;
    }
    return 0;
}

T_NODE * proc_lookup(T_NODE * up, T_NODE * target) {
    if (up == NULL) {
        return NULL;
    }

    if (is_procedure_body(up) && is_matching(up, target)) {
        return up;
    } else {
        return proc_lookup(up->next, target);
    }
}
T_NODE * step(T_NODE * up, int stack_offset, T_BUFFER * buffer);

int add_local_symbol(T_NODE * up, T_BUFFER * buffer, char is_anon) {

    if (is_anon == 0 && up->desc != NULL && up->desc->type == BR_O) {
        
        add_local_symbol(up->desc, buffer, 1);
    }
   
    asm_add_variable(up, buffer);
    if (!is_anon)
        buffer->local_symbol[buffer->local_symbol_count++] = up;
    else {
        switch (variable_size(up)) {
            case 1: 
                buffer->local_symbol[buffer->local_symbol_count++] = &anonymous_char;
                break;
            case 2:
                buffer->local_symbol[buffer->local_symbol_count++] = &anonymous_short;
                break;
            case 4:
                buffer->local_symbol[buffer->local_symbol_count++] = &anonymous_int;
                break;
            default:
                buffer->local_symbol[buffer->local_symbol_count++] = up;
                //error_elt(up->elt, "Unable to add local symbol.");
                break;
        }
        
    }

    if (is_anon == 0 && up->desc != NULL && up->desc->type == BR_O) {
        //Setting array pointer value for main symbol
        asm_mov_eax_esp(buffer);
        asm_add_eax_value(buffer, 4);
        asm_store_variable_eax(get_variable_offset(up, buffer), buffer);

    }


    return buffer->local_symbol_count - 1;
}

void unstack_local_symbol(T_BUFFER * buffer) {
    if (buffer->local_symbol_count > 0) {
        asm_remove_variable(buffer, variable_size(buffer->local_symbol[buffer->local_symbol_count - 1]));
    }
    buffer->local_symbol_count--;
}

int add_global_symbol(T_NODE * up, T_BUFFER * buffer) {


    buffer->global_symbol[buffer->global_symbol_count++] = up;
    up->offset = buffer->length;
    

    if (is_array_access(up)) {
        buffer->length += 4;

        if (up->next != NULL && up->next->type == EQ) {
            if (up->next->next->type == STR) {
                memcpy( &(buffer->buffer[buffer->length]), up->next->next->elt->str+1, up->next->next->elt->len-2);
                buffer->length += up->next->next->elt->len-2;
                buffer->buffer[buffer->length++] = 0;

                printf("Copied: %d\n", up->next->next->elt->len-2);
            } else {
                error_elt(up->elt,"Unsupported declaration");
            }
        } else {
            buffer->length += variable_size(up->desc);
        }
        
        printf("Current buffer length: %d\n", buffer->length);
        int value = up->offset + 4 + ELF_ENTRY_VADDR + sizeof(T_ELF) + sizeof(T_ELF_PRG32_HDR);
        memcpy( &(buffer->buffer[up->offset]), &value , sizeof(value));

    } else {
        buffer->length += variable_size(up);

        if (up->next->type == EQ) {
            
            int value = resolve(up->next->next, 0, 1);
            printf("value = %d\n", value);
            memcpy( &(buffer->buffer[up->offset]), &value , variable_size(up));
        }

    }



    return up->offset;
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
    int s_count = buffer->local_symbol_count;
    while (up != NULL) {
        up = line(up, stack_offset, buffer);
        if (up != NULL)
            up = up->next;
    }
    
    asm_remove_variable(buffer, get_variable_dynamic_offset((s_count == 0) ? s_count : s_count - 1, buffer));
    buffer->local_symbol_count = s_count;
    return up;
}

T_NODE * block_or_line(T_NODE * up, T_NODE * alternative, int stack_offset, T_BUFFER * buffer) {
    int s_count = buffer->local_symbol_count;
    T_NODE * last = alternative;
    if (up != NULL)
        block(up, stack_offset, buffer);
    else
        last = line(get_token(alternative, NEXT), stack_offset, buffer);
    asm_remove_variable(buffer, get_variable_dynamic_offset((s_count == 0) ? s_count : s_count - 1, buffer));
    buffer->local_symbol_count = s_count;
    return last;        
}

T_NODE * prog_arg(T_NODE * proc, int c) {
    T_NODE * arg = proc->desc->desc;
    int count = 0;
    if (arg->elt == NULL) arg = arg->next;
    while (c < count && arg != NULL) {
        arg = arg->next;
        count++;
    }
    return arg;
}

void stack_parameters(T_NODE * up, T_BUFFER * buffer, T_NODE * proc, int count) {
    if (up->elt == NULL || up->type == COM || is_type(up->type))
        return  stack_parameters(up->next, buffer, proc, count);

    display_elt_ctxt("STACK parameter: ", up->elt);  

    if (up->elt != NULL) {
        T_NODE * n = prog_arg(proc, count);
        while (n != NULL && n->type != EXPR) n = n->next;
        if (n == NULL) error_elt(up->elt, "Syntax Error");
        display_elt_ctxt("found arg: ", n->elt);
        int doffset = add_local_symbol(n, buffer, 1);
        one(up, -1, buffer);
        asm_store_variable(n, get_variable_dynamic_offset(doffset, buffer), buffer);
        if (up->next != NULL)
            line(up->next, doffset, buffer);
        
        while(up != NULL && up->type != PAR_C && up->type != COM) up = up->next;
    }
    
    if (up != NULL) {
        stack_parameters(up, buffer, proc, count + 1);
    } 
}

void parameter_scan(T_NODE * up, T_BUFFER * buffer) {
    if (up == NULL)
        return;
    else if (is_variable_decl(up, buffer)) {
        buffer->local_symbol[buffer->local_symbol_count++] = up;
    }
    parameter_scan(up->next, buffer);
}

void retrieve_setup(T_NODE * up, T_BUFFER * buffer) {
        if (is_array_access(up)) {
            puts("Array retrieve");
            line(get_token_3(up, DESC, DESC, NEXT), -1, buffer);
            T_NODE * decl_n = variable_decl_lookup(up, buffer);
            if (decl_n->prev->type == PTR && decl_n->desc == NULL) {
                asm_imul_eax_value(buffer, type_size(variable_decl_lookup(up, buffer)->prev->prev->type));
            } else {
                asm_imul_eax_value(buffer, type_size(variable_decl_lookup(up, buffer)->prev->type));
            }

            asm_mov_ebx_eax(buffer); 
        } else {
            asm_xor_ebx_ebx(buffer);
        }

        if (is_global(up, buffer)) {
            asm_load_eax(variable_decl_lookup(up, buffer)->offset + ELF_ENTRY_VADDR + sizeof(T_ELF) + sizeof(T_ELF_PRG32_HDR), buffer);
        } else {
            asm_mov_eax_esp(buffer);
            asm_add_eax_value(buffer, get_variable_offset(up, buffer));
        }     
  
        if (is_array_access(up)) {
            asm_mov_eax_eax_addr(buffer);
        }

        asm_add_ebx_eax(buffer);
}


T_NODE * retrieve_expression(T_NODE * up, T_BUFFER * buffer) {
        retrieve_setup(up, buffer);

        if (is_pointer_access(up)) {
            asm_mov_eax_ebx_addr(buffer);
            asm_mov_ebx_eax(buffer);
        } 

        if (is_array_access(up)) {

            T_NODE * decl_n = variable_decl_lookup(up, buffer);
            if (decl_n->prev->type == PTR && decl_n->desc == NULL) {
                asm_load_eax(0, buffer);
                asm_retrieve_variable_indirect_vs(type_size(variable_decl_lookup(up, buffer)->prev->prev->type), buffer);
            } else {
                asm_load_eax(0, buffer);
                asm_retrieve_variable_indirect_vs(variable_size(variable_decl_lookup(up, buffer)), buffer);
            }            
            
        } else {
            asm_retrieve_variable_indirect_vs(type_size(variable_decl_lookup(up, buffer)->prev->type), buffer);
        }
}

T_NODE * handle_expression(T_NODE * up, int stack_offset, T_BUFFER * buffer) {
    T_NODE * last = up;
    if (up->next != NULL && ( up->next->type == EQ || up->next->type == ADDADD || up->next->type == SUBSUB)) {
        T_NODE * next = up->next->next;
        if (up->next->type == ADDADD || up->next->type == SUBSUB) next = up->next;
        last = line(next, -1, buffer);
        int local_offset = add_local_symbol(&anonymous_int, buffer, 1);
        asm_store_variable_eax(get_variable_dynamic_offset(local_offset, buffer), buffer); 
        retrieve_setup(up, buffer);

        if (is_pointer_access(up)) {
            asm_mov_eax_ebx_addr(buffer);
            asm_mov_ebx_eax(buffer);
        }

        asm_retrieve_variable_eax(get_variable_dynamic_offset(local_offset, buffer), buffer);
        asm_store_variable_indirect_vs(type_size(variable_decl_lookup(up, buffer)->prev->type), buffer);
        unstack_local_symbol(buffer);
    } else {
        retrieve_expression(up, buffer);
    }

    return last;
}


T_NODE * step(T_NODE * up, int stack_offset, T_BUFFER * buffer) {
    display_elt_ctxt("STEP: ", up->elt);    

    if (up == NULL) error("INVALID STEP");

    if (is_variable_decl(up, buffer)) {
        display_elt_ctxt("Variable declaration: ", up->elt);

        return handle_expression(up, add_local_symbol(up, buffer, 0), buffer);
    } else if (is_number(up->elt) || up->type == CCHAR) {
        
        asm_load_eax(resolve(up, 0, 0), buffer);
        return up;

     } else if(is_procedure_call(up)){
        display_elt_ctxt("Procedure call: ", up->elt);

        T_NODE * proc = proc_lookup(buffer->top, up);
        if (proc == NULL) error_elt(up->elt, "No matching proc declaration.");
        int offset = proc->offset;
        

        if ( get_token(up, DESC)->type == PAR_O)
            stack_parameters(get_token_2(up, DESC, DESC), buffer, proc, 0);
        else 
            error("Invalid procedure call");

        asm_call(buffer, offset - buffer->length);
        
        return up;
    } else if (up->type == EXPR) {
        display_elt_ctxt("Variable assign: ", up->elt);
        
        return handle_expression(up, stack_offset, buffer);
    } else if (up->type == SUP || up->type == INF || up->type == EQEQ || up->type == NEQ) {
        
        int doffset = add_local_symbol(&anonymous_int, buffer, 1);
        asm_store_variable_eax(get_variable_dynamic_offset(doffset, buffer), buffer);
        T_NODE * end = line(up->next, -1, buffer);
        asm_retrieve_variable_ebx(get_variable_dynamic_offset(doffset, buffer), buffer);
        unstack_local_symbol(buffer);
        asm_cmp_eax_ebx(buffer);
        if (up->type == NEQ)
            asm_jump_equal(buffer, 10);
        else if (up-> type == INF)
            asm_jump_less_eq(buffer, 10);
        else if (up->type == SUP)
            asm_jump_greater_eq(buffer, 10);
        else asm_jump_not_equal(buffer, 10); 
        asm_load_eax(1, buffer);
        asm_jump(buffer, 5);
        asm_load_eax(0, buffer);
        
        if (stack_offset != -1) {
            asm_store_variable(buffer->local_symbol[stack_offset], get_variable_dynamic_offset(stack_offset, buffer), buffer);
        }
        return end;   

    } else if (up->type == RET) {
        T_NODE * last = last = line(up->next, -1, buffer);
        asm_remove_variable(buffer, get_all_variable_offset(buffer));
        asm_ret(buffer);      

        return last;

    } else if (up->type == IF){

        puts("IF STATEMENT");
        U8 * jgptr = NULL;
        U8 * jmptr = NULL; 
        U32 offset = 0;
        U32 offset2 = 0;
        
        int doffset = add_local_symbol(&anonymous_int, buffer, 1);
        block(get_token_3(up, DESC, DESC, NEXT), doffset, buffer);
        unstack_local_symbol(buffer);

        asm_test_eax(buffer);
        jgptr = asm_jump_equal(buffer, 0);
        offset = buffer->length;


        puts("IF FIRST BODY");
        T_NODE * last = block_or_line(get_token_5_null(up, DESC, NEXT, NEXT, DESC, NEXT), up, -1, buffer);

        jmptr = asm_jump(buffer, 0);
        offset2 = buffer->length;

        asm_update_jump_length(jgptr, buffer, offset);

        puts("IF SECOND BODY");
        if (last->next != NULL && last->next->type == ELSE) {
            last = block_or_line(get_token_3_null(last->next, DESC, DESC, NEXT), last->next, -1, buffer);
        }
            
        asm_update_jump_length(jmptr, buffer, offset2);

        return last;

    } else if (up->type == FOR) {

        T_NODE * last = line(get_token_3(up, DESC, DESC, NEXT), -1, buffer);
        U32 offset = buffer->length;

        int doffset = add_local_symbol(&anonymous_int, buffer, 1);        
        last = line(last->next, doffset, buffer);
        unstack_local_symbol(buffer);

        asm_test_eax(buffer);
        U8 * jcondptr = asm_jump_equal(buffer, 0);        
        U32 offset3 = buffer->length;
    
        T_NODE * end = block_or_line(get_token_4_null(up, DESC, NEXT, NEXT, DESC), up, -1, buffer);
        
        line(last->next, -1, buffer);
        asm_jump(buffer, offset - buffer->length - 5);
        
        asm_update_jump_length(jcondptr, buffer, offset3);
        
        return end;
        
    } else if (up->type == WHILE) {


        U32 offset = buffer->length;
        int doffset = add_local_symbol(&anonymous_int, buffer, 1);              
        T_NODE * last = line(get_token_3(up, DESC, DESC, NEXT), doffset, buffer);
        unstack_local_symbol(buffer);

        asm_test_eax(buffer);
        U8 * jcondptr = asm_jump_equal(buffer, 0);          
        U32 offset3 = buffer->length;

        last = block_or_line(get_token_4_null(up, DESC, NEXT, NEXT, DESC), up, -1, buffer);
        asm_jump(buffer, offset - buffer->length - 5);
        asm_update_jump_length(jcondptr, buffer, offset3);
    
        return last;
    } else if (up->type == DO) {

        U32 offset = buffer->length;
        T_NODE * last = block_or_line(get_token_3_null(up, DESC, DESC, NEXT), up, -1, buffer);
       
        int doffset = add_local_symbol(&anonymous_int, buffer, 1);
        line(get_token_4(last,NEXT,DESC,DESC,NEXT), doffset, buffer);
        unstack_local_symbol(buffer);
        asm_test_eax(buffer);
        U8 * jcondptr2 = asm_jump_equal(buffer, 0);              
        U32 offset2 = buffer->length;

        asm_jump(buffer, offset - buffer->length - 5);
        asm_update_jump_length(jcondptr2, buffer, offset2);

        return last;
    } else if (up->type == EQ) {

        error_elt(up->elt, "Should not be here.");
        return up->next;
    } else if (up->type == ADD || up->type == SUB) {
        T_NODE * last;
        
        if (stack_offset != -1) {
            last = one(get_token(up,NEXT), stack_offset, buffer);
            if (up->type == ADD)
                asm_add_variable_and_store(buffer->local_symbol[stack_offset], get_variable_dynamic_offset(stack_offset, buffer), buffer);
            if (up->type == SUB) 
                asm_sub_variable_and_store(buffer->local_symbol[stack_offset], get_variable_dynamic_offset(stack_offset, buffer), buffer);
            
        } else {
            
            stack_offset = add_local_symbol(&anonymous_int, buffer, 1);
            asm_store_variable(&anonymous_int, get_variable_dynamic_offset(stack_offset, buffer), buffer);
            last = one(get_token(up,NEXT), stack_offset, buffer);
            if (up->type == ADD)
                asm_add_variable_and_store(buffer->local_symbol[stack_offset], get_variable_dynamic_offset(stack_offset, buffer), buffer);
            if (up->type == SUB)
                asm_sub_variable_and_store(buffer->local_symbol[stack_offset], get_variable_dynamic_offset(stack_offset, buffer), buffer);
            last = line(last->next, stack_offset, buffer);    
            asm_retrieve_variable(&anonymous_int, get_variable_dynamic_offset(stack_offset, buffer), buffer);     
            unstack_local_symbol(buffer);     
        }
        return last;

    } else if (up->type == ADDADD || up->type == SUBSUB) {
        retrieve_expression(up->prev, buffer);
        if (up->type == ADDADD)
            asm_add_eax_value(buffer, 1);
        if (up->type == SUBSUB)
            asm_sub_eax_value(buffer, 1);                
        
        return up;
    } else if (up->type == AND) {
        retrieve_setup(up->next, buffer);
        asm_mov_eax_ebx(buffer);

        return up->next;
    } else if (up->type == DIV || ((up->type == PTR) && (up->prev != NULL) && (up->prev->type == EXPR))) { 
        int doffset = -1;
        if (stack_offset == -1) {
            doffset = add_local_symbol(&anonymous_int, buffer, 1);
            asm_store_variable(&anonymous_int, get_variable_dynamic_offset(doffset, buffer), buffer);
        }
        
        T_NODE * last = one(get_token(up,NEXT), -1, buffer);
        asm_mov_ebx_eax(buffer);
        
        if (stack_offset != -1) {
            asm_retrieve_variable_eax(get_variable_dynamic_offset(stack_offset, buffer), buffer);
        } else {
            asm_retrieve_variable_eax(get_variable_dynamic_offset(doffset, buffer), buffer);
        }
        
        if (up->type == DIV)
            asm_idiv_ebx(buffer);
        else 
            asm_imul_ebx(buffer);

        if (stack_offset == -1) {
            unstack_local_symbol(buffer); 
        } else {
            asm_store_variable(&anonymous_int, get_variable_dynamic_offset(stack_offset, buffer), buffer);
        }
        return last;
    } else if (up->type == PTR) {
        return one(up->next, stack_offset, buffer);
    } else if (up->type == BR_O) {
        return up->next;
    }
    
    return up;
}

T_NODE * root_step(T_NODE * up, T_BUFFER * buffer) {
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

        return block(get_token_4(up, DESC, NEXT, NEXT, DESC), -1, buffer);

    } else if (is_variable_decl(up, buffer)) {
        display_elt_ctxt("Global variable declaration: ", up->elt);

        add_global_symbol(up, buffer);
        return get_token(up,NEXT);
    } //else if (up->elt != NULL) error_elt(up->elt, "Unknow token.");
    return up;
}

void display_node(T_NODE * node, int spacing) {
    if (node == NULL) 
        return;

    for (int i = 0;i < spacing;i++) putchar('-');
    display_elt(node->elt);

    if (node->prev != NULL && node->prev->type == VOID) {
        printf("[VOID]");
    }
    
    if (node->prev && node->prev->type == INT) {
        printf("[INT]");
    }

    if (node->prev && node->prev->type == CHAR) {
        printf("[CHAR]");
    }
    
    putchar('\n');
    display_node(node->desc, spacing+2);
    display_node(node->next, spacing);
}

void write_output(char * filename,  T_BUFFER * buffer) {
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

void main(int c, char** argv) {
    dummy.prev = &dummy;
    dummy.type = INT;
    dummy.elt = NULL;

    
    elt_int.next = NULL;
    elt_int.str = "__ANONYMOUS";
    elt_int.len = strlen(elt_int.str);
    anonymous_int.elt = &elt_int;
    anonymous_int.prev = &anonymous_int;
    anonymous_int.type = INT;

    anonymous_short.elt = &elt_int;
    anonymous_short.prev = &anonymous_short;
    anonymous_short.type = SHORT;

    anonymous_char.elt = &elt_int;
    anonymous_char.prev = &anonymous_char;
    anonymous_char.type = CHAR;

    printf("Reading file %s\n",argv[1]);
    int size;
    char * filedata = read_file(argv[1], &size);
    T_ELT * elt = tokenize(filedata, size);
    display_all_elt(elt);
    T_NODE * up = add_desc_node(NULL, NULL);

    printf("CREATING AST\n");
    create_node_expr(up, elt);
    printf("After AST\n");
    display_node(up,0);
    
    
    T_BUFFER * buffer = (T_BUFFER *)malloc(sizeof(T_BUFFER));
    buffer->length = 0;
    buffer->top = up;
    buffer->local_symbol_count = 0;
    
    while(up != NULL) {
        root_step(up, buffer);
        up = up->next; 
    };
    write_output("out", buffer);
    free(buffer);
    free(filedata);
}