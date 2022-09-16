#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "At least one argument please\n");
    }
    FILE *binary = fopen(argv[1], "w");

    int i = 2;
    while (argv[i] != NULL) {
        char charac = atoi(argv[i]);
        fprintf(binary, "%c", charac);
        i++;
    }
    fclose(binary);

    return 0;
}