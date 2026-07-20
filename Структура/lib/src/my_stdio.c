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

// ============================================
// ПРЕОБРАЗОВАНИЯ ЧИСЕЛ
// ============================================

/* Преобразование беззнакового числа в строку с заданным основанием */
static int unsigned_to_string(unsigned int value, char* buf, int base, int uppercase) {
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    char temp[32];
    int pos = 0;

    if (value == 0) {
        temp[pos++] = '0';
    }
    else {
        while (value > 0) {
            temp[pos++] = digits[value % base];
            value /= base;
        }
    }

    /* Переворачиваем строку */
    for (int i = 0; i < pos; i++) {
        buf[i] = temp[pos - 1 - i];
    }
    return pos;
}

/* Преобразование знакового числа в строку */
static int signed_to_string(int value, char* buf) {
    unsigned int unsigned_val;
    int negative = 0;

    if (value < 0) {
        negative = 1;
        unsigned_val = (unsigned int)(-value);
    }
    else {
        unsigned_val = (unsigned int)value;
    }

    int len = unsigned_to_string(unsigned_val, buf, 10, 0);

    /* Добавляем знак минус в начало */
    if (negative) {
        for (int i = len; i > 0; i--) buf[i] = buf[i - 1];
        buf[0] = '-';
        len++;
    }
    return len;
}

// ============================================
// MY_PRINTF
// ============================================

int my_printf(IN const char* format, OUT char** error_pos, ...) {
    /* Проверка на NULL */
    if (format == NULL) {
        if (error_pos != NULL) *error_pos = NULL;
        return -1;
    }

    va_list args;
    va_start(args, error_pos);

    output_buffer_t ob;
    output_init(&ob, 1);

    const char* current = format;
    const char* error_sym = NULL;
    int result = -1;

    /* Основной цикл по форматной строке */
    while (*current != '\0') {
        /* Обычный символ — выводим как есть */
        if (*current != '%') {
            int char_len = utf8_char_length((unsigned char)*current);
            if (output_string(&ob, current, char_len) != 0) goto write_error;
            current += char_len;
            continue;
        }

        const char* start = current;
        current++;

        /* Обработка %% — вывод процента */
        if (*current == '\0') { error_sym = start; goto format_error; }
        if (*current == '%') {
            if (output_char(&ob, '%') != 0) goto write_error;
            current++;
            continue;
        }

        /* Парсинг флагов: '-' (влево) и '0' (заполнение нулями) */
        int left_align = 0, zero_pad = 0;
        while (*current == '-' || *current == '0') {
            if (*current == '-') left_align = 1;
            else if (*current == '0') zero_pad = 1;
            current++;
        }

        /* Парсинг ширины поля */
        int width = 0;
        if (*current == '*') {
            width = va_arg(args, int);
            if (width < 0) {
                left_align = 1;
                zero_pad = 0;
                width = -width;
            }
            current++;
        }
        else if (is_digit_char(*current)) {
            while (is_digit_char(*current)) {
                width = width * 10 + (*current - '0');
                current++;
            }
        }
