char dos_name[] = "dos.library";
char gfx_name[] = "graphics.library";
char intuition_name[] = "intuition.library";
void * dos_lib_ptr;
char hello_msg[] = "hello\n";
char error_msg[] = "An error as occurred\n";
char font_error_msg[] = "Couldn't load font.\n";
char fmt[] = "%ld\n";
char font[] = "topaz.font";
int MEMF_ANY = 0;
int MEMF_CHIP = 2;
int MEMF_FAST = 4;

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

    return;
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

void set_bp(void * mem_block, int bplptl, int bplpth) {

    asm {
        move.l mem_block, d0
        move.l bplpth, a6
        move.l d0,(a6)
    }
    
    return;
}


int is_mouse_pressed() {
    int ret = 0;
    asm {
        btst.b #6, ($BFE001).l
        bne.w loop
        move.l #1, ret 
        loop :
    }

    return ret;
}

int is_mouse_pressed2() {
    int ret = 0;
    asm {
        btst.b #7, ($BFE001).l
        bne.w loop
        move.l #1, ret 
        loop :
    }

    return ret;
}

void remake_display(void * lib_ptr) {
    asm {
        move.l lib_ptr, a6
        jsr -384(a6);
    }
    return;
}

// Clear current display LoadView(NULL)
void display(void * gfx_lib_ptr) {
    asm {
        move.l gfx_lib_ptr, a6
        move.l #0,a1
        jsr -222(a6); LoadView(NULL)
        jsr -270(a6); WaitTOF
        jsr -270(a6); WaitTOF
    }
    return;
}

void wait_vbl() {
    asm {
        loop :
        move.l	($dff004).l, d0
        and.l	#$1ff00, d0
        cmp.l	#77568, d0
        bne.w	loop
    }
    return;
}

short display_font(char * mem_block, void * font_ptr) {
    short modulo;
    asm {
        move.l mem_block, a0
        move.l font_ptr, a1
        move.w 38(a1), modulo
        move.l mem_block, a3
        move.l #0, d3
        loop_char :
        move.l 34(a1), a2
        move.l d3, d4
        muls.w #40, d4
        adda.l d4, a2
        move.l #0, d1
        loopz :
        move.l #0, d0
        loop : 
        move.l (a2)+, (a3)+
        addi.l #2, d0
        cmp.l #20, d0
        bne.w loop
        addi.l #1, d1
        adda.l #152, a2
        cmp.l #8, d1
        bne.w loopz
        addi.l #1, d3
        cmp.l #4, d3
        bne.w loop_char
        
        
    }
    return modulo;
}


void * open_font(void * gfx_lib_ptr, char * font_name) {
    void * font;
    short size = 8;
    int t = 0;
    void * toto = &t;
    asm {
        move.l gfx_lib_ptr, a6
        move.l toto, a0
        move.l font_name, (a0)
	    jsr -72(a6)
        move.l d0, font
    }

    return font;
}

void close_font(void * gfx_lib_ptr, char * font_ptr) {
    asm {
        move.l gfx_lib_ptr, a6
        move.l font_ptr, a1
        jsr -78(a6)
    } 

    return;   
}

short key_press() {
    short code = 0;
    asm {     
        move.l #0, d0
        move.b ($BFEC01).l, d0
        not.b d0
        ror.b #1, d0
        move.w d0, code
    }
    return code;
}


