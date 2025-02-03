#! /bin/bash

mkdir -pv build
cd build
cmake ..
make

cd ..
cp -v build/c_api/src/librrg.so ui/resources/librrg.so

rm -rf build/*
