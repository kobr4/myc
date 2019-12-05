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
    free(buffer);
}

static void jsr_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4E, 0xB9, 0x00, 0x04, 0xB9, 0x8C };
    jsr(buffer, 0x4B98C);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void moveq_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x72, 0x80 };
    moveq(buffer, 0x80, D1);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void addi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x06, 0x81, 0x00, 0x00, 0x00, 0x20 };
    add_imm(buffer, D1, 0x20, 4);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void subi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x04, 0x01, 0x00, 0x20 };
    sub_imm(buffer, D1, 0x20, 1);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void move_imm_u8_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x12, 0x3C, 0x00, 0x20 };
    move_imm(buffer, D1, 0x20, 1);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void tst_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4A, 0x81 };
    tst(buffer, D1, 4);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void jmp_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4E, 0xF9, 0x00, 0x06, 0x5A, 0x70 };
    jmp_abs(buffer, 0x65A70);
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);    
}

static void parse_jsr_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x4E, 0xaE, 0xFD, 0xD8 };
    asm_line(buffer, "jsr -552(a6)");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);    
}

static void parse_move_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x11, 0xFC, 0x00, 0x64, 0xFF, 0xE0 };
    asm_line(buffer, "move.b #64, (FFFFFFE0).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);    
}

static void parse_move_test2(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x31, 0xFC, 0x03, 0xE8, 0xFF, 0xE0 };
    asm_line(buffer, "move.w #03E8, (FFFFFFE0).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);    
}

static void parse_move_test3(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x21, 0xFC, 0x05, 0xF5, 0xE1, 0x00, 0xFF, 0xE0 };
    asm_line(buffer, "move.l #05F5E100, (FFFFFFE0).l");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);    
}

static void parse_move_test4(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x12, 0x3C, 0x00, 0x20 };
    asm_line(buffer, "move.b #20, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);    
}

static void parse_move_test5(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x31, 0xF8, 0x01, 0xA5, 0xFF, 0xE0 };
    asm_line(buffer, "move.w (FFFF01A5).w, (FFFFFFE0).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);    
}

static void parse_move_test6(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x21, 0xC1, 0xFF, 0xE0 };
    asm_line(buffer, "move.l d1, (FFFFFFE0).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_move_test7(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x38, 0x7C, 0x60, 0x00 };
    asm_line(buffer, "move.w #6000, a4");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_adda_test(void **state) {
    T_BUFFER * buffer2 = create_buffer();
    adda_reg(buffer2, D0, A4);
    T_BUFFER * buffer = create_buffer();
    asm_line(buffer, "adda.l d0, a4");
    assert_memory_equal(buffer->buffer, buffer2->buffer, buffer2->length);
    free(buffer);
    free(buffer2);
}

static void parse_adda_test2(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xD2, 0xD0 };
    asm_line(buffer, "adda.w (a0), a1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_adda_test3(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xD2, 0XF8, 0xF1, 0x00 };
    asm_line(buffer, "adda.w (FFFFF100).w, a1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_lea_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x41, 0xF8, 0xFF, 0xF4 };
    asm_line(buffer, "lea.l (FFFFFFF4).w, a0");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_divu_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x82, 0xF8, 0xF3, 0x14 };
    asm_line(buffer, "divu.w (FFFFF314).w, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_divs_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x81, 0xFC ,0x00, 0x0A };
    asm_line(buffer, "divs.w #A, d0");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_mulu_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xC4, 0xFC, 0x00, 0x19 };
    asm_line(buffer, "mulu.w #19, d2");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_muls_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0xC3, 0xFC, 0x00, 0X19 };
    asm_line(buffer, "muls.w #19, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_cmpi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x0C, 0X38, 0x00, 0x26, 0xFF, 0xE4 };
    asm_line(buffer, "cmpi.b #26, (FFFFFFE4).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_cmpi_test2(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x0C, 0x78, 0x41, 0x70, 0xFF, 0xB0 };
    asm_line(buffer, "cmpi.w #4170, (FFFFFFB0).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_cmpi_test3(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x0C, 0x82, 0x00, 0x00, 0x00, 0x00 };
    asm_line(buffer, "cmpi.l #0, d2");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_andi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x02, 0x01, 0x00, 0x36 };
    asm_line(buffer, "andi.b #36, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_addi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x06, 0x01, 0x00, 0x20 };
    asm_line(buffer, "addi.b #20, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_subi_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x04, 0x01, 0x00, 0x20 };
    asm_line(buffer, "subi.b #20, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_ori_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x00, 0x01, 0x00, 0x36 };
    asm_line(buffer, "ori.b #36, d1");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_ori_test2(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x00, 0x78, 0x20, 0x05, 0xF1, 0x00 };
    asm_line(buffer, "ori.w #2005, (FFFFF100).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_eori_test(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x0A, 0x38, 0x00, 0x80, 0xF0, 0x0E };
    asm_line(buffer, "eori.b #80, (FFFFF00E).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
    free(buffer);
}

static void parse_eori_test2(void **state) {
    T_BUFFER * buffer = create_buffer();
    U8 tab[] = { 0x0A, 0x78, 0x31, 0x19, 0xF0, 0x10 };
    asm_line(buffer, "eori.w #3119, (FFFFF010).w");
    assert_memory_equal(buffer->buffer, tab, sizeof(tab));
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
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}