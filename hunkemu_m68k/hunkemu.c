#include "Musashi/m68k.h"
#include <stdio.h>
#include <stdlib.h>

#define ENTRY_POINT_ADDR 0x2FFF

unsigned char memory[0xFFFFFF];

unsigned int  m68k_read_memory_8(unsigned int address) {
    return memory[address];
}

unsigned int  m68k_read_memory_16(unsigned int address) {
    return (memory[address] << 8) | memory[address+1];
}

unsigned int  m68k_read_memory_32(unsigned int address) {
    return (m68k_read_memory_16(address) << 16) | m68k_read_memory_16(address+2);
}

void m68k_write_memory_8(unsigned int address, unsigned int value) {
    memory[address] = value & 0xFF;
}

void m68k_write_memory_16(unsigned int address, unsigned int value) {
    memory[address] = (value & 0xFF00) >> 8;
    memory[address+1] = (value & 0xFF);  
}

void m68k_write_memory_32(unsigned int address, unsigned int value) {
    memory[address] = (value & 0xFF000000) >> 24;
    memory[address+1] = (value & 0x00FF0000) >> 16;
    memory[address+2] = (value & 0x0000FF00) >> 8;
    memory[address+3] = (value & 0x000000FF) >> 0;    
}

void error(const char * message) {
    puts(message);
    exit(0);
}

unsigned short read16(FILE * f) {
    unsigned char u8h;
    unsigned char u8l;
    fread(&u8h, sizeof(char), 1, f);
    fread(&u8l, sizeof(char), 1, f);  

    return u8h << 8 | u8l;
}

unsigned int read32(FILE * f) {
    return (read16(f) << 16) | read16(f);
 }

int load_hunk(const char * filename) {
    FILE * f = fopen(filename, "rb");
    if (f == NULL) error("Can't open file.");

    unsigned int magic = read32(f);
    if (magic != 0x03F3) error("Wrong file");

    read32(f);read32(f);read32(f);read32(f);read32(f);//skip 5 int

    unsigned int magic_code = read32(f);
    if (magic_code != 0x3E9) error("Wrong file");

    unsigned int length_code = read32(f);
    fread(&memory[ENTRY_POINT_ADDR], length_code * 4, 1, f);

    unsigned int magic_reloc32 = read32(f);
    if (magic_reloc32 == 0x3EC) {
        unsigned int n = read32(f);
        read32(f);
        for (int i = 0;i < n; i++) {
            unsigned int offset = read32(f);
            m68k_write_memory_32(offset + ENTRY_POINT_ADDR, 
                m68k_read_memory_32(offset + ENTRY_POINT_ADDR) + ENTRY_POINT_ADDR);
        }
    }


    fclose(f);

    return length_code * 4;
}


int main(int argc, char **argv) {
    for (int i = 0;i < 0xFFFF;i++) memory[i] = 0; 

    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    m68k_pulse_reset();

    int length = load_hunk(argv[1]);

    m68k_set_reg(M68K_REG_PC, ENTRY_POINT_ADDR);
    m68k_set_reg(M68K_REG_SP, 0xFFFFF0);


    while(1) {
        int cycles = m68k_execute(1);
        unsigned int pc = m68k_get_reg(NULL, M68K_REG_PC);

        if (pc > (ENTRY_POINT_ADDR + length) || pc < ENTRY_POINT_ADDR) {
            return m68k_get_reg(NULL, M68K_REG_D0);
        }
    }
    
    return 0;
}