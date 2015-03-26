#!/bin/sh

cd ..
aclocal
autoheader
autoconf
automake --add-missing

cd build
../configure --enable-debug
cd src/common
make
cd ../protocol
make
cd ../RC
make

cd ../../../scripts
