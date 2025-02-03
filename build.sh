#! /bin/bash

LIBRRG_PATH="build/c_api/src/rrg/librrg.so"
LIBRRG_RESOURCES_PATH="ui/resources/librrg.so"
LIBRELAY_PATH="build/c_api/src/relay/librelay.so"
LIBRELAY_RESOURCES_PATH="ui/resources/librelay.so"

RRG_LIVE_TEST_PATH="build/c_api/rrg_live_test"
RELAY_LIVE_TEST_PATH="build/c_api/relay_live_test"

mkdir -pv build
cd build
cmake .. || exit 1
make || exit 1

cd ..
cp -v $LIBRRG_PATH $LIBRRG_RESOURCES_PATH
cp -v $LIBRRG_PATH ./librrg.so
cp -v $RRG_LIVE_TEST_PATH ./rrg_live_test

rm -rf build/
