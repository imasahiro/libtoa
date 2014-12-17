libtoa - an optimized to-ascii converter
=====

[![Build Status](https://secure.travis-ci.org/imasahiro/libtoa.png)](http://travis-ci.org/imasahiro/libtoa)

Copyright (C) 2014 Masahiro Ide

Introduction
---
libtoa is an optimized version of integer-to-ascii convert for C with following properties:
* header-file only
* no external dependencies


API
---

1.1 integer to ascii convertion
```
static inline int libtoa_put_int8(char *buffer, int bufsiz, int8_t val)
static inline int libtoa_put_int16(char *buffer, int bufsiz, int16_t val)
static inline int libtoa_put_int32(char *buffer, int bufsiz, int32_t val)
static inline int libtoa_put_int64(char *buffer, int bufsiz, int64_t val)

static inline int libtoa_put_uint8(char *buffer, int bufsiz, uint8_t val)
static inline int libtoa_put_uint16(char *buffer, int bufsiz, uint16_t val)
static inline int libtoa_put_uint32(char *buffer, int bufsiz, uint32_t val)
static inline int libtoa_put_uint64(char *buffer, int bufsiz, uint64_t val)
```

1.2 Hexadecimal to ascii convertion
```
static inline int libtoa_put_hex8_lower(char *buffer, int bufsiz, uint8_t val)
static inline int libtoa_put_hex16_lower(char *buffer, int bufsiz, uint16_t val)
static inline int libtoa_put_hex32_lower(char *buffer, int bufsiz, uint32_t val)
static inline int libtoa_put_hex64_lower(char *buffer, int bufsiz, uint64_t val)
static inline int libtoa_put_hex8_upper(char *buffer, int bufsiz, uint8_t val)
static inline int libtoa_put_hex16_upper(char *buffer, int bufsiz, uint16_t val)
static inline int libtoa_put_hex32_upper(char *buffer, int bufsiz, uint32_t val)
static inline int libtoa_put_hex64_upper(char *buffer, int bufsiz, uint64_t val)
```

1.3 pointer to ascii convertion
```
static inline int libtoa_put_pointer(char *buffer, int bufsiz, void *ptr)
```

1.4  charctor to ascii convertion
```
static inline int libtoa_put_char(char *buffer, int bufsiz, char ch)
```

1.5 memcpy
```
static inline int libtoa_put_string(char *buffer, int bufsiz, char *str, int len)
```

TODO
---
[ ] support double-to-ascii convertion (or use grisu)
