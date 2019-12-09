char gfx_name[] = "graphics.library";
char intuition_name[] = "intuition.library";

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

void remake_display(void * lib_ptr) {
    asm {
        move.l lib_ptr, a6
        jsr -384(a6);
    }
    return;
}

void display(void * gfx_lib_ptr) {
    asm {
        move.l gfx_lib_ptr, a6
        move.l #0,a1
        jsr -222(a6)
        jsr -270(a6)
        jsr -270(a6)
    }
    return;
}

void loop() {
    asm {
        loop :
        btst.b #6, ($BFE001).l
        bne.w loop
    }

    return;
}

int main(int argc, char **argv) {
    
    void * gfx_lib_ptr = open_library(gfx_name);
    if (gfx_lib_ptr == 0) return -1;
    display(gfx_lib_ptr);
    loop();

    void * int_lib_ptr = open_library(intuition_name);
    remake_display(int_lib_ptr);
    close_library(int_lib_ptr);
    close_library(gfx_lib_ptr);
    return 0;
}