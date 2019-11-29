#!/bin/bash

gcc -D X86 main.c -o main

OUTBIN="./out"

SOURCE="sample.c"
ITEM="1: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "96" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample2.c"
ITEM="2: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "11" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="fibo.c"
ITEM="3: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "55" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample3.c"
ITEM="4: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "10" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample4.c"
ITEM="4: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "3" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample5.c"
ITEM="5: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "3" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"  
fi

SOURCE="sample6.c"
ITEM="6: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "20" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"  
fi

SOURCE="sample7.c"
ITEM="7: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "10" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample8.c"
ITEM="8: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "10" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample9.c"
ITEM="9: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "10" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample10.c"
ITEM="10: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "10" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample11.c"
ITEM="11: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "25" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample12.c"
ITEM="12: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "96" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample13.c"
ITEM="13: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "50" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample14.c"
ITEM="14: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "52" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample15.c"
ITEM="15: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "51" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample16.c"
ITEM="16: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "52" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample17.c"
ITEM="17: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "4" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample18.c"
ITEM="18: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "16" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample19.c"
ITEM="19: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "76" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample20.c"
ITEM="20: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "76" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample21.c"
ITEM="21: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "5" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample22.c"
ITEM="22: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "7" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample23.c"
ITEM="23: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "1" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi

SOURCE="sample24.c"
ITEM="24: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "8" ] 
then
    echo -e "$ITEM [\e[92mPASSED\e[0m]"
else 
    echo -e "$ITEM [\e[31mFAILED\e[0m]"
fi