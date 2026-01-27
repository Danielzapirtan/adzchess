#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Prime checking function */
int isPrime(long long n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (long long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

/* Global variables */
long long mi = 0;
long long n = 0;
long long nmax = 0;
long long p = 0;
long long pr = 0;
long long r = 0;

int main() {
L10:
    /* Goldbach */
L15:
    mi = 1024 * 1024;
L20:
    n = 1536 * mi;
L30:
    nmax = 1792 * mi;
L40:
    if (n > nmax) goto L80;
L50:
    goto L1000;
L60:
    printf("%lld", n);
    printf("%s", "=");
    printf("%lld", p);
    printf("%s", "+");
    printf("%lld", n - p);
    printf("\n");
L65:
    n = n + 2;
L70:
    goto L40;
L80:
    return 0;
L900:
    printf("%s", "Counterexample: ");
    printf("%lld", n);
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
