#!/bin/sh

cd firmware
./makelibs.sh
make clean all doc
cd ../
