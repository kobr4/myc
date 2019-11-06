#!/bin/bash

OUTBIN="./out"

SOURCE="sample.c"
echo "1: $SOURCE"

./main tests/$SOURCE > /dev/null
$OUTBIN
if [ $? = "95" ] 
then
    echo "PASSED"
else 
    echo "FAILED"
fi

SOURCE="sample2.c"
echo "2: $SOURCE"

./main tests/$SOURCE > /dev/null
$OUTBIN
if [ $? = "11" ] 
then
    echo "PASSED"
else 
    echo "FAILED"    
fi

SOURCE="fibo.c"
echo "3: $SOURCE"

./main tests/$SOURCE > /dev/null
$OUTBIN
if [ $? = "55" ] 
then
    echo "PASSED"
else 
    echo "FAILED"    
fi
