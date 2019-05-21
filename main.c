#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct T_ELT {
    char * str;
    int len;
    struct T_ELT * next;
} T_ELT;

typedef struct T_NODE {
    struct T_NODE * desc;
    struct T_NODE * next;
    T_ELT * elt;
    struct T_NODE * asc;
} T_NODE;

enum keyword {
    VOID,
    INT,
    CHAR,
    PAR_C,
    PAR_O,
    ACC_C,
    ACC_O,
    PTR,
    OP,
    END,
    EXPR,
    COM
};

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

char * read_file(char * filename, int * size) {
    
    FILE * file = fopen(filename, "r");
    fseek(file, 0L, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char * fout = (char *)malloc(sizeof(char)**size);
    fread(fout, sizeof(char), *size, file);
    return fout;
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

T_NODE * add_next_node(T_NODE * node, T_ELT * elt) {
    T_NODE * new_n = (T_NODE *)malloc(sizeof(T_NODE));
    new_n->desc = NULL;
    new_n->next = NULL;
    new_n->elt = elt;
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
                if (current_exp != -1) {
                    current = add_token(current, &input[current_exp], c-current_exp);
                    current_exp = -1;
                }            
                current = add_token(current, &input[c], 1);
                break;
            default:
                if (current_exp == -1) {
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

enum keyword type(T_ELT * elt) {
    enum keyword result = EXPR;
    if (strncmp(elt->str, "void", elt->len) == 0) result = VOID;
    else if (strncmp(elt->str, "int", elt->len) == 0) result = INT;
    else if (strncmp(elt->str, "char", elt->len) == 0) result = CHAR;
    else if (strncmp(elt->str, "(", elt->len) == 0) result = PAR_O;
    else if (strncmp(elt->str, ")", elt->len) == 0) result = PAR_C;
    else if (strncmp(elt->str, "{", elt->len) == 0) result = ACC_O;
    else if (strncmp(elt->str, "}", elt->len) == 0) result = ACC_C;
    else if (strncmp(elt->str, "*", elt->len) == 0) result = PTR;
    else if (strncmp(elt->str, "+", elt->len) == 0) result = OP;
    else if (strncmp(elt->str, ";", elt->len) == 0) result = END;
    else if (strncmp(elt->str, ",", elt->len) == 0) result = COM;
    return result;
} 

void create_node_expr(T_NODE * up, T_ELT * current) {
    T_NODE * current_n = NULL;
    display_elt(current);
    putchar(' ');

    if (current == NULL) 
        return;

    if (up == NULL)
        return;
    

    switch (type(current)) {
        case END:
        case PAR_C:
        case ACC_C:
            //current_n = add_next_node(up, current);
            current_n = add_next_node(up->asc, current);
            create_node_expr(current_n->asc, current->next);
            break;
        case COM:
            create_node_expr(up, current->next);
            break;
        case PAR_O:
        case ACC_O:
            current_n = add_desc_node(up, current);
            current_n = add_desc_node(current_n, NULL);
            create_node_expr(current_n, current->next);
            break;
        case EXPR:
            current_n = add_next_node(up, current);
            create_node_expr(current_n, current->next);
            break;
        default:
            create_node_expr(up, current->next);
            break;
    }
}


void display_node(T_NODE * node, int spacing) {
    if (node == NULL) 
        return;

    for (int i = 0;i < spacing;i++) putchar('-');
    display_elt(node->elt);
    putchar('\n');
    display_node(node->desc, spacing+2);
    display_node(node->next, spacing);
}

/*
T_NODE * create_tree(T_ELT * head) {
    T_NODE * up = NULL;
    T_ELT * current = head;
    T_NODE * current_n = NULL;
    do {
        switch (type(current)) {
            case VOID:
            case INT:
            case CHAR:
                current_n = add_simple_node(up, current);
                break;
            case EXPR:
                current_n = add_simple_node(up, current);
                break;
            default:
                current_n = add_simple_node(up, current);
                break;
        }
        current = current->next;
    } while (current != NULL);

    return up;
}
*/

void main(int c, char** argv) {
    printf("Reading file %s\n",argv[1]);
    int size;
    char * filedata = read_file(argv[1], &size);
    T_ELT * elt = tokenize(filedata, size);
    display_all_elt(elt);
    T_NODE * up = add_desc_node(NULL, NULL);
    create_node_expr(up, elt);
    display_node(up,0);
}