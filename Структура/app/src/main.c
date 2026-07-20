/*
Основная программа

Куковенко Елена Евгеньевна
Группа МК-101
*/

#include "my_stdio.h"
#include <windows.h>
#include <locale.h>

int main(void) {
    // ============================================
    // ЗАГОЛОВОК
    // ============================================
    my_printf("======= Demonstration of my_printf and my_scanf =======\n", NULL);
    my_printf("Hello, world!\n", NULL);

    // ============================================
    // ДЕМОНСТРАЦИЯ ФОРМАТИРОВАНИЯ ЧИСЕЛ
    // ============================================
    my_printf("\n=== Number formatting demonstration ===\n", NULL);
    my_printf("Integer: %d\n", NULL, 42);                              // %d — знаковое целое
    my_printf("Negative integer: %d\n", NULL, -42);                    // %d — отрицательное число
    my_printf("Unsigned integer: %u\n", NULL, 3000000000U);            // %u — беззнаковое целое
    my_printf("Hexadecimal (lowercase): %x\n", NULL, 0xABCDEF);        // %x — шестнадцатеричное (маленькие буквы)
    my_printf("Hexadecimal (uppercase): %X\n", NULL, 0xABCDEF);        // %X — шестнадцатеричное (большие буквы)

    // ============================================
    // ДЕМОНСТРАЦИЯ ФОРМАТИРОВАНИЯ С ШИРИНОЙ
    // ============================================
    my_printf("\n=== Width formatting demonstration ===\n", NULL);
    my_printf("Width 8: '%8d'\n", NULL, 42);                           // Выравнивание по правому краю
    my_printf("Width 8, left align: '%-8d'\n", NULL, 42);              // Выравнивание по левому краю
    my_printf("Width 8, zero padding: '%08d'\n", NULL, 42);            // Заполнение нулями
    my_printf("HEX width 2, zero padding: '%02X'\n", NULL, 0xA);       // HEX с шириной 2
    my_printf("HEX width 8, zero padding: '%08X'\n", NULL, 0xABCD);    // HEX с шириной 8

    // ============================================
    // ДЕМОНСТРАЦИЯ ВЫВОДА СТРОК И СИМВОЛОВ
    // ============================================
    my_printf("\n=== Strings and chars demonstration ===\n", NULL);
    my_printf("String: %*s\n", NULL, 456, "Hello, World!");            // Ширина из аргумента
    my_printf("String width 20: '%20s'\n", NULL, "test");              // Строка с шириной
    my_printf("String width 20, left align: '%-20s'\n", NULL, "test"); // Строка с выравниванием влево
    my_printf("Char width 5: '%5c'\n", NULL, 'X');                     // Символ с шириной
    my_printf("Percent sign: %%\n", NULL);                             // Вывод символа процента

    // ============================================
    // ИНСТРУКЦИЯ ПО ВВОДУ
    // ============================================
    my_printf("\n========== INPUT DEMONSTRATION ==========\n", NULL);
    my_printf("Input examples:\n", NULL);
    my_printf("  - For integer: just enter a number (e.g. 123)\n", NULL);
    my_printf("  - For string: enter text (e.g. Hello)\n", NULL);
    my_printf("  - For HEX: enter hex number (e.g. FF or 1A)\n", NULL);
    my_printf("  - For character: enter one character (e.g. A)\n", NULL);
    my_printf("========================================\n\n", NULL);

    // ============================================
    // ВВОД ЦЕЛОГО ЧИСЛА (%d)
    // ============================================
    my_printf("Enter integer: ", NULL);
    int a;
    int ret = my_scanf("%d", NULL, &a);
    my_printf("You entered: %d, bytes returned: %d\n", NULL, a, ret);

    // ============================================
    // ВВОД БЕЗЗНАКОВОГО ЧИСЛА (%u)
    // ============================================
    my_printf("Enter unsigned integer: ", NULL);
    unsigned int u;
    ret = my_scanf("%u", NULL, &u);
    my_printf("You entered: %u, bytes returned: %d\n", NULL, u, ret);

    // ============================================
    // ВВОД ШЕСТНАДЦАТЕРИЧНОГО ЧИСЛА (%x)
    // ============================================
    my_printf("Enter hexadecimal number: ", NULL);
    unsigned int hex;
    ret = my_scanf("%x", NULL, &hex);
    my_printf("You entered: %X, bytes returned: %d\n", NULL, hex, ret);

    // ============================================
    // ВВОД СТРОКИ (%s)
    // ============================================
    my_printf("Enter string: ", NULL);
    char str[100];
    ret = my_scanf("%s", NULL, str);
    my_printf("You entered: %s, bytes returned: %d\n", NULL, str, ret);

    // ============================================
    // ВВОД СИМВОЛА (%c)
    // ============================================
    my_printf("Enter character: ", NULL);
    char c;
    ret = my_scanf(" %c", NULL, &c);
    my_printf("You entered: %c, bytes returned: %d\n", NULL, c, ret);

    // ============================================
    // ЗАВЕРШЕНИЕ
    // ============================================
    my_printf("\nDone.\n", NULL);

    system("pause");  // Ожидание нажатия клавиши
    return 0;
}
