#include <stdio.h>
#include <stdlib.h>

#define SERIES_MAX 46
int fibonacci(int n);
int main(void) {
    int num;
    while (scanf("%d", &num) == 1){
        printf("%d\n", fibonacci(num));
    }

    return 0;
}

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {

        return fibonacci(n - 1) + fibonacci (n - 2);
    }
}
