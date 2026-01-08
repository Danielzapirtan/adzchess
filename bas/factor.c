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
int k = 0;
int l = 0;
int p = 0;

int main() {
L10:
    /* factor */
L20:
    p = 2047;
L30:
    k = 3;
L40:
    if (k > p / k) goto L110;
L50:
    l = p / k;
L53:
    l = l * k;
L55:
    if (p > l) goto L90;
L60:
    printf("%d", k);
    printf("%s", " ");
L70:
    p = p / k;
L80:
    goto L40;
L90:
    k = k + 2;
L100:
    goto L40;
L110:
    if (p < 2) goto L120;
L115:
    printf("%d", p);
    printf("%s", " ");
L120:
    printf("\n");
L130:
    return 0;
    return 0;
}
