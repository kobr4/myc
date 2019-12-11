#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "asm_m68k.c"

int variable_size(T_NODE * up) {
    return 4;
}

T_BUFFER * create_buffer() {
    T_BUFFER * buffer = (T_BUFFER *)malloc(sizeof(T_BUFFER));
    memset(buffer->buffer, 0, sizeof(T_BUFFER));
    buffer->length = 0;
    return buffer;
}

static void rts_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4E, 0x75 };
    rts(buffer);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void jsr_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4E, 0xB9, 0x00, 0x04, 0xB9, 0x8C };
    jsr(buffer, 0x4B98C);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void moveq_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x72, 0x80 };
    moveq(buffer, 0x80, D1);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void addi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x06, 0x81, 0x00, 0x00, 0x00, 0x20 };
    add_imm(buffer, D1, 0x20, 4);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void subi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x04, 0x01, 0x00, 0x20 };
    sub_imm(buffer, D1, 0x20, 1);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void move_imm_u8_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x12, 0x3C, 0x00, 0x20 };
    move_imm(buffer, D1, 0x20, 1);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void tst_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4A, 0x81 };
    tst(buffer, D1, 4);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void jmp_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4E, 0xF9, 0x00, 0x06, 0x5A, 0x70 };
    jmp_abs(buffer, 0x65A70);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);    
}

static void parse_jsr_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4E, 0xaE, 0xFD, 0xD8 };
    asm_line(NULL, buffer, "jsr -552(a6)");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);    
}

static void parse_move_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x11, 0xFC, 0x00, 0x64, 0xFF, 0xE0 };
    asm_line(NULL, buffer, "move.b #$64, ($FFFFFFE0).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);    
}

static void parse_move_test2(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x31, 0xFC, 0x03, 0xE8, 0xFF, 0xE0 };
    asm_line(NULL, buffer, "move.w #$03E8, ($FFFFFFE0).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);    
}

static void parse_move_test3(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x23, 0xFC, 0x05, 0xF5, 0xE1, 0x00, 0xFF, 0xFF, 0xFF, 0xE0 };
    asm_line(NULL, buffer, "move.l #$05F5E100, ($FFFFFFE0).l");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);    
}

static void parse_move_test4(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x12, 0x3C, 0x00, 0x20 };
    asm_line(NULL, buffer, "move.b #$20, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);    
}

static void parse_move_test5(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x31, 0xF8, 0x01, 0xA5, 0xFF, 0xE0 };
    asm_line(NULL, buffer, "move.w ($FFFF01A5).w, ($FFFFFFE0).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);    
}

