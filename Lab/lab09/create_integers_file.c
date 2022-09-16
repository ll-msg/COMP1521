#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    
    // get a wrong number of arguments
    if (argc != 4) {
        fprintf(stderr, "Please give 3 arguments\n");
        return 1;
    }

    FILE *integer = fopen(argv[1], "w");
    int i = atoi(argv[2]);
    int j = atoi(argv[3]);

    while (i <= j) {
        fprintf(integer, "%d", i);
        fprintf(integer, "\n");
        i++;
    }

    fclose(integer);
    return 0;
}