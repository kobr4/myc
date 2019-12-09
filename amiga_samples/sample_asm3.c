char dos_name[] = "dos.library";
char gfx_name[] = "graphics.library";
char intuition_name[] = "intuition.library";
void * dos_lib_ptr;
char hello_msg[] = "hello\n";
char error_msg[] = "An error as occurred\n";
char fmt[] = "%ld\n";
int MEMF_CHIP = 2;

void * open_library(char * lib_name) {
    void * lib_ptr;
    asm {
        move.l ($4).w, a6
        move.l lib_name, a1
        move.l #0, d0
        jsr -552(a6)
        move.l d0, lib_ptr      
    }
    return lib_ptr;
}

void close_library(void * lib_ptr) {
    asm {
        move.l ($4).w, a6
        move.l lib_ptr, a1
        jsr -414(a6)        
    }
    return;
}

void vprintf(int a) {
    int * a_ptr = &a; 
    void * l_dos_lib_ptr = dos_lib_ptr;
    char * l_fmt = fmt;
    asm {
        move.l l_dos_lib_ptr, a6
        move.l a_ptr, d2
        move.l l_fmt, d1
        jsr -954(a6)
    }
}

void dos_put_str(char * msg) {
    void * l_dos_lib_ptr = dos_lib_ptr;
    asm {
        move.l l_dos_lib_ptr, a6
        move.l msg, d1
        jsr -948(a6)        
    }
    return;
}

void * alloc_mem(int size, int flag) {
    void * mem_block;
    asm {
        move.l ($4).w, a6
        move.l size, d0
        move.l flag, d1
        jsr -198(a6)
        move.l d0, mem_block
    }

    return mem_block;
}

void free_mem(void * mem_block, int size) {
    asm {
        move.l ($4).w, a6
        move.l mem_block, a1
        move.l size, d0
        jsr -210(a6)
    }

    return;
}


int main(int argc, char **argv) {
    
    dos_lib_ptr = open_library(dos_name);
    if (dos_lib_ptr == 0) dos_put_str(error_msg);

    dos_put_str(hello_msg);
    vprintf(dos_lib_ptr);

    void * mem_block = alloc_mem(8000, MEMF_CHIP);
    if (mem_block == 0) dos_put_str(error_msg);
    free_mem(mem_block, 8000);

    vprintf(MEMF_CHIP);
    close_library(dos_lib_ptr);
    return 0;
}