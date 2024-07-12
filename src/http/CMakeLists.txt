add_library(http_client client.cpp)

target_link_libraries(
  http_client
  cpr
  jsoncpp
)

add_subdirectory(tests)
