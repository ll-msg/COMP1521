#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    char *original = getenv("HOME");
    char file_path[200000];
    sprintf(file_path, "%s/.diary", original);

    FILE *file = 0;
    if ((file = fopen(file_path, "a")) == NULL) {
        file = fopen(file_path, "w");
    }
    for (int i = 1; argv[i] != NULL; i++) {
        fputs(argv[i], file);
        fputs(" ", file);
    }
    fputs("\n", file);
    fclose(file);
    return 0;
}