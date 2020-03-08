#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bignum.h"

#define max(x, y) (((x) > (y)) ? (x) : (y))

/*assume b is non-negative*/
unsigned long long power(unsigned long long a, int b) {
    if (!b)
        return 1;
    unsigned long long c = a;
    for (int i = 1; i < b; i++)
        c *= a;
    return c;
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
ubgi new_ubgi(char *val) {
    ubgi new;
    unsigned long long tmp = 0;
    int flag = 0;
    int size_dec = strlen(val);
    if (size_dec > 19) {
        /*Actual size of ubgi value, it is up to 2^(size-1)*/
        int size = 1024;
        char binary[size];
        dec2bin(binary, val);

        new.is_ptr = 1;
        new.size = (strlen(binary) % 64) ? strlen(binary) / 64 + 1
                                         : strlen(binary) / 64;
        new.ptr = malloc(new.size * sizeof(unsigned long long));

        for (int i = 0; i < strlen(binary); i++) {
            if (binary[i] - '0') {
                new.ptr[i / 64] |= ((unsigned long long)1 << (i % 64));
            }
        }
    } else {
        for (int i = size_dec - 1; i >= 0; i--) {
            tmp += (val[i] - '0') * power(10, size_dec - 1 - i);
        }
        if (tmp & 0x8000000000000000 && size_dec == 19) {
            new.is_ptr = 1;
            new.size = 1;
            new.ptr = malloc(sizeof(unsigned long long));
            *(new.ptr) = tmp;
        } else {
            new.is_ptr = 0;
            new.filled = tmp;
            new.left = MAX_INT63 - tmp;
        }
    }
    return new;
};

/*Print the value*/
void print_ubgi(ubgi a) {
    if (!a.is_ptr)
        printf("%llu\n", a.val);
    else if (a.size == 1)
        printf("%llu\n", *(a.ptr));
    else {
        printf("%llu ", (a.ptr)[a.size - 1]);
        for (int i = a.size - 2; i > 0; i--) {
            printf("%019llu ", (a.ptr)[i]);
        }
        printf("%019llu\n", (a.ptr)[0]);
    }
}

/*Free the memory allocation of large number*/
void drop_ubgi(ubgi *a) {
    if (a->is_ptr)
        free(a->ptr);
    a->size = 0;
    a->is_ptr = 0;
    a->val = 0;
}

void copy_ubgi(ubgi *dst, ubgi src) {
    if (!src.is_ptr) {
        dst->filled = src.filled;
        dst->left = src.left;
        dst->is_ptr = src.is_ptr;
    } else {
        dst->is_ptr = src.is_ptr;
        dst->size = src.size;
        dst->ptr = malloc(dst->size * sizeof(unsigned long long));
        memcpy(dst->ptr, src.ptr, dst->size * sizeof(unsigned long long));

    }
}

/*Add two large number*/
void addll(ubgi *c, ubgi a, ubgi b) {
    unsigned long long carry = 0;
    c->is_ptr = 1;
    c->size = max(a.size, b.size) + 1;
    c->ptr = malloc(c->size * sizeof(unsigned long long));
    for (int i = 0; i < a.size && i < b.size; i++) {
        if (MAX_INT64 - a.ptr[i] < b.ptr[i]) {
            c->ptr[i] = b.ptr[i] - (MAX_INT64 - a.ptr[i]) - 1 + carry;
            carry = 1;
        } else {
            if (MAX_INT64 - a.ptr[i] - b.ptr[i] < carry) {
                c->ptr[i] = carry - (MAX_INT64 - a.ptr[i] - b.ptr[i]) - 1;
                carry = 1;
            } else {
                c->ptr[i] = carry + a.ptr[i] + b.ptr[i];
                carry = 0;
            }
        }
    }
    if (a.size < b.size) {
        for (int i = a.size; i < b.size; i++) {
            if (MAX_INT64 - b.ptr[i] < carry) {
                c->ptr[i] = carry - (MAX_INT64 - b.ptr[i]) - 1;
                carry = 1;
            } else {
                c->ptr[i] = carry + b.ptr[i];
                carry = 0;
            }
        }
    }
    if ((b.size < a.size)) {
        for (int i = b.size; i < a.size; i++) {
            if (MAX_INT64 - a.ptr[i] < carry) {
                c->ptr[i] = carry - (MAX_INT64 - a.ptr[i]) - 1;
                carry = 1;
            } else {
                c->ptr[i] = carry + a.ptr[i];
                carry = 0;
            }
        }
    }
    if (carry == 1)
        c->ptr[c->size - 1] = carry;
    else {
        c->size--;
        c->ptr = realloc(c->ptr, c->size * sizeof(unsigned long long));
    }
}

/*Only a is large number*/
void addl(ubgi *c, ubgi a, ubgi b) {
    unsigned long long carry = 0;
    c->is_ptr = 1;
    c->size = a.size + 1;
    c->ptr = malloc(c->size * sizeof(unsigned long long));
    if (MAX_INT64 - a.ptr[0] < b.val) {
        c->ptr[0] = b.val - (MAX_INT64 - a.ptr[0]) - 1 + carry;
        carry = 1;
    } else {
        c->ptr[0] = b.val + a.ptr[0];
    }
    for (int i = 1; i < a.size; i++) {
        if (MAX_INT64 - a.ptr[i] < carry) {
            c->ptr[i] = carry - (MAX_INT64 - a.ptr[i]) - 1;
            carry = 1;
        } else {
            c->ptr[i] = carry + a.ptr[i];
            carry = 0;
        }
    }
    if (carry == 1)
        c->ptr[c->size - 1] = carry;
    else {
        c->size--;
        c->ptr = realloc(c->ptr, c->size * sizeof(unsigned long long));
    }
}

/*Neither a and b are large number*/
void addnl(ubgi *c, ubgi a, ubgi b) {
    if (b.left < a.val) {
        c->is_ptr = 1;
        c->size = 1;
        c->ptr = malloc(c->size * sizeof(unsigned long long));
        c->ptr[0] = a.val - (MAX_INT64 - b.val) - 1;
    } else {
        c->is_ptr = 0;
        c->val = a.val + b.val;
    }
}
ubgi add(ubgi a, ubgi b) {
    ubgi c;
    if (a.is_ptr)
        if (b.is_ptr) 
            addll(&c, a, b);
        else         
            addl(&c, a, b);
    else
        if (b.is_ptr)         
            addl(&c, b, a);
        else        
            addnl(&c, a, b);
    return c;
}


/*Add two large number*/
void addll_dec(ubgi *c, ubgi a, ubgi b) {
    unsigned long long carry = 0;
    c->is_ptr = 1;
    c->size = max(a.size, b.size) + 1;
    c->ptr = malloc(c->size * sizeof(unsigned long long));
    for (int i = 0; i < a.size && i < b.size; i++) {
        if (MAX_INT64_DEC - a.ptr[i] < b.ptr[i]) {
            c->ptr[i] = b.ptr[i] - (MAX_INT64_DEC - a.ptr[i]) - 1 + carry;
            carry = 1;
        } else {
            if (MAX_INT64_DEC - a.ptr[i] - b.ptr[i] < carry) {
                c->ptr[i] = carry - (MAX_INT64_DEC - a.ptr[i] - b.ptr[i]) - 1;
                carry = 1;
            } else {
                c->ptr[i] = carry + a.ptr[i] + b.ptr[i];
                carry = 0;
            }
        }
    }
    if (a.size < b.size) {
        for (int i = a.size; i < b.size; i++) {
            if (MAX_INT64_DEC - b.ptr[i] < carry) {
                c->ptr[i] = carry - (MAX_INT64_DEC - b.ptr[i]) - 1;
                carry = 1;
            } else {
                c->ptr[i] = carry + b.ptr[i];
                carry = 0;
            }
        }
    }
    if ((b.size < a.size)) {
        for (int i = b.size; i < a.size; i++) {
            if (MAX_INT64_DEC - a.ptr[i] < carry) {
                c->ptr[i] = carry - (MAX_INT64_DEC - a.ptr[i]) - 1;
                carry = 1;
            } else {
                c->ptr[i] = carry + a.ptr[i];
                carry = 0;
            }
        }
    }
    if (carry == 1)
        c->ptr[c->size - 1] = carry;
    else {
        c->size--;
        c->ptr = realloc(c->ptr, c->size * sizeof(unsigned long long));
    }
}

/*Only a is large number*/
void addl_dec(ubgi *c, ubgi a, ubgi b) {
    unsigned long long carry = 0;
    c->is_ptr = 1;
    c->size = a.size + 1;
    c->ptr = malloc(c->size * sizeof(unsigned long long));
    if (MAX_INT64_DEC - a.ptr[0] < b.val) {
        c->ptr[0] = b.val - (MAX_INT64_DEC - a.ptr[0]) - 1 + carry;
        carry = 1;
    } else {
        c->ptr[0] = b.val + a.ptr[0];
    }
    for (int i = 1; i < a.size; i++) {
        if (MAX_INT64_DEC - a.ptr[i] < carry) {
            c->ptr[i] = carry - (MAX_INT64_DEC - a.ptr[i]) - 1;
            carry = 1;
        } else {
            c->ptr[i] = carry + a.ptr[i];
            carry = 0;
        }
    }
    if (carry == 1)
        c->ptr[c->size - 1] = carry;
    else {
        c->size--;
        c->ptr = realloc(c->ptr, c->size * sizeof(unsigned long long));
    }
}

/*Neither a and b are large number*/
void addnl_dec(ubgi *c, ubgi a, ubgi b) {
    if (MAX_INT64_DEC - a.val < b.val) {
        c->is_ptr = 1;
        c->size = 2;
        c->ptr = malloc(c->size * sizeof(unsigned long long));
        c->ptr[0] = a.val - (MAX_INT64_DEC - b.val) - 1;
        c->ptr[1] += 1; 
    } else {
        c->is_ptr = 0;
        c->val = a.val + b.val;
    }
}

ubgi add_dec(ubgi a, ubgi b) {
    ubgi c;
    if (a.is_ptr)
        if (b.is_ptr) 
            addll_dec(&c, a, b);
        else         
            addl_dec(&c, a, b);
    else
        if (b.is_ptr)         
            addl_dec(&c, b, a);
        else        
            addnl_dec(&c, a, b);
    return c;
}

ubgi rshift(ubgi a){
    ubgi c;
    int carry=0;
    if(a.is_ptr){
        if(a.size == 1){
            c.is_ptr = 0;
            c.val = a.ptr[0] >> 1;
            return c;
        }
        else
            c.is_ptr = 1;
        /*Check whether reduce size or not*/
        if(!((a.ptr)[a.size-1] & 0xfffffffffffffffe)){
            /*ptr[msb] == 1*/
            c.size = a.size - 1;
            c.ptr = malloc(c.size * sizeof(unsigned long long));
            carry = 1;
        }
        else
        {
            c.size = a.size;
            c.ptr = malloc(c.size * sizeof(unsigned long long));
        }
        
        for(int i=c.size-1; i>=0; i--){
            c.ptr[i] = (a.ptr[i] >> 1);
            if(carry)
                c.ptr[i] |= 0x8000000000000000;
            if(a.ptr[i] & 0x1)
                carry = 1;
        }
    }
    else{
        c.is_ptr = 0;
        c.val = a.val >> 1;
    }
    return c;
}/*
void add_mul(ubgi *c, ubgi a, ubgi b){
    unsigned long long carry = 0;
    for(int i=c->size-1 ;i>=b.size; i--){
        if (MAX_INT64 - c->ptr[i] < a.ptr[i]) {
            c->ptr[i] = c->ptr[i] - (MAX_INT64 - a.ptr[i]) - 1 + carry;
            carry = 1;
        } else {
            if (MAX_INT64 - a.ptr[i] - c->ptr[i] < carry) {
                c->ptr[i] = carry - (MAX_INT64 - a.ptr[i] - c->ptr[i]) - 1;
                carry = 1;
            } else {
                c->ptr[i] = carry + a.ptr[i] + c->ptr[i];
                carry = 0;
            }
        }
    }
}*//*
void mul_ll(ubgi *c, ubgi a, ubgi b){
    for(int i=0; i<b.size; i++){
        c->ptr[i] = b.ptr[i];
    }
    for(int i=0; i<b.size * 64; i++){
        if(c->ptr[0] & 0x1)
            add_mul(&c, a, b);
        *c = rshift(*c);
    }
}*/
/*
ubgi mul(ubgi a, ubgi b){
    ubgi c;
    if(a.is_ptr && b.is_ptr)
        c.size = a.is_ptr + b.is_ptr;
    else if(a.is_ptr && !b.is_ptr){
        c.size = a.is_ptr + 1;
    }
    else if(!a.is_ptr && b.is_ptr){
        c.size = b.is_ptr + 1;
    }
    else
        c.size = 2;
    c.is_ptr = 1;
    c.ptr = malloc(c.size * sizeof(unsigned long long));

    unsigned long long count = c.size * 64;

    for(int i=)
    for(unsigned long long i=0; i<count; i++){

    }

    return c;
}
*/