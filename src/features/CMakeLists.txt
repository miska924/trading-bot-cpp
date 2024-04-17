add_library(feature feature.cpp)
add_library(ema_feature ema_feature.cpp)

target_link_libraries(
    feature
    market
    vector_view
)

target_link_libraries(
    ema_feature
    feature
    market
    vector_view
)

add_subdirectory(tests)
