add_library(tinkoff_api tinkoff_api.cpp)

target_link_libraries(
  tinkoff_api
  vector_view
)

add_subdirectory(tests)
add_subdirectory(contracts)
