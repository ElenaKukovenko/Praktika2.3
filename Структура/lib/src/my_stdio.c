/*
my_printf_scanf.c - Реализация my_printf и my_scanf (UTF-8)

Куковенко Елена Евгеньевна
Группа МК-101
*/

#include "my_stdio.h"
#include <limits.h>
#include <windows.h>

#ifndef EOF
#define EOF (-1)
#endif

#include <io.h>
#define WRITE(fd, buf, size) _write(fd, buf, size)

#define BUFFER_SIZE 1024

// ============================================
// БУФЕР ВЫВОДА
// ============================================

typedef struct {
    char buf[BUFFER_SIZE];   // Буфер для накопления вывода
    int  pos;                // Текущая позиция в буфере
    int  total;              // Всего выведено байт
    int  fd;                 // Файловый дескриптор (для fallback)
} output_buffer_t;

/* Инициализация буфера вывода */
static void output_init(output_buffer_t* ob, int fd) {
    ob->pos = 0;
    ob->total = 0;
    ob->fd = fd;
}

/* Сброс буфера — запись накопленных данных в консоль */
static int output_flush(output_buffer_t* ob) {
    if (ob->pos > 0) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written;
        /* Пытаемся записать через Windows API (поддержка UTF-8) */
        if (WriteConsoleA(hConsole, ob->buf, ob->pos, &written, NULL)) {
            ob->total += written;
        }
        else {
            /* Если не вышло — используем _write (для перенаправления в файл) */
            int written_bytes = WRITE(ob->fd, ob->buf, ob->pos);
            if (written_bytes < 0) return -1;
            ob->total += written_bytes;
        }
        ob->pos = 0;
    }
    return 0;
}

/* Вывод одного символа с буферизацией */
static int output_char(output_buffer_t* ob, char c) {
    ob->buf[ob->pos++] = c;
    if (ob->pos >= BUFFER_SIZE) {
        if (output_flush(ob) != 0) return -1;
    }
    return 0;
}

/* Вывод строки заданной длины */
static int output_string(output_buffer_t* ob, const char* s, int len) {
    for (int i = 0; i < len; i++) {
        if (output_char(ob, s[i]) != 0) return -1;
    }
    return 0;
}
// ============================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ UTF-8
// ============================================

/* Определение длины UTF-8 символа по первому байту */
static int utf8_char_length(unsigned char c) {
    if (c < 0x80) return 1;           // 0xxxxxxx — 1 байт
    if ((c & 0xE0) == 0xC0) return 2;  // 110xxxxx — 2 байта
    if ((c & 0xF0) == 0xE0) return 3;  // 1110xxxx — 3 байта
    if ((c & 0xF8) == 0xF0) return 4;  // 11110xxx — 4 байта
    return 1;
}

/* Проверка, является ли символ пробельным (только ASCII) */
static int is_space_ascii(char c) {
    return (c == ' ' || c == '\t' || c == '\n' ||
        c == '\r' || c == '\v' || c == '\f');
}

/* Проверка, является ли символ десятичной цифрой */
static int is_digit_char(char c) {
    return (c >= '0' && c <= '9');
}
