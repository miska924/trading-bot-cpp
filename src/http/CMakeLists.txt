add_library(http_client client.cpp)

target_link_libraries(
  http_client
  drogon
  jsoncpp
  trantor
  yaml-cpp
  z
  uuid
  ssl
  crypto
)

add_subdirectory(tests)
