add_library(trader trader.cpp)
add_library(simple_trader simple_trader.cpp)
add_library(sltp_trader sltp_trader.cpp)

target_link_libraries(
    trader
)

target_link_libraries(
    simple_trader
    PRIVATE
    trader
)

target_link_libraries(
    sltp_trader
    PRIVATE
    trader
    atr_feature
)

add_subdirectory(tests)
