add_library(backtesting backtesting.cpp)

target_include_directories(backtesting
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)
