set(
  PROTO_FILES
  "common.proto"
  "instruments.proto"
  "marketdata.proto"
  "operations.proto"
  "orders.proto"
  "sandbox.proto"
  "stoporders.proto"
  "users.proto"
)

set(PROTO_SRCS "")
set(PROTO_HDRS "")

foreach(file ${PROTO_FILES})
  get_filename_component(file_we ${file} NAME_WE)
  set(grpc_file "${CMAKE_CURRENT_BINARY_DIR}/${file_we}.grpc.pb.cc")
  set(pb_file "${CMAKE_CURRENT_BINARY_DIR}/${file_we}.pb.cc")
  set(pb_hdr "${CMAKE_CURRENT_BINARY_DIR}/${file_we}.pb.h")
  list(APPEND PROTO_SRCS "${grpc_file}" "${pb_file}")
  list(APPEND PROTO_HDRS "${pb_hdr}")
  add_custom_command(
    OUTPUT "${grpc_file}" "${pb_file}" "${pb_hdr}"
    COMMAND "${Protobuf_PROTOC_EXECUTABLE}"
    ARGS "--grpc_out=${CMAKE_CURRENT_BINARY_DIR}"
         "--cpp_out=${CMAKE_CURRENT_BINARY_DIR}"
         "-I${CMAKE_CURRENT_SOURCE_DIR}"
         "-I${Protobuf_INCLUDE_DIR}"
         "-I${ABSL_INCLUDE_DIR}"
         "-I${GRPC_INCLUDE_DIR}"
         "--plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}"
         "${file}"
    DEPENDS "${file}" "${GRPC_CPP_PLUGIN_EXECUTABLE}"
  )
endforeach()

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(tinkoff_contracts STATIC ${PROTO_SRCS} ${PROTO_HDRS})