static void parse_move_test6(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x21, 0xC1, 0xFF, 0xE0 };
    asm_line(NULL, buffer, "move.l d1, ($FFFFFFE0).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_move_test7(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x38, 0x7C, 0x60, 0x00 };
    asm_line(NULL, buffer, "move.w #$6000, a4");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_adda_test(void **state) {
    T_BUFFER * buffer2 = create_buffer();
    adda_reg(buffer2, D0, A4);
    T_BUFFER * buffer = create_buffer();
    asm_line(NULL, buffer, "adda.l d0, a4");
    assert_memory_equal(buffer->buffer, buffer2->buffer, buffer2->length);
    free(buffer);
    free(buffer2);
}

static void parse_adda_test2(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xD2, 0xD0 };
    asm_line(NULL, buffer, "adda.w (a0), a1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_adda_test3(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xD2, 0XF8, 0xF1, 0x00 };
    asm_line(NULL, buffer, "adda.w ($FFFFF100).w, a1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_lea_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x41, 0xF8, 0xFF, 0xF4 };
    asm_line(NULL, buffer, "lea.l ($FFFFFFF4).w, a0");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_divu_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x82, 0xF8, 0xF3, 0x14 };
    asm_line(NULL, buffer, "divu.w ($FFFFF314).w, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_divs_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x81, 0xFC ,0x00, 0x0A };
    asm_line(NULL, buffer, "divs.w #$A, d0");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_mulu_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xC4, 0xFC, 0x00, 0x19 };
    asm_line(NULL, buffer, "mulu.w #$19, d2");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_muls_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xC3, 0xFC, 0x00, 0X19 };
    asm_line(NULL, buffer, "muls.w #$19, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_cmpi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x0C, 0X38, 0x00, 0x26, 0xFF, 0xE4 };
    asm_line(NULL, buffer, "cmpi.b #$26, ($FFFFFFE4).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_cmpi_test2(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x0C, 0x78, 0x41, 0x70, 0xFF, 0xB0 };
    asm_line(NULL, buffer, "cmpi.w #$4170, ($FFFFFFB0).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_cmpi_test3(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x0C, 0x82, 0x00, 0x00, 0x00, 0x00 };
    asm_line(NULL, buffer, "cmpi.l #$0, d2");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_andi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x02, 0x01, 0x00, 0x36 };
    asm_line(NULL, buffer, "andi.b #$36, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_addi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x06, 0x01, 0x00, 0x20 };
    asm_line(NULL, buffer, "addi.b #$20, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_subi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x04, 0x01, 0x00, 0x20 };
    asm_line(NULL, buffer, "subi.b #$20, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_ori_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x00, 0x01, 0x00, 0x36 };
    asm_line(NULL, buffer, "ori.b #$36, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_ori_test2(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x00, 0x78, 0x20, 0x05, 0xF1, 0x00 };
    asm_line(NULL, buffer, "ori.w #$2005, ($FFFFF100).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_eori_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x0A, 0x38, 0x00, 0x80, 0xF0, 0x0E };
    asm_line(NULL, buffer, "eori.b #$80, ($FFFFF00E).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_eori_test2(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x0A, 0x78, 0x31, 0x19, 0xF0, 0x10 };
    asm_line(NULL, buffer, "eori.w #$3119, ($FFFFF010).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_beq_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x67, 0x00, 0x00, 0x10 };
    asm_line(NULL, buffer, "beq.w #$10");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_ble_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x6F, 0x00, 0x00, 0x10 };
    asm_line(NULL, buffer, "ble.w #$10");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_bge_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x6C, 0x00, 0x00, 0x10 };
    asm_line(NULL, buffer, "bge.w #$10");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_bne_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x66, 0x00, 0x00, 0x10 };
    asm_line(NULL, buffer, "bne.w #$10");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_bcc_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x64, 0x00, 0x00, 0x10 };
    asm_line(NULL, buffer, "bcc.w #$10");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_bcs_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x65, 0x00, 0x00, 0x10 };
    asm_line(NULL, buffer, "bcs.w #$10");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_bvc_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x68, 0x00, 0x00, 0x10 };
    asm_line(NULL, buffer, "bvc.w #$10");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_bvs_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x69, 0x00, 0x00, 0x10 };
    asm_line(NULL, buffer, "bvs.w #$10");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_bhi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x62, 0x00, 0x00, 0x10 };
    asm_line(NULL, buffer, "bhi.w #$10");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_bra_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x60, 0x00, 0x4E, 0x1A };
    asm_line(NULL, buffer, "bra.w #$4E1A");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}


static void parse_bsr_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x61, 0x00, 0x4E, 0x1A };
    asm_line(NULL, buffer, "bsr.w #$4E1A");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_movea_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x38, 0x7C, 0x60, 0x00 };
    asm_line(NULL, buffer, "movea.w #$6000, a4");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_beq_label_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x67, 0x00, 0xFF, 0xFE };
    asm_block(buffer, "toto :\nbeq.w toto\n");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_beq_backpatch_label_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x67, 0x00, 0x00, 0x02 };
    asm_block(buffer, "beq.w toto\ntoto :\n");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}


