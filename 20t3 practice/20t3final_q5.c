#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // open the existing file
    FILE *original = fopen(argv[2], "r");
    // get the number of bytes
    int byte = atoi(argv[1]);
    // open the target file
    FILE *target = fopen(argv[3], "w");

    fseek(original, 0, SEEK_END);
    int count = ftell(original);
    int position = count - byte;

    if (position <= 0) {
        return 0;
    } else {
        char str[position + 1];
        fseek(original, 0, SEEK_SET);
        fread(str, 1, position, original);
        fwrite(str, 1, position, target);
    }

    fclose(original);
    fclose(target);
    return 0;
}