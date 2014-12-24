#include "libtoa.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

enum libtoa_format_flag {
    /* length */
    HALF = 1 << 0,
    HALF_HALF = 1 << 1,
    LONG = 1 << 2,
    LONG_LONG = 1 << 3,
    LONG_DOUBLE = 1 << 4,

    /* flag */
    SHARP = 1 << 5,
    SPACE = 1 << 6,
    ZERO = 1 << 7,
    PLUS = 1 << 8,

    /* have precision */
    PRECISION = 1 << 9,

    /* print lower case charctor */
    LOWER_CASE = 1 << 10,

    /* type */
    PRINT_HEX = 1 << 11,
    PRINT_INT = 1 << 12,
    PRINT_UINT = 1 << 13,
    PRINT_POINTER = 1 << 14,
    PRINT_STRING = 1 << 15,
    PRINT_CHAR = 1 << 16,
    PRINT_DOUBLE_E = 1 << 17,
    PRINT_DOUBLE_F = 1 << 18,
    PRINT_DOUBLE_G = 1 << 19,
    PRINT_DOUBLE_HEX = 1 << 20,
    PRINT_PERCENT = 1 << 21,
};

#define IS_FLAG(v, flag) (((v)&flag) == flag)

// #define IS_SHARP(flag) IS_FLAG(flag, SHARP)
// #define IS_SPACE(flag) IS_FLAG(flag, SPACE)
// #define IS_PLUS(flag) IS_FLAG(flag, PLUS)
#define IS_ZERO(flag) IS_FLAG(flag, ZERO)
// #define IS_PRECISION(flag) IS_FLAG(flag, PRECISION)
// #define IS_LOWERCASE(flag) IS_FLAG(flag, LOWER_CASE)
// #define IS_HALF(flag) IS_FLAG(flag, HALF)
#define IS_HALFHALF(flag) IS_FLAG(flag, HALF_HALF)
// #define IS_LONG(flag) IS_FLAG(flag, LONG)
#define IS_LONGLONG(flag) IS_FLAG(flag, LONG_LONG)
// #define IS_LONGDOUBLE(flag) IS_FLAG(flag, LONG_DOUBLE)
//
// #define IS_SINT(flag) IS_FLAG(flag, PRINT_INT)
// #define IS_UINT(flag) IS_FLAG(flag, PRINT_UINT)
// #define IS_HEX(flag) IS_FLAG(flag, PRINT_HEX)
// #define IS_PERCENT(flag) IS_FLAG(flag, PRINT_PERCENT)
// #define IS_POINTER(flag) IS_FLAG(flag, PRINT_POINTER)
// #define IS_STRING(flag) IS_FLAG(flag, PRINT_STRING)
// #define IS_CHAR(flag) IS_FLAG(flag, PRINT_CHAR)
// #define IS_DOUBLE_E(flag) IS_FLAG(flag, PRINT_DOUBLE_E)
// #define IS_DOUBLE_F(flag) IS_FLAG(flag, PRINT_DOUBLE_F)
// #define IS_DOUBLE_G(flag) IS_FLAG(flag, PRINT_DOUBLE_G)
// #define IS_DOUBLE_A(flag) IS_FLAG(flag, PRINT_DOUBLE_HEX)

static int is_digit(char c)
{
    return '0' <= c && c <= '9';
}

static int is_flag(char c)
{
    switch (c) {
    case '+':
    case ' ':
    case '#':
    case '0':
        return 1;
    }
    return 0;
}

