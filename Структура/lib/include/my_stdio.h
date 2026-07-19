/*
Заголовочный файл с объявлениями my_printf и my_scanf

Куковенко Елена Евгеньевна
МК 101
*/

#ifndef MY_PRINTF_SCANF_H
#define MY_PRINTF_SCANF_H

#include <stdarg.h>   // Для va_list, va_start, va_arg, va_end

// Направление параметров
#define IN
#define OUT

int my_printf(IN const char* format, OUT char** p, ...);
int my_scanf(IN const char* format, OUT char** p, ...);

#endif
