#!/bin/bash -x

opts="-Ofast"
topdir=$(pwd)

gcc=$DEVKITARM/bin/arm-none-eabi-gcc
ld=$DEVKITARM/bin/arm-none-eabi-ld

mkdir -p wasm3-build/include
mkdir -p wasm3-build/lib

mkdir -p wasm4-native-build/include
mkdir -p wasm4-native-build/lib

mkdir -p data

# compile WASM3
cd external/wasm3/source
$gcc -c *.c $opts
$ld -r -o libwasm3.a *.o
rm *.o
mv libwasm3.a $topdir/wasm3-build/lib
cp *.h $topdir/wasm3-build/include
cd $topdir

# compile the WASM-4 runtime
cd external/wasm4/runtimes/native/src
$gcc -c *.c backend/wasm_wasm3.c $opts -I$topdir/wasm3-build/include
$ld -r -o libwasm4-native.a *.o
rm *.o
mv libwasm4-native.a $topdir/wasm4-native-build/lib
cp *.h $topdir/wasm4-native-build/include
cd $topdir

# download the platformer test and convert it to a C header
cd data
curl -o game.wasm https://wasm4.org/carts/platformer-test.wasm
xxd -i game.wasm > game.h
cd ..

# compile the NDS rom
make

