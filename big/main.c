#include "bignum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fib(int n) {
    big f[3];
    char f0[100] = "0", f1[100] = "1";
    f[0] = new_big(f0);
    f[1] = new_big(f1);
    FILE *fp = fopen("./helper/out.txt", "w");
/*    
    for(int i=0; i<2; i++){
        printf("f(%d) = ", i);
        print_big(f[i]);
    }
*/
    for (int i = 2; i <= n; i++) {
        f[2] = add_big(f[0], f[1]);
        copy_big(&f[0], f[1]);
        copy_big(&f[1], f[2]);
        //printf("f(%d) = ", i);
        //print_big(f[2]);
    }
    printf("f(%d) = ", n);
    char *out = print_big(f[2]);
    for(int i=0; i<3; i++)
        drop_big(&f[i]);

    char *output = malloc(10 + strlen(out));
    sprintf(output, "%d\n%s\n", n, out);
    
    fputs(output, fp);
    free(out);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 2)
        return -1;

    unsigned int n = strtoul(argv[1], NULL, 10);
    if (!n)
        return -2;
    fib(n);
}