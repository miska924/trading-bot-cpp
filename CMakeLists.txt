cmake_minimum_required(VERSION 3.27)
project(TradingBot VERSION 1.0 LANGUAGES CXX)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем путь к заголовочным файлам
include_directories(include)

# Добавляем исходный код
add_subdirectory(src)

# Подключаем GoogleTest
enable_testing()
add_subdirectory(tests)

# Добавляем подкаталог app, где находится main.cpp
add_subdirectory(app)
