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
apt-get install -y git libjsoncpp-dev g++ ninja-build libyaml-cpp-dev gnuplot wget unzip uuid-dev zlib1g-dev python3-pip libssl-dev
pip3 install cmake

ln -sf /usr/include/jsoncpp/json/ /usr/include/json

mkdir -p deps
cd deps

export GOOGLE_TEST_VERSION='1.14.0'
wget https://github.com/google/googletest/archive/refs/tags/v${GOOGLE_TEST_VERSION}.zip -O googletest.zip
unzip googletest.zip

export MATPLOTPP_VERSION='1.2.1'
wget https://github.com/alandefreitas/matplotplusplus/archive/refs/tags/v${MATPLOTPP_VERSION}.zip -O matplotplusplus.zip
unzip matplotplusplus.zip

export TRANTOR_VERSION='1.5.19'
wget https://github.com/an-tao/trantor/archive/refs/tags/v${TRANTOR_VERSION}.zip -O trantor.zip
unzip trantor.zip

export DROGON_VERSION='1.9.5'
wget https://github.com/drogonframework/drogon/archive/refs/tags/v${DROGON_VERSION}.zip -O drogon.zip
unzip drogon.zip

cd googletest-${GOOGLE_TEST_VERSION} && mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make install -j8
cd ../..

cd matplotplusplus-${MATPLOTPP_VERSION} && mkdir -p build && cd build
cmake .. -DMATPLOTPP_BUILD_WITH_SANITIZERS=false -DMATPLOTPP_BUILD_EXAMPLES=false
make install -j8
cd ../..

cd trantor-${TRANTOR_VERSION} && mkdir -p build && cd build
cmake ..
make install -j8
cd ../..

cd drogon-${DROGON_VERSION} && mkdir -p build && cd build
cmake .. -DBUILD_EXAMPLES=OFF -DUSE_SUBMODULE=OFF -DBUILD_CTL=OFF -DBUILD_ORM=OFF -DBUILD_BROTLI=OFF -DBUILD_YAML_CONFIG=OFF
make install -j8
cd ../..

cd ..
# rm deps -rf
