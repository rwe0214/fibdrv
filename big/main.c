#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.h"

void fib(int n)
{
    big f[3];
    char f0[100] = "0", f1[100] = "1";
    f[0] = new_big(f0);
    f[1] = new_big(f1);
    FILE *fp = fopen("./helper/out.txt", "w");

    for (int i = 2; i <= n; i++) {
        f[2] = add_big(f[0], f[1]);
        copy_big(&f[0], f[1]);
        copy_big(&f[1], f[2]);
    }
    printf("f(%d) = ", n);
    char *out = print_big(f[2]);
    for (int i = 0; i < 3; i++)
        drop_big(&f[i]);

    char *output = malloc(10 + strlen(out));
    sprintf(output, "%d\n%s\n", n, out);

    fputs(output, fp);
    free(out);
    fclose(fp);
}

void fib_fast(int k)
{
    int bs = 0, saved = k;
    while (k) {
        bs++;
        k /= 2;
    }
    k = saved;
    FILE *fp = fopen("./helper/out.txt", "w");
    big t1, t2, a, b;
    char f0[100] = "0", f1[100] = "1";
    a = new_big(f0);
    b = new_big(f1);
    for (int i = bs; i > 0; i--) {
        t1 = mul_big(a, (sub_big(lshift_big(b), a)));
        t2 = add_big(mul_big(b, b), mul_big(a, a));
        copy_big(&a, t1);
        copy_big(&b, t2);

        if ((32 - __builtin_clz(k)) == i && k > 0) {
            t1 = add_big(a, b);
            copy_big(&a, b);
            copy_big(&b, t1);
            k &= ~(1 << (i - 1));
        }
    }

    printf("f(%d) = ", saved);
    char *ans = print_big(a);
    char *out = malloc(10 + strlen(ans));
    sprintf(out, "%d\n%s\n", saved, ans);

    fputs(out, fp);
    fclose(fp);
    drop_big(&a);
    drop_big(&b);
    drop_big(&t1);
    drop_big(&t2);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        return -1;

    unsigned int n = strtoul(argv[1], NULL, 10);
    if (!n)
        return -2;

    fib_fast(n);
    return 0;
}