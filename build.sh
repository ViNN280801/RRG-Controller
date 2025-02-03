#! /bin/bash

mkdir -pv build
cd build
cmake ..
make

cd ..
cp -v build/c_api/src/librrg.so ui/resources/librrg.so
cp -v build/c_api/src/librrg.so ./librrg.so
cp -v build/c_api/rrg_test ./rrg_test

# rm -rf build/