static void parse_move_to_local_symbol_test(void **state) {
    T_ELT elt_int;
    T_NODE anonymous_int;
    elt_int.next = NULL;
    elt_int.str = "toto";
    elt_int.len = strlen(elt_int.str);
    anonymous_int.elt = &elt_int;
    anonymous_int.prev = &anonymous_int;
    anonymous_int.type = INT;


    T_BUFFER * buffer = create_buffer();
    buffer->local_symbol[0] = &anonymous_int;
    buffer->local_symbol_count++;

    U8 tab[] = { 0x2F, 0x41, 0x0, 0x00 };
    asm_line(NULL, buffer, "move.l d1, toto");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_move_from_local_symbol_test(void **state) {
    T_ELT elt_int;
    T_NODE anonymous_int;
    elt_int.next = NULL;
    elt_int.str = "toto";
    elt_int.len = strlen(elt_int.str);
    anonymous_int.elt = &elt_int;
    anonymous_int.prev = &anonymous_int;
    anonymous_int.type = INT;


    T_BUFFER * buffer = create_buffer();
    buffer->local_symbol[0] = &anonymous_int;
    buffer->local_symbol_count++;

    U8 tab[] = { 0x22, 0x2F, 0x0, 0x00 };
    asm_line(NULL, buffer, "move.l toto, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_lsl_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xF5, 0xC0 };
    asm_line(NULL, buffer, "lsl.w d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_lsr_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xF4, 0xC0 };
    asm_line(NULL, buffer, "lsr.w d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_asl_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xE1, 0xC0 };
    asm_line(NULL, buffer, "asl.w d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_asr_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xE0, 0xC0 };
    asm_line(NULL, buffer, "asr.w d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_neg_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x44, 0x78, 0xFE, 0x26 };
    asm_line(NULL, buffer, "neg.w ($FFFFFE26).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_not_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x46, 0x78, 0xFE, 0x26 };
    asm_line(NULL, buffer, "not.w ($FFFFFE26).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_clr_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x42, 0x78, 0x85, 0x00 };
    asm_line(NULL, buffer, "clr.w ($FFFF8500).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_rts_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4E, 0x75 };
    asm_line(NULL, buffer, "rts");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);
}

static void parse_btst_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x08, 0x39, 0x00, 0x06, 0x00, 0xBF, 0xe0, 0x01 };
    asm_line(NULL, buffer, "btst.b #6,($BFE001).l");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer);    
}


static void parse_swap_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4a, 0x40 };
    asm_line(NULL, buffer, "swap d0");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    assert_int_equal(buffer->length, sizeof(tab));
    free(buffer); 
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(rts_test),
        cmocka_unit_test(jsr_test),
        cmocka_unit_test(moveq_test),
        cmocka_unit_test(addi_test),
        cmocka_unit_test(subi_test),
        cmocka_unit_test(move_imm_u8_test),
        cmocka_unit_test(tst_test),
        cmocka_unit_test(jmp_test),
        cmocka_unit_test(parse_jsr_test),
        cmocka_unit_test(parse_move_test),
        cmocka_unit_test(parse_move_test2),
        cmocka_unit_test(parse_move_test3),
        cmocka_unit_test(parse_move_test4),
        cmocka_unit_test(parse_move_test5),
        cmocka_unit_test(parse_move_test6),
        cmocka_unit_test(parse_move_test7),
        cmocka_unit_test(parse_adda_test),
        cmocka_unit_test(parse_adda_test2),
        cmocka_unit_test(parse_adda_test3),
        cmocka_unit_test(parse_lea_test),
        cmocka_unit_test(parse_divu_test),
        cmocka_unit_test(parse_divs_test),
        cmocka_unit_test(parse_mulu_test),
        cmocka_unit_test(parse_muls_test),
        cmocka_unit_test(parse_cmpi_test),        
        cmocka_unit_test(parse_cmpi_test2),
        cmocka_unit_test(parse_cmpi_test3),
        cmocka_unit_test(parse_andi_test),
        cmocka_unit_test(parse_addi_test),
        cmocka_unit_test(parse_subi_test),
        cmocka_unit_test(parse_ori_test),
        cmocka_unit_test(parse_ori_test2),
        cmocka_unit_test(parse_eori_test),
        cmocka_unit_test(parse_eori_test2),
        cmocka_unit_test(parse_beq_test),
        cmocka_unit_test(parse_ble_test),
        cmocka_unit_test(parse_bne_test),
        cmocka_unit_test(parse_bge_test),
        cmocka_unit_test(parse_bcc_test),
        cmocka_unit_test(parse_bcs_test),
        cmocka_unit_test(parse_bvc_test),
        cmocka_unit_test(parse_bvs_test),
        cmocka_unit_test(parse_bhi_test),
        cmocka_unit_test(parse_bra_test),
        cmocka_unit_test(parse_bsr_test),
        cmocka_unit_test(parse_movea_test),
        cmocka_unit_test(parse_beq_label_test),
        cmocka_unit_test(parse_move_to_local_symbol_test),
        cmocka_unit_test(parse_move_from_local_symbol_test),
        cmocka_unit_test(parse_lsl_test),
        cmocka_unit_test(parse_lsr_test),
        cmocka_unit_test(parse_asl_test),
        cmocka_unit_test(parse_asr_test),
        cmocka_unit_test(parse_neg_test),
        cmocka_unit_test(parse_not_test), 
        cmocka_unit_test(parse_clr_test),
        cmocka_unit_test(parse_rts_test),
        cmocka_unit_test(parse_btst_test),  
        cmocka_unit_test(parse_beq_backpatch_label_test),        
        cmocka_unit_test(parse_swap_test),       
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}