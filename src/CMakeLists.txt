<<<<<<< HEAD
add_subdirectory(strategies)
add_subdirectory(markets)
add_subdirectory(helpers)
add_subdirectory(fitter)
add_subdirectory(features)
add_subdirectory(plotting)
=======
include(FetchContent)

FetchContent_Declare(
  matplotplusplus
  GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus
  GIT_TAG origin/master
)
FetchContent_GetProperties(matplotplusplus)
if(NOT matplotplusplus_POPULATED)
    FetchContent_Populate(matplotplusplus)
    add_subdirectory(${matplotplusplus_SOURCE_DIR} ${matplotplusplus_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_library(autofit_strategy autofit_strategy.cpp)
add_library(backtest_market backtest_market.cpp)
add_library(dummy_market dummy_market.cpp)
add_library(dummy_strategy dummy_strategy.cpp)
add_library(ema_feature ema_feature.cpp)
add_library(feature feature.cpp)
add_library(fitter fitter.cpp)
add_library(macd_strategy macd_strategy.cpp)
add_library(market market.cpp)
add_library(multidim_vector multidim_vector.cpp)
add_library(plotting plotting.cpp)
add_library(strategy strategy.cpp)
add_library(vector_view vector_view.cpp)

target_include_directories(market
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/markets
)
target_include_directories(strategy
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/strategies
)
target_include_directories(dummy_strategy
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/strategies
)
target_include_directories(backtest_market
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/markets
)
target_include_directories(dummy_market
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/markets
)
target_include_directories(feature
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/features
)
target_include_directories(ema_feature
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/features
)
target_include_directories(macd_strategy
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/strategies
)
target_include_directories(plotting
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/plotting
)
target_include_directories(fitter
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/fitter
)
target_include_directories(vector_view
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/helpers
)
target_include_directories(multidim_vector
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include/helpers
)
target_link_libraries(
  market
  vector_view
)
target_link_libraries(
  backtest_market
  vector_view
)
target_link_libraries(
  plotting
  matplot
  vector_view
)
target_link_libraries(
  macd_strategy
  matplot
  vector_view
)
target_link_libraries(
  fitter
  strategy
  backtest_market
  market
  plotting
  matplot
  vector_view
  multidim_vector
)
target_link_libraries(
  autofit_strategy
  strategy
  backtest_market
  market
  plotting
  matplot
  macd_strategy
  ema_feature
  feature
  fitter
  vector_view
  multidim_vector
)
>>>>>>> 33e4c84 (Use average fit around instead of min)
