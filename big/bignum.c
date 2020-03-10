#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bignum.h"

#define max(x, y) (((x) > (y)) ? (x) : (y))

/*assume b is non-negative*/
uint64_t power(uint64_t a, int b) {
    if (!b)
        return 1;
    uint64_t c = a;
    for (int i = 1; i < b; i++)
        c *= a;
    return c;
}

void double_dabble(int n, const uint64_t *arr, char **result)
{
    int nbits = 64*n;         /* length of arr in bits */
    int nscratch = nbits/3;   /* length of scratch in bytes */
    char *scratch = calloc(1 + nscratch, sizeof *scratch);
    int i, j, k;
    int smin = nscratch-2;    /* speed optimization */

    for (i=n-1; i >= 0; --i) {
        for (j=0; j < 64; ++j) {
            /* This bit will be shifted in on the right. */
            int shifted_in = (arr[i] & ((uint64_t)1 << (63-j)))? 1: 0;
            /* Add 3 everywhere that scratch[k] >= 5. */
            for (k=smin; k < nscratch; ++k)
              scratch[k] += (scratch[k] >= 5)? 3: 0;

            /* Shift scratch to the left by one position. */
            if (scratch[smin] >= 8)
              smin -= 1;
            for (k=smin; k < nscratch-1; ++k) {
                scratch[k] <<= 1;
                scratch[k] &= 0xF;
                scratch[k] |= (scratch[k+1] >= 8);
            }

            /* Shift in the new bit from arr. */
            scratch[nscratch-1] <<= 1;
            scratch[nscratch-1] &= 0xF;
            scratch[nscratch-1] |= shifted_in;
        }
    }

    /* Remove leading zeros from the scratch space. */
    for (k=0; k < nscratch-1; ++k)
      if (scratch[k] != 0) break;
    nscratch -= k;
    memmove(scratch, scratch+k, nscratch+1);

    for (k=0; k < nscratch; ++k)
        scratch[k] += '0';

    /* Resize and return the resulting string. */
    *result = realloc(scratch, nscratch+1);
}

/*Convert decimal number string into binary number*/
void dec2bin(char *bin, char *dec) {
    int len = strlen(dec);
    int cnt = 0;

    for (int i = 0; i < len; i++)
        dec[i] -= '0';

    if (!dec[0] && len == 1) {
        bin[0] = '0';
        bin[1] = '\0';
        return;
    }

    while (1) {
        int zero = 1;
        for (int i = 0; i < len; i++)
            if (dec[i]) {
                zero = 0;
                break;
            }
        if (zero)
            break;

        bin[cnt++] = (dec[len - 1] & 0x1) + '0';

        if (dec[len - 1] & 1)
            dec[len - 1]--;

        for (int i = 0; i < len; i++)
            if (dec[i] & 0x1) {
                dec[i] >>= 1;
                dec[i + 1] += 10;
            } else
                dec[i] >>= 1;
    }
    bin[cnt] = '\0';
    return;
}

/*Construct a ubgi structure*/
big new_big(char *val){
    big new;
    int size = 1024;
    char binary[size];
    dec2bin(binary, val);

    new.len = (strlen(binary) % 64) ? strlen(binary) / 64 + 1
                                        : strlen(binary) / 64;
    new.val = malloc(new.len * sizeof(uint64_t));

    for (int i = 0; i < strlen(binary); i++) {
        if (binary[i] - '0') {
            new.val[i / 64] |= ((uint64_t)1 << (i % 64));
        }
    }
    return new;
}

/*Print the value*/
char *print_big(big a) {
    char *text;
    double_dabble(a.len, a.val, &text);
    printf("%s\n", text);
    return text;
}

/*Free the memory allocation of large number*/
void drop_big(big *a) {
    if(a->val)
        free(a->val);
    a->len = 0;
}

void copy_big(big *dst, big src){
    dst->len = src.len;
    dst->val = malloc(dst->len * sizeof(uint64_t));
    memcpy(dst->val, src.val, dst->len * sizeof(uint64_t));
}

