#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "elf32.h"

//#define X86
//#define M68K

#ifdef X86
#include "asm_x86.c"
#elif defined M68K
#include "hunk.h"
#include "asm_m68k.c"
#endif

T_NODE dummy;

T_ELT elt_int;
T_NODE anonymous_int;
T_NODE anonymous_short;
T_NODE anonymous_char;



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

T_NODE * get_token_alternative(T_NODE * up, char a, char b) {
    T_NODE * n = get_token_null(up, a);
    if (n == NULL)
        return get_token_null(up, b);
    return n;       
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
#ifdef X86
        //Setting array pointer value for main symbol
        asm_mov_eax_esp(buffer);
        asm_add_eax_value(buffer, 4);
        asm_store_variable_eax(get_variable_offset(up, buffer), buffer);
#endif
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
#ifdef X86                
                asm_imul_eax_value(buffer, type_size(variable_decl_lookup(up, buffer)->prev->prev->type));
#endif
            } else {

#ifdef X86                
                asm_imul_eax_value(buffer, type_size(variable_decl_lookup(up, buffer)->prev->type));
#endif
            }

#ifdef X86
            asm_mov_ebx_eax(buffer);
#endif             
        } else {
#ifdef X86            
            asm_xor_ebx_ebx(buffer);
#endif            
        }

        if (is_global(up, buffer)) {
#ifdef X86            
            asm_load_eax(variable_decl_lookup(up, buffer)->offset + ELF_ENTRY_VADDR + sizeof(T_ELF) + sizeof(T_ELF_PRG32_HDR), buffer);
#endif            
        } else {
#ifdef X86            
            asm_mov_eax_esp(buffer);
            asm_add_eax_value(buffer, get_variable_offset(up, buffer));
#endif        
        }     
  
        if (is_array_access(up)) {
#ifdef X86
            asm_mov_eax_eax_addr(buffer);
#endif        
        }
#ifdef X86
        asm_add_ebx_eax(buffer);
#endif
}


T_NODE * retrieve_expression(T_NODE * up, T_BUFFER * buffer) {
        retrieve_setup(up, buffer);

        if (is_pointer_access(up)) {
#ifdef X86   
            asm_mov_eax_ebx_addr(buffer);
            asm_mov_ebx_eax(buffer);
#endif
        } 

        if (is_array_access(up)) {

            T_NODE * decl_n = variable_decl_lookup(up, buffer);
            if (decl_n->prev->type == PTR && decl_n->desc == NULL) {
#ifdef X86   
                asm_load_eax(0, buffer);
                asm_retrieve_variable_indirect_vs(type_size(variable_decl_lookup(up, buffer)->prev->prev->type), buffer);
#endif
            } else {
#ifdef X86   
                asm_load_eax(0, buffer);
                asm_retrieve_variable_indirect_vs(variable_size(variable_decl_lookup(up, buffer)), buffer);
#endif
            }            
            
        } else {
#ifdef X86   
            asm_retrieve_variable_indirect_vs(type_size(variable_decl_lookup(up, buffer)->prev->type), buffer);
#endif
        }
}

