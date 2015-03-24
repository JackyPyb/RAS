#!/bin/sh

cd ..
aclocal
autoheader
autoconf
automake --add-missing

cd build
../configure
cd src/common
make
cd ../protocol
make
cd ../AL
make

cd ../../../scripts
