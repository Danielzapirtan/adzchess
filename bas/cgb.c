#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Prime checking function */
int isPrime(int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

/* Global variables */
int mi = 0;
int n = 0;
int nmax = 0;
int p = 0;
int pr = 0;
int r = 0;

int main() {
L10:
    /* Goldbach */
L32:
    n = 6;
L33:
    nmax = 10000;
L35:
    mi = 2;
L40:
    if (n > nmax) goto L80;
L50:
    goto L1000;
L60:
    printf("%d", n);
    printf("%s", "=");
    printf("%d", p);
    printf("%s", "+");
    printf("%d", n - p);
    printf("\n");
L65:
    n = n + 2;
L70:
    goto L40;
L80:
    printf("%s", "ok ");
    printf("%d", nmax);
    printf("\n");
L90:
    return 0;
L900:
    printf("%s", "Counterexample: ");
    printf("%d", n);
    printf("\n");
L910:
    return 0;
L1000:
    p = 3;
L1010:
    if (p > n - p) goto L900;
L1020:
    r = p;
L1030:
    pr = isPrime(r);
L1035:
    if (pr == 0) goto L1080;
L1040:
    r = n - r;
L1050:
    if (r == p) goto L60;
L1060:
    goto L1030;
L1080:
    p = p + 2;
L1090:
    goto L1010;
    return 0;
}
