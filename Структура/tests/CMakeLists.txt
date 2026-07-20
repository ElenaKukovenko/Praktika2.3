add_executable(test_my_stdio test_printf_scanf.c)
target_link_libraries(test_my_stdio my_stdio)
# enable_testing()
# add_test(NAME test_all COMMAND test_my_stdio)
