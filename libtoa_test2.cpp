#include "libtoa.h"
#include <string.h>
#include <limits.h>
#include <assert.h>

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
            test_failed++;                                               \
        }                                                                \
        test_id++;                                                       \
    } while (0)

int main(int argc, char const *argv[])
{
    int test_id = 0;
    int test_failed = 0;
#define CHECK2(FUNC, B1, FMT, TYPE, MAX, MIN, V1, V2, V3) \
    do {                                                  \
        CHECK(B1, FMT, FUNC, (TYPE)0);                    \
        CHECK(B1, FMT, FUNC, (TYPE)V1);                   \
        CHECK(B1, FMT, FUNC, (TYPE)V2);                   \
        CHECK(B1, FMT, FUNC, (TYPE)V3);                   \
        CHECK(B1, FMT, FUNC, (TYPE)-V1);                  \
        CHECK(B1, FMT, FUNC, (TYPE)-V2);                  \
        CHECK(B1, FMT, FUNC, (TYPE)-V3);                  \
        CHECK(B1, FMT, FUNC, (TYPE)MAX);                  \
        CHECK(B1, FMT, FUNC, (TYPE)MIN);                  \
    } while (0)
    CHECK2(libtoa_put_int16, 3, "%hd", int16_t, SHRT_MAX, SHRT_MIN, 12, 789, 12345);
    CHECK2(libtoa_put_int32, 3, "%d", int32_t, INT_MAX, INT_MIN, 12, 789, 1234567);
    CHECK2(libtoa_put_uint16, 3, "%hu", uint16_t, USHRT_MAX, SHRT_MIN, 12, 789, 12345);
    CHECK2(libtoa_put_uint32, 3, "%u", uint32_t, UINT_MAX, INT_MIN, 12, 789, 1234567);

    CHECK2(libtoa_put_int16, 20, "%hd", int16_t, SHRT_MAX, SHRT_MIN, 12, 789, 12345);
    CHECK2(libtoa_put_int32, 20, "%d", int32_t, INT_MAX, INT_MIN, 12, 789, 1234567);
    CHECK2(libtoa_put_uint16, 20, "%hu", uint16_t, USHRT_MAX, SHRT_MIN, 12, 789, 12345);
    CHECK2(libtoa_put_uint32, 20, "%u", uint32_t, UINT_MAX, INT_MIN, 12, 789, 1234567);

    CHECK2(libtoa_put_hex32_lower, 20, "%x", unsigned, INT_MAX, INT_MIN, 0x12, 0x789, 0x1234567);
    CHECK2(libtoa_put_hex32_lower, 3, "%x", unsigned, INT_MAX, INT_MIN, 0x12, 0x789, 0x1234567);
    fprintf(stderr, "# tests: %d, # failed: %d\n", test_id, test_failed);
    assert(test_failed == 0);
    return 0;
}
