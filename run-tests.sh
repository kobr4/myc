#!/bin/bash

OUTBIN="./out"

SOURCE="sample.c"
echo "1: $SOURCE"

./main tests/$SOURCE > /dev/null
$OUTBIN
if [ $? = "96" ] 
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

SOURCE="sample3.c"
echo "4: $SOURCE"

./main tests/$SOURCE > /dev/null
$OUTBIN
if [ $? = "7" ] 
then
    echo "PASSED"
else 
    echo "FAILED"    
fi

SOURCE="sample4.c"
echo "4: $SOURCE"

./main tests/$SOURCE > /dev/null
$OUTBIN
if [ $? = "3" ] 
then
    echo "PASSED"
else 
    echo "FAILED"    
fi

SOURCE="sample5.c"
echo "5: $SOURCE"

./main tests/$SOURCE > /dev/null
$OUTBIN
if [ $? = "3" ] 
then
    echo "PASSED"
else 
    echo "FAILED"    
fi

SOURCE="sample6.c"
echo "6: $SOURCE"

./main tests/$SOURCE > /dev/null
$OUTBIN
if [ $? = "10" ] 
then
    echo "PASSED"
else 
    echo "FAILED"    
fi

SOURCE="sample7.c"
echo "7: $SOURCE"

./main tests/$SOURCE > /dev/null
$OUTBIN
if [ $? = "10" ] 
then
    echo "PASSED"
else 
    echo "FAILED"    
fi

SOURCE="sample8.c"
echo "8: $SOURCE"

./main tests/$SOURCE > /dev/null
$OUTBIN
if [ $? = "10" ] 
then
    echo "PASSED"
else 
    echo "FAILED"    
fi