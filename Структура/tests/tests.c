/*
Тесты для my_printf и my_scanf

Куковенко Елена Евгеньевна
Группа МК-101
*/

#include "my_stdio.h"
#include <stdio.h>
#include <stdlib.h>

static int test_passed = 0;
static int test_failed = 0;

static void print_test_result(const char* test_name, int passed)
{
    if (passed) {
        printf("PASSED %s\n", test_name);
        test_passed++;
    }
    else {
        printf("FAILED %s\n", test_name);
        test_failed++;
    }
}

// ============================================
// ТЕСТЫ ДЛЯ MY_PRINTF
// ============================================

static void run_printf_tests(void)
{
    printf("\n=== Testing my_printf ===\n\n");

    // Тест 1: Целые числа
    printf("Test 1: Integers\n");
    my_printf("  %%d: %d, %d, %d\n", NULL, 123, -456, 0);
    print_test_result("printf: integers", 1);

    // Тест 2: Беззнаковые
    printf("\nTest 2: Unsigned\n");
    my_printf("  %%u: %u, %u\n", NULL, 123, 3000000000U);
    print_test_result("printf: unsigned", 1);

    // Тест 3: Hex
    printf("\nTest 3: Hex\n");
    my_printf("  %%x: %x, %%X: %X\n", NULL, 0xABCDEF, 0xABCDEF);
    print_test_result("printf: hex", 1);

    // Тест 4: Строки
    printf("\nTest 4: Strings\n");
    my_printf("  %%s: %s, %s\n", NULL, "Hello", "World");
    print_test_result("printf: strings", 1);

    // Тест 5: Символы
    printf("\nTest 5: Characters\n");
    my_printf("  %%c: %c, %c\n", NULL, 'A', 'Z');
    print_test_result("printf: chars", 1);

    // Тест 6: Форматирование
    printf("\nTest 6: Formatting\n");
    my_printf("  Width 8: |%8d|\n", NULL, 42);
    my_printf("  Width 8 left: |%-8d|\n", NULL, 42);
    my_printf("  Zero pad: |%08d|\n", NULL, 42);
    my_printf("  Hex width 2: |%02X|\n", NULL, 0xA);
    print_test_result("printf: formatting", 1);

    // Тест 7: Комбинация
    printf("\nTest 7: Combination\n");
    my_printf("  |%-10s|%08d|%X|\n", NULL, "Hi", 42, 0xABC);
    print_test_result("printf: combination", 1);

    // Тест 8: Percent
    printf("\nTest 8: Percent\n");
    my_printf("  100%% complete\n", NULL);
    print_test_result("printf: percent", 1);

    // Тест 9: Ошибка
    printf("\nTest 9: Error handling\n");
    char* error_pos;
    int result = my_printf("Invalid: %q\n", &error_pos, 123);
    if (result == -1 && error_pos != NULL) {
        my_printf("  Error detected at: %s\n", NULL, error_pos);
        print_test_result("printf: error handling", 1);
    }
    else {
        print_test_result("printf: error handling", 0);
    }
}

// ============================================
// ТЕСТЫ ДЛЯ MY_SCANF (ручные)
// ============================================

static void run_scanf_tests(void)
{
    printf("\n=== Testing my_scanf (manual) ===\n\n");

    int num;
    unsigned int u_num;
    unsigned int hex;
    char str[100];
    char c;
    int ret;

    // Тест 1: Целое число
    printf("Test 1: Read integer\n");
    printf("  Enter an integer (e.g. 123): ");
    ret = my_scanf("%d", NULL, &num);
    my_printf("  Read: %d, bytes: %d\n", NULL, num, ret);
    print_test_result("scanf: integer", 1);

    // Тест 2: Беззнаковое число
    printf("\nTest 2: Read unsigned\n");
    printf("  Enter an unsigned (e.g. 3000000000): ");
    ret = my_scanf("%u", NULL, &u_num);
    my_printf("  Read: %u, bytes: %d\n", NULL, u_num, ret);
    print_test_result("scanf: unsigned", 1);

    // Тест 3: Hex число
    printf("\nTest 3: Read hex\n");
    printf("  Enter a hex number (e.g. FF): ");
    ret = my_scanf("%x", NULL, &hex);
    my_printf("  Read: 0x%X, bytes: %d\n", NULL, hex, ret);
    print_test_result("scanf: hex", 1);

    // Тест 4: Строка
    printf("\nTest 4: Read string\n");
    printf("  Enter a string (e.g. Hello): ");
    ret = my_scanf("%s", NULL, str);
    my_printf("  Read: '%s', bytes: %d\n", NULL, str, ret);
    print_test_result("scanf: string", 1);

    // Тест 5: Символ
    printf("\nTest 5: Read character\n");
    printf("  Enter a character (e.g. A): ");
    ret = my_scanf(" %c", NULL, &c);
    my_printf("  Read: '%c', bytes: %d\n", NULL, c, ret);
    print_test_result("scanf: char", 1);
}

// ============================================
// ОСНОВНАЯ ФУНКЦИЯ
// ============================================

int main(void)
{
    printf("\n========================================\n");
    printf("  TESTING MY_PRINTF AND MY_SCANF\n");
    printf("========================================\n\n");

    // Запуск тестов printf
    run_printf_tests();

    // Запуск тестов scanf (требуют ручного ввода)
    printf("\n========================================\n");
    printf("  SCANF TESTS (manual input required)\n");
    printf("========================================\n");
    run_scanf_tests();

    // Результаты
    printf("\n========================================\n");
    printf("  TEST RESULTS: %d passed, %d failed\n", test_passed, test_failed);
    printf("========================================\n\n");

    if (test_failed == 0) {
        printf("ALL TESTS PASSED!\n\n");
        return 0;
    }
    else {
        printf("SOME TESTS FAILED!\n\n");
        return 1;
    }
}
