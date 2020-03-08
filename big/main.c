#include "bignum.h"
#include <stdio.h>

void fib() {
    ubgi f[3];
    char f0[100] = "0", f1[100] = "1";
    f[0] = new_ubgi(f0);
    f[1] = new_ubgi(f1);
    
    for(int i=0; i<2; i++){
        printf("f(%d) = ", i);
        print_ubgi(f[i]);
    }

    for (int i = 2; i < 301; i++) {
        f[2] = add_dec(f[0], f[1]);
        copy_ubgi(&f[0], f[1]);
        copy_ubgi(&f[1], f[2]);
        printf("f(%d) = ", i);
        print_ubgi(f[2]);
    }
    
    for(int i=0; i<3; i++)
        drop_ubgi(&f[i]);
}

int main() {
    fib();
    /*char num[100] = "170141183460469231731687303715884105728";
    ubgi a = new_ubgi(num);
    for(int i=0; i<200; i++){
        a = rshift(a);
        print_ubgi(a);
    }*/
    return 0;
}