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
int h = 0;
int k = 0;
int n = 0;
int nmax = 0;

int main() {
L10:
    /* tabpr */
L20:
    n = 2;
L30:
    nmax = 8192 * 8192;
L40:
    if (n > nmax) goto L90;
L50:
    goto L1000;
L60:
    printf("%d", n);
    printf("\n");
L70:
    n = n + 1;
L80:
    goto L40;
L90:
    return 0;
L1000:
    k = 2;
L1010:
    if (k > n / k) goto L60;
L1020:
    if (n > n / k * k) goto L1040;
L1030:
    goto L70;
L1040:
    k = k + 1;
L1050:
    goto L1010;
L1060:
    return 0;
L2000:
    h = isPrime(n);
L2010:
    if (h == 1) goto L60;
L2020:
    goto L70;
    return 0;
}
