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
    uint64_t val;
    struct {
        uint64_t filled : 63;
        uint64_t left;
        /*Indicate that whether ubgi is a large number or not*/
        uint8_t is_ptr : 1;
    };
    struct {
        /*[lsb][...]...[...][msb]*/
        uint64_t *ptr;
        /*Length of ptr array*/
        uint64_t size : 63;
    };
} ubgi;

typedef struct UBIGNUM {
    uint64_t *val;
    int len;
} big;

uint64_t power(uint64_t a, int b);
void double_dabble(int n, const uint64_t *arr, char **result);
void dec2bin(char *bin, char *dec);
big new_big(char *val);
char *print_big(big a);
void print_hex_big(big a);
void drop_big(big *a);
void copy_big(big *dst, big src);
big add_big(big a, big b);
big sub_big(big a, big b);
big lshift_big(big a);
big mul_big(big a, big b);

#endif