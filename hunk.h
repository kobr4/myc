#ifndef HUNK
#define HUNK

#include "main.h"
#include <stdlib.h>
#include <stdio.h>

#define SETUP_LENGTH 6

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
int is_array_access(T_NODE * n);
void write_hunk_reloc32(FILE * f, T_BUFFER * buffer, U32 start) {

    int count = 0;
    for (int i = 0;i < buffer->global_symbol_count; i++) {
            if (is_array_access(buffer->global_symbol[i])) {
                count++;
            }
    }
    printf("Number of symbol to relocate: %d\n", count);
    U32 hunk_reloc32 =  l_endian(0x3EC);
    U32 hunk_offset_count = l_endian(count);
    U32 hunk_num = l_endian(0);
    //U32 start_offset = l_endian(start);
    U32 zero = 0;
    fwrite(&hunk_reloc32, sizeof(U32), 1, f);
    fwrite(&hunk_offset_count, sizeof(U32), 1, f);

    if (count != 0) {
        fwrite(&hunk_num, sizeof(U32), 1, f);
        //fwrite(&start_offset, sizeof(U32), 1, f);
        for (int i = 0;i < buffer->global_symbol_count; i++) {
            if (is_array_access(buffer->global_symbol[i])) {
                U32 hunk_reloc_offset = l_endian(buffer->global_symbol[i]->offset + SETUP_LENGTH);
                printf("Relocating: %d\n", hunk_reloc_offset);
                fwrite(&hunk_reloc_offset, sizeof(U32), 1, f);
            }
        }
        fwrite(&zero, sizeof(U32), 1, f); 
    }
       
    
}


void write_hunk(char * filename, T_BUFFER * buffer) {
    puts("Writing hunk");
    U32 hunk_end = l_endian(0x3F2);

    T_BUFFER * setup_buffer = (T_BUFFER*)malloc(sizeof(T_BUFFER));
    setup_buffer->length = 0;
    write_setup(setup_buffer, buffer->main_offset + 4);
    printf("SETUP length: %d\n", setup_buffer->length);

    if (setup_buffer->length != SETUP_LENGTH) error("Error setup_length constant should be updated.");

    FILE * f = fopen(filename, "wb");
    if (f == NULL) error("Unable to create destination file.");

    T_HUNK_CODE code;
    code.magic = l_endian(0x3E9);
    code.n = buffer->length / 4 + 1;
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
    header.hunk_sizes = (U32*)malloc(sizeof(U32)*2);
    //header.hunk_sizes[0] = l_endian(code.n * 4 + 3 * sizeof(U32));
    
    header.hunk_sizes[0] = l_endian(code.n * 4  + 1000); 

    fwrite(&header.magic, sizeof(header.magic), 1, f);
    fwrite(&header.empty_list, sizeof(header.empty_list), 1, f);
    fwrite(&header.table_size, sizeof(header.table_size), 1, f);
    fwrite(&header.first_hunk, sizeof(header.first_hunk), 1, f);
    fwrite(&header.last_hunk, sizeof(header.last_hunk), 1, f);
    fwrite(header.hunk_sizes, sizeof(U32) * 1, 1, f);

    fwrite(&code.magic, sizeof(code.magic), 1, f);

    int code_length = l_endian(code.n + setup_buffer->length / 4 + 1);
    printf("CODE LENGTH %d bytes\n", (code.n + setup_buffer->length / 4 + 1) * 4);
    fwrite(&code_length, 4, 1, f);
    fwrite(setup_buffer->buffer, setup_buffer->length, 1, f);
    printf("[SETUP]Writing: %d bytes\n", setup_buffer->length);
    fwrite(code.code, buffer->length, 1, f);
    printf("[MAIN]Writing: %d bytes\n", buffer->length);
    U8 pad = 0;
    for (int i = setup_buffer->length + buffer->length;i < (code.n + setup_buffer->length / 4 + 1) * 4; i++) {
            printf("[PAD]Writing: 1 byte\n");
            fwrite(&pad, sizeof(U8), 1, f);      
    }

    

    write_hunk_reloc32(f, buffer, setup_buffer->length);
    fwrite(&hunk_end, sizeof(U32), 1, f);


    
    fclose(f);
}

#endif