T_NODE * handle_expression(T_NODE * up, int stack_offset, T_BUFFER * buffer) {
    T_NODE * last = up;
    if (up->next != NULL && ( up->next->type == EQ || up->next->type == ADDADD || up->next->type == SUBSUB)) {
        T_NODE * next = up->next->next;
        if (up->next->type == ADDADD || up->next->type == SUBSUB) next = up->next;
        last = line(next, -1, buffer);
        int local_offset = add_local_symbol(&anonymous_int, buffer, 1);
#ifdef X86   
        asm_store_variable_eax(get_variable_dynamic_offset(local_offset, buffer), buffer); 
#endif
        retrieve_setup(up, buffer);

        if (is_pointer_access(up)) {
#ifdef X86   
            asm_mov_eax_ebx_addr(buffer);
            asm_mov_ebx_eax(buffer);
#endif
        }
#ifdef X86   
        asm_retrieve_variable_eax(get_variable_dynamic_offset(local_offset, buffer), buffer);
        asm_store_variable_indirect_vs(type_size(variable_decl_lookup(up, buffer)->prev->type), buffer);
#endif
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
#ifdef X86        
        asm_load_eax(resolve(up, 0, 0), buffer);
#endif        
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
#ifdef X86
        asm_call(buffer, offset - buffer->length);
#endif       
        return up;
    } else if (up->type == EXPR) {
        display_elt_ctxt("Variable assign: ", up->elt);
        
        return handle_expression(up, stack_offset, buffer);
    } else if (up->type == SUP || up->type == INF || up->type == EQEQ || up->type == NEQ) {
        
        int doffset = add_local_symbol(&anonymous_int, buffer, 1);
#ifdef X86      
        asm_store_variable_eax(get_variable_dynamic_offset(doffset, buffer), buffer);
#endif
        T_NODE * end = line(up->next, -1, buffer);
#ifdef X86
        asm_retrieve_variable_ebx(get_variable_dynamic_offset(doffset, buffer), buffer);
#endif
        unstack_local_symbol(buffer);
#ifdef X86
        asm_cmp_eax_ebx(buffer);
#endif
        if (up->type == NEQ) {
#ifdef X86
            asm_jump_equal(buffer, 10);
#endif
        } else if (up-> type == INF) {
#ifdef X86
            asm_jump_less_eq(buffer, 10);
#endif
        } else if (up->type == SUP) {
#ifdef X86            
            asm_jump_greater_eq(buffer, 10);
#endif
        } else {
#ifdef X86            
            asm_jump_not_equal(buffer, 10);
#endif
        } 
#ifdef X86
        asm_load_eax(1, buffer);
        asm_jump(buffer, 5);
        asm_load_eax(0, buffer);
#endif        
        if (stack_offset != -1) {            
            asm_store_variable(buffer->local_symbol[stack_offset], get_variable_dynamic_offset(stack_offset, buffer), buffer);
        }
        return end;   

    } else if (up->type == RET) {
        T_NODE * last = last = line(up->next, -1, buffer);
            
        asm_remove_variable(buffer, get_all_variable_offset(buffer));
#ifdef X86
        asm_ret(buffer);
#elif defined M68K
        rts(buffer);      
#endif
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

#ifdef X86
        asm_test_eax(buffer);
        jgptr = asm_jump_equal(buffer, 0);
#endif
        offset = buffer->length;

        puts("IF FIRST BODY");
        T_NODE * last = block_or_line(get_token_5_null(up, DESC, NEXT, NEXT, DESC, NEXT), up, -1, buffer);

#ifdef X86
        jmptr = asm_jump(buffer, 0);
#endif
        offset2 = buffer->length;

#ifdef X86
        asm_update_jump_length(jgptr, buffer, offset);
#endif
        puts("IF SECOND BODY");
        if (last->next != NULL && last->next->type == ELSE) {
            last = block_or_line(get_token_3_null(last->next, DESC, DESC, NEXT), last->next, -1, buffer);
        }

#ifdef X86            
        asm_update_jump_length(jmptr, buffer, offset2);
#endif

        return last;

    } else if (up->type == FOR) {

        T_NODE * last = line(get_token_3(up, DESC, DESC, NEXT), -1, buffer);
        U32 offset = buffer->length;

        int doffset = add_local_symbol(&anonymous_int, buffer, 1);        
        last = line(last->next, doffset, buffer);
        unstack_local_symbol(buffer);

#ifdef X86 
        asm_test_eax(buffer);
        U8 * jcondptr = asm_jump_equal(buffer, 0);        
#endif
        U32 offset3 = buffer->length;
    
        T_NODE * end = block_or_line(get_token_4_null(up, DESC, NEXT, NEXT, DESC), up, -1, buffer);
        
        line(last->next, -1, buffer);

#ifdef X86         
        asm_jump(buffer, offset - buffer->length - 5);        
        asm_update_jump_length(jcondptr, buffer, offset3);
#endif        

        return end;
        
    } else if (up->type == WHILE) {


        U32 offset = buffer->length;
        int doffset = add_local_symbol(&anonymous_int, buffer, 1);              
        T_NODE * last = line(get_token_3(up, DESC, DESC, NEXT), doffset, buffer);
        unstack_local_symbol(buffer);

#ifdef X86 
        asm_test_eax(buffer);
        U8 * jcondptr = asm_jump_equal(buffer, 0);          
#endif
        U32 offset3 = buffer->length;

        last = block_or_line(get_token_4_null(up, DESC, NEXT, NEXT, DESC), up, -1, buffer);
#ifdef X86 
        asm_jump(buffer, offset - buffer->length - 5);
        asm_update_jump_length(jcondptr, buffer, offset3);
#endif    
        return last;
    } else if (up->type == DO) {

        U32 offset = buffer->length;
        T_NODE * last = block_or_line(get_token_3_null(up, DESC, DESC, NEXT), up, -1, buffer);
       
        int doffset = add_local_symbol(&anonymous_int, buffer, 1);
        line(get_token_4(last,NEXT,DESC,DESC,NEXT), doffset, buffer);
        unstack_local_symbol(buffer);
#ifdef X86 
        asm_test_eax(buffer);
        U8 * jcondptr2 = asm_jump_equal(buffer, 0);
#endif                      
        U32 offset2 = buffer->length;

#ifdef X86 
        asm_jump(buffer, offset - buffer->length - 5);
        asm_update_jump_length(jcondptr2, buffer, offset2);
#endif
        return last;
    } else if (up->type == EQ) {

        error_elt(up->elt, "Should not be here.");
        return up->next;
    } else if (up->type == ADD || up->type == SUB) {
        T_NODE * last;
        
        if (stack_offset != -1) {
            last = one(get_token_alternative(up, DESC, NEXT), stack_offset, buffer);
#ifdef X86 
            if (up->type == ADD)
                asm_add_variable_and_store(buffer->local_symbol[stack_offset], get_variable_dynamic_offset(stack_offset, buffer), buffer);
            if (up->type == SUB) 
                asm_sub_variable_and_store(buffer->local_symbol[stack_offset], get_variable_dynamic_offset(stack_offset, buffer), buffer);
#endif            
        } else {
            
            stack_offset = add_local_symbol(&anonymous_int, buffer, 1);            
            asm_store_variable(&anonymous_int, get_variable_dynamic_offset(stack_offset, buffer), buffer);
            last = one(get_token_alternative(up, DESC, NEXT), stack_offset, buffer);
#ifdef X86            
            if (up->type == ADD)
                asm_add_variable_and_store(buffer->local_symbol[stack_offset], get_variable_dynamic_offset(stack_offset, buffer), buffer);
            if (up->type == SUB)
                asm_sub_variable_and_store(buffer->local_symbol[stack_offset], get_variable_dynamic_offset(stack_offset, buffer), buffer);
#endif
            last = line(last->next, stack_offset, buffer);                
            asm_retrieve_variable(&anonymous_int, get_variable_dynamic_offset(stack_offset, buffer), buffer); 
            unstack_local_symbol(buffer);     
        }
        return last;

    } else if (up->type == ADDADD || up->type == SUBSUB) {
        retrieve_expression(up->prev, buffer);

#ifdef X86         
        if (up->type == ADDADD)
            asm_add_eax_value(buffer, 1);
        if (up->type == SUBSUB)
            asm_sub_eax_value(buffer, 1);                
#endif        
        return up;
    } else if (up->type == AND) {
        retrieve_setup(up->next, buffer);
#ifdef X86 
        asm_mov_eax_ebx(buffer);
#endif
        return up->next;
    } else if (up->type == DIV || ((up->type == PTR) && (up->prev != NULL) && (up->prev->type == EXPR))) { 
        int doffset = -1;
        if (stack_offset == -1) {
            doffset = add_local_symbol(&anonymous_int, buffer, 1); 
            asm_store_variable(&anonymous_int, get_variable_dynamic_offset(doffset, buffer), buffer);
        }
        
        T_NODE * last = one(get_token(up,NEXT), -1, buffer);
#ifdef X86 
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
#endif

        return last;
    } else if (up->type == PTR) {
        return one(up->next, stack_offset, buffer);
    } else if (up->type == BR_O) {
        return up->next;
    } else if (up->type == PAR_O) {
        line(up->desc->next, stack_offset, buffer);
        return up->asc;
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
#ifdef X86     
    write_output("out", buffer);
#elif defined M68K 
    write_hunk("hunk", buffer);
#endif

    free(buffer);
    free(filedata);
}