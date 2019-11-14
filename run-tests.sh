#!/bin/bash

OUTBIN="./out"

SOURCE="sample.c"
echo "1: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "96" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"
fi

SOURCE="sample2.c"
echo "2: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "11" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"
fi

SOURCE="fibo.c"
echo "3: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "55" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"
fi

SOURCE="sample3.c"
echo "4: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "7" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"
fi

SOURCE="sample4.c"
echo "4: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "3" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"
fi

SOURCE="sample5.c"
echo "5: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "3" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"  
fi

SOURCE="sample6.c"
echo "6: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "10" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"  
fi

SOURCE="sample7.c"
echo "7: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "10" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"
fi

SOURCE="sample8.c"
echo "8: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "10" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"
fi

SOURCE="sample9.c"
echo "9: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "10" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"
fi

SOURCE="sample10.c"
echo "10: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "10" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"
fi

SOURCE="sample11.c"
echo "11: $SOURCE"

./main tests/$SOURCE > /dev/null
COMPIL=$?
$OUTBIN
RUN=$?
if [ $COMPIL = "0" ] && [ $RUN = "25" ] 
then
    echo -e "[\e[92mPASSED\e[0m]"
else 
    echo -e "[\e[31mFAILED\e[0m]"
fi
