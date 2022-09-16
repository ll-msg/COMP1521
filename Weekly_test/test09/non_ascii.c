#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    int charac = 0;
    int i = 0;
    FILE *test = fopen(argv[1], "r");
    if (test == NULL) {
        printf("%s: byte %d is non-ASCII\n", argv[1], 0);
        return 0;
    }
    while ((charac = fgetc(test)) != EOF) {
        if (charac <= 255 && charac >= 128) {
            printf("%s: byte %d is non-ASCII\n", argv[1], i);
            return 0;
        }
        i++;
    }
    printf("%s is all ASCII\n", argv[1]);
    fclose(test);
    return 0;
}