#!/bin/bash

gcc asm_m68k_test.c -o asm_m68k_test -lcmocka
COMPIL=$?

if [ $COMPIL = "0" ] 
then
./asm_m68k_test
fi