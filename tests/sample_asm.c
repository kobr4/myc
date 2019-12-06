char dos_name[] = "dos.library";
char hello[] = "HELLO WORLD";

int dos_put_str(char * msg, char * doslib) {
    asm {
        move.l (4).w, a6
        move.l doslib, d1
        move.l d1, a1
        move.l #0, d0
        jsr -552(a6)  
        move.l d0, a6
        move.l msg, d1
        jsr -948(a6)                 
    }
    return 77;
}

int main(int argc, char **argv) {
    return dos_put_str(hello, dos_name);
}