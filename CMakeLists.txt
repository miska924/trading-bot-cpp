cmake_minimum_required(VERSION 3.5)
project(TradingBot VERSION 1.0 LANGUAGES CXX)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_ENABLE_INSTALL ON)


include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  matplotplusplus
  GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus
  GIT_TAG origin/master
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_GetProperties(matplotplusplus)
if(NOT matplotplusplus_POPULATED)
    FetchContent_Populate(matplotplusplus)
    add_subdirectory(${matplotplusplus_SOURCE_DIR} ${matplotplusplus_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  gRPC
  GIT_REPOSITORY https://github.com/grpc/grpc
  GIT_TAG        v1.63.0
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
set(gRPC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(
  gRPC
)

set(PROTO_PATH "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/src")
set(Protobuf_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/src")
set(Protobuf_PROTOC_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-build/third_party/protobuf/protoc")
message(STATUS "Protobuf_PROTOC_EXECUTABLE: ${Protobuf_PROTOC_EXECUTABLE}")
set(Protobuf_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-build/third_party/protobuf/src/libprotobuf.a")
set(Protobuf_LIBRARIES "${Protobuf_LIBRARY}")
set(Protobuf_INCLUDE_DIRS "${Protobuf_INCLUDE_DIR}")
set(GRPC_CPP_PLUGIN_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-build/grpc_cpp_plugin")

set(ABSL_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/third_party/abseil-cpp/")
set(GRPC_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/grpc-src/include")


enable_testing()
include(GoogleTest)

include_directories(${ABSL_INCLUDE_DIR})
include_directories(${Protobuf_INCLUDE_DIR})
include_directories(${GRPC_INCLUDE_DIR})
include_directories(src)

add_subdirectory(src)
add_subdirectory(app)


