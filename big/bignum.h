#ifndef __LINENOISE_H
#define __LINENOISE_H
#include <stdint.h>

#ifndef MAX_INT64
#define MAX_INT64 0xFFFFFFFFFFFFFFFF // 18446744073709551615
#endif
#ifndef MAX_INT64_DEC
#define MAX_INT64_DEC 0x8AC7230489E7FFFF // 18446744073709551615
#endif
#ifndef MAX_INT63
#define MAX_INT63 0x7FFFFFFFFFFFFFFF // 9223372036854775807
#endif

typedef union UBIGNUMI {
    unsigned long long val;
    struct {
        unsigned long long filled : 63;
        unsigned long long left;
        /*Indicate that whether ubgi is a large number or not*/
        uint8_t is_ptr : 1;
    };
    struct {
        /*[lsb][...]...[...][msb]*/
        unsigned long long *ptr;
        /*Length of ptr array*/
        unsigned long long size : 63;
    };
} ubgi;

unsigned long long power(unsigned long long a, int b);
void dec2bin(char *bin, char *dec);
ubgi new_ubgi(char *val);
void print_ubgi(ubgi a);
void drop_ubgi(ubgi *a);
void copy_ubgi(ubgi *dst, ubgi src);
void addll(ubgi *c, ubgi a, ubgi b);
void addl(ubgi *c, ubgi a, ubgi b);
void addnl(ubgi *c, ubgi a, ubgi b);
ubgi add(ubgi a, ubgi b);
void addll_dec(ubgi *c, ubgi a, ubgi b);
void addl_dec(ubgi *c, ubgi a, ubgi b);
void addnl_dec(ubgi *c, ubgi a, ubgi b);
ubgi add_dec(ubgi a, ubgi b);
ubgi rshift(ubgi a);

#endif