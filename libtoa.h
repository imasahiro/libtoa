#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIBTOA_LIBTOA_H
#define LIBTOA_LIBTOA_H

static inline int libtoa_get_hex_length(uint64_t val)
{
    if (val) {
        int bits = sizeof(long long) * 8 - __builtin_clzll(val);
        return (bits + 3) >> 2;
    }
    else {
        return 1;
    }
}

static inline int libtoa_put_hex_core(char *buffer, int bufsiz, uint64_t val, const char chars[])
{
    int orig_len, len;
    orig_len = len = libtoa_get_hex_length(val);
    if (len > bufsiz) {
        int n = len - bufsiz;
        val >>= n * 4;
        len -= n;
    }
    len *= 4;
    do {
        len -= 4;
        *buffer++ = chars[(val >> len) & 0xf];
    } while (len != 0);
    return orig_len;
}

static inline int libtoa_put_hex64_lower(char *buffer, int bufsiz, uint64_t val)
{
    static const char chars[] = "0123456789abcdef";
    return libtoa_put_hex_core(buffer, bufsiz, val, chars);
}

static inline int libtoa_put_hex8_lower(char *buffer, int bufsiz, uint8_t val)
{
    return libtoa_put_hex64_lower(buffer, bufsiz, (uint64_t)val);
}

static inline int libtoa_put_hex16_lower(char *buffer, int bufsiz, uint16_t val)
{
    return libtoa_put_hex64_lower(buffer, bufsiz, (uint64_t)val);
}

static inline int libtoa_put_hex32_lower(char *buffer, int bufsiz, uint32_t val)
{
    return libtoa_put_hex64_lower(buffer, bufsiz, (uint64_t)val);
}

static inline int libtoa_put_hex64_upper(char *buffer, int bufsiz, uint64_t val)
{
    static const char chars[] = "0123456789ABCDEF";
    return libtoa_put_hex_core(buffer, bufsiz, val, chars);
}

static inline int libtoa_put_hex8_upper(char *buffer, int bufsiz, uint8_t val)
{
    return libtoa_put_hex64_upper(buffer, bufsiz, (uint64_t)val);
}

static inline int libtoa_put_hex16_upper(char *buffer, int bufsiz, uint16_t val)
{
    return libtoa_put_hex64_upper(buffer, bufsiz, (uint64_t)val);
}

static inline int libtoa_put_hex32_upper(char *buffer, int bufsiz, uint32_t val)
{
    return libtoa_put_hex64_upper(buffer, bufsiz, (uint64_t)val);
}

static inline const uint64_t *libtoa_get_pow_of_10_table(void)
{
    static const uint64_t table[] = {
        1ULL,
        10ULL,
        100ULL,
        1000ULL,
        10000ULL,
        100000ULL,
        1000000ULL,
        10000000ULL,
        100000000ULL,
        1000000000ULL,
        10000000000ULL,
        100000000000ULL,
        1000000000000ULL,
        10000000000000ULL,
        100000000000000ULL,
        1000000000000000ULL,
        10000000000000000ULL,
        100000000000000000ULL,
        1000000000000000000ULL,
        10000000000000000000ULL,
        UINT64_MAX,
    };
    return table;
}

static const char *libtoa_get_digit_table(void)
{
    // Lookup table of digit pair for division/modulo of 0-99.
    // http://www.slideshare.net/andreialexandrescu1/three-optimization-tips-for-c-15708507
    static const char digits99[] = {
        "00010203040506070809"
        "10111213141516171819"
        "20212223242526272829"
        "30313233343536373839"
        "40414243444546474849"
        "50515253545556575859"
        "60616263646566676869"
        "70717273747576777879"
        "80818283848586878889"
        "90919293949596979899"
    };
    return digits99;
}

static unsigned libtoa_log10u64(uint64_t v)
{
    /* from http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10 */
    const uint64_t *pow10table = libtoa_get_pow_of_10_table();
    unsigned t, ilog2_;
    ilog2_ = (unsigned)((sizeof(uint64_t) * 8) - __builtin_clzll((v)-1));
    t = ((ilog2_ + 1) * 1233) / 4096;
    return t + (v >= pow10table[t]);
}

