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

        /* Проверка спецификатора */
        char spec = *current;
        if (spec == '\0') { error_sym = start; goto format_error; }
        if (spec != 'd' && spec != 'u' && spec != 'x' && spec != 'X' &&
            spec != 's' && spec != 'c') {
            error_sym = current;
            goto format_error;
        }
        current++;

        /* Обработка числовых спецификаторов */
        if (spec == 'd' || spec == 'u' || spec == 'x' || spec == 'X') {
            int signed_num = (spec == 'd');
            int base = 10, uppercase = 0;
            if (spec == 'x' || spec == 'X') { base = 16; uppercase = (spec == 'X'); }

            char num_buf[32];
            int num_len = 0, negative = 0;

            /* Получаем значение из аргументов */
            if (signed_num) {
                int val = va_arg(args, int);
                num_len = signed_to_string(val, num_buf);
                if (num_buf[0] == '-') negative = 1;
            }
            else {
                unsigned int val = va_arg(args, unsigned int);
                num_len = unsigned_to_string(val, num_buf, base, uppercase);
            }

            int sign_len = negative ? 1 : 0;
            int digit_len = num_len - sign_len;
            int pad = (width > num_len) ? width - num_len : 0;

            /* Вывод с учётом выравнивания */
            if (left_align) {
                /* Выравнивание влево: число, потом пробелы */
                if (output_string(&ob, num_buf, num_len) != 0) goto write_error;
                for (int i = 0; i < pad; i++) output_char(&ob, ' ');
            }
            else {
                if (zero_pad) {
                    /* Заполнение нулями: знак, нули, цифры */
                    if (negative) output_char(&ob, '-');
                    for (int i = 0; i < pad; i++) output_char(&ob, '0');
                    output_string(&ob, num_buf + sign_len, digit_len);
                }
                else {
                    /* Обычное выравнивание вправо: пробелы, число */
                    for (int i = 0; i < pad; i++) output_char(&ob, ' ');
                    output_string(&ob, num_buf, num_len);
                }
            }
        }
        else if (spec == 's') {
            /* Вывод строки */
            const char* str = va_arg(args, const char*);
            if (str == NULL) str = "(null)";

            int str_len = 0;
            while (str[str_len] != '\0') str_len++;
            int pad = (width > str_len) ? width - str_len : 0;

            if (left_align) {
                if (output_string(&ob, str, str_len) != 0) goto write_error;
                for (int i = 0; i < pad; i++) output_char(&ob, ' ');
            }
            else {
                for (int i = 0; i < pad; i++) output_char(&ob, ' ');
                if (output_string(&ob, str, str_len) != 0) goto write_error;
            }
        }
        else if (spec == 'c') {
            /* Вывод символа */
            char ch = (char)va_arg(args, int);
            int pad = (width > 1) ? width - 1 : 0;

            if (left_align) {
                if (output_char(&ob, ch) != 0) goto write_error;
                for (int i = 0; i < pad; i++) output_char(&ob, ' ');
            }
            else {
                for (int i = 0; i < pad; i++) output_char(&ob, ' ');
                if (output_char(&ob, ch) != 0) goto write_error;
            }
        }
    }

    /* Сброс буфера */
    if (output_flush(&ob) != 0) goto write_error;
    result = ob.total;
    goto finish;

format_error:
    /* Ошибка в форматной строке */
    if (error_pos != NULL) *error_pos = (char*)error_sym;
    result = -1;
    goto finish;

write_error:
    /* Ошибка записи */
    result = -1;
    goto finish;

finish:
    va_end(args);
    return result;
}

// ============================================
// БУФЕР ВВОДА
// ============================================

static HANDLE hConsoleIn = INVALID_HANDLE_VALUE;
static char input_buffer[1024];      // Буфер для ввода
static int input_pos = 0;            // Текущая позиция в буфере
static int input_len = 0;            // Длина данных в буфере
static int input_initialized = 0;    // Флаг инициализации

