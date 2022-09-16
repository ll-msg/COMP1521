#include <stdio.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Please give one argument\n");
    }
    FILE *bytes = fopen(argv[1], "r");
    long i = 0;
    int charac;
    while ((charac = fgetc(bytes)) != EOF) {
        printf("byte %4ld: %3d 0x%02x", i, charac, charac);
        if (isprint(charac) != 0) {
            printf(" '%c'", charac);
        }
        printf("\n");
        i++;
    }
    fclose(bytes);
    return 0;
}