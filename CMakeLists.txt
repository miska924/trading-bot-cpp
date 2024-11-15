cmake_minimum_required(VERSION 3.22)
project(TradingBot VERSION 1.0 LANGUAGES CXX)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

find_package(Matplot++ REQUIRED)
# find_package(yaml-cpp REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(GTest REQUIRED)
# find_package(ZLIB REQUIRED)
# find_package(Trantor REQUIRED)
# find_package(OpenSSL REQUIRED)
find_package(cpr REQUIRED)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sanitize=address -fno-sanitize=undefined")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fno-sanitize=address -fno-sanitize=undefined")

# set(ENV{LD_LIBRARY_PATH} "/usr/local/lib:$ENV{LD_LIBRARY_PATH}")

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
enable_testing()
include(GoogleTest)
include(CTest)

include_directories(src)

add_subdirectory(src)
add_subdirectory(app)