#if LIBTOA_UNROLL_INT_TO_ASCII
static int libtoa_put_int_core2(char *buffer, uint64_t val)
{
    int len = 0;
    // val = aaaabbbbccccddddeeee
    if (val <= 99999999) { // val = ddddeeee
        if (val <= 9999) {
            // val = eeee
            unsigned e1 = (val / 100) * 2;
            unsigned e2 = (val % 100) * 2;
            if (val >= 1000) {
                *buffer++ = digits99[e1 + 0];
            }
            if (val >= 100) {
                *buffer++ = digits99[e1 + 1];
            }
            if (val >= 10) {
                *buffer++ = digits99[e2 + 0];
            }
            *buffer++ = digits99[e2 + 1];
        }
        else {
            // val = ddddeeee
            unsigned d = (val / 10000);
            unsigned e = (val % 10000);
            unsigned d1 = (d / 100) * 2;
            unsigned d2 = (d % 100) * 2;
            unsigned e1 = (e / 100) * 2;
            unsigned e2 = (e % 100) * 2;
            if (d >= 1000) {
                *buffer++ = digits99[d1 + 0];
            }
            if (d >= 100) {
                *buffer++ = digits99[d1 + 1];
            }
            if (d >= 10) {
                *buffer++ = digits99[d2 + 0];
            }
            *buffer++ = digits99[d2 + 1];
            *buffer++ = digits99[e1 + 0];
            *buffer++ = digits99[e1 + 1];
            *buffer++ = digits99[e2 + 0];
            *buffer++ = digits99[e2 + 1];
        }
    }
    else if (val <= 9999999999999999) {
        // val = bbbbccccddddeeee
        unsigned bc = val / 100000000;
        unsigned de = val % 100000000;
        unsigned b = bc / 10000;
        unsigned c = bc % 10000;
        unsigned d = de / 10000;
        unsigned e = de % 10000;

        unsigned b1 = (b / 100) * 2;
        unsigned b2 = (b % 100) * 2;
        unsigned c1 = (c / 100) * 2;
        unsigned c2 = (c % 100) * 2;
        unsigned d1 = (d / 100) * 2;
        unsigned d2 = (d % 100) * 2;
        unsigned e1 = (e / 100) * 2;
        unsigned e2 = (e % 100) * 2;
        if (bc >= 100000000) {
            *buffer++ = digits99[b1 + 0];
        }
        if (bc >= 10000000) {
            *buffer++ = digits99[b1 + 1];
        }
        if (bc >= 1000000) {
            *buffer++ = digits99[b2 + 0];
        }
        if (bc >= 100000) {
            *buffer++ = digits99[b2 + 1];
        }
        if (bc >= 10000) {
            *buffer++ = digits99[c1 + 0];
        }
        if (bc >= 1000) {
            *buffer++ = digits99[c1 + 1];
        }
        if (bc >= 100) {
            *buffer++ = digits99[c2 + 0];
        }
        if (bc >= 10) {
            *buffer++ = digits99[c2 + 1];
        }
        *buffer++ = digits99[d1 + 0];
        *buffer++ = digits99[d1 + 1];
        *buffer++ = digits99[d2 + 0];
        *buffer++ = digits99[d2 + 1];
        *buffer++ = digits99[e1 + 0];
        *buffer++ = digits99[e1 + 1];
        *buffer++ = digits99[e2 + 0];
        *buffer++ = digits99[e2 + 1];
    }
    else {
        // val = aaaabbbbccccddddeeee
        unsigned a = val / 10000000000000000;
        uint64_t bcde = val % 10000000000000000;
        unsigned a1 = (a / 100) * 2;
        unsigned a2 = (a % 100) * 2;
        uint64_t bc = bcde / 100000000;
        uint64_t de = bcde % 100000000;
        unsigned b = bc / 10000;
        unsigned c = bc % 10000;
        unsigned d = de / 10000;
        unsigned e = de % 10000;

        unsigned b1 = (b / 100) * 2;
        unsigned b2 = (b % 100) * 2;
        unsigned c1 = (c / 100) * 2;
        unsigned c2 = (c % 100) * 2;
        unsigned d1 = (d / 100) * 2;
        unsigned d2 = (d % 100) * 2;
        unsigned e1 = (e / 100) * 2;
        unsigned e2 = (e % 100) * 2;

        if (a >= 1000) {
            *buffer++ = digits99[a1];
        }
        if (a >= 100) {
            *buffer++ = digits99[a1 + 1];
        }
        if (a >= 10) {
            *buffer++ = digits99[a2];
        }
        *buffer++ = digits99[a2 + 1];
        *buffer++ = digits99[b1 + 0];
        *buffer++ = digits99[b1 + 1];
        *buffer++ = digits99[b2 + 0];
        *buffer++ = digits99[b2 + 1];
        *buffer++ = digits99[c1 + 0];
        *buffer++ = digits99[c1 + 1];
        *buffer++ = digits99[c2 + 0];
        *buffer++ = digits99[c2 + 1];
        *buffer++ = digits99[d1 + 0];
        *buffer++ = digits99[d1 + 1];
        *buffer++ = digits99[d2 + 0];
        *buffer++ = digits99[d2 + 1];
        *buffer++ = digits99[e1 + 0];
        *buffer++ = digits99[e1 + 1];
        *buffer++ = digits99[e2 + 0];
        *buffer++ = digits99[e2 + 1];
    }
    return len;
}
#endif

