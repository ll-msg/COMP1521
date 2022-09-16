#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    struct stat s;
    int i = 1;
    long sum = 0;
    while (i < argc) {
        char *pathname = argv[i];
        if (stat(pathname, &s) != 0) {
            perror(pathname);
            exit(1);
        }
        printf("%s: %ld bytes\n", argv[i], (long)s.st_size);
        sum = sum + s.st_size;
        i++;
    }
    printf("Total: %ld bytes\n", sum);
    return 0;
}