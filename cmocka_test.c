#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "asm_m68k.c"

T_BUFFER * create_buffer() {
    T_BUFFER * buffer = (T_BUFFER *)malloc(sizeof(T_BUFFER));
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
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}