static inline int libtoa_put_int_core(char *buffer, int bufsiz, int neg, uint64_t val)
{
    const char *digits = libtoa_get_digit_table();
    char *p;
    int len = ((val != 0) ? libtoa_log10u64(val) : 1);
    int rest = 0;
    if (neg && bufsiz > 1) {
        bufsiz -= 1;
        *buffer++ = '-';
    }
    if (len > bufsiz) {
        rest = len - bufsiz;
        len -= rest;
        val /= libtoa_get_pow_of_10_table()[rest];
    }

    p = buffer + len;
    while (val >= 100) {
        unsigned idx = val % 100 * 2;
        *--p = digits[idx + 1];
        *--p = digits[idx];
        val /= 100;
    }
    if (val < 10) {
        *--p = '0' + val;
    }
    else {
        *--p = digits[val * 2 + 1];
        *--p = digits[val * 2];
    }
    return neg + len + rest;
}

static inline int libtoa_put_int8(char *buffer, int bufsiz, int8_t val)
{
    uint8_t v = val >= 0 ? val : -(uint8_t)(val);
    return libtoa_put_int_core(buffer, bufsiz, val < 0, v);
}

static inline int libtoa_put_uint8(char *buffer, int bufsiz, uint8_t val)
{
    return libtoa_put_int_core(buffer, bufsiz, 0, (uint64_t)val);
}

static inline int libtoa_put_int16(char *buffer, int bufsiz, int16_t val)
{
    uint16_t v = val >= 0 ? val : -(uint16_t)(val);
    return libtoa_put_int_core(buffer, bufsiz, val < 0, v);
}

static inline int libtoa_put_uint16(char *buffer, int bufsiz, uint16_t val)
{
    return libtoa_put_int_core(buffer, bufsiz, 0, (uint64_t)val);
}

static inline int libtoa_put_int32(char *buffer, int bufsiz, int32_t val)
{
    uint32_t v = val >= 0 ? val : -(uint32_t)(val);
    return libtoa_put_int_core(buffer, bufsiz, val < 0, v);
}

static inline int libtoa_put_uint32(char *buffer, int bufsiz, uint32_t val)
{
    return libtoa_put_int_core(buffer, bufsiz, 0, (uint64_t)val);
}

static inline int libtoa_put_int64(char *buffer, int bufsiz, int64_t val)
{
    uint64_t v = val >= 0 ? val : -(uint64_t)(val);
    return libtoa_put_int_core(buffer, bufsiz, val < 0, v);
}

static inline int libtoa_put_uint64(char *buffer, int bufsiz, uint64_t val)
{
    return libtoa_put_int_core(buffer, bufsiz, 0, val);
}

static inline int libtoa_put_string(char *buffer, int bufsiz, char *str, int len)
{
    int i;
    if (len > bufsiz) {
        len = bufsiz;
    }
    for (i = 0; *str && i < len; ++i) {
        *buffer++ = *str++;
    }
    return i;
}

static inline int libtoa_put_char(char *buffer, int bufsiz, char ch)
{
    if (bufsiz > 1) {
        *buffer++ = ch;
        return 1;
    }
    return 0;
}

static inline int libtoa_put_pointer(char *buffer, int bufsiz, void *ptr)
{
    uint64_t val = (uint64_t)ptr;
    if (bufsiz > (int)sizeof("0x")) {
        *buffer++ = '0';
        *buffer++ = 'x';
    }
    return 2 + libtoa_put_hex64_lower(buffer, bufsiz - 2, val);
}

#endif /* end of include guard */
#ifdef __cplusplus
}
#endif
