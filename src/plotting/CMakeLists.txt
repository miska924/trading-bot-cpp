add_library(plotting plotting.cpp)

target_link_libraries(
  plotting
  matplot
  vector_view
)