static int libtoa_vsnprintf_core(char *buffer, size_t size, const char *fmt, va_list ap)
{
    char c;
    int idx = 0;
    int len = 0;
    for (;;) {
        unsigned flag = 0;
        int width = 0;
        int precision = 0;
        const char *prev = fmt;
        /* skip until '%' */
        while ((c = *fmt++) != '\0') {
            if (c == '%')
                break;
        }
        if (prev + 1 != fmt) {
            len = put_string_const(buffer, size, prev, fmt - prev - 1);
            buffer += len;
            size -= len;
        }
        if (c == '\0')
            break;
        if ((c = *fmt++) == '\0')
            break;
        /* parse format place holder */
        /* step 1. parse parameter
         * syntax:: parameter -> Digits '$'
         * */
        if (is_digit(c)) {
            char last = c;
            prev = fmt;
            idx = c - '0';
            while ((c = *fmt++) != '\0') {
                if (!is_digit(c))
                    break;
                idx = idx * 10 + c - '0';
            }
            if (c != '$') {
                // parse failed.
                idx = 0;
                fmt = prev;
                c = last;
            }
            if (c == '\0')
                break;
        }
        /* step 2. parse flag
         * syntax:: flag -> [+ #0]*
         */
        if (is_flag(c)) {
            do {
                switch (c) {
                case '+':
                    flag |= PLUS;
                    break;
                case ' ':
                    flag |= SPACE;
                    break;
                case '#':
                    flag |= SHARP;
                    break;
                case '0':
                    flag |= ZERO;
                    break;
                default:
                    goto finish_parse_flag;
                }
            } while ((c = *fmt++) != '\0');
        finish_parse_flag:
            if (c == '\0')
                break;
        }
        /* step 3. parse width
         * syntax:: width -> [0-9]+
         */
        if (is_digit(c)) {
            width = c - '0';
            while ((c = *fmt++) != '\0') {
                if (!is_digit(c))
                    break;
                width = width * 10 + c - '0';
            }
            if (c == '\0')
                break;
        }
        /* step 4. parse precision
         * syntax:: precision -> '.' ('*' / [0-9]+)
         */
        if (c == '.') {
            precision = c;
            c = *fmt++;
            if (c == '*') {
                len = put_string2(buffer, size, width, IS_ZERO(flag), ap);
                buffer += len;
                size -= len;
                continue;
            }
            else if (is_digit(c)) {
                precision = c - '0';
                flag |= PRECISION;
                while ((c = *fmt++) != '\0') {
                    if (!is_digit(c))
                        break;
                    idx = idx * 10 + c - '0';
                }
            }
            if (c != '\0')
                break;
        }
        /* 5. parse length and parse type
         * syntax:: length -> 'hh' / 'h' / 'll' / 'l' / 'L' / 'z' / 'j' / 't'
         * syntax:: type -> [diufFeEgGxXscpaAn%]
         */
        switch (c) {
        case 'h':
            /* parse h(h?)[diuxXc] */
            c = *fmt++;
            if (c == 'h') {
                c = *fmt++;
                flag |= HALF_HALF;
            }
            else {
                flag |= HALF;
            }
            switch (c) {
            case 'd':
            case 'i':
                if (IS_HALFHALF(flag))
                    len = put_char_i(buffer, size, width, IS_ZERO(flag), ap);
                else
                    len = put_short_i(buffer, size, width, IS_ZERO(flag), ap);
                break;
            case 'u':
                if (IS_HALFHALF(flag))
                    len = put_char_u(buffer, size, width, IS_ZERO(flag), ap);
                else
                    len = put_short_u(buffer, size, width, IS_ZERO(flag), ap);
                break;
            case 'x':
                if (IS_HALFHALF(flag))
                    len = put_hex_char_lower(buffer, size, width, IS_ZERO(flag), ap);
                else
                    len = put_hex_short_lower(buffer, size, width, IS_ZERO(flag), ap);
                break;
            case 'X':
                if (IS_HALFHALF(flag))
                    len = put_hex_char_upper(buffer, size, width, IS_ZERO(flag), ap);
                else
                    len = put_hex_short_upper(buffer, size, width, IS_ZERO(flag), ap);
                break;
            }
            break;
        case 'l':
            /* parse l(l?)[diuxXc] */
            c = *fmt++;
            if (c == 'l') {
                c = *fmt++;
                flag |= LONG_LONG;
            }
            else {
                flag |= LONG;
            }
            switch (c) {
            case 'd':
            case 'i':
                if (IS_LONGLONG(flag))
                    len = put_long_long_i(buffer, size, width, IS_ZERO(flag), ap);
                else
                    len = put_long_i(buffer, size, width, IS_ZERO(flag), ap);
                break;
            case 'u':
                if (IS_LONGLONG(flag))
                    len = put_long_long_u(buffer, size, width, IS_ZERO(flag), ap);
                else
                    len = put_long_u(buffer, size, width, IS_ZERO(flag), ap);
                break;
            case 'x':
                if (IS_LONGLONG(flag))
                    len = put_hex_long_long_lower(buffer, size, width, IS_ZERO(flag), ap);
                else
                    len = put_hex_long_lower(buffer, size, width, IS_ZERO(flag), ap);
                break;
            case 'X':
                if (IS_LONGLONG(flag))
                    len = put_hex_long_long_upper(buffer, size, width, IS_ZERO(flag), ap);
                else
                    len = put_hex_long_upper(buffer, size, width, IS_ZERO(flag), ap);
                break;
            }
            break;
        case 'p':
            len = put_pointer(buffer, size, width, IS_ZERO(flag), ap);
            break;
        case 'c':
            len = put_char(buffer, size, ap);
            break;
        case 's':
            len = put_string(buffer, size, width, IS_ZERO(flag), ap);
            break;
        case '%':
            len = put_string_const(buffer, size, "%", 1);
            break;
        case 'L':
            flag |= LONG_DOUBLE;
            return -1;
        case 'e':
        case 'E':
        case 'f':
        case 'F':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
            return -1;
        case 'd':
        case 'i':
            len = put_int_i(buffer, size, width, IS_ZERO(flag), ap);
            break;
        case 'u':
            len = put_int_u(buffer, size, width, IS_ZERO(flag), ap);
            break;
        case 'x':
            len = put_hex_int_lower(buffer, size, width, IS_ZERO(flag), ap);
            break;
        case 'X':
            len = put_hex_int_upper(buffer, size, width, IS_ZERO(flag), ap);
            break;
        case 'n':
        default:
            // error : syntax-error or unsupported-formatter
            return -1;
        }
        buffer += len;
        size -= len;
    }
    return len;
}

int libtoa_snprintf(char *__restrict str, size_t size, const char *__restrict fmt, ...)
{
    int ret = 0;
    // toa_bytecode_t code = {};
    va_list ap;
    va_start(ap, fmt);
    ret = libtoa_vsnprintf_core(str, size, fmt, ap);
    va_end(ap);
    return ret;
}