big add_big(big a, big b){
    big c;
    uint64_t carry = 0;
    c.len = max(a.len, b.len) + 1;
    c.val = malloc(c.len * sizeof(uint64_t));
    for (int i = 0; i < a.len && i < b.len; i++) {
        if (MAX_INT64 - a.val[i] < b.val[i]) {
            c.val[i] = b.val[i] - (MAX_INT64 - a.val[i]) - 1 + carry;
            carry = 1;
        } else {
            if (MAX_INT64 - a.val[i] - b.val[i] < carry) {
                c.val[i] = carry - (MAX_INT64 - a.val[i] - b.val[i]) - 1;
                carry = 1;
            } else {
                c.val[i] = carry + a.val[i] + b.val[i];
                carry = 0;
            }
        }
    }
    if (a.len < b.len) {
        for (int i = a.len; i < b.len; i++) {
            if (MAX_INT64 - b.val[i] < carry) {
                c.val[i] = carry - (MAX_INT64 - b.val[i]) - 1;
                carry = 1;
            } else {
                c.val[i] = carry + b.val[i];
                carry = 0;
            }
        }
    }
    if ((b.len < a.len)) {
        for (int i = b.len; i < a.len; i++) {
            if (MAX_INT64 - a.val[i] < carry) {
                c.val[i] = carry - (MAX_INT64 - a.val[i]) - 1;
                carry = 1;
            } else {
                c.val[i] = carry + a.val[i];
                carry = 0;
            }
        }
    }
    if (carry == 1)
        c.val[c.len - 1] = carry;
    else {
        c.len--;
        c.val = realloc(c.val, c.len * sizeof(uint64_t));
    }
    return c;
}

/*Assume that a is always larger than b*/
big sub_big(big a, big b){
    big c;
    c.len = (a.len > b.len)?a.len:(a.len+1);
    c.val = malloc(c.len * sizeof(uint64_t));

    for(int i=0; i<a.len-1 && i<b.len; i++){
        if(a.val[i] < b.val[0]){
            a.val[i+1]--;
            c.val[i] = MAX_INT64 - (b.val[i] - a.val[i] - 1);
        }
        else
            c.val[i] = a.val[i] - b.val[i];
    }
    if(a.len == b.len)
        c.val[c.len] = a.val[a.len] - b.val[b.len];
    else
        c.val[c.len] = a.val[a.len];

    return c;
}

// void cut_big(big *dst, big src, int start, int end){
//     dst->len = end - start;
//     dst->val = malloc(dst->len * sizeof(uint64_t));
//     int k=0;
//     for(int i=start; i<end; i++)
//         dst->val[k++] = src.val[i];
// }

// big mul(big a, int b){
//     big c;
// }

// big mul(big a, big b){
//     big c;
//     c.len = 2;
//     c.val = malloc(c.len * sizeof(uint64_t));
//     c.val[0] = b.val[0];

//     int shifted;
//     for(int i=0; i<64; i++){
//         if(c.val[0] & 0x1)
//             c.val[1] += a.val[0];
//         shifted = c.val[1] & 0x1;
//         c.val[1] >>= 1;
//         c.val[0] >>= 1;
//         if(shifted)
//             c.val[0] |= 0x8000000000000000;
//     }
//     return c;
// }

// /*a and b's len are the same*/
// big karatsuba_big(big a, big b){
//     big a_h, a_l, b_h, b_l, c;

//     if(a.len == 1 && b.len == 1){
//         return mul(a, b);
//     }

//     int len = a.len;
//     int k = len/2;
//     c.len = 2*a.len;
//     c.val = malloc(c.len * sizeof(uint64_t));

//     cut_big(&a_l, a, 0, k);
//     cut_big(&a_h, a, k, a.len);
//     cut_big(&b_l, b, 0, k);
//     cut_big(&b_h, b, k, b.len);

//     big tmp0, tmp1, tmp2, a_hl, b_hl;

//     a_hl = add_big(a_h, a_l);
//     b_hl = add_big(b_h, b_l);

//     tmp0 = karatsuba_big(a_h, b_h);
//     tmp1 = karatsuba_big(a_l, b_l);
//     tmp2 = karatsuba_big(a_hl, b_hl);
    
//     tmp2 = sub_big(tmp2, add_big(tmp0, tmp1));

//     mul(tmp0, power(10, 2*k));

//     return c;
// }