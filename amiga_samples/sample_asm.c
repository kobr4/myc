char dos_name[] = "dos.library";
char hello[] = "Hello World !";

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

void dos_put_str(char * msg, void * dos_lib_ptr) {
    asm {
        move.l dos_lib_ptr, a6
        move.l msg, d1
        jsr -948(a6)        
    }
    return;
}

int main(int argc, char **argv) {
    
    void * dos_lib_ptr = open_library(dos_name);
    dos_put_str(hello, dos_lib_ptr);
    close_library(dos_lib_ptr);
    return 0;
}