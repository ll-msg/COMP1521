#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    int charac;
    int arr[2000000] = {0};
    int i = 0;
    FILE *test = fopen(argv[1], "r+");
    while ((charac = fgetc(test)) != EOF) {
        if (charac < 128 || charac > 255) {
            arr[i] = charac;
            i++;
        }
    }
    FILE *test1 = fopen(argv[1], "w");
    int index = 0;
    fseek(test1, 0, SEEK_SET);
    while (index < i) {
        fputc(arr[index], test1);
        index++;
    }
    fclose(test);
    fclose(test1);
    return 0;
}