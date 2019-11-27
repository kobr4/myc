#ifndef HUNK
#define HUNK

#include "main.h"
#include <stdlib.h>
#include <stdio.h>


typedef struct T_HUNK_HEADER {
    U32 magic;
    U32 empty_list;
    U32 table_size;
    U32 first_hunk;
    U32 last_hunk;
    U32 * hunk_sizes;
} T_HUNK_HEADER;

typedef struct T_HUNK_CODE {
    U32 magic;
    U32 n;
    U32 * code;
} T_HUNK_CODE;

;

U32 l_endian(U32 in) {
    return (in & 0xFF000000) >> 24 | (in & 0x00FF0000) >> 8 | (in & 0x0000FF00) << 8 | (in & 0x000000FF) << 24; 
}

U16 sl_endian(U16 in) {
    return (in & 0xFF00) >> 8 | (in & 0xFF) << 8; 
}


void write_setup(T_BUFFER * buffer, int main_offset);

void write_hunk(char * filename, T_BUFFER * buffer) {
    puts("Writing hunk");
    U32 hunk_end = l_endian(0x3F2);

    T_BUFFER * setup_buffer = (T_BUFFER*)malloc(sizeof(T_BUFFER));
    setup_buffer->length = 0;
    write_setup(setup_buffer, buffer->main_offset + 4);
    
    FILE * f = fopen(filename, "wb");
    if (f == NULL) error("Unable to create destination file.");

    T_HUNK_CODE code;
    code.magic = l_endian(0x3E9);
    
    code.n = buffer->length / 4;
 //   U32 code_n = l_endian(code.n);
    code.code = (U32*)malloc(code.n * sizeof(U32));
    
    U8 * codeAsU8 = (U8*)code.code;
    for (int i = 0;i < code.n;i++) code.code[i] = 0;
    for(int i = 0;i < buffer->length; i++) {
        codeAsU8[i] = buffer->buffer[i];
    }

    T_HUNK_HEADER header;
    header.magic = l_endian(0x03F3);
    header.empty_list = 0;
    header.table_size = l_endian(1);
    header.first_hunk = 0;
    header.last_hunk = 0;
    header.hunk_sizes = (U32*)malloc(sizeof(U32)*1);
    //header.hunk_sizes[0] = l_endian(code.n * 4 + 3 * sizeof(U32));
    header.hunk_sizes[0] = l_endian(code.n * 4  + 200); 
    

    fwrite(&header.magic, sizeof(header.magic), 1, f);
    fwrite(&header.empty_list, sizeof(header.empty_list), 1, f);
    fwrite(&header.table_size, sizeof(header.table_size), 1, f);
    fwrite(&header.first_hunk, sizeof(header.first_hunk), 1, f);
    fwrite(&header.last_hunk, sizeof(header.last_hunk), 1, f);
    fwrite(header.hunk_sizes, sizeof(U32) * 1, 1, f);

    fwrite(&code.magic, sizeof(code.magic), 1, f);

//    fwrite(&code_n, sizeof(code.n), 1, f);
    
    int code_length = l_endian(code.n + setup_buffer->length / 4 + 1);
    printf("Code length %d vs %ld\n", code.n + setup_buffer->length / 4 + 1, setup_buffer->length + code.n * sizeof(U32));
    fwrite(&code_length, 4, 1, f);
    //fwrite(setup_buffer->buffer, (setup_buffer->length / 4) * sizeof(U32), 1, f);
    fwrite(setup_buffer->buffer, setup_buffer->length, 1, f);

    fwrite(code.code, code.n * sizeof(U32), 1, f);
    U8 pad = 0;
    for (int i = setup_buffer->length + code.n * sizeof(U32); i < (code.n + setup_buffer->length / 4 + 1) * 4; i++) {
        printf("PAD %d\n",i);
        fwrite(&pad, sizeof(U8), 1, f);
    }

    
    fwrite(&hunk_end, sizeof(U32), 1, f);
    fclose(f);
}

#endif