/* Инициализация ввода */
static void init_input() {
    if (!input_initialized) {
        hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
        SetConsoleMode(hConsoleIn, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
        input_initialized = 1;
    }
}

/* Чтение строки из консоли с поддержкой UTF-8 */
static int read_console_line(char* buffer, int max_len) {
    init_input();

    if (max_len <= 0) return 0;

    wchar_t wide_buf[256];
    DWORD chars_read;

    /* Читаем как UTF-16 */
    if (!ReadConsoleW(hConsoleIn, wide_buf, max_len - 1, &chars_read, NULL)) {
        return -1;
    }

    /* Удаляем \r\n в конце */
    if (chars_read > 0 && wide_buf[chars_read - 1] == L'\n') {
        chars_read--;
        if (chars_read > 0 && wide_buf[chars_read - 1] == L'\r') {
            chars_read--;
        }
    }

    wide_buf[chars_read] = L'\0';

    /* Конвертируем UTF-16 → UTF-8 */
    int len = WideCharToMultiByte(CP_UTF8, 0, wide_buf, -1, buffer, max_len, NULL, NULL);
    if (len <= 0) return -1;

    return len - 1;
}

/* Получение следующего символа из буфера */
static int get_next_char() {
    if (input_pos >= input_len) {
        /* Буфер пуст — читаем новую строку */
        char new_buf[1024];
        int len = read_console_line(new_buf, sizeof(new_buf));
        if (len <= 0) {
            input_len = 0;
            input_pos = 0;
            return EOF;
        }

        for (int i = 0; i < len; i++) {
            input_buffer[i] = new_buf[i];
        }
        input_len = len;
        input_pos = 0;
    }

    if (input_pos >= input_len) {
        return EOF;
    }

    return (unsigned char)input_buffer[input_pos++];
}

/* Возврат одного символа в буфер (ungetc) */
static void unget_char(int c) {
    if (c != EOF && input_pos > 0) {
        input_pos--;
        input_buffer[input_pos] = (char)c;
    }
}

// ============================================
// MY_SCANF
// ============================================

int my_scanf(IN const char* format, OUT char** error_pos, ...) {
    if (format == NULL) {
        if (error_pos != NULL) *error_pos = NULL;
        return -1;
    }

    init_input();

    va_list args;
    va_start(args, error_pos);

    const char* current = format;
    const char* error_sym = NULL;
    int result = -1;
    int total_bytes = 0;
    int spec_index = 0;
    int conversion_error = 0;

    int ch;

    while (*current != '\0' && !conversion_error) {
        /* Обработка обычных символов форматной строки */
        if (*current != '%') {
            if (is_space_ascii(*current)) {
                /* Пробел в формате — пропускаем пробелы во входном потоке */
                while (1) {
                    ch = get_next_char();               
                    if (ch == EOF) {
                        result = total_bytes;
                        goto finish;
                    }
                    if (!is_space_ascii(ch)) {
                        unget_char(ch);
                        break;
                    }
                }
            }
            else {
                /* Обычный символ — должен совпадать */
                int char_len = utf8_char_length((unsigned char)*current);
                for (int i = 0; i < char_len; i++) {
                    ch = get_next_char();
                    if (ch == EOF) {
                        result = total_bytes;
                        goto finish;
                    }
                    if (ch != (unsigned char)current[i]) {
                        unget_char(ch);
                        result = total_bytes;
                        goto finish;
                    }
                    total_bytes++;
                }
                current += char_len - 1;
            }
            current++;
            continue;
        }

        const char* start = current;
        current++;

        /* Обработка %% — ожидаем символ % во входном потоке */
        if (*current == '\0') { error_sym = start; goto format_error; }
        if (*current == '%') {
            ch = get_next_char();
            if (ch == EOF) {
                result = total_bytes;
                goto finish;
            }
            if (ch != '%') {
                unget_char(ch);
                result = total_bytes;
                goto finish;
            }
            total_bytes++;
            current++;
            continue;
        }

        /* Пропускаем флаги (в scanf они не поддерживаются) */
        while (*current == '-' || *current == '0') current++;

        /* Ширина (поддерживается, но не обязательна) */
        int width = 0;
        if (*current == '*') {
            error_sym = current;
            goto format_error;
        }
        if (is_digit_char(*current)) {
            while (is_digit_char(*current)) {
                width = width * 10 + (*current - '0');
                current++;
            }
        }

        /* Проверка спецификатора */
        char spec = *current;
        if (spec == '\0') { error_sym = start; goto format_error; }
        if (spec != 'd' && spec != 'u' && spec != 'x' && spec != 'X' &&
            spec != 's' && spec != 'c') {
            error_sym = current;
            goto format_error;
        }
        current++;

        int success = 0;

        /* Обработка числовых спецификаторов */
        if (spec == 'd' || spec == 'u' || spec == 'x' || spec == 'X') {
            /* Пропускаем пробелы перед числом */
            do {
                ch = get_next_char();
                if (ch == EOF) {
                    result = total_bytes;
                    goto finish;
                }
            } while (is_space_ascii(ch));
            unget_char(ch);

            char num_buf[32];
            int buf_len = 0;
            int had_sign = 0;

            /* Читаем первый символ числа */
            ch = get_next_char();
            if (ch == EOF) {
                result = total_bytes;
                goto finish;
            }

            /* Обрабатываем знак */
            if (ch == '-' || ch == '+') {
                num_buf[buf_len++] = (char)ch;
                had_sign = 1;
                if (width > 0 && buf_len >= width) goto convert_number;
                ch = get_next_char();
                if (ch == EOF) goto convert_number;
            }

            /* Читаем цифры */
            int is_hex = (spec == 'x' || spec == 'X');
            while (1) {
                if (width > 0 && buf_len >= width) break;
                if (is_hex) {
                    if (!((ch >= '0' && ch <= '9') ||
                        (ch >= 'a' && ch <= 'f') ||
                        (ch >= 'A' && ch <= 'F'))) break;
                }
                else {
                    if (!is_digit_char((char)ch)) break;
                }
                num_buf[buf_len++] = (char)ch;
                total_bytes++;
                ch = get_next_char();
                if (ch == EOF) break;
            }
            if (ch != EOF) unget_char(ch);

        convert_number:
            if (buf_len == 0 || (had_sign && buf_len == 1)) {
                result = total_bytes;
                goto finish;
            }

            num_buf[buf_len] = '\0';

            /* Преобразуем строку в число */
            if (spec == 'd') {
                int int_result;
                if (string_to_int(num_buf, buf_len, &int_result) != 0) {
                    conversion_error = 1;
                    spec_index++;
                    break;
                }
                int* ptr = va_arg(args, int*);
                if (ptr != NULL) *ptr = int_result;
            }
            else {
                unsigned int uint_result;
                int conv_result = (spec == 'u')
                    ? string_to_unsigned(num_buf, buf_len, &uint_result)
                    : string_to_hex(num_buf, buf_len, &uint_result);
                if (conv_result != 0) {
                    conversion_error = 1;
                    spec_index++;
                    break;
                }
                unsigned int* ptr = va_arg(args, unsigned int*);
                if (ptr != NULL) *ptr = uint_result;
            }
            success = 1;
        }
        else if (spec == 's') {
            /* Чтение строки */
            char* str = va_arg(args, char*);
            int len = read_console_line(str, 99);

            if (len <= 0) {
                result = total_bytes;
                goto finish;
            }

            total_bytes += len;
            success = 1;
        }
        else if (spec == 'c') {
            /* Чтение символа */
            char* ch_ptr = va_arg(args, char*);
            ch = get_next_char();
            if (ch == EOF) {
                result = total_bytes;
                goto finish;
            }
            if (ch_ptr != NULL) *ch_ptr = (char)ch;
            total_bytes++;
            success = 1;
        }

        if (success) spec_index++;
    }

    if (conversion_error) {
        result = -(spec_index + 1);
        goto finish;
    }
    result = total_bytes;
    goto finish;

format_error:
    /* Ошибка в форматной строке */
    if (error_pos != NULL) *error_pos = (char*)error_sym;
    result = -1;
    goto finish;

finish:
    va_end(args);
    return result;
}
