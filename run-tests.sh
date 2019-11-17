#!/bin/bash

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