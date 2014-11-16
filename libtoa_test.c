#include "libtoa.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define CHECK(BUFFSIZE, EXPECTED_FORMAT, FUNC, VALUE)                    \
    do {                                                                 \
        char buf1[BUFFSIZE + 1] = {};                                    \
        char buf2[BUFFSIZE + 1] = {};                                    \
        int len1 = snprintf(buf1, BUFFSIZE + 1, EXPECTED_FORMAT, VALUE); \
        int len2 = FUNC(buf2, BUFFSIZE, VALUE);                          \
        if (memcmp(buf1, buf2, BUFFSIZE + 1) != 0 || len1 != len2) {     \
            fprintf(stderr, "id=%d, line=%d.\n", test_id, __LINE__);     \
            fprintf(stderr, "expected: '%s'(%d)\n", buf1, len1);         \
            fprintf(stderr, "result  : '%s'(%d)\n", buf2, len2);         \
        }                                                                \
        test_id++;                                                       \
    } while (0)

int main(int argc, char const *argv[])
{
    int test_id = 0;
#define CHECK2(FUNC, B1, FMT, MAX, MIN, V1, V2, V3) \
    do {                                            \
        CHECK(B1, FMT, FUNC, 0);                    \
        CHECK(B1, FMT, FUNC, V1);                   \
        CHECK(B1, FMT, FUNC, V2);                   \
        CHECK(B1, FMT, FUNC, V3);                   \
        CHECK(B1, FMT, FUNC, -V1);                  \
        CHECK(B1, FMT, FUNC, -V2);                  \
        CHECK(B1, FMT, FUNC, -V3);                  \
        CHECK(B1, FMT, FUNC, MAX);                  \
        CHECK(B1, FMT, FUNC, MIN);                  \
    } while (0)
    CHECK2(libtoa_put_int32, 20, "%d", INT_MAX, INT_MIN, 12, 789, 1234567);
    CHECK2(libtoa_put_hex32_upper, 20, "%X", INT_MAX, INT_MIN, 0x12, 0x789, 0x1234567);
    CHECK2(libtoa_put_hex32_upper, 3, "%X", INT_MAX, INT_MIN, 0x12, 0x789, 0x1234567);
    return 0;
}
