# Библиотека my_stdio
add_library(my_stdio STATIC src/my_stdio.c)
target_include_directories(my_stdio PUBLIC include)

# Для динамической библиотеки
add_library(my_stdio_shared SHARED src/my_stdio.c)
target_include_directories(my_stdio_shared PUBLIC include)

# Для Windows нужно подключить kernel32
target_link_libraries(my_stdio kernel32)
target_link_libraries(my_stdio_shared kernel32)
