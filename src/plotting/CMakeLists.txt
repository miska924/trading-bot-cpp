add_library(plotting plotting.cpp)

target_link_libraries(
  plotting
  PRIVATE
  Matplot++::matplot
  vector_view
)
