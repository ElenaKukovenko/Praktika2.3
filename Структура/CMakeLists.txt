cmake_minimum_required(VERSION 3.10)
project(Praktika2_3)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Добавляем поддиректории (ВАЖНО: lib ПЕРВЫМ!)
add_subdirectory(lib)
add_subdirectory(add)
add_subdirectory(tests)

# Включаем тестирование
enable_testing()

# Информация
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
