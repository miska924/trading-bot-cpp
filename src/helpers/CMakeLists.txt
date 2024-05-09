add_library(vector_view vector_view.cpp)
add_library(multidim_vector multidim_vector.cpp)
add_library(test_data test_data.cpp)
add_library(http_connection http_connection.cpp)

target_link_libraries(
    http_connection
    ${CURL_LIBRARIES}
)
