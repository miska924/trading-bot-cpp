add_library(trader trader.cpp)
add_library(simple_trader simple_trader.cpp)

target_link_libraries(
    trader
)

target_link_libraries(
    simple_trader
    PRIVATE
    trader
)

add_subdirectory(tests)
