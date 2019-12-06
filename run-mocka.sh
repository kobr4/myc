#!/bin/bash

gcc cmocka_test.c -o mockatests -lcmocka
COMPIL=$?

if [ $COMPIL = "0" ] 
then
./mockatests
fi