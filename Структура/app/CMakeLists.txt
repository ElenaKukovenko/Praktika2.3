# Исходные файлы приложения
set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

# Создаём исполняемый файл
add_executable(my_stdiio ${APP_SOURCES})

# Связываем с библиотекой
target_link_libraries(my_stdiio my_stdio)

# Указываем папку с заголовками
target_include_directories(my_stdiio PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/include
)

# Устанавливаем выходную папку
set_target_properties(my_stdiio PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/add
)
