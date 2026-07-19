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
