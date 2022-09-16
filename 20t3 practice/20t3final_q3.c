#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int num_first;
    int num_second;
    if (getenv(argv[1]) == NULL) {
        num_first = 42;
    } else {
        num_first = atoi(getenv(argv[1]));
    }
    if (getenv(argv[2]) == NULL) {
        num_second = 42;
    } else {
        num_second = atoi(getenv(argv[2]));
    }
    int differ = num_first - num_second;

    if (differ >= 10 || differ <= -10) {
        printf("0\n");
    } else {
        printf("1\n");
    }
    return 0;
}