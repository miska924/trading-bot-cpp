add_library(tinkoff_client client.cpp)
add_library(tinkoff_client_utils utils.cpp)

target_link_libraries(
  tinkoff_client_utils
  tinkoff_client
)

target_link_libraries(
  tinkoff_client
  http_client
  tinkoff_client_utils
  date_time
)

add_subdirectory(tests)