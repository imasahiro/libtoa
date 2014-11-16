libtoa - an optimized to-ascii converter
=====

Copyright (C) 2014 Masahiro Ide

Introduction
---
libtoa is an optimized version of integer-to-ascii convert for C with following properties:
* header-file only
* no external dependencies


API
---
```
int libtoa_put_hex_lower(char *buffer, int bufsiz, uint64_t val)
int libtoa_put_hex_upper(char *buffer, int bufsiz, uint64_t val)
int libtoa_put_int8(char *buffer, int bufsiz, int8_t val)
int libtoa_put_uint8(char *buffer, int bufsiz, uint8_t val)
int libtoa_put_int16(char *buffer, int bufsiz, int16_t val)
int libtoa_put_uint16(char *buffer, int bufsiz, uint16_t val)
int libtoa_put_int32(char *buffer, int bufsiz, int32_t val)
int libtoa_put_uint32(char *buffer, int bufsiz, uint32_t val)
int libtoa_put_int64(char *buffer, int bufsiz, int64_t val)
int libtoa_put_uint64(char *buffer, int bufsiz, uint64_t val)
int libtoa_put_string(char *buffer, int bufsiz, char *str, int len)
int libtoa_put_char(char *buffer, int bufsiz, char ch)
int libtoa_put_pointer(char *buffer, int bufsiz, void *ptr)
```
