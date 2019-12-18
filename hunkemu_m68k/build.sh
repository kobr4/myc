#!/bin/bash

if [[ -f Musashi ]]
then
    echo "Musashi not found, cloning GIT repo"
    git clone git@github.com:kstenerud/Musashi.git
fi
cd Musashi
make
cd ..
gcc Musashi/m68kcpu.o Musashi/m68kops.o Musashi/softfloat/softfloat.o hunkemu.c -o hunkemu
cp ./hunkemu ..