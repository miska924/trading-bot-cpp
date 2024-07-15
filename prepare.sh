#!/bin/bash

set -e

rm -rf deps
rm -rf build

if [[ $UID != 0 ]]; then
    echo "Please run this script with sudo:"
    echo "sudo $0 $*"
    exit 1
fi

apt-get update
apt-get install -y cmake libcurl4-openssl-dev libjsoncpp-dev g++ wget unzip libssl-dev
# pip3 install cmake

ln -sf /usr/include/jsoncpp/json/ /usr/include/json

mkdir -p deps
cd deps

export GOOGLE_TEST_VERSION='1.14.0'
wget https://github.com/google/googletest/archive/refs/tags/v${GOOGLE_TEST_VERSION}.zip -O googletest.zip
unzip googletest.zip

export MATPLOTPP_VERSION='1.2.1'
wget https://github.com/alandefreitas/matplotplusplus/archive/refs/tags/v${MATPLOTPP_VERSION}.zip -O matplotplusplus.zip
unzip matplotplusplus.zip

export CPR_VERSION='1.10.5'
wget https://github.com/libcpr/cpr/archive/refs/tags/${CPR_VERSION}.zip -O cpr.zip
unzip cpr.zip

cd googletest-${GOOGLE_TEST_VERSION} && mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make install -j8
cd ../..

cd matplotplusplus-${MATPLOTPP_VERSION} && mkdir -p build && cd build
cmake .. -DMATPLOTPP_BUILD_WITH_SANITIZERS=false -DMATPLOTPP_BUILD_EXAMPLES=false
make install -j8
cd ../..

cd cpr-${CPR_VERSION} && mkdir -p build && cd build
cmake .. -DCPR_USE_SYSTEM_CURL=ON
make install -j8
cd ../..

cd ..
# rm deps -rf