int main(int argc, char **argv) {
    
    void * gfx_lib_ptr = open_library(gfx_name);
    if (gfx_lib_ptr == 0) return -1;
    display(gfx_lib_ptr);

    dos_lib_ptr = open_library(dos_name);
    if (dos_lib_ptr == 0) dos_put_str(error_msg);
    
    dos_put_str(hello_msg);

    void * font_ptr = open_font(gfx_lib_ptr, font);
    if (font_ptr == 0) dos_put_str(font_error_msg);

    void * mem_block = alloc_mem(8000, MEMF_CHIP);
    if (mem_block == 0) dos_put_str(error_msg);

    char * ptr = mem_block;
    for (int i = 0;i < 8000; i++) {
        ptr[i] = 1;
    }

    short olddmareq = 0;
    short oldintena = 0;
    short oldintreq = 0;
    short oldadkcon = 0;

    asm {

        move.w ($DFF102).l, d0 ;DMACONR
        or.w #$8000, d0
        move.w d0, olddmareq
        move.w ($DFF01C).l, d0 ;INTENAR
        or.w #$8000, d0
        move.w d0, oldintena
        move.w ($DFF01E).l, d0 ;INTREQR
        or.w #$8000, d0
        move.w d0, oldintreq
        move.w ($DFF010).l, d0 ;ADKCONR
        or.w #$8000, d0
        move.w d0, oldadkcon

        move.l	($4).l, a6
        jsr -132(a6)	; Forbid

        move.w  #$3200, ($DFF100).l ;BPLCON0
        move.w  #$0000, ($DFF102).l ;BPLCON1

        move.w  #$0000, ($DFF108).l ;BPL1MOD
        move.w  #$0000, ($DFF10A).l ;BPL2MOD

        move.w  #$2c81, ($DFF08E).l ;DIWSTRT
        move.w  #$c8d1, ($DFF090).l ;DIWSTOP
        move.w  #$0038, ($DFF092).l ;DDFSTRT
        move.w  #$00d0, ($DFF094).l ;DDFSTOP    

        move.w  #$8100, ($DFF096).l ;DMACON BITPLANE ON
        move.w  #255, ($DFF096).l   ;DMACON EVERYTHING ELSE OFF
        move.w  #$C000, ($DFF09A).l ;INTENA
        move.w  #$3FFF, ($DFF09A).l ;INTENA 
                 
        move.w  #$FFF, ($DFF180).l  ;COLOR 0 to WHITE
        move.w  #$F00, ($DFF19E).l  ;COLOR 15 to BLACK
    
    }  
    int f_counter = 0;

    short modulo = display_font(mem_block, font_ptr);

    void * mem_ptr = mem_block;
    short k = 0;
    while (k != 96) {
        

        set_bp(mem_ptr, 14676194, 14676192);
        set_bp(mem_ptr, 14676198, 14676196);
        set_bp(mem_ptr, 14676202, 14676200);
        set_bp(mem_ptr, 14676206, 14676204);
        set_bp(mem_ptr, 14676210, 14676208);
        set_bp(mem_ptr, 14676214, 14676212);  

        k = key_press();
        
        if (is_mouse_pressed()) {
            mem_ptr = mem_ptr - 320;
        }

        if (is_mouse_pressed2()) {
            mem_ptr = mem_ptr + 320;            
        }

        f_counter++; 
        
        wait_vbl();
        
    }

    
    asm {
        move.w  #$7fff, ($DFF096).l
        move.w  olddmareq, ($DFF096).l
        move.w  #$7fff, ($DFF09A).l
        move.w  oldintena, ($DFF09A).l
        move.w  #$7fff, ($DFF01E).l
        move.w  oldintreq, ($DFF01E).l
        move.w  #$7fff, ($DFF010).l
        move.w  oldadkcon, ($DFF010).l

        move.l  ($4).l,a6
        jsr -138(a6)    ; Permit   

             
    }

    void * int_lib_ptr = open_library(intuition_name);
    remake_display(int_lib_ptr);
    free_mem(mem_block, 8000);
    vprintf(f_counter);
    vprintf(modulo);
    


    if (font_ptr != 0) close_font(gfx_lib_ptr, font_ptr);
    close_library(gfx_lib_ptr);
    close_library(int_lib_ptr);
    close_library(dos_lib_ptr);

    while(1) {
        k = key_press();
        vprintf(k);
    }

    return 0;
}