#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int16_t extract_bits(int start, int end, int values);

int main(int argc, char* argv[]) {
    struct stat s;
    int i = 1;
    int arr[4];
    while (i < argc) {
        char *pathname = argv[i];
        if (stat(pathname, &s) != 0) {
            perror(pathname);
            exit(1);
        }
        int num = s.st_mode;

        int16_t mask = extract_bits(0, 2, num);
        arr[0] = mask;
        mask = extract_bits(3, 5, num);
        mask = mask >> 3;
        arr[1] = mask;
        mask = extract_bits(6, 8, num);
        mask = mask >> 6;
        arr[2] = mask;
        mask = extract_bits(13, 15, num);
        mask = mask >> 12;
        arr[3] = mask;

        int index = 2;
        if (arr[3] == 4) {
            printf("d");
        } else {
            printf("-");
        }
        while (index >= 0) {
            if (arr[index] == 0) {
                printf("---");
            }
            if (arr[index] == 1) {
                printf("--x");
            }
            if (arr[index] == 2) {
                printf("-w-");
            }
            if (arr[index] == 3) {
                printf("-wx");
            }
            if (arr[index] == 4) {
                printf("r--");
            }
            if (arr[index] == 5) {
                printf("r-x");
            }
            if (arr[index] == 6) {
                printf("rw-");
            }
            if (arr[index] == 7) {
                printf("rwx");
            }
            index--;
        }
        printf(" %s\n", argv[i]);
        i++;
    }
    return 0;
}

int16_t extract_bits(int start, int end, int values) {
    int low_bit = start;
    int high_bit = end;
    int16_t value = values;

    int16_t mask;

    int mask_size = high_bit - low_bit + 1;

    mask = 1;
    mask = mask << mask_size;
    mask = mask - 1;
    mask = mask << low_bit;

    // get a value with the bits outside the range low_bit..high_bit set to zero
    int16_t extracted_bits = value & mask;

    return extracted_bits;
}