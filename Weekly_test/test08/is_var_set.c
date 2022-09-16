#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (getenv(argv[1]) != NULL) {
        printf("1\n");
    } else {
        printf("0\n");
    }
    return 0